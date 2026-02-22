#include "CodePopup.h"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

bool CodePopup::init() {
	if (!Popup::init(400.0f, 240.0f))
            return false;
	
	this->setTitle("RobTop's Offering");

	auto winSize = CCDirector::sharedDirector()->getWinSize();

	auto caption = CCLabelBMFont::create(
		"An offering is required from all\n"
		"who wish to enter RobTop's lair", "goldFont.fnt");
	caption->setAlignment(kCCTextAlignmentCenter);
	caption->setScale(0.8f);
	caption->setPosition(m_mainLayer->getContentSize().width / 2, m_mainLayer->getContentSize().height / 2 + 45.0f);
	m_mainLayer->addChild(caption);

	auto prompt = CCLabelBMFont::create("Enter $20 Steam Code:", "bigFont.fnt");
	prompt->setScale(0.6f);
	prompt->setPosition(m_mainLayer->getContentSize().width / 2, m_mainLayer->getContentSize().height / 2);
	m_mainLayer->addChild(prompt);

	auto textBG = CCScale9Sprite::create("square02_small.png", { 0.0f, 0.0f, 40.0f, 40.0f });
	textBG->setContentSize({ 270.0f, 30.0f });
	textBG->setPosition(m_mainLayer->getContentSize().width / 2, m_mainLayer->getContentSize().height / 2 - 40.0f);
	textBG->setOpacity(100);
	m_mainLayer->addChild(textBG);

	auto textArea = CCTextInputNode::create(250.0f, 30.0f, "XXXXX-XXXXX-XXXXX", "bigFont.fnt");
	textArea->setLabelPlaceholderColor({ 120, 170, 240 });
	textArea->setMaxLabelScale(0.5f);
	textArea->setPosition(m_mainLayer->getContentSize().width / 2, m_mainLayer->getContentSize().height / 2 - 40.0f);
	m_mainLayer->addChild(textArea);

	m_button1 = ButtonSprite::create("Cancel", 0, false, "goldFont.fnt", "GJ_button_01.png", 0.0f, 1.0f);
	auto btn1 = CCMenuItemSpriteExtra::create(m_button1, this, menu_selector(CodePopup::onCancel));
	btn1->setPosition(m_mainLayer->getContentSize().width / 2 - 60.0f, m_mainLayer->getContentSize().height / 2 - 90.0f);
	m_buttonMenu->addChild(btn1);

	m_button2 = ButtonSprite::create("Submit", 0, false, "goldFont.fnt", "GJ_button_01.png", 0.0f, 1.0f);
	auto btn2 = CCMenuItemSpriteExtra::create(m_button2, this, menu_selector(CodePopup::onSubmit));
	btn2->setPosition(m_mainLayer->getContentSize().width / 2 + 60.0f, m_mainLayer->getContentSize().height / 2 - 90.0f);
	m_buttonMenu->addChild(btn2);

	m_mainLayer->setKeypadEnabled(true);
	m_mainLayer->setTouchEnabled(true);

	return true;
}

void CodePopup::onSubmit(CCObject*) {
	m_loading = LoadingCircle::create();
	m_loading->setParentLayer(this);
	m_loading->setFade(true);
	m_loading->show();
	m_loading->setKeyboardEnabled(true);
	
	this->runAction(CCSequence::create(
		CCDelayTime::create(1.34f),
		CCCallFunc::create(
			this,
			callfunc_selector(CodePopup::onFinish)
		),
		nullptr
	));
}

void CodePopup::onFinish() {
	m_loading->removeFromParentAndCleanup(true);
	this->onClose(nullptr);
}

void CodePopup::onCancel(CCObject* sender) {
	this->onClose(sender);
}

CodePopup* CodePopup::create() {
	CodePopup* pRet = new CodePopup();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return nullptr;
}