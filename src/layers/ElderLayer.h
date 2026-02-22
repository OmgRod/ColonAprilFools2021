#ifndef __ELDERLAYER_H__
#define __ELDERLAYER_H__

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class ElderLayer : public CCLayer {
protected:
	CCPoint m_obNext;
	CCMenu* m_buttonMenu;

protected:
	virtual bool init();

	void onExit(CCObject*);
	void addButton(const char* frameName, const char* text);
	void keyBackClicked();
	void enterButtons();

public:
	static ElderLayer* create();
	static void scene();
};

#endif