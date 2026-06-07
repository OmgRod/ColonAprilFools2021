#pragma once
#include "DialogAPIIncludes.hpp"

class DialogApi {
  public:
	static VP_DialogObject *create(std::string const &character = "", std::string const &text = "", int characterFrame = 1, float textScale = 1.0f, bool skippable = false, cocos2d::ccColor3B color = cocos2d::ccWHITE, std::function<void()> callback = nullptr);
	static VP_DialogObject *create(std::string const &character = "", std::string const &text = "", cocos2d::CCNode* characterFrame = nullptr, float textScale = 1.0f, bool skippable = false, cocos2d::ccColor3B color = cocos2d::ccWHITE, std::function<void()> callback = nullptr);
    static VP_DialogObject *create(matjson::Value data = nullptr, std::function<void()> callback = nullptr);
    static VP_DialogLayer *create(DialogObject *object = nullptr, int background = 1, std::function<void()> callback = nullptr);
	static VP_DialogLayer *create(DialogObject *object = nullptr, std::string background = "", std::function<void()> callback = nullptr);
    static VP_DialogLayer* createWithJson(matjson::Value json);
	static VP_DialogLayer* createWithJson(matjson::Value json , std::function<void()> m_callback);
	static VP_DialogLayer* create(cocos2d::CCArray *objects = nullptr, int background = 1, std::function<void()> callback = nullptr);

	static VP_DialogLayer *create(cocos2d::CCArray *objects = nullptr, std::string background = "", std::function<void()> callback = nullptr);

	static VP_DialogLayer *createDialogLayer(DialogObject *object = nullptr, cocos2d::CCArray *objects = nullptr, std::string background = "",std::function<void()> callback = nullptr);

	static VP_DialogLayer *createDialogLayer(DialogObject *object = nullptr, cocos2d::CCArray *objects = nullptr, int background = 2,std::function<void()> callback = nullptr);
};