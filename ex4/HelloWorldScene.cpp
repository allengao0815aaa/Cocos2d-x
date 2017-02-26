#include <algorithm>
#include "HelloWorldScene.h"
#include "Monster.h"
#define database UserDefault::getInstance()
#pragma execution_character_set("utf-8")
USING_NS_CC;
using namespace std;

void stopCallBack();

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	//用tilemap创建背景
	TMXTiledMap* tmx = TMXTiledMap::create("map.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor());
	addChild(tmx, 0);

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
							origin.y + visibleSize.height/2));
	addChild(player, 3);
	lastCid = 'D';

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/Marker Felt.ttf";
	ttfConfig.fontSize = 45;

	//本地存储已杀怪物数
	if (!database->getIntegerForKey("killed")) {
		database->setIntegerForKey("killed", 0);
	}
	killed = database->getIntegerForKey("killed");
	auto str = String::createWithFormat("%d", killed);
	killedText = Label::createWithTTF(ttfConfig, str->getCString());
	killedText->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - killedText->getContentSize().height));
	addChild(killedText);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x+14*pT->getContentSize().width,origin.y + visibleSize.height - 2*pT->getContentSize().height));
	addChild(pT,1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0,0);

	//静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	//攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113*i,0,113,113)));
		attack.pushBack(frame);
	}
	
	//死亡动画
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	
	//运动动画
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}

	//Label
	auto menuLabel1 = Label::createWithTTF(ttfConfig, "W");
	auto menuLabel2 = Label::createWithTTF(ttfConfig, "S");
	auto menuLabel3 = Label::createWithTTF(ttfConfig, "A");
	auto menuLabel4 = Label::createWithTTF(ttfConfig, "D");
	auto menuLabel5 = Label::createWithTTF(ttfConfig, "X");
	auto menuLabel6 = Label::createWithTTF(ttfConfig, "Y");
	menuLabel1->setColor(Color3B::RED);
	menuLabel2->setColor(Color3B::RED);
	menuLabel3->setColor(Color3B::RED);
	menuLabel4->setColor(Color3B::RED);
	menuLabel1->enableShadow(Color4B::BLACK);
	menuLabel2->enableShadow(Color4B::BLACK);
	menuLabel3->enableShadow(Color4B::BLACK);
	menuLabel4->enableShadow(Color4B::BLACK);
	menuLabel5->enableOutline(Color4B::GREEN, 5);
	menuLabel6->enableOutline(Color4B::GREEN, 5);
	//menuItem
	auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_1(HelloWorld::moveEvent, this,'W'));
	auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_1(HelloWorld::moveEvent, this,'S'));
	auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_1(HelloWorld::moveEvent, this,'A'));
	auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_1(HelloWorld::moveEvent, this,'D'));
	auto item5 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'Y'));

	item3->setPosition(Vec2(origin.x+item3->getContentSize().width, origin.y+item3->getContentSize().height));
	item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
	item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
	item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
	item5->setPosition(Vec2(origin.x+visibleSize.width-item5->getContentSize().width, item3->getPosition().y));
	
	auto menu = Menu::create(item1, item2, item3, item4, item5, NULL);
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 1);

	schedule(schedule_selector(HelloWorld::updateKilled), 0.0001f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::monsterFunc), 5.0f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::hitByMonster), 0.0001f, kRepeatForever, 0);

	return true;
}

void HelloWorld::moveEvent(Ref*,char cid) {
	auto fac = Factory::getInstance();
	if (fac->checkRunning()) return;
	double tmp, playerX = player->getPositionX(), playerY = player->getPositionY();
	auto playerSize = player->getContentSize();
	auto animation = Animation::createWithSpriteFrames(run, 0.05f);
	auto animate = Animate::create(animation);
	player->runAction(animate);
	
	switch (cid) {
		case 'W': {
			tmp = min(playerY + 40, visibleSize.height - playerSize.height / 2.0);
			auto pos = Vec2(playerX, tmp);
			auto moveTo = MoveTo::create(0.3f, pos);
			player->runAction(moveTo);
			break;
		}
		case 'A': {
			if (lastCid != 'A') player->setFlipX(true);
			lastCid = 'A';
			tmp = max(playerX - 40, playerSize.width / 2.0);
			auto pos = Vec2(tmp, playerY);
			auto moveTo = MoveTo::create(0.3f, pos);
			player->runAction(moveTo);
			break;
		}
		case 'S': {
			tmp = max(playerY - 40, playerSize.height / 2.0);
			auto pos = Vec2(playerX, tmp);
			auto moveTo = MoveTo::create(0.3f, pos);
			player->runAction(moveTo);
			break;
		}
		case 'D': {
			if (lastCid != 'D') player->setFlipX(false);
			lastCid = 'D';
			tmp = min(playerX + 40, visibleSize.width - playerSize.width / 2.0);
			auto pos = Vec2(tmp, playerY);
			auto moveTo = MoveTo::create(0.3f, pos);
			player->runAction(moveTo);
			break;
		}
	}
}

void HelloWorld::actionEvent(Ref*, char cid) {
	auto fac = Factory::getInstance();
	switch (cid) {
		case 'X': {
			fac->setRunning();
			player->stopAllActions();
			auto animation1 = Animation::createWithSpriteFrames(dead, 0.1f);
			auto animate1 = Animate::create(animation1);
			auto animation2 = Animation::createWithSpriteFrames(idle, 0.1f);
			auto animate2 = Animate::create(animation2);
			auto seq = Sequence::create(animate1, animate2, CallFunc::create(stopCallBack), NULL);
			player->runAction(seq);
			pT->runAction(ProgressTo::create(1, pT->getPercentage() - 20));
			break;
		}
		case 'Y': {
			if (fac->checkRunning()) return;
			fac->setRunning();
			auto animation1 = Animation::createWithSpriteFrames(attack, 0.05f);
			auto animate1 = Animate::create(animation1);
			auto animation2 = Animation::createWithSpriteFrames(idle, 0.1f);
			auto animate2 = Animate::create(animation2);
			auto seq = Sequence::create(animate1, animate2, CallFunc::create(stopCallBack), NULL);
			if (attackMonster()) {
				pT->runAction(ProgressTo::create(1, pT->getPercentage() + 20));
				killed++;
				database->setIntegerForKey("killed", killed);
			}
			player->runAction(seq);
			break;
		}
	}
}

//杀怪数更新
void HelloWorld::updateKilled(float dt) {
	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/Marker Felt.ttf";
	ttfConfig.fontSize = 45;
	auto str = String::createWithFormat("%d", killed);
	killedText->setString(str->getCString());
}

//怪物新建和移动
void HelloWorld::monsterFunc(float dt) {
	auto fac = Factory::getInstance();
	auto m = fac->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	m->setPosition(x, y);
	addChild(m, 3);
	fac->moveMonster(player->getPosition(), 1.0f);
}

//被怪物攻击
void HelloWorld::hitByMonster(float dt) {
	auto fac = Factory::getInstance();
	Sprite* collision = fac->collider(player->getBoundingBox());
	if (collision != NULL) {
		fac->removeMonster(collision);
		actionEvent(this, 'X');
	}
}

//攻击怪物
bool HelloWorld::attackMonster() {
	Rect playerRect = player->getBoundingBox();
	Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY(),
		playerRect.getMaxX() - playerRect.getMinX() + 80, playerRect.getMaxY() - playerRect.getMinY());
	auto fac = Factory::getInstance();
	Sprite* collision = fac->collider(attackRect);
	if (collision != NULL) {
		fac->removeMonster(collision);
		return true;
	}
	return false;
}

//结束人物正在进行动作的状态
void stopCallBack() {
	auto fac = Factory::getInstance();
	fac->endRunning();
}
