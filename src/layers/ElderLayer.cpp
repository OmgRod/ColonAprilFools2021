#include "ElderLayer.h"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

bool ElderLayer::init() {
	GameManager::sharedState()->fadeInMusic("secretLoop.mp3");

	this->addEventListener(
        KeybindSettingPressedEventV3(Mod::get(), "give-master-emblem"),
        [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
            if (down && !repeat) {
                FLAlertLayer::create("Success", "You gave the <cp>Master Emblem</c> to RobTop.", "OK")->show();
            }
        }
    );

	auto director = CCDirector::sharedDirector();
	auto winSize = director->getWinSize();

	auto bg = CCSprite::create("GJ_gradientBG.png");
	auto bgSize = bg->getTextureRect().size;
	bg->setAnchorPoint({ 0.0f, 0.0f });
	bg->setScaleX((winSize.width + 10.0f) / bgSize.width);
	bg->setScaleY((winSize.height + 10.0f) / bgSize.height);
	bg->setPosition({ -5.0f, -5.0f });
	bg->setColor({ 60, 0, 60 });
	this->addChild(bg);

	auto misc = CCMenu::create();
	this->addChild(misc);

	m_buttonMenu = CCMenu::create();
	this->addChild(m_buttonMenu);

	m_obNext = ccp(0.0f, 75.0f);

	auto backBtn = CCMenuItemSpriteExtra::create(
		CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
		this,
		menu_selector(ElderLayer::onExit)
	);
	backBtn->setPosition((-winSize.width / 2) + 25.0f, (winSize.height / 2) - 25.0f);
	misc->addChild(backBtn);

	auto reqBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Req", 220, false, "bigFont.fnt", "GJ_button_04.png", 25.0f, 0.4f),
		this,
		menu_selector(ElderLayer::onReq)
	);
	reqBtn->setPosition((winSize.width / 2) - 25.0f, -(winSize.height / 2) + 20.0f);
	misc->addChild(reqBtn);

	if (auto discordSpr = CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png")) {
		discordSpr->setScale(1.3f);
		auto discordBtn = CCMenuItemSpriteExtra::create(
			discordSpr,
			this,
			menu_selector(ElderLayer::onDiscord)
		);

		discordBtn->setPosition((winSize.width / 2) - 25.0f, (winSize.height / 2) - 25.0f);
		misc->addChild(discordBtn);
	}

	auto title = CCLabelBMFont::create("RobTop's Lair", "goldFont.fnt");
	title->setPosition(winSize.width / 2, winSize.height / 2 + 130.0f);
	title->setScale(1.2f);
	this->addChild(title);

	this->addButton("GJ_chatBtn_001.png", "Moderator Lounge", [=](CCObject*) {
		FLAlertLayer::create("Oh no!", "You do not have permission to open the <cp>Moderator Lounge</c>.", "OK")->show();
	});
	this->addButton("GJ_deleteBtn_001.png", "Delete Comments", [=](CCObject*) {
		FLAlertLayer::create("Oh no!", "You do not have permission to <cr>delete comments</c>.", "OK")->show();
	});
	this->addButton("GJ_reportBtn_001.png", "Comment Ban", [=](CCObject*) {
		FLAlertLayer::create("Oh no!", "You do not have permission to use the <cy>Comment Ban</c> tool.", "OK")->show();
	});
	this->addButton("GJ_musicOnBtn_001.png", "Whitelist Artist", [=](CCObject*) {
		FLAlertLayer::create("Oh no!", "You do not have permission to <cg>whitelist artists</c>.", "OK")->show();
	});
	this->addButton("GJ_levelLeaderboardBtn_001.png", "Unfreeze Leaderboards", [=](CCObject*) {
		FLAlertLayer::create("Oh no!", "You do not have permission to <cb>unfreeze leaderboards</c>.", "OK")->show();
	});

	this->runAction(
		CCSequence::create(
			CCDelayTime::create(0.5f),
			CCCallFunc::create(this, callfunc_selector(ElderLayer::enterButtons)),
			nullptr
		)
	);

	this->setKeypadEnabled(true);
	return true;
}

void ElderLayer::onExit(CCObject*) {
	GameManager::sharedState()->fadeInMusic("menuLoop.mp3");
	CCDirector::sharedDirector()->popSceneWithTransition(0.5f, kPopTransitionFade);
}

void ElderLayer::keyBackClicked() {
	this->onExit(nullptr);
}

void ElderLayer::addButton(const char* frameName, const char* text, std::function<void(CCObject*)> func) {
	auto sprite = CCSprite::createWithSpriteFrameName(frameName);
	auto winSize = CCDirector::sharedDirector()->getWinSize();
	auto size = sprite->getTextureRect().size;
	sprite->setScale(winSize.height / size.height * 0.12f);
	auto btn = CCMenuItemExt::createSpriteExtra(
		sprite,
		func
	);
	btn->setPosition(m_obNext.x - 150.0f, m_obNext.y);
	btn->setOpacity(0);
	m_buttonMenu->addChild(btn);

	auto label = CCLabelBMFont::create(text, "bigFont.fnt");
	CCPoint labelPos;
	label->convertToNodeSpace(labelPos);
	label->setAnchorPoint(labelPos);
	label->setPosition(m_obNext.x - 120.0f, m_obNext.y - 10.0f);
	label->setScale(0.7f);
	label->setOpacity(0);
	m_buttonMenu->addChild(label);

	m_obNext.y = m_obNext.y - 45.0f;
}

void ElderLayer::enterButtons() {
	for (unsigned int i = 0; i < m_buttonMenu->getChildrenCount(); i += 2) {
		auto node1 = static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(i));
		auto node2 = static_cast<CCNode*>(m_buttonMenu->getChildren()->objectAtIndex(i + 1));
		auto delay1 = CCDelayTime::create(0.1f * i);
		auto fade1 = CCFadeIn::create(0.7f);
		auto delay2 = CCDelayTime::create(0.1f * i);
		auto fade2 = CCFadeIn::create(0.7f);
		if (node1 && delay1 && fade1) {
			node1->runAction(CCSequence::createWithTwoActions(delay1, fade1));
		}
		if (node2 && delay2 && fade2) {
			node2->runAction(CCSequence::createWithTwoActions(delay2, fade2));
		}
	}
}

ElderLayer* ElderLayer::create() {
	ElderLayer* pRet = new ElderLayer();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		return pRet;
	}
	else {
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

void ElderLayer::scene() {
	auto scene = CCScene::create();
	scene->addChild(ElderLayer::create());
	CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}

void ElderLayer::onDiscord(CCObject* sender) {
	web::openLinkInBrowser("https://discord.gg/geometrydash");
}

void ElderLayer::onReq(CCObject* sender) {
    // auto glm = GameLevelManager::sharedState();

    // if (!glm->requestUserAccess()) return;

    // m_uploadPopup = UploadActionPopup::create(this, "Loading...");
    // m_uploadPopup->show();

	// that popup wouldnt close so nvm
	FLAlertLayer::create("Oh no!", "You are most likely not a moderator, but I dont know... Maybe there are moderators that use this mod..?", "OK")->show();
}

// void ElderLayer::uploadActionFailed(int id, int response) {
//     if (response != 0x2b || !m_uploadPopup)
//         return;

//     m_uploadPopup->showFailMessage("Failed. Nothing found.");
// }

// void ElderLayer::uploadActionFinished(int id, int response) {
//     if (response != 0x2b || !m_uploadPopup) return;

//     auto gm = GameManager::sharedState();

//     int diff = gm->m_sessionAttempts - gm->m_sessionAttempts2;

//     if (diff == 2) {
//         m_uploadPopup->showSuccessMessage("Success! Elder Moderator access granted.");
//     }
//     else if (diff == 1) {
//         m_uploadPopup->showSuccessMessage("Success! Moderator access granted.");
//     }
//     else if (gm->m_canGetLevelSaveData) {
//         m_uploadPopup->showSuccessMessage("Success! Leaderboard Moderator access granted.");
//     }
// }
