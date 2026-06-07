// by cosmella/viper :)

#include "DialogAPIIncludes.hpp"
#include "DialogAPI.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

CCSprite *IsSprite(std::string def) {
	const char *str = def.c_str();
	CCSprite *sprite = CCSprite::create(str);
	if (sprite) {
		return sprite;
	}
	sprite = CCSprite::createWithSpriteFrameName(str);
	if (sprite) {
		return sprite;
	}

	return nullptr;
}

VP_DialogObject *VP_DialogObject::create(std::string const &character, std::string const &text, int characterFrame, float textScale, bool skippable, cocos2d::ccColor3B color) {
	auto ret = new VP_DialogObject();
	if (ret->init(character, text, characterFrame, textScale, skippable, color)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

VP_DialogObject *VP_DialogObject::create(std::string const &character, std::string const &text, CCNode *characterFrame, float textScale, bool skippable, cocos2d::ccColor3B color) {
	auto ret = new VP_DialogObject();
	if (ret->init(character, text, 0, textScale, skippable, color)) {
		ret->autorelease();
		if (characterFrame)
			ret->CustomChar = characterFrame;
		return ret;
	}
	delete ret;
	return nullptr;
}

VP_DialogObject *VP_DialogObject::create(matjson::Value data) {
	std::string title = "Unknown";
	if (data.contains("title") && data["title"].isString())
		title = data["title"].asString().unwrapOr("Unknown");
	std::string message = "N/A";
	if (data.contains("message") && data["message"].isString())
		message = data["message"].asString().unwrapOr("N/A");

	// from https://github.com/FigmentBoy/DialogHandler/blob/master/DialogBox/Dialog.cpp?plain=1#L31
	int index;
	int pos = 0;
	std::string plrname;

	while ((index = message.find("{user}", pos)) != std::string::npos) {
		if (plrname.empty()) {
			plrname = getPlayerName();
		}
		message.replace(index, 6, plrname);

		pos = index + 1;
	}

	while ((index = title.find("{user}", pos)) != std::string::npos) {
		if (plrname.empty()) {
			plrname = getPlayerName();
		}
		title.replace(index, 6, plrname);

		pos = index + 1;
	}
	// end of copying

	if (data.contains("portrait")) {
		if (data["portrait"].isNumber()) {
			return VP_DialogObject::create(title, message, data["portrait"].asUInt().unwrapOr(1), 1.0f, false, ccWHITE);
		} else if (data["portrait"].isString()) {
			if (auto spr = IsSprite(data["portrait"].asString().unwrapOr(""))) {
				return VP_DialogObject::create(title, message, spr, 1.0f, false, ccWHITE);
			} else {
				return VP_DialogObject::create(title, message, 1, 1.0f, false, ccWHITE);
			}
		} else {
			return VP_DialogObject::create(title, message, 1, 1.0f, false, ccWHITE);
		}
	} else {
		return VP_DialogObject::create(title, message, 1, 1.0f, false, ccWHITE);
	}
	return VP_DialogObject::create("uh oh", "woops", 1, 1.0f, false, ccWHITE);
}

void VP_DialogObject::AddCallback(std::function<void()> callback) {
	this->callbackOnObject = callback;
};

void VP_DialogLayer::addCallbackCustom(std::function<void()> m_callback) {
	m_DelegateCallBack *newdelgate = new m_DelegateCallBack;
	this->addChild(newdelgate);
	newdelgate->autorelease();
	newdelgate->m_callback = m_callback;
	this->m_delegate = newdelgate;
}

void VP_DialogLayer::setBackground(std::string backgroundBG) {
    auto fields = m_fields.self();
    
	fields->m_BackgroundOverride = backgroundBG;
	if (!this->m_mainLayer)
		return;

	if (!fields->m_BackgroundSpr) {
		fields->m_BackgroundSpr = m_mainLayer->getChildByType<CCScale9Sprite *>(0);
	}
	if (fields->m_BackgroundSpr) {
		cocos2d::CCSprite *sprite = IsSprite(backgroundBG);
		if (sprite) {
			cocos2d::CCSize ContentSize = fields->m_BackgroundSpr->getContentSize();
			cocos2d::CCSpriteFrame *spriteFrame = sprite->displayFrame();
			fields->m_BackgroundSpr->setSpriteFrame(spriteFrame);
			fields->m_BackgroundSpr->setContentSize(ContentSize);
			fields->m_BackgroundAlreadySet = backgroundBG;
		}
	}
}

void VP_DialogLayer::setBackground(int backgroundBG) {
	std::string num = "";

	if (backgroundBG < 10) {
		num = fmt::format("0{}", backgroundBG);
	} else {
		num = backgroundBG;
	};

	VP_DialogLayer::setBackground(fmt::format("GJ_square{}.png", num));
};

void m_DelegateCallBack::dialogClosed(DialogLayer *p0) {
	if (this->m_callback) {
		this->m_callback();
	}
};
DialogLayer *VP_DialogLayer::create(matjson::Value data) {
	std::pair<CCArray *, int> letsgo;
	letsgo.second = 2;
	letsgo.first = CCArray::create();
	if (!data.isArray() && !data.isObject()) {
		letsgo.first->addObject(DialogObject::create("uh oh", "it seems like the json provided is invalid", 2, 1.0f, false, ccWHITE));
		return VP_DialogLayer::createWithObjects(letsgo.first, letsgo.second);
	}
	if (data.contains("lines") && data["lines"].isArray()) {
		for (const auto lols : data["lines"].asArray().unwrap()) {
			letsgo.first->addObject(VP_DialogObject::create(lols));
		}
	}
	if (letsgo.first->count() < 1)
		letsgo.first->addObject(DialogObject::create("uh oh", "it seems lines is empty", 2, 1.0f, false, ccWHITE));

	if (data.contains("color")) {
		if (data["color"].isNumber()) {
			letsgo.second = data["color"].asInt().unwrapOr(2);
		} else if (data["color"].isString()) {
			DialogLayer *x = VP_DialogLayer::createWithObjects(letsgo.first, letsgo.second);
			reinterpret_cast<VP_DialogLayer *>(x)->setBackground(data["color"].asString().unwrapOr("0"));
			return x;
		}
	}
	return VP_DialogLayer::createWithObjects(letsgo.first, letsgo.second);
}


VP_DialogObject *DialogApi::create(std::string const &character, std::string const &text, int characterFrame, float textScale, bool skippable, cocos2d::ccColor3B color, std::function<void()> callback ) {
	VP_DialogObject *c = VP_DialogObject::create(character, text, characterFrame, textScale, skippable, color);
	if (callback) {
		c->AddCallback(callback);
	}
	return c;
};
VP_DialogObject *DialogApi::create(std::string const &character, std::string const &text , CCNode *characterFrame , float textScale, bool skippable, cocos2d::ccColor3B color, std::function<void()> callback) {
	VP_DialogObject *c = VP_DialogObject::create(text, character, characterFrame, textScale, skippable, color);
	if (callback) {
		c->AddCallback(callback);
	}
	return c;
};
VP_DialogObject *DialogApi::create(matjson::Value data, std::function<void()> callback) {
	VP_DialogObject *c = VP_DialogObject::create(data);
	if (callback) {
		c->AddCallback(callback);
	}
    return c;
};

VP_DialogLayer *DialogApi::create(DialogObject *object, int background, std::function<void()> callback) {
	#ifdef GEODE_IS_MACOS
		CCArray* macosFix = CCArray::create();
		macosFix->addObject(object);
		VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(DialogLayer::createWithObjects(macosFix, background));
	#else
		VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(DialogLayer::create(object, background));
	#endif
	if (callback) {
		c->addCallbackCustom(callback);
	}
	return c;
};

VP_DialogLayer *DialogApi::create(DialogObject *object, std::string background, std::function<void()> callback) {
	#ifdef GEODE_IS_MACOS
		CCArray* macosFix = CCArray::create();
		macosFix->addObject(object);
		VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(DialogLayer::createWithObjects(macosFix, 2));
	#else
		VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(DialogLayer::create(object, 2));
	#endif
	if (!background.empty()) {
		c->setBackground(background);
	}
	if (callback) {
		c->addCallbackCustom(callback);
	}
	return c;
};
VP_DialogLayer *DialogApi::createWithJson(matjson::Value json) {
	VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(VP_DialogLayer::create(json));
	return c;
};

VP_DialogLayer *DialogApi::createWithJson(matjson::Value json, std::function<void()> m_callback) {
	VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(VP_DialogLayer::create(json));
	c->addCallbackCustom(m_callback);
	return c;
};

VP_DialogLayer *DialogApi::create(CCArray *objects, int background, std::function<void()> callback) {
	VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(VP_DialogLayer::createWithObjects(objects, background));
	if (callback) {
		c->addCallbackCustom(callback);
	}
	return c;
};

VP_DialogLayer *DialogApi::create(CCArray *objects, std::string background, std::function<void()> callback) {
	VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(VP_DialogLayer::createWithObjects(objects, 2));
	if (callback) {
		c->addCallbackCustom(callback);
	}
	if (!background.empty()) {
		c->setBackground(background);
	};
	return c;
};

VP_DialogLayer *DialogApi::createDialogLayer(DialogObject *object, CCArray *objects, std::string background, std::function<void()> callback) {
	VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(VP_DialogLayer::createDialogLayer(object, objects, 2));
	if (callback) {
		c->addCallbackCustom(callback);
	}
	if (!background.empty()) {
		c->setBackground(background);
	};
	return c;
};
VP_DialogLayer *DialogApi::createDialogLayer(DialogObject *object, CCArray *objects, int background, std::function<void()> callback) {
	VP_DialogLayer *c = reinterpret_cast<VP_DialogLayer *>(VP_DialogLayer::createDialogLayer(object, objects, background));
	if (callback) {
		c->addCallbackCustom(callback);
	}
	return c;
};
