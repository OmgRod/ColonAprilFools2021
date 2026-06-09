#include <Geode/Geode.hpp>
#include <Geode/modify/SecretLayer3.hpp>
#include "../layers/CreditsLayer.h"
#include "../api/DialogAPI.hpp"

using namespace geode::prelude;

class $modify(MySecretLayer3, SecretLayer3) {
public:
    struct Fields {
        CCSprite* coin;
    };

    bool init() {
		if (!SecretLayer3::init()) return false;

        CCSpriteFrameCache::get()->addSpriteFramesWithFile("GJ_ShopSheet01.plist");
        CCTextureCache::sharedTextureCache()->addImage("GJ_ShopSheet01.png", false); // file does exist ofc. it's a spritesheet!

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto fields = m_fields.self();

        auto mod = Mod::get();
        auto gm = GameManager::sharedState();
        bool freed = mod->getSettingValue<bool>("ignore-free-monster") || gm->getUGV("13");

        if (freed) {
            if (auto bars = this->getChildByID("bars")) bars->setVisible(false);
            if (auto demon = this->getChildByID("eyes")) demon->setPosition({ 99999999.f, 99999999.f }); // this one is very annoying
            if (auto keys = this->getChildByID("keys-menu")) keys->setVisible(false);

            if (!mod->getSavedValue<bool>("coin-collected")) {
                if (mod->getSavedValue<bool>("entered-destroy")) {
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
                        if (auto scratch = AnimatedShopKeeper::create(ShopType::Secret)) {
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

                            if (!mod->getSavedValue<bool>("scratch-dialog")) {
                                this->runAction(CCSequence::create(
                                    CCDelayTime::create(2.f),
                                    CCCallFunc::create(this, callfunc_selector(MySecretLayer3::onScratch)),
                                    nullptr
                                ));
                            }
                        } else {
                            log::warn("scratch didnt work somehow");
                        }
                    }
                }
            }
        }

        return true;
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
        
        FMODAudioEngine::sharedEngine()->playEffect("highscoreGet02.ogg"); // file does exist ofc, but it's somewhere inside gd's exe i think
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

        Mod::get()->setSavedValue("scratch-dialog", true);
    }

    void onBack(CCObject* sender) {
        if (Mod::get()->getSavedValue<bool>("coin-unlocked") && !Mod::get()->getSavedValue<bool>("coin-collected", false)) {
            Mod::get()->setSavedValue("coin-collected", true);
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