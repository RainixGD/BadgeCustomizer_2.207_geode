#include "./BadgeCustomizerManager.h"

void BadgeCustomizerManager::init() {
	loadingStatus = loadData();
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

BadgeCustomizerManager::DataLoadingResult BadgeCustomizerManager::loadData() {
	std::ifstream file("Resources/badgeCustomizer.json");
	if (!file) return FileNotFound;
	std::ostringstream buffer;
	buffer << file.rdbuf();
	std::string fileContent = buffer.str();

	file.close();
	try {
		auto root = nlohmann::json::parse(fileContent);

		if (!root.is_object()) return ParsingError;

		for (const auto& [key, value] : root.items()) {
			if (!can_convert_to_int(key)) return ParsingError;

			int badgeId = std::stoi(key);
			if (badgeId < 1 || !value.is_string()) return ParsingError;

			badges[badgeId] = value.get<std::string>();
		}
	}
	catch (const std::exception& e) {
		return ParsingError;
	}
	return OK;
}

void BadgeCustomizerManager::createErrorLabel(CCLayer* layer) {
	std::string errorText;
	switch (loadingStatus) {
	case BadgeCustomizerManager::FileNotFound:
		errorText = "Can't find 'badgeCustomizer.json' in ./Resources";
		break;
	case BadgeCustomizerManager::ParsingError:
		errorText = "Can't parse 'badgeCustomizer.json'";
		break;
	}

	auto size = CCDirector::sharedDirector()->getWinSize();

	auto errorLabel = CCLabelBMFont::create(errorText.c_str(), "bigFont.fnt");
	errorLabel->setColor({ 255, 0, 0 });
	errorLabel->setScale(0.6);
	errorLabel->setPosition({ size.width / 2, size.height - 10 });
	layer->addChild(errorLabel);
}

void BadgeCustomizerManager::onMenuLayer(CCLayer* layer) {
	if (loadingStatus != OK) {
		createErrorLabel(layer);
		return;
	}
}

void BadgeCustomizerManager::onProfilePage_loadPageFromUserInfo(ProfilePage* self, GJUserScore* score) {
	if (loadingStatus != OK) return;

	int modValue = score->m_modBadge;

	auto layer = dynamic_cast<CCNode*>(self->getChildren()->objectAtIndex(0));
	auto usernameMenu = layer->getChildByID("username-menu");
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

void BadgeCustomizerManager::onCommentCell_loadFromComment(CommentCell* cell, GJComment* comment) {
	if (loadingStatus != OK) return;

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