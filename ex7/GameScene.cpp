#include "GameScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <regex>
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameScene::init() {
    if (!Layer::init())
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    score_field = TextField::create("Score", "Arial", 30);
    score_field->setPosition(Size(visibleWidth / 4, visibleHeight / 4 * 3));
    this->addChild(score_field, 2);

	submit_button = Button::create();
	submit_button->setTitleText("Submit");
	submit_button->setTitleFontSize(30);
	submit_button->setPosition(Size(visibleWidth / 4, visibleHeight / 4));
	submit_button->addTouchEventListener(CC_CALLBACK_2(GameScene::Submit, this, score_field->getString().c_str()));
	this->addChild(submit_button, 2);

    rank_field = TextField::create("", "Arial", 30);
    rank_field->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4 * 3));
    this->addChild(rank_field, 2);

	rank_button = Button::create();
	rank_button->setTitleText("Rank");
	rank_button->setTitleFontSize(30);
	rank_button->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4));
	rank_button->addTouchEventListener(CC_CALLBACK_2(GameScene::Rank, this));
	this->addChild(rank_button, 2);

    return true;
}

void GameScene::Submit(Ref* sender, Widget::TouchEventType type, const char* score) {
	if (type == Widget::TouchEventType::ENDED) {
		HttpRequest* request = new HttpRequest();
		request->setUrl("http://localhost:8080/submit");
		request->setRequestType(HttpRequest::Type::POST);
		request->setResponseCallback(CC_CALLBACK_2(GameScene::onHttpRequestCompleted1, this));

		char postData[100];
		strcpy(postData, "score=");
		strcat(postData, score);
		request->setRequestData(postData, strlen(postData));
		request->setTag("Score");

		vector<string> headers;
		headers.push_back("Cookies: GAMESESSIONID=" + Global::gameSessionId);
		request->setHeaders(headers);

		//cocos2d::network::HttpClient::getInstance()->enableCookies(NULL);
		cocos2d::network::HttpClient::getInstance()->send(request);
		request->release();
	}
}

void GameScene::onHttpRequestCompleted1(HttpClient* sender, HttpResponse* response) {
	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
		log("response failed");
		return;
	}
	std::vector<char> *buffer = response->getResponseData();
	string str = Global::toString(buffer);

	rapidjson::Document d;
	d.Parse<0>(str.c_str());
	if (d.IsObject() && d["result"].GetBool()) {
		score_field->setString(d["info"].GetString());
	}
}

void GameScene::Rank(Ref* sender, Widget::TouchEventType type) {
	if (type == Widget::TouchEventType::ENDED) {
		HttpRequest* request = new HttpRequest();
		request->setUrl("http://localhost:8080/rank?top=10");
		request->setRequestType(HttpRequest::Type::GET);
		request->setResponseCallback(CC_CALLBACK_2(GameScene::onHttpRequestCompleted2, this));
		request->setTag("Rank");

		vector<string> headers;
		headers.push_back("Cookies: GAMESESSIONID=" + Global::gameSessionId);
		request->setHeaders(headers);

		//cocos2d::network::HttpClient::getInstance()->enableCookies(NULL);
		cocos2d::network::HttpClient::getInstance()->send(request);
		request->release();
	}
}

void GameScene::onHttpRequestCompleted2(HttpClient* sender, HttpResponse* response) {
	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
		log("response failed");
		return;
	}
	std::vector<char> *buffer = response->getResponseData();
	string str = Global::toString(buffer);

	rapidjson::Document d;
	d.Parse<0>(str.c_str());
	if (d.IsObject() && d["result"].GetBool()) {
		string board = d["info"].GetString();
		if (board != "") board.erase(0, 1);
		int len = board.size();
		for (int i = 0; i < len; i++) {
			if (board[i] == '|') board[i] = '\n';
		}
		rank_field->setString(board);
	}
}
