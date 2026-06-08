#include "CreditsLayer.h"
#include <cue/RepeatingBackground.hpp>

bool CreditsLayer::init() {
    if (!CCLayer::init()) return false;

#ifdef GEODE_IS_DESKTOP
    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);
#else
    this->setKeyboardEnabled(false);
    this->setKeypadEnabled(false);
#endif

    GameManager::sharedState()->fadeInMenuMusic();

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = cue::RepeatingBackground::create("game_bg_01_001.png");
    bg->setColor({ 40, 125, 255 });
    this->addChild(bg);

#ifndef GEODE_IS_DESKTOP
    addBackButton(
        this,
        [this](cocos2d::CCMenuItem*) {
            keyBackClicked();
        },
        BackButtonStyle::Pink
    );
#endif

    m_sprite = CCSprite::create("credits.png"_spr);
    float scale = winSize.width / m_sprite->getContentWidth();
    m_sprite->setScale(scale);
    float scaledHeight = m_sprite->getContentHeight() * scale;
    m_sprite->setPosition({ winSize.width / 2, -(scaledHeight / 2) });

    this->addChild(m_sprite);

    float targetY = winSize.height + (scaledHeight / 2);

    auto animation = CCSequence::create(
        CCMoveTo::create(40.f, { winSize.width / 2, targetY }),
        CallFuncExt::create([this]() {
            keyBackClicked();
        }),
        nullptr
    );

    m_sprite->runAction(animation);

    return true;
}

void CreditsLayer::keyBackClicked() {
    auto transition = CCTransitionFade::create(0.5f, MenuLayer::scene(false));
    CCDirector::sharedDirector()->replaceScene(transition);
}
