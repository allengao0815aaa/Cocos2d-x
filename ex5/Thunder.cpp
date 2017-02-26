#include "Thunder.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <algorithm>
using namespace std;

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Thunder::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool Thunder::init() {
    if ( !Layer::init() ) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();

    auto bgsprite = Sprite::create("background.jpg");
    bgsprite->setPosition(visibleSize / 2);
    //bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
    //     visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, 0);

    player = Sprite::create("player.png");
    player->setPosition(visibleSize.width / 2, player->getContentSize().height + 5);
    player->setName("player");
    this->addChild(player, 1);

    addEnemy(5);

    preloadMusic();
    playBgm();

    addTouchListener();
    addKeyboardListener();
    addCustomListener();

	win = false;
	schedule(schedule_selector(Thunder::update), 0.01f, kRepeatForever, 0);
    
    return true;
}

void Thunder::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/fire.wav");
	audio->preloadEffect("music/explore.wav");
}

void Thunder::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

void Thunder::addEnemy(int n) {
    enemys.resize(n * 3);
    for(int i = 0; i < 3; ++i) {
        char enemyPath[20];
        sprintf(enemyPath, "stone%d.png", 3 - i);
        double width  = (visibleSize.width - 20) / (n + 1.0),
               height = visibleSize.height - (50 * (i + 1));
        for(int j = 0; j < n; ++j) {
            auto enemy = Sprite::create(enemyPath);
            enemy->setAnchorPoint(Vec2(0.5, 0.5));
            enemy->setScale(0.5, 0.5);
            enemy->setPosition(width * (j + 1), height);
            enemys[i * n + j] = enemy;
			addChild(enemy);
        }
    }
}

void Thunder::addTouchListener(){
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener->clone(), player);
}

void Thunder::addKeyboardListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener->clone(), player);
}

void Thunder::update(float f) {
	double newplayerX = player->getPositionX() + move, newplayerY = player->getPositionY();
	auto playerSize = player->getContentSize();
	newplayerX = max(newplayerX, playerSize.width / 2.0);
	newplayerX = min(newplayerX, visibleSize.width - playerSize.width / 2.0);
	newplayerY = max(newplayerY, playerSize.height / 2.0);
	newplayerY = min(newplayerY, visibleSize.height - playerSize.height / 2.0);
	player->setPosition(newplayerX, newplayerY);

	bool overflag = false;
	static double count = 0;
	static int dir = 1;
	count += f;
	if (count > 1) {
		count = 0.0; dir = -dir;
		if (dir > 0) {
			for (unsigned i = 0; i < enemys.size(); i++) {
				if (enemys[i] != NULL) {
					enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(0, -20));
					if (enemys[i]->getPositionY() < enemys[i]->getContentSize().height / 2.0)
						overflag = true;
				}
			}
			if (overflag) {
				GameOver();
				return;
			}
		}
	}

	for (unsigned i = 0; i < bullet.size(); i++) {
		if (bullet[i] != NULL) {
			bullet[i]->setPosition(bullet[i]->getPositionX(), bullet[i]->getPositionY() + 5);
			if (bullet[i]->getPositionY() > visibleSize.height - 10) {
				bullet[i]->removeFromParentAndCleanup(true);
				bullet[i] = NULL;
			}
		}
	}

	bool allKilled = true;
	for (unsigned i = 0; i < enemys.size(); i++) {
		if (enemys[i] != NULL) {
			allKilled = false;
			enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(dir, 0));

			if (player->getPosition().getDistance(enemys[i]->getPosition()) < 30) {
				auto fadeOut = FadeOut::create(0.5f);
				player->runAction(fadeOut);
				GameOver();
				return;
			}

			for (unsigned j = 0; j < bullet.size(); j++) {
				if (bullet[j] != NULL && bullet[j]->getPosition().getDistance(enemys[i]->getPosition()) < 30) {
					EventCustom e("meet");
					e.setUserData(&make_pair(i, j));
					_eventDispatcher->dispatchEvent(&e);
					break;
				}
			}
		}
    }
	if (allKilled) {
		if (!win) {
			auto label = Label::createWithTTF("You Win!", "fonts/Marker Felt.ttf", 50);
			label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
			addChild(label);
			win = true;
		}
	}
}

void Thunder::fire() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("music/fire.wav");
    auto newbullet = Sprite::create("bullet.png");
	newbullet->setPosition(player->getPosition());
	bullet.push_back(newbullet);
	addChild(newbullet);
}

void Thunder::addCustomListener() {
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

bool Thunder::onTouchBegan(Touch *touch, Event *unused_event) {
	Vec2 position = touch->getLocation();
	if (position.getDistance(player->getPosition()) < 20) {
		player->setPositionX(position.x);
		return true;
	} else {
		fire();
	}
	return false;
}

void Thunder::onTouchMoved(Touch *touch, Event *unused_event) {
	Vec2 position = touch->getLocation();
	player->setPositionX(position.x);
}

void Thunder::onTouchEnded(Touch *touch, Event *unused_event) {
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
    switch (code) {
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
			player->setPosition(player->getPositionX() - 1, player->getPositionY());
			move -= 5;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
			player->setPosition(player->getPositionX() + 1, player->getPositionY());
			move += 5;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
            fire();
            break;
        default:
            break;
    }
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_A:
			player->setPosition(player->getPositionX() - 1, player->getPositionY());
			move += 5;
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_D:
			player->setPosition(player->getPositionX() + 1, player->getPositionY());
			move -= 5;
			break;
		default:
			break;
	}
}

void Thunder::meet(EventCustom* event) {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("music/explore.wav");
	pair<int, int>* now = (pair<int, int>*)(event->getUserData());
	int i = now->first, j = now->second;
	enemys[i]->removeFromParentAndCleanup(true);
	enemys[i] = NULL;
	bullet[j]->removeFromParentAndCleanup(true);
	bullet[j] = NULL;
}

void Thunder::GameOver() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("music/explore.wav");
	auto label = Label::createWithTTF("Game Over!", "fonts/Marker Felt.ttf", 50);
	label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(label);
	auto blinkAction = Blink::create(1.5f, 6);
	label->runAction(blinkAction);
	unschedule(schedule_selector(Thunder::update));
	_eventDispatcher->removeAllEventListeners();
}