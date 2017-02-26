#include "MenuSence.h"
#include "GameSence.h"
USING_NS_CC;

Scene* MenuSence::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MenuSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init()
{

    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);

	auto title = Sprite::create("gold-miner-text.png");
	title->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 3 / 4));
	this->addChild(title, 1);
	auto rotateBy = RotateBy::create(0.8f, 360);
	title->runAction(rotateBy);
	
	auto startGold = Sprite::create("menu-start-gold.png");
	startGold->setPosition(Vec2(visibleSize.width * 4 / 5, visibleSize.height / 4));
	this->addChild(startGold, 1);

	auto startButton = MenuItemImage::create("start-0.png", "start-1.png", CC_CALLBACK_1(MenuSence::startMenuCallback, this));
	startButton->setPosition(Vec2(visibleSize.width * 4 / 5, visibleSize.height / 4 + 50));
	auto menu = Menu::create(startButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);

    return true;
}

void MenuSence::startMenuCallback(Ref* pSender) {
	auto scene = GameSence::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(1, scene, Color3B(0, 0, 0)));
}
