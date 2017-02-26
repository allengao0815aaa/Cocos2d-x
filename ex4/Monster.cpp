#include"Monster.h"
USING_NS_CC;

Factory* Factory::factory = NULL;

Factory::Factory() {
	initSpriteFrame();
	playerRunning = false;
}

Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}

void Factory::initSpriteFrame(){
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258-48*i,0,42,42)));
		monsterDead.pushBack(frame);
	}
}

//�½�����
Sprite* Factory::createMonster() {
	Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364,0,42,42)));
	monster.pushBack(mons);
	return mons;
}

//�Ƴ�����
void Factory::removeMonster(Sprite* sp) {
	Animation* anim = Animation::createWithSpriteFrames(monsterDead, 0.1f);
	Animate* ani = Animate::create(anim);
	Sequence* seq = Sequence::create(ani, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), NULL);
	sp->runAction(seq);
	monster.eraseObject(sp);
}

//�ƶ�����
void Factory::moveMonster(Vec2 playerPos, float time){
	cocos2d:Vector<Sprite*>::iterator it;
	for (it = monster.begin(); it != monster.end(); it++) {
		Vec2 mosterPos = (*it)->getPosition();
		Vec2 direction = playerPos - mosterPos;
		direction.normalize();
		(*it)->runAction(MoveBy::create(time, direction*30));
	}
}

//��ײ���
Sprite* Factory::collider(Rect rect) {
	cocos2d:Vector<Sprite*>::iterator it;
	for (it = monster.begin(); it != monster.end(); it++) {
		if (rect.containsPoint((*it)->getPosition())) return *it;
	}
	return NULL;
}

//����������������ʵ����������ִ��һ������ʱ���޷�ִ����������
void Factory::setRunning() {
	playerRunning = true;
}

bool Factory::checkRunning() {
	return playerRunning;
}

void Factory::endRunning() {
	playerRunning = false;
}
