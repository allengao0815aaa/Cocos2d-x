#include <algorithm>
#include "HelloWorldScene.h"
#pragma execution_character_set("utf-8")
USING_NS_CC;
using namespace std;

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

	auto bg = Sprite::create("background.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	addChild(bg, 0);

	//����һ����ͼ
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//����ͼ�������ص�λ�и�����ؼ�֡
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//ʹ�õ�һ֡��������
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
							origin.y + visibleSize.height/2));
	addChild(player, 3);


	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/Marker Felt.ttf";
	ttfConfig.fontSize = 45;
	
	//����ʱ
	time = Label::createWithTTF(ttfConfig, "180");
	//����ʱ�����Ե��õ�����
	schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);
	//����ʱ������
	dtime = 180;
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getContentSize().height));
	addChild(time);

	//hp��
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//ʹ��hp������progressBar
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

	//��̬����
	idle.reserve(1);
	idle.pushBack(frame0);

	//��������
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113*i,0,113,113)));
		attack.pushBack(frame);
	}
	
	//��������
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	
	//�˶�����
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
	auto item5 = MenuItemLabel::create(menuLabel5, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'X'));
	auto item6 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'Y'));

	item3->setPosition(Vec2(origin.x+item3->getContentSize().width,origin.y+item3->getContentSize().height));
	item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
	item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
	item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
	item5->setPosition(Vec2(origin.x+visibleSize.width-item5->getContentSize().width,item1->getPosition().y));
	item6->setPosition(Vec2(item5->getPosition().x-item6->getContentSize().width,item3->getPosition().y));
	
	auto menu = Menu::create(item1, item2, item3, item4, item5, item6, NULL);
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 1);

	return true;
}

void HelloWorld::moveEvent(Ref*,char cid) {
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
			tmp = min(playerX + 40, visibleSize.width - playerSize.width / 2.0);
			auto pos = Vec2(tmp, playerY);
			auto moveTo = MoveTo::create(0.3f, pos);
			player->runAction(moveTo);
			break;
		}
	}
}

void HelloWorld::actionEvent(Ref*, char cid) {
	switch (cid) {
		case 'X': {
			auto animation1 = Animation::createWithSpriteFrames(dead, 0.1f);
			auto animate1 = Animate::create(animation1);
			auto animation2 = Animation::createWithSpriteFrames(idle, 0.1f);
			auto animate2 = Animate::create(animation2);
			auto seq = Sequence::create(animate1, animate2, nullptr);
			player->runAction(seq);
			pT->runAction(ProgressTo::create(1, pT->getPercentage() - 20));
			break;
		}
		case 'Y': {
			auto animation1 = Animation::createWithSpriteFrames(attack, 0.1f);
			auto animate1 = Animate::create(animation1);
			auto animation2 = Animation::createWithSpriteFrames(idle, 0.1f);
			auto animate2 = Animate::create(animation2);
			auto seq = Sequence::create(animate1, animate2, nullptr);
			player->runAction(seq);
			pT->runAction(ProgressTo::create(1, pT->getPercentage() + 20));
			break;
		}
	}
}

void HelloWorld::updateTime(float dt) {
	dtime = dtime - 1;
	if (dtime < 0) dtime = 180;
	auto str = String::createWithFormat("%d", dtime);
	time->setString(str->getCString());
}
