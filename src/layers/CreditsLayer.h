#include <Geode/Geode.hpp>

using namespace geode::prelude;

class CreditsLayer : public CCLayer {
protected:
    CCSprite* m_sprite;

	bool init();
    void keyBackClicked();

public:
	CREATE_FUNC(CreditsLayer);
};
