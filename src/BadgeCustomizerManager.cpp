#include "./BadgeCustomizerManager.h"
#include "./ErrorsManager/ErrorsManager.h"

void BadgeCustomizerManager::init() {
	isOk = loadData();
}

bool BadgeCustomizerManager::can_convert_to_int(const std::string& str) {
	try {
		std::stoi(str);
		return true;
	}
	catch (const std::invalid_argument& e) {
		return false;
	}
	catch (const std::out_of_range& e) {
		return false;
	}
}

bool BadgeCustomizerManager::hasFileExtension(const std::string& fileName, const std::string& extension) {
    std::string pattern = ".*\\." + extension + "$";
    std::regex regexPattern(pattern, std::regex::icase);
    return std::regex_match(fileName, regexPattern);
}

bool BadgeCustomizerManager::loadData() {
    std::ifstream file("Resources/badgeCustomizer.json");
    if (!file) {
        ErrorsManager::addError("Badge Customizer: File 'Resources/badgeCustomizer.json' not found or cannot be opened.", ErrorsManager::Error);
        return false;
    }

    std::ostringstream buffer;
    try {
        buffer << file.rdbuf();
    }
    catch (const std::ios_base::failure& e) {
        ErrorsManager::addError("Badge Customizer: Failed to read from file 'Resources/badgeCustomizer.json'. IOError: " + std::string(e.what()), ErrorsManager::Error);
        file.close();
        return false;
    }

    std::string fileContent = buffer.str();
    file.close();

    if (fileContent.empty()) {
        ErrorsManager::addError("Badge Customizer: File 'Resources/badgeCustomizer.json' is empty.", ErrorsManager::Error);
        return false;
    }

    try {
        auto root = nlohmann::json::parse(fileContent);

        if (!root.is_object()) {
            ErrorsManager::addError("Badge Customizer: JSON root must be an object.", ErrorsManager::Error);
            return false;
        }

        for (const auto& [key, value] : root.items()) {
            if (!can_convert_to_int(key)) {
                ErrorsManager::addError("Badge Customizer: Invalid badge ID. Keys must be convertible to integers. Key: '" + key + "'", ErrorsManager::Error);
                return false;
            }

            int badgeId = std::stoi(key);
            if (badgeId < 1) {
                ErrorsManager::addError("Badge Customizer: Badge ID must be greater than or equal to 1. Invalid ID: " + std::to_string(badgeId), ErrorsManager::Error);
                return false;
            }

            if (!value.is_string()) {
                ErrorsManager::addError("Badge Customizer: Badge value must be a string. Key: " + std::to_string(badgeId), ErrorsManager::Error);
                return false;
            }

            std::string badgePath = value.get<std::string>();

            if (!hasFileExtension(badgePath, "png")) {
                ErrorsManager::addError("Badge Customizer: Badge value must have a .png file extension. Invalid value: '" + badgePath + "' for badge ID: " + std::to_string(badgeId), ErrorsManager::Error);
                return false;
            }

            badges[badgeId] = std::move(badgePath);
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        ErrorsManager::addError("Badge Customizer: JSON parsing error: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const nlohmann::json::type_error& e) {
        ErrorsManager::addError("Badge Customizer: JSON type error: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const std::ios_base::failure& e) {
        ErrorsManager::addError("Badge Customizer: I/O error: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const std::exception& e) {
        ErrorsManager::addError("Badge Customizer: Unknown error: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }

    return true;
}

void BadgeCustomizerManager::onProfilePage_loadPageFromUserInfo(ProfilePage* self, GJUserScore* score) {
	if (!isOk) return;

	int modValue = score->m_modBadge;

	auto layer = dynamic_cast<CCNode*>(self->getChildren()->objectAtIndex(0));
	auto usernameMenu = layer->getChildByID("username-menu");
	if (!usernameMenu) return;
	auto badge = usernameMenu->getChildByID("mod-badge");
	if (!badge) return;
	
	if (badges.find(modValue) == badges.end()) return;

	if (!usernameMenu->getChildByID("badge-customizer-custom-badge-icon")) {
		auto newBadge = CCSprite::create(badges[modValue].c_str());
		if (newBadge == nullptr)
			newBadge = CCSprite::createWithSpriteFrameName("edit_delBtnSmall_001.png");
		newBadge->setID("badge-customizer-custom-badge-icon");
		newBadge->setPosition(badge->getPosition());
		usernameMenu->addChild(newBadge);
	}

	badge->removeFromParent();
	usernameMenu->updateLayout();
}

void BadgeCustomizerManager::onCommentCell_loadFromComment(CommentCell* cell, GJComment* comment) {
	if (!isOk) return;

	int modValue = comment->m_modBadge;

	auto mainMenu = cell->m_mainLayer->getChildByID("main-menu");
	if (!mainMenu) return;
	auto userMenu = mainMenu->getChildByID("user-menu");
	if (!userMenu) return;
	auto usernameMenu = userMenu->getChildByID("username-menu");
	if (!usernameMenu) return;
	auto badge = usernameMenu->getChildByID("mod-badge");
	if (!badge) return;

	if (badges.find(modValue) == badges.end()) return;

	auto newBadge = CCSprite::create(badges[modValue].c_str());
	if (newBadge == nullptr)
		newBadge = CCSprite::createWithSpriteFrameName("edit_delBtnSmall_001.png");
	newBadge->setPosition(badge->getPosition());
	usernameMenu->addChild(newBadge);

	badge->removeFromParent();
}

BadgeCustomizerManager* BadgeCustomizerManager::getInstance() {
	if (!instance) {
		instance = new BadgeCustomizerManager();
		instance->init();
	}
	return instance;
}
BadgeCustomizerManager* BadgeCustomizerManager::instance = nullptr;