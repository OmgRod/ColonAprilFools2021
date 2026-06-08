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

	m_textArea = TextInput::create(250.f, "XXXXX-XXXXX-XXXXX", "bigFont.fnt");
	m_textArea->getInputNode()->setLabelPlaceholderColor({ 120, 170, 240 });
	m_textArea->getInputNode()->setMaxLabelScale(0.5f);
	// m_textArea->setMaxCharCount(15); // android hates this for some reason...
	m_textArea->setPosition(m_mainLayer->getContentSize().width / 2, m_mainLayer->getContentSize().height / 2 - 40.0f);
	m_mainLayer->addChild(m_textArea);

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
		CCDelayTime::create(4.f/3.f), // 1.33333...
		CCCallFunc::create(
			this,
			callfunc_selector(CodePopup::onFinish)
		),
		nullptr
	));
}

void CodePopup::onFinish() {
	m_loading->removeFromParentAndCleanup(true);

    if (static_cast<std::string>(
            m_textArea->getInputNode()->getString()
        ).length() == 15) {

        if (m_successCallback) {
            m_successCallback();
        }

        this->onClose(nullptr);

		auto dialog = CCArray::create();
		dialog->addObject(DialogObject::create(
			"The Shopkeeper",
			"It appears that <cl>RubRub</c> has accepted your <co>humble offering</c>.",
			5,
			1.f,
			false,
			ccWHITE
		));
		dialog->addObject(DialogObject::create(
			"The Shopkeeper",
			"You may now pass.",
			5,
			1.f,
			false,
			ccWHITE
		));
		dialog->addObject(DialogObject::create(
			"The Shopkeeper",
			"<cg>The rest is up to you.</c>",
			6,
			1.f,
			false,
			ccWHITE
		));

		auto dialogLayer = DialogLayer::createWithObjects(dialog, 1);
		dialogLayer->addToMainScene();
		dialogLayer->animateIn(DialogAnimationType::FromTop);
	} else {
        FLAlertLayer::create(
            "Oops!",
            "Your Steam code isn't <cr>15 characters long</c>. Please try again!",
            "OK"
        )->show();
    }
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
