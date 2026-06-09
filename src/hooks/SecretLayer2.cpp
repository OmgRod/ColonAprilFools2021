#include <Geode/Geode.hpp>
#include <Geode/modify/SecretLayer2.hpp>

using namespace geode::prelude;

class $modify(SecretLayer2) {
public:
    void onSubmit(CCObject* sender) {
        std::string input = m_searchInput->getString();
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "destroy") {
            m_messageLabel->setString("Gah! I knew this day would come!");
            m_messageLabel->setColor({ 255, 150, 0 });
            m_searchInput->setString("");
            Mod::get()->setSavedValue("entered-destroy", true);
        } else {
            SecretLayer2::onSubmit(sender);
        }
    }
};