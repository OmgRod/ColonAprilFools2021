#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// new BlackLayer
class $modify(MyMenuLayer, MenuLayer) {
    struct Fields {
        TextArea* m_text;
    };

    bool init() {
        if (!MenuLayer::init()) return false;

        auto ccd = CCDirector::sharedDirector();
        auto ol = OverlayManager::get();
        auto fields = m_fields.self();

        auto winSize = ccd->getWinSize();
        
        if (Mod::get()->getSavedValue<bool>("typed-clubstep")) {
            FMODAudioEngine::sharedEngine()->pauseAllAudio();

            auto winSize = ccd->getWinSize();

            auto blocker = CCLayerColor::create(
                ccc4(0, 0, 0, 255),
                winSize.width,
                winSize.height
            );

            ol->addChild(blocker, ccd->getRunningScene()->getHighestChildZ() + 1);

            auto text = fields->m_text;

            text = TextArea::create("", "chatFont.fnt", 1.f, ccd->getWinSize().width, { 0.5f, 0.5f }, 1.f, false);
            text->setPosition({ winSize.width / 2, winSize.height / 2 });
            text->setZOrder(blocker->getZOrder() + 1);
            ol->addChild(text);

            auto seq = CCArray::create();
            seq->addObject(CCDelayTime::create(5.f));

            std::vector<std::string> lines = {
                "So this is how it all ends...",
                "You are stronger than I thought.",
                "Heh heh...",
                "Perhaps we will meet again.",
                "I'll be watching you.",
                "...",
            };

            for (auto const& str : lines) {
                seq->addObject(CallFuncExt::create([text, str] {
                    text->setString(str);
                    text->fadeInCharacters(0.3f, 0.2f, false, TextFadeInStyle::Fade);
                }));

                seq->addObject(
                    CCDelayTime::create((0.3f * str.length()) + 0.5f)
                );
            }

            seq->addObject(CallFuncExt::create([]() {
                Mod::get()->setSavedValue("typed-clubstep", false);
                Mod::get()->setSavedValue("coin-unlocked", true);
                Mod::get()->saveData();
                game::exit(true);
            }));

            ol->runAction(CCSequence::create(seq));
        }

        return true;
    }
};
