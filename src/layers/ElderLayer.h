#include <Geode/Geode.hpp>
#include "../api/DialogAPI.hpp"

using namespace geode::prelude;

class ElderLayer : public CCLayer {
protected:
	CCPoint m_obNext;
	CCMenu* m_buttonMenu;

protected:
	bool init();

	void onExit(CCObject*);
	void addButton(const char* frameName, const char* text, std::function<void(CCObject*)> func);
	void keyBackClicked();
	void enterButtons();
	void onDiscord(CCObject*);
	void onReq(CCObject*);
	void moreRobDialog();
	void showDialog();
	void onEnterTransitionDidFinish();

public:
	static ElderLayer* create();
	static void scene();
};