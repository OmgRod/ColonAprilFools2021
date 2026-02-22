#ifndef __CodePopup_H__
#define __CodePopup_H__

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class CodePopup : public geode::Popup {
protected:
	LoadingCircle* m_loading;
	ButtonSprite* m_button1;
	ButtonSprite* m_button2;

	bool init();

	void onSubmit(CCObject*);
	void onCancel(CCObject*);
	void onFinish();

public:
	static CodePopup* create();
};

#endif