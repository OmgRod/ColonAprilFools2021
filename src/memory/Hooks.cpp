#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/SecretLayer3.hpp>
#include "../layers/CodePopup.h"
#include "../layers/ElderLayer.h"

using namespace geode::prelude;

#define ZORDER 4

bool g_Coin = false;

enum {
    kCoin = 314,
    kScratch
};

namespace {
    struct UnlockTxtLayout {
        static const CCSize spriteAreaPx;
        static const CCPoint buttonPosPx;
    };

    const CCSize UnlockTxtLayout::spriteAreaPx = { 423.0f, 72.0f };
    const CCPoint UnlockTxtLayout::buttonPosPx = { 117.0f, 41.0f };
}

$on_mod(Loaded) {
    listenForKeybindSettingPresses("toggle-coin", [](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down && !repeat) {
            g_Coin = !g_Coin;
            log::info("Toggled coin: {}", g_Coin);
        }
    });
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
        
        if (auto unlockTxt = this->getChildByID("tap-more-hint")) {
            auto newUnlockTxt = CCSprite::create("unlockTxtUpdated.png"_spr);
            newUnlockTxt->setPosition(unlockTxt->getPosition());
            newUnlockTxt->setID("tap-more-hint");
            
            unlockTxt->removeFromParentAndCleanup(true);

            this->addChild(newUnlockTxt);

            auto spr = CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
            spr->setScale(0.6f);
            auto btn = CCMenuItemSpriteExtra::create(
                spr,
                this,
                menu_selector(MyGJGarageLayer::onUnlock)
            );

            const auto contentSize = newUnlockTxt->getContentSize();
            const auto sprPosition = newUnlockTxt->getPosition();

            m_fields->localPos = ccp(
                sprPosition.x - (contentSize.width / 2) + (contentSize.width * (UnlockTxtLayout::buttonPosPx.x / UnlockTxtLayout::spriteAreaPx.width)),
                sprPosition.y - (contentSize.height / 2) + (contentSize.height * (UnlockTxtLayout::buttonPosPx.y / UnlockTxtLayout::spriteAreaPx.height))
            );

            btn->setPosition(m_fields->localPos);

            m_fields->menu = CCMenu::create();
            m_fields->menu->setPosition({ 0.f, 0.f });
            m_fields->menu->addChild(btn);

            this->addChild(m_fields->menu);
        }

		return true;
    }

    void onUnlock(CCObject* sender) {
        auto old = static_cast<CCMenuItemSpriteExtra*>(sender);
        old->removeFromParentAndCleanup(true);
        
        auto spr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
        spr->setScale(0.6f);
        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(MyGJGarageLayer::onLayer)
        );
        btn->setPosition(m_fields->localPos);
        m_fields->menu->addChild(btn);

        auto popup = CodePopup::create();
        popup->show();
    }

    void onLayer(CCObject* sender) {
        auto old = static_cast<CCMenuItemSpriteExtra*>(sender);
        old->removeFromParentAndCleanup(true);

        auto spr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
        spr->setScale(0.6f);
        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(MyGJGarageLayer::onScene)
        );
        btn->setPosition(m_fields->localPos);
        m_fields->menu->addChild(btn);
        
        auto popup = CodePopup::create();
        CCScene::get()->addChild(popup, 999);
        popup->show();
    }

    void onScene(CCObject* sender) {
        ElderLayer::scene();
    }
};

class $modify(SecretLayer3) {
public:
    bool init() {
		if (!SecretLayer3::init()) return false;

        auto pRet = this;

        if (GameManager::sharedState()->getUGV("13")) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();

            if (g_Coin) {
                auto coin = GameObject::createWithFrame("secretCoin_01_001.png");
                coin->setTag(kCoin);
                coin->setPosition(winSize / 2);
                this->addChild(coin);
                coin->setZOrder(ZORDER);

                auto frames = CCArray::create();
                for (unsigned int i = 1; i <= 4; ++i) {
                    frames->addObject(
                        CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                            CCString::createWithFormat("secretCoin_0%i_001.png", i)->getCString()
                        )
                    );
                }
                coin->runAction(
                    CCRepeatForever::create(
                        CCAnimate::create(
                            CCAnimation::createWithSpriteFrames(frames, 0.115f)
                        )
                    )
                );

                this->setTouchEnabled(true);
            } else {
                auto scratch = AnimatedShopKeeper::create(ShopType::Secret);
                scratch->setTag(kScratch);
                scratch->setPosition({ winSize.width / 2 - 1.0f, winSize.height / 2 + 10.0f });
                scratch->setScale(0.8f);

                auto animSprite = static_cast<CCSprite*>(scratch->getChildren()->objectAtIndex(0));

                this->addChild(scratch);
                scratch->setZOrder(ZORDER);
                scratch->startAnimating();

                CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
                CCTexture2D::PVRImagesHavePremultipliedAlpha(false);

                if (auto overlay = CCSprite::create("the_fucking_void.png"_spr)) {
                    overlay->setScale(2.25f);
                    overlay->setBlendFunc({ GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA });
                    animSprite->addChild(overlay);
                    overlay->setZOrder(68);
                }

                auto bars = CCSprite::createWithSpriteFrameName("dungeon_bars_001.png");
                bars->setPosition({ winSize.width / 2 - 2.0f, winSize.height / 2 });
                this->addChild(bars);
                bars->setZOrder(ZORDER + 1);
            }
        }

        return pRet;
    }
};
