#include "BreakoutScene.h"
#include <string>
using namespace std;
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

void Breakout::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* Breakout::createScene() {
    auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    scene->getPhysicsWorld()->setGravity(Point(0, 0));

    auto layer = Breakout::create(scene->getPhysicsWorld());

    scene->addChild(layer);
    return scene;
}

bool Breakout::init(PhysicsWorld* world) {
    if (!Layer::init()) {
        return false;
    }

	this->setPhysicsWorld(world);
    visibleSize = Director::getInstance()->getVisibleSize();

    preloadMusic();
    playBgm();

    addBackground();
    addEdge();
    addPlayer();
	addRope();

	addContactListener();
    addTouchListener();
    addKeyboardListener();

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/Marker Felt.ttf";
	ttfConfig.fontSize = 45;
	time = Label::createWithTTF(ttfConfig, "Time: 0");
	time->setPosition(Vec2(visibleSize.width - time->getContentSize().width,
		visibleSize.height - time->getContentSize().height));
	dtime = 0;
	this->addChild(time);
    this->schedule(schedule_selector(Breakout::update), 1);
	hp = Label::createWithTTF(ttfConfig, "HP: 10");
	hp->setPosition(Vec2(hp->getContentSize().width, visibleSize.height - hp->getContentSize().height));
	dhp = 10;
	this->addChild(hp);
	this->schedule(schedule_selector(Breakout::updateHP), 0.001f);
    return true;
}

Breakout * Breakout::create(PhysicsWorld * world) {
	Breakout* pRet = new(std::nothrow) Breakout();
	if (pRet && pRet->init(world)) {
		pRet->autorelease();
		return pRet;
	}
	delete pRet;
	pRet = NULL;
	return NULL;
}

void Breakout::preloadMusic() {
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("music/meet_stone.wav");
}

void Breakout::playBgm() {
    SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

void Breakout::addBackground() {
    auto bgsprite = Sprite::create("black_hole_bg0.jpg");
    bgsprite->setPosition(visibleSize / 2);
    bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, 0);

    auto ps = ParticleSystemQuad::create("black_hole.plist");
    ps->setPosition(visibleSize / 2);
    this->addChild(ps);
}

void Breakout::addEdge() {
    auto edgeSp = Sprite::create();
    auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
	boundBody->setDynamic(false);
    boundBody->setTag(0);
    edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
    edgeSp->setPhysicsBody(boundBody);
    this->addChild(edgeSp);
}

void Breakout::addPlayer() {
    player = Sprite::create("player.png");
    player->setAnchorPoint(Vec2(0.5, 0.5));
    player->setPhysicsBody(PhysicsBody::createCircle(35));
	player->getPhysicsBody()->setGroup(-1);
	player->getPhysicsBody()->setCategoryBitmask(0xFFFFFFFF);
	player->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	player->getPhysicsBody()->setCollisionBitmask(0xFFFFFFFF);
    player->setPosition(visibleSize / 2);
    player->getPhysicsBody()->setTag(1);
    player->getPhysicsBody()->setAngularVelocityLimit(0);
    addChild(player);
}

void Breakout::addRope() {
	rope = Sprite::create("rope.png");
	rope->setPhysicsBody(PhysicsBody::createBox(rope->getContentSize()));
	rope->getPhysicsBody()->setGroup(-2);
	rope->getPhysicsBody()->setCategoryBitmask(0xFFFFFFFF);
	rope->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	rope->getPhysicsBody()->setCollisionBitmask(0xFFFFFFFF);
	rope->getPhysicsBody()->setTag(2);
	rope->setPosition(visibleSize.width/2 + 91, visibleSize.height - 10);
	addChild(rope);

	skull = Sprite::create("skull.png");
	skull->setPhysicsBody(PhysicsBody::createCircle(skull->getContentSize().width / 2));
	skull->getPhysicsBody()->setGroup(-2);
	skull->getPhysicsBody()->setCategoryBitmask(0xFFFFFFFF);
	skull->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	skull->getPhysicsBody()->setCollisionBitmask(0xFFFFFFFF);
	skull->getPhysicsBody()->setTag(5);
	skull->setPosition(rope->getPosition() + Vec2(-70, 0));
	addChild(skull);

	skull2 = Sprite::create("skull.png");
	skull2->setPhysicsBody(PhysicsBody::createCircle(skull2->getContentSize().width / 2));
	skull2->getPhysicsBody()->setGroup(-2);
	skull2->getPhysicsBody()->setCategoryBitmask(0xFFFFFFFF);
	skull2->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	skull2->getPhysicsBody()->setCollisionBitmask(0xFFFFFFFF);
	skull2->getPhysicsBody()->setTag(5);
	skull2->setPosition(rope->getPosition() + Vec2(70, 0));
	addChild(skull2);

	skull->getPhysicsBody()->setVelocity(Point(-20, -20));

	PhysicsJointDistance* distanceJoint = PhysicsJointDistance::construct(
		rope->getPhysicsBody(), skull->getPhysicsBody(),
		Vec2(-60, 0), Vec2(0, 0));
	m_world->addJoint(distanceJoint);

	PhysicsJointDistance* distanceJoint2 = PhysicsJointDistance::construct(
		rope->getPhysicsBody(), skull2->getPhysicsBody(),
		Vec2(60, 0), Vec2(0, 0));
	m_world->addJoint(distanceJoint2);
}

