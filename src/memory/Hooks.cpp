#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/SecretLayer3.hpp>
#include <Geode/modify/SecretLayer2.hpp>
#include <Geode/modify/SecretLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../layers/CodePopup.h"
#include "../layers/ElderLayer.h"
#include "../layers/CreditsLayer.h"
#include "../api/DialogAPIIncludes.hpp"
#include "Geode/ui/OverlayManager.hpp"

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

            Mod::get()->setSavedValue<bool>("shopkeeper-dialog-1", true);
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

            Mod::get()->setSavedValue<bool>("lair-unlocked", true);
        };

        popup->show();
    }

    void onScene(CCObject* sender) {
        ElderLayer::scene();
    }
};

class $modify(MySecretLayer3, SecretLayer3) {
public:
    struct Fields {
        CCSprite* coin;
    };

    bool init() {
		if (!SecretLayer3::init()) return false;

        auto pRet = this;

        CCSpriteFrameCache::get()->addSpriteFramesWithFile("GJ_ShopSheet01.plist");
        CCTextureCache::sharedTextureCache()->addImage("GJ_ShopSheet01.png", false);

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto fields = m_fields.self();

        auto mod = Mod::get();
        auto gm = GameManager::sharedState();
        bool freed = mod->getSettingValue<bool>("ignore-free-monster") || gm->getUGV("13");

        log::debug("Freed monster: {}, ugv? {}, mod setting? {}", freed, gm->getUGV("13"), mod->getSettingValue<bool>("ignore-free-monster"));

        if (freed && mod->getSavedValue<bool>("entered-destroy")) {
            if (auto bars = this->getChildByID("bars")) bars->setVisible(false);
            if (auto demon = this->getChildByID("eyes")) demon->setPosition({ 99999999.f, 99999999.f }); // this one is very annoying
            if (auto keys = this->getChildByID("keys-menu")) keys->setVisible(false);

            if (mod->getSavedValue<bool>("coin-unlocked") && !mod->getSavedValue<bool>("coin-collected", false)) {
                auto coinSprite = CCSprite::createWithSpriteFrameName("secretCoin_01_001.png");
                
                auto coinBtn = CCMenuItemSpriteExtra::create(
                    coinSprite,
                    this,
                    menu_selector(MySecretLayer3::onCoinClicked)
                );

                auto frames = CCArray::create();
                for (unsigned int i = 1; i <= 4; ++i) {
                    auto name = CCString::createWithFormat("secretCoin_01_00%i.png", i)->getCString();
                    auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
                    if (frame) frames->addObject(frame);
                }

                auto anim = CCAnimation::createWithSpriteFrames(frames, 0.115f);
                coinSprite->runAction(CCRepeatForever::create(CCAnimate::create(anim)));

                auto menu = CCMenu::create();
                menu->setPosition({ winSize.width / 2, winSize.height / 2 });
                menu->setZOrder(4);
                this->addChild(menu);

                coinBtn->setPosition({ 0.f, 0.f });
                menu->addChild(coinBtn);
                
                this->setTouchEnabled(true);
            } else {
                auto scratch = AnimatedShopKeeper::create(ShopType::Secret);
                if (!scratch) {
                    return pRet;
                }

                scratch->setTag(315);
                scratch->setPosition({
                    winSize.width / 2.f - 1.f,
                    winSize.height / 2.f + 10.f
                });
                scratch->setScale(0.8f);

                this->addChild(scratch, 4);

                scratch->startAnimating();

                auto animSprite = scratch->getChildByType<CCSprite*>(0);

                if (animSprite) {
                    if (auto overlay = CCSprite::create("the_fucking_void.png"_spr)) {
                        overlay->setScale(2.25f);
                        overlay->setBlendFunc({
                            GL_DST_COLOR,
                            GL_ONE_MINUS_SRC_ALPHA
                        });

                        animSprite->addChild(overlay, 68);
                    }
                } else {
                    log::warn("AnimatedShopKeeper sprite child not found");
                }

                auto bars = CCSprite::createWithSpriteFrameName("dungeon_bars_001.png");
                if (bars) {
                    bars->setPosition({
                        winSize.width / 2.f - 2.f,
                        winSize.height / 2.f
                    });

                    this->addChild(bars, 4 + 1);
                }

                mod->setSavedValue("scratch-dialog", true);

                this->runAction(CCSequence::create(
                    CCDelayTime::create(2.f),
                    CCCallFunc::create(this, callfunc_selector(MySecretLayer3::onScratch)),
                    nullptr
                ));
            }
        }

        return pRet;
    }

    void onCoinClicked(CCObject* sender) {
        auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
        
        button->setEnabled(false);
        button->stopAllActions();
        
        auto localPos = button->getPosition();

        auto worldPos = button->getParent()->convertToWorldSpace(localPos);
        auto targetLayerPos = this->convertToNodeSpace(worldPos);

        auto particles = CCParticleSystemQuad::create("coinPickupEffect.plist", false);
        particles->setPosition(targetLayerPos);
        particles->setZOrder(4);
        this->addChild(particles);

        auto wave1 = CCCircleWave::create(25.0f, 0.f, 1.f, false);
        wave1->m_color = { 255, 200, 0 };
        wave1->setPosition(targetLayerPos);
        wave1->setZOrder(4);
        this->addChild(wave1);

        auto wave2 = CCCircleWave::create(5.0f, 0.f, 0.5f, true);
        wave2->m_color = { 255, 255, 0 };
        wave2->setPosition(targetLayerPos);
        wave2->setZOrder(4);
        this->addChild(wave2);

        ccBezierConfig bezier;
        bezier.controlPoint_1 = ccp(localPos.x + 10.f, localPos.y + 180.0f);
        bezier.controlPoint_2 = ccp(localPos.x + 30.f, localPos.y - 150.0f);
        bezier.endPosition    = ccp(localPos.x, localPos.y - 300.0f);

        auto bezierAction = CCBezierTo::create(1.0f, bezier);
        auto fadeAction   = CCFadeOut::create(0.2f);
        auto delayAction  = CCDelayTime::create(0.5f);

        button->runAction(bezierAction);
        button->runAction(CCSequence::create(delayAction, fadeAction, nullptr));
        
        button->runAction(CCSequence::create(
            CCDelayTime::create(1.1f),
            CCCallFunc::create(button, callfunc_selector(CCNode::removeFromParent)),
            nullptr
        ));
        
        FMODAudioEngine::sharedEngine()->playEffect("highscoreGet02.ogg");

        Mod::get()->setSavedValue("coin-collected", true);
    }

    void onScratch() {
        auto dialog = CCArray::create();
        dialog->addObject(DialogObject::create(
            "Scratch",
            "... There must be a way out of here somewhere.",
            12,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "Huh? Who's there?",
            14,
            1.f,
            false,
            ccWHITE
        ));
        auto text = fmt::format("<cl>{}</c>! Who let you down here?", getPlayerName());
        dialog->addObject(DialogObject::create(
            "Scratch",
            text,
            10,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "Well, I have nothing to offer anymore. <cr>RubRub found me.</c>",
            12,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "Somebody must have ratted me out. <cg>But who?</c>",
            11,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "I'd ask for your help, but I dont think theres anything you can do...",
            8,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "... Wait a second! The <cp>Emblem</c> I sold to you!",
            12,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "Do you still have it?",
            13,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "If we <cy>return</c> it to RubRub, <cg>he might just let me go.</c>",
            10,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "<cg>You must search for RubRub and return his Emblem.</c>",
            9,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "<cg>If anybody asks, you never saw me.</c>",
            13,
            1.f,
            false,
            ccWHITE
        ));
        dialog->addObject(DialogObject::create(
            "Scratch",
            "...",
            9,
            1.f,
            false,
            ccWHITE
        ));

        auto layer = DialogLayer::createWithObjects(dialog, 2);
        layer->addToMainScene();
        layer->animateIn(DialogAnimationType::FromTop);
    }

    void onBack(CCObject* sender) {
        if (Mod::get()->getSavedValue<bool>("coin-unlocked")) {
            auto scene = CCScene::create();
            auto layer = CreditsLayer::create();
            scene->addChild(layer);
            auto transition = CCTransitionFade::create(0.5f, scene);
            CCDirector::sharedDirector()->replaceScene(transition);
        } else {
            SecretLayer3::onBack(sender);
        }
    }
};

class $modify(SecretLayer2) {
public:
    void onSubmit(CCObject* sender) {
        std::string input = m_searchInput->getString();
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "destroy") {
            m_messageLabel->setString("Gah! I knew this day would come!");
            m_messageLabel->setColor({ 255, 150, 0 });
            m_searchInput->setString("");
            Mod::get()->setSavedValue("entered-destroy", true);
        } else {
            SecretLayer2::onSubmit(sender);
        }
    }
};

class $modify(SecretLayer) {
public:
    void onSubmit(CCObject* sender) {
        std::string input = m_searchInput->getString();

        if (Mod::get()->getSavedValue<bool>("emblem-given")) {
            if (input == "CLUBSTEP") {
                Mod::get()->setSavedValue("typed-clubstep", true);
                Mod::get()->saveData();
                game::exit(true);
            } else {
                SecretLayer::onSubmit(sender);
            }
        } else {
            SecretLayer::onSubmit(sender);
        }
    }
};

// new BlackLayer
class $modify(MyMenuLayer, MenuLayer) {
    struct Fields {
        TextArea* m_text;
    };

    bool init() {
        if (!MenuLayer::init()) return false;

        auto ccd = CCDirector::sharedDirector();
        auto ol = OverlayManager::get();
        auto fields = m_fields.self();

        auto winSize = ccd->getWinSize();
        
        if (Mod::get()->getSavedValue<bool>("typed-clubstep")) {
            FMODAudioEngine::sharedEngine()->pauseAllAudio();

            auto winSize = ccd->getWinSize();

            auto blocker = CCLayerColor::create(
                ccc4(0, 0, 0, 255),
                winSize.width,
                winSize.height
            );

            ol->addChild(blocker, ccd->getRunningScene()->getHighestChildZ() + 1);

            auto text = fields->m_text;

            text = TextArea::create("", "chatFont.fnt", 1.f, ccd->getWinSize().width, { 0.5f, 0.5f }, 1.f, false);
            text->setPosition({ winSize.width / 2, winSize.height / 2 });
            text->setZOrder(blocker->getZOrder() + 1);
            ol->addChild(text);

            auto seq = CCArray::create();
            seq->addObject(CCDelayTime::create(5.f));

            std::vector<std::string> lines = {
                "So this is how it all ends...",
                "You are stronger than I thought.",
                "Heh heh...",
                "Perhaps we will meet again.",
                "I'll be watching you.",
                "...",
            };

            for (auto const& str : lines) {
                seq->addObject(CallFuncExt::create([text, str] {
                    text->setString(str);
                    text->fadeInCharacters(0.3f, 0.2f, false, TextFadeInStyle::Fade);
                }));

                seq->addObject(
                    CCDelayTime::create((0.3f * str.length()) + 0.5f)
                );
            }

            seq->addObject(CallFuncExt::create([]() {
                Mod::get()->setSavedValue("typed-clubstep", false);
                Mod::get()->setSavedValue("coin-unlocked", true);
                Mod::get()->saveData();
                game::exit(true);
            }));

            ol->runAction(CCSequence::create(seq));
        }

        return true;
    }
};
