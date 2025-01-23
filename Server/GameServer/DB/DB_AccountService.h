#pragma once
#include "DB_Class.h"

class DBConnection;

class DB_AccountService {
public:

	static bool FetchUserInfo(const std::wstring& username, const std::wstring& password, int32& userId, int32& goldAmount);

	// 유저의 골드 변경
	static bool ChangeGold(DBConnection* dbConn, int32 userId, int32 amount, shared_ptr<Room> room);

	// 로그인 상태 업데이트
	static std::shared_ptr<UserStatus> LoginUser(int32 userDbId, bool isLoggedIn);

	// 회원가입
	static bool RegisterUser(std::wstring username, std::wstring password, int32& userDbId);
};
