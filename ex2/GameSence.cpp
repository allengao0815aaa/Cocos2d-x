#include "GameSence.h"
#include <time.h>
#include <stdlib.h>

USING_NS_CC;

Scene* GameSence::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameSence::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameSence::init()
{

	if (!Layer::init())
	{
		return false;
	}

	srand((unsigned)time(NULL));

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	stoneLayer = Layer::create();
	stoneLayer->ignoreAnchorPointForPosition(true);
	stoneLayer->setPosition(Vec2(0, 0));

	stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(560, 480));
	stoneLayer->addChild(stone);
	this->addChild(stoneLayer, 4);

	mouseLayer = Layer::create();
	mouseLayer->ignoreAnchorPointForPosition(true);
	mouseLayer->setPosition(Vec2(0, visibleSize.height / 2));

	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));
	mouse->setPosition(visibleSize.width / 2, 0);
	mouseLayer->addChild(mouse);
	this->addChild(mouseLayer, 2);

	auto label = MenuItemLabel::create(Label::createWithTTF("Shoot", "fonts/Marker Felt.ttf", 60), CC_CALLBACK_1(GameSence::shootMenuCallback, this));
	label->ignoreAnchorPointForPosition(true);
	label->setPosition(Vec2(700, 485));
	auto menu = Menu::create(label, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);

	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto location = touch->getLocation();
	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	this->addChild(cheese, 1);
	
	auto nodeSpace = mouseLayer->convertToNodeSpace(location);
	auto moveTo = MoveTo::create(1, nodeSpace);
	auto fadeOut = FadeOut::create(1.0f);
	auto target = TargetedAction::create(cheese, fadeOut);
	auto seq = Sequence::create(moveTo, target, nullptr);
	mouse->runAction(seq);

	auto shootButtion = MenuItem::create();
	return true;
}

void GameSence::shootMenuCallback(Ref* pSender) {

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto newStone = Sprite::create("stone.png");
	newStone->setPosition(Vec2(560, 480));
	stoneLayer->addChild(newStone, 4);
	
	auto diamond = Sprite::create("diamond.png");
	diamond->setPosition(mouseLayer->convertToWorldSpace(mouse->getPosition()));
	this->addChild(diamond, 3);

	auto blinkAction = Blink::create(1.5f, 10);
	diamond->runAction(blinkAction);
	
	auto worldSpace2 = Vec2(rand()%((int)visibleSize.width-30)+15, rand()%((int)visibleSize.height-30)+15);
	auto worldSpace1 = mouseLayer->convertToWorldSpace(mouse->getPosition());
	auto nodeSpace1 = stoneLayer->convertToNodeSpace(worldSpace1);
	auto nodeSpace2 = mouseLayer->convertToNodeSpace(worldSpace2);

	auto moveTo1 = MoveTo::create(1.0f, nodeSpace1);
	auto moveTo2 = MoveTo::create(1.0f, nodeSpace2);
	auto fadeOut = FadeOut::create(0.3f);
	auto seq = Sequence::create(moveTo1, fadeOut, nullptr);
	auto target = TargetedAction::create(mouse, moveTo2);
	auto mySpawn = Spawn::createWithTwoActions(seq, target);
	newStone->runAction(mySpawn);
}
