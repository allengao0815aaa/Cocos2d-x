#include "LoginScene.h"
#include "cocostudio/CocoStudio.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "Global.h"
#include "GameScene.h"
#include <regex>
#define database UserDefault::getInstance()
using std::to_string;
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;
USING_NS_CC;

using namespace cocostudio::timeline;

#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace  rapidjson;

Scene* LoginScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LoginScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoginScene::init()
{
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    textField = TextField::create("Player Name", "Arial", 30);
    textField->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
    this->addChild(textField, 2);

	auto button = Button::create();
	button->setTitleText("Login");
	button->setTitleFontSize(30);
	button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));
	button->addTouchEventListener(CC_CALLBACK_2(LoginScene::Login, this, textField->getString().c_str()));
	this->addChild(button, 2);
    
	if (database->getStringForKey("SessionId") == "") {
		database->setStringForKey("SessionId", Global::gameSessionId);
	}
	Global::gameSessionId = database->getStringForKey("SessionId");

	auto auto_button = Button::create();
	auto_button->setTitleText("Auto Login");
	auto_button->setTitleFontSize(30);
	auto_button->setPosition(Size(visibleWidth / 2, visibleHeight / 4));
	auto_button->addTouchEventListener(CC_CALLBACK_2(LoginScene::AutoLogin, this));
	this->addChild(auto_button, 2);

	return true;
}

void LoginScene::Login(Ref* sender, Widget::TouchEventType type, const char* username) {
	if (type == Widget::TouchEventType::ENDED) {
		HttpRequest* request = new HttpRequest();
		request->setUrl("http://localhost:8080/login");
		request->setRequestType(HttpRequest::Type::POST);
		request->setResponseCallback(CC_CALLBACK_2(LoginScene::onHttpRequestCompleted, this));
		char postData[100];
		strcpy(postData, "username=");
		strcat(postData, username);
		request->setRequestData(postData, strlen(postData));
		request->setTag("Login");
		cocos2d::network::HttpClient::getInstance()->send(request);
		request->release();
	}
}

void LoginScene::onHttpRequestCompleted(HttpClient* sender, HttpResponse* response) {
	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
		log("response failed");
		return;
	}
	std::vector<char> *buffer1 = response->getResponseHeader();
	string str1 = Global::toString(buffer1);
	std::vector<char> *buffer2 = response->getResponseData();
	string str2 = Global::toString(buffer2);

	rapidjson::Document d;
	d.Parse<0>(str2.c_str());
	if (d.IsObject() && d["result"].GetBool()) {
		Global::gameSessionId = Global::getSessionIdFromHeader(str1);
		database->setStringForKey("SessionId", Global::gameSessionId);
		auto scene = GameScene::createScene();
		Director::getInstance()->replaceScene(scene);
	}
}

void LoginScene::AutoLogin(Ref* sender, Widget::TouchEventType type) {
	if (type == Widget::TouchEventType::ENDED) {
		auto scene = GameScene::createScene();
		Director::getInstance()->replaceScene(scene);
	}
}