void Breakout::addContactListener() {
	auto touchListener = EventListenerPhysicsContact::create();
	touchListener->onContactBegin = CC_CALLBACK_1(Breakout::onConcactBegan, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

void Breakout::addTouchListener(){
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(Breakout::onTouchBegan,this);
    touchListener->onTouchMoved = CC_CALLBACK_2(Breakout::onTouchMoved,this);
    touchListener->onTouchEnded = CC_CALLBACK_2(Breakout::onTouchEnded,this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void Breakout::addKeyboardListener() {
    auto keboardListener = EventListenerKeyboard::create();
    keboardListener->onKeyPressed = CC_CALLBACK_2(Breakout::onKeyPressed, this);
    keboardListener->onKeyReleased = CC_CALLBACK_2(Breakout::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

bool Breakout::onTouchBegan(Touch *touch, Event *unused_event) {
    Vec2 position = touch->getLocation();
	if (position.getDistance(player->getPosition()) < 20) {
		double newplayerX = position.x, newplayerY = position.y;
		auto playerSize = player->getContentSize();
		newplayerX = max(newplayerX, playerSize.width / 2.0);
		newplayerX = min(newplayerX, visibleSize.width - playerSize.width / 2.0);
		newplayerY = max(newplayerY, playerSize.height / 2.0);
		newplayerY = min(newplayerY, visibleSize.height - playerSize.height / 2.0);
		player->setPosition(newplayerX, newplayerY);
		return true;
	}
	return false;
}

void Breakout::onTouchMoved(Touch *touch, Event *unused_event) {
	Vec2 position = touch->getLocation();
	double newplayerX = position.x, newplayerY = position.y;
	auto playerSize = player->getContentSize();
	newplayerX = max(newplayerX, playerSize.width / 2.0);
	newplayerX = min(newplayerX, visibleSize.width - playerSize.width / 2.0);
	newplayerY = max(newplayerY, playerSize.height / 2.0);
	newplayerY = min(newplayerY, visibleSize.height - playerSize.height / 2.0);
	player->setPosition(newplayerX, newplayerY);
}

void Breakout::onTouchEnded(Touch *touch, Event *unused_event) {
}

bool Breakout::onConcactBegan(PhysicsContact& contact) {
	auto sp1 = (Sprite*)contact.getShapeA()->getBody()->getNode();
	auto sp2 = (Sprite*)contact.getShapeB()->getBody()->getNode();
	Vec2 location;
	int tag1 = contact.getShapeA()->getBody()->getTag();
	int tag2 = contact.getShapeB()->getBody()->getTag();
	if (tag1 == 1) {
		if (tag2 == 2) return false;
		if (tag2 == 3) {
			dhp += 5;
			sp2->removeFromParentAndCleanup(true);
			return true;
		}
		if (tag2 == 4) {
			allExplode();
			sp2->removeFromParentAndCleanup(true);
			return true;
		}
		if (tag2 == 5) {
			dhp = 0;
			return true;
		}
		location = sp2->getPosition();
		sp2->removeFromParentAndCleanup(true);
	} else {
		if (tag1 == 2) return false;
		if (tag1 == 3) {
			dhp += 5;
			sp1->removeFromParentAndCleanup(true);
			return true;
		}
		if (tag1 == 4) {
			allExplode();
			sp1->removeFromParentAndCleanup(true);
			return true;
		}
		if (tag1 == 5) {
			dhp = 0;
			return true;
		}
		location = sp1->getPosition();
		sp1->removeFromParentAndCleanup(true);
	}
	dhp = dhp - 1;
	SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav");
	auto ps = ParticleSystemQuad::create("explode.plist");
	ps->setPosition(location);
	this->addChild(ps);
	return true;
}

void Breakout::updateHP(float dt) {
	if (player == NULL) return;
	Vec2 dir = player->getPosition() - skull->getPosition();
	dir.normalize();
	skull->getPhysicsBody()->setVelocity(dir * 30);
	auto str = String::createWithFormat("HP: %d", dhp);
	hp->setString(str->getCString());
	if (dhp <= 0) {
		SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav");
		auto ps = ParticleSystemQuad::create("explode.plist");
		Vec2 location = player->getPosition();
		ps->setPosition(location);
		player->removeFromParentAndCleanup(true);
		player = NULL;
		this->addChild(ps);

		auto label = Label::createWithTTF("Game Over!", "fonts/Marker Felt.ttf", 50);
		label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		addChild(label);
		auto blinkAction = Blink::create(1.5f, 6);
		label->runAction(blinkAction);
		this->unschedule(schedule_selector(Breakout::update));
		this->unschedule(schedule_selector(Breakout::updateHP));
		_eventDispatcher->removeAllEventListeners();
	}
}

void Breakout::update(float f) {
    newEnemys();
	dtime = dtime + 1;
	auto str = String::createWithFormat("Time: %d", dtime);
	time->setString(str->getCString());
	if (dtime >= 30) {
		auto label = Label::createWithTTF("You Win!", "fonts/Marker Felt.ttf", 50);
		label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
		this->addChild(label);
		auto rotateBy = RotateBy::create(0.8f, 360);
		label->runAction(rotateBy);
		this->unschedule(schedule_selector(Breakout::update));
		this->unschedule(schedule_selector(Breakout::updateHP));
		_eventDispatcher->removeAllEventListeners();
	}
}

void Breakout::newEnemys() {
	if (enemys.size() > 50) return;
    int newNum = 2;
    while (newNum--) {
        int type = 0;
        if (CCRANDOM_0_1() > 0.85) { type = 2; }
        else if (CCRANDOM_0_1() > 0.6) { type = 1; }

        Point location = Vec2(0, 0);
        switch (rand() % 4)
        {
        case 0:
            location.y = visibleSize.height;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 1:
            location.x = visibleSize.width;
            location.y = rand() % (int)(visibleSize.height);
            break;
        case 2:
            location.y = 0;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 3:
            location.x = 0;
            location.y = rand() % (int)(visibleSize.height);
            break;
        default:
            break;
        }
        addEnemy(type, location);
    }
}

void Breakout::addEnemy(int type, Point p) {
	string mname;
	int mtag;
	if (dtime == 13) {
		mname = "medicine.png";
		mtag = 3;
	} else
	if (dtime == 20) {
		mname = "bomb.png";
		mtag = 4;
	}
	if (dtime == 13 || dtime == 20) {
		auto item = Sprite::create(mname);
		item->setPhysicsBody(PhysicsBody::createCircle(item->getContentSize().height / 2));
		item->setAnchorPoint(Vec2(0.5, 0.5));
		item->getPhysicsBody()->setGroup(-2);
		item->getPhysicsBody()->setCategoryBitmask(0xFFFFFFFF);
		item->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
		item->getPhysicsBody()->setCollisionBitmask(0xFFFFFFFF);
		item->getPhysicsBody()->setTag(mtag);
		item->setPosition(p);
		if (rand() % 100 < 50) {
			item->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.2));
		}
		else {
			item->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.2));
		}
		item->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
		addChild(item);
	}

    char path[100];
    int tag;
    switch (type)
    {
    case 0:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    case 1:
        sprintf(path, "stone2.png");
        tag = 20;
        break;
    case 2:
        sprintf(path, "stone3.png");
        tag = 30;
        break;
    default:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    }
    auto re = Sprite::create(path);
    re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2));
    re->setAnchorPoint(Vec2(0.5, 0.5));
    re->setScale(0.5, 0.5);
    re->setPosition(p);
	re->getPhysicsBody()->setGroup(-2);
	re->getPhysicsBody()->setCategoryBitmask(0xFFFFFFFF);
	re->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	re->getPhysicsBody()->setCollisionBitmask(0xFFFFFFFF);
    re->getPhysicsBody()->setTag(tag);
    if (rand() % 100 < 50) {
        re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
    }
    else {
        re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
    }
    re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
    enemys.pushBack(re->getPhysicsBody());
    addChild(re);
}

void Breakout::allExplode() {
	SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav");
	for (int i = 0; i < enemys.size(); i++) {
		auto sp3 = (Sprite*)enemys.at(i)->getNode();
		if (sp3 == NULL) continue;
		auto ps = ParticleSystemQuad::create("explode.plist");
		ps->setPosition(sp3->getPosition());
		this->addChild(ps);
		sp3->removeFromParentAndCleanup(true);
	}
}

void Breakout::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
    switch (code)
    {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        player->getPhysicsBody()->setVelocity(Point(-200, player->getPhysicsBody()->getVelocity().y));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        player->getPhysicsBody()->setVelocity(Point(200, player->getPhysicsBody()->getVelocity().y));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
        player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, 200));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
        player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, -200));
        break;
    default:
        break;
    }
}

void Breakout::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
    switch (code)
    {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(-200, 0));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(200, 0));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, 200));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, -200));
        break;
    default:
        break;
    }
}
