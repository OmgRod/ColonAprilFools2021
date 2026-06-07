#include "CreditsLayer.h"
#include <cue/RepeatingBackground.hpp>

bool CreditsLayer::init() {
    if (!CCLayer::init()) return false;

    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

    FMODAudioEngine::sharedEngine()->playMusic("menuLoop.mp3", true, 0.5f, 2);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = cue::RepeatingBackground::create("game_bg_01_001.png");
    bg->setColor({ 40, 125, 255 });
    this->addChild(bg);

    addBackButton(
        this,
        [this](cocos2d::CCMenuItem*) {
            keyBackClicked();
        },
        BackButtonStyle::Pink
    );

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
