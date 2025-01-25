#pragma once
#include "./includes.h"

#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

class BadgeCustomizerManager {
	bool isOk = false;
	std::unordered_map<unsigned int, std::string> badges;
	static BadgeCustomizerManager* instance;

	void init();

	bool can_convert_to_int(const std::string& str);

	bool hasFileExtension(const std::string& fileName, const std::string& extension);
	bool loadData();

	BadgeCustomizerManager() {};
public:

	void onProfilePage_loadPageFromUserInfo(ProfilePage* self, GJUserScore* score);
	void onCommentCell_loadFromComment(CommentCell* cell, GJComment* comment);

	static BadgeCustomizerManager* getInstance();
};