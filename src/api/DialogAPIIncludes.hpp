#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/DialogLayer.hpp>
#include <Geode/modify/DialogObject.hpp>

using namespace geode::prelude;

inline std::string getPlayerName() {
	// fix for coopeeo custom name
	if (Mod *coopeeo_customname = Loader::get()->getLoadedMod("coopeeo.customname")) {
		return coopeeo_customname->getSettingValue<std::string>("thename");
	}
	return GameManager::get()->m_playerName;
}

class VP_DialogObject : public DialogObject {
  public:
	static VP_DialogObject *create(std::string const &character, std::string const &text, int characterFrame, float textScale, bool skippable, cocos2d::ccColor3B color);
	static VP_DialogObject *create(std::string const &character, std::string const &text, cocos2d::CCNode *characterFrame, float textScale, bool skippable, cocos2d::ccColor3B color);
	static VP_DialogObject *create(matjson::Value data);

	void AddCallback(std::function<void()> callback);

	geode::prelude::Ref<cocos2d::CCNode> CustomChar;
	std::function<void()> callbackOnObject;
};

class m_DelegateCallBack : public DialogDelegate, public cocos2d::CCNode {
  public:
	std::function<void()> m_callback;
	virtual void dialogClosed(DialogLayer *p0);
};

class $modify(VP_DialogLayer, DialogLayer) {
	struct Fields {
		CCNode *m_cur_Customprofile;
		geode::prelude::CCScale9Sprite *m_BackgroundSpr;
		std::string m_BackgroundOverride;
		CCNode *m_pfpPosFixer;
		std::string m_BackgroundAlreadySet;
		CCNode* m_pendingPortrait = nullptr;
	};
    static DialogLayer* create(matjson::Value data);
	void addCallbackCustom(std::function<void()> m_callback);
	void setBackground(std::string backgroundBG);
	void setBackground(int backgroundBG);

	bool init(DialogObject *p0, cocos2d::CCArray *p1, int p2) {
        if (!DialogLayer::init(p0, p1, p2))
		    return false;

        VP_DialogLayer* Convert = reinterpret_cast<VP_DialogLayer*>(this);
        Convert->m_fields->m_pfpPosFixer = CCNode::create();
        Convert->m_fields->m_pfpPosFixer->setPositionX(-143);
        m_mainLayer->addChild(Convert->m_fields->m_pfpPosFixer);

        return true;
    }
	void displayDialogObject(DialogObject* p0) {
        if (!p0) {
            DialogLayer::displayDialogObject(p0);
            return;
        }

        VP_DialogLayer* Convert = reinterpret_cast<VP_DialogLayer*>(this);
        if (!Convert) {
            DialogLayer::displayDialogObject(p0);
            return;
        }

        DialogLayer::displayDialogObject(p0);

        auto& fields = Convert->m_fields;

        if (!fields->m_BackgroundOverride.empty() &&
            fields->m_BackgroundAlreadySet != fields->m_BackgroundOverride) {
            Convert->setBackground(fields->m_BackgroundOverride);
        }

        if (fields->m_cur_Customprofile) {
            fields->m_cur_Customprofile->removeFromParent();
            fields->m_cur_Customprofile = nullptr;
        }

        auto* vpObj = typeinfo_cast<VP_DialogObject*>(p0);
        if (!vpObj) return;

        if (vpObj->CustomChar) {
            fields->m_cur_Customprofile = vpObj->CustomChar;

            fields->m_pendingPortrait = vpObj->CustomChar;
            fields->m_pendingPortrait->retain();

            this->scheduleOnce(
                schedule_selector(VP_DialogLayer::attachPendingPortrait),
                0.0f
            );
        }

        if (vpObj->callbackOnObject) {
            Convert->addCallbackCustom(vpObj->callbackOnObject);
        }
    }

    void attachPendingPortrait(float) {
        auto fields = m_fields.self();

        if (!fields->m_pendingPortrait)
            return;

        if (!fields->m_pfpPosFixer)
            return;

        if (fields->m_cur_Customprofile &&
            fields->m_cur_Customprofile != fields->m_pendingPortrait) {
            fields->m_cur_Customprofile->removeFromParent();
        }

        fields->m_pfpPosFixer->addChild(fields->m_pendingPortrait);

        fields->m_pendingPortrait->release();
        fields->m_pendingPortrait = nullptr;
    }
};
