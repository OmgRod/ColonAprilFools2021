#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include "../layers/CodePopup.h"
#include "../layers/ElderLayer.h"

using namespace geode::prelude;

namespace {
    struct UnlockTxtLayout {
        static const CCSize spriteAreaPx;
        static const CCPoint buttonPosPx;
    };

    const CCSize UnlockTxtLayout::spriteAreaPx = { 423.0f, 72.0f };
    const CCPoint UnlockTxtLayout::buttonPosPx = { 117.0f, 41.0f };
}

class $modify(MyGJGarageLayer, GJGarageLayer) {
public:
    struct Fields {
        CCPoint localPos;
        CCMenu* menu;
    };

    bool init() {
		if (!GJGarageLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto fields = m_fields.self();

        auto mod = Mod::get();
        
        if (mod->getSavedValue<bool>("scratch-dialog")) {
            if (auto unlockTxt = this->getChildByID("tap-more-hint")) {
                auto newUnlockTxt = CCSprite::create("unlockTxtUpdated.png"_spr);
                newUnlockTxt->setPosition(unlockTxt->getPosition());
                newUnlockTxt->setID("tap-more-hint"_spr);
                
                unlockTxt->setVisible(false);

                this->addChild(newUnlockTxt);

                auto spr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
                spr->setScale(0.6f);
                CCMenuItemSpriteExtra* btn = nullptr;
                if (mod->getSavedValue<bool>("lair-unlocked")) {
                    btn = CCMenuItemSpriteExtra::create(
                        spr,
                        this,
                        menu_selector(MyGJGarageLayer::onScene)
                    );
                } else if (mod->getSavedValue<bool>("shopkeeper-dialog-1")) {
                    btn = CCMenuItemSpriteExtra::create(
                        spr,
                        this,
                        menu_selector(MyGJGarageLayer::onLayer)
                    );
                } else {
                    spr = CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
                    spr->setScale(0.6f);
                    btn = CCMenuItemSpriteExtra::create(
                        spr,
                        this,
                        menu_selector(MyGJGarageLayer::onUnlock)
                    );
                }
                

                const auto contentSize = newUnlockTxt->getContentSize();
                const auto sprPosition = newUnlockTxt->getPosition();

                fields->localPos = ccp(
                    sprPosition.x - (contentSize.width / 2) + (contentSize.width * (UnlockTxtLayout::buttonPosPx.x / UnlockTxtLayout::spriteAreaPx.width)),
                    sprPosition.y - (contentSize.height / 2) + (contentSize.height * (UnlockTxtLayout::buttonPosPx.y / UnlockTxtLayout::spriteAreaPx.height))
                );

                btn->setPosition(fields->localPos);

                fields->menu = CCMenu::create();
                fields->menu->setPosition({ 0.f, 0.f });
                fields->menu->addChild(btn);

                this->addChild(fields->menu);
            }
        }

		return true;
    }

    void onUnlock(CCObject* sender) {
        auto fields = m_fields.self();

        if (auto old = static_cast<CCMenuItemSpriteExtra*>(sender)) {
            old->setEnabled(false);
            old->setVisible(false);
            
            auto spr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
            spr->setScale(0.6f);
            auto btn = CCMenuItemSpriteExtra::create(
                spr,
                this,
                menu_selector(MyGJGarageLayer::onLayer)
            );
            btn->setPosition(fields->localPos);
            fields->menu->addChild(btn);

            auto gm = GameManager::sharedState();

            auto dialog = CCArray::create();
            dialog->addObject(DialogObject::create(
                "The Shopkeeper",
                fmt::format("<cl>{}</c>? What are you doing with the key to <cp>RubRub's lair</c>?", getPlayerName()).c_str(),
                5,
                1.f,
                false,
                ccWHITE
            ));
            dialog->addObject(DialogObject::create(
                "The Shopkeeper",
                "Only those chosen by <cl>RubRub himself</c> are allowed past this point.",
                5,
                1.f,
                false,
                ccWHITE
            ));
            dialog->addObject(DialogObject::create(
                "The Shopkeeper",
                "I'd love to let you in for supporting my business...",
                6,
                1.f,
                false,
                ccWHITE
            ));
            dialog->addObject(DialogObject::create(
                "The Shopkeeper",
                "... But I'm afraid I cannot let you pass.",
                6,
                1.f,
                false,
                ccWHITE
            ));
            dialog->addObject(DialogObject::create(
                "The Shopkeeper",
                "<cg>RubRub demands an offering from all who wish to set foot into his lair.</c>",
                5,
                1.f,
                false,
                ccWHITE
            ));
            dialog->addObject(DialogObject::create(
                "The Shopkeeper",
                "<cg>Return once you have something of value to provide.</c>",
                5,
                1.f,
                false,
                ccWHITE
            ));

            auto dialogLayer = DialogLayer::createWithObjects(dialog, 1);
            dialogLayer->addToMainScene();
            dialogLayer->animateIn(DialogAnimationType::FromTop);

            Mod::get()->setSavedValue("shopkeeper-dialog-1", true);
        }
    }

    void onLayer(CCObject* sender) {
        auto popup = CodePopup::create();

        popup->m_successCallback = [this, sender]() {
            auto fields = m_fields.self();

            if (auto old = static_cast<CCMenuItemSpriteExtra*>(sender)) {
                old->setEnabled(false);
                old->setVisible(false);

                auto spr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
                spr->setScale(0.6f);

                auto btn = CCMenuItemSpriteExtra::create(
                    spr,
                    this,
                    menu_selector(MyGJGarageLayer::onScene)
                );

                btn->setPosition(fields->localPos);
                fields->menu->addChild(btn);
            }

            Mod::get()->setSavedValue("lair-unlocked", true);
        };

        popup->show();
    }

    void onScene(CCObject* sender) {
        ElderLayer::scene();
    }
};