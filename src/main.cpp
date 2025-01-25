#include "./includes.h"

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/CommentCell.hpp>

#include "./BadgeCustomizerManager.h"

#include "./ErrorsManager/ErrorsManager.h"

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		BadgeCustomizerManager::getInstance(); // to load all the json errors
		ErrorsManager::onMenuLayer(this);
		return true;
	}
};

class $modify(ProfilePage) {
	void loadPageFromUserInfo(GJUserScore * score) {
		ProfilePage::loadPageFromUserInfo(score);
		BadgeCustomizerManager::getInstance()->onProfilePage_loadPageFromUserInfo(this, score);
	}
};

class $modify(CommentCell) {
	void loadFromComment(GJComment* comment) {
		CommentCell::loadFromComment(comment);
		BadgeCustomizerManager::getInstance()->onCommentCell_loadFromComment(this, comment);
	}
};