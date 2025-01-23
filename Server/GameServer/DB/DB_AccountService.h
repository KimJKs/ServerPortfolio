#pragma once
#include "DB_Class.h"

class DBConnection;

class DB_AccountService {
public:

	static bool FetchUserInfo(const std::wstring& username, const std::wstring& password, int32& userId, int32& goldAmount);

	// ������ ��� ����
	static bool ChangeGold(DBConnection* dbConn, int32 userId, int32 amount, shared_ptr<Room> room);

	// �α��� ���� ������Ʈ
	static std::shared_ptr<UserStatus> LoginUser(int32 userDbId, bool isLoggedIn);

	// ȸ������
	static bool RegisterUser(std::wstring username, std::wstring password, int32& userDbId);
};
