#include <Geode/Geode.hpp>
#include <Geode/modify/SecretLayer.hpp>

using namespace geode::prelude;

class $modify(SecretLayer) {
public:
    void onSubmit(CCObject* sender) {
        if (m_searchInput) {
            std::string input = m_searchInput->getString();

            if (Mod::get()->getSavedValue<bool>("emblem-given") && !Mod::get()->getSavedValue<bool>("typed-clubstep")) {
                if (input == "CLUBSTEP") {
                    Mod::get()->setSavedValue("typed-clubstep", true);
                    Mod::get()->saveData();
                    game::exit(true);
                } else {
                    SecretLayer::onSubmit(sender);
                }
            } else {
                SecretLayer::onSubmit(sender);
            }
        } else {
            SecretLayer::onSubmit(sender);
        }
    }
};
