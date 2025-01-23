#include "pch.h"
#include "DB_AccountService.h"
#include "DBConnection.h"
#include "DBBind.h"
#include "DBConnectionPool.h"
#include "DBManager.h"
#include "Room.h"


bool DB_AccountService::FetchUserInfo(const std::wstring& username, const std::wstring& password, int32& userId, int32& goldAmount) {
	DBConnection* dbConn = GDBConnectionPool->Pop();
	ASSERT_CRASH(dbConn != nullptr);
	LOG_EVENT(Color::WHITE, L"[DB] Fetching user info. Username=%s\n", username.c_str());

	bool success = false;

	try {
		DBBind<2, 2> dbBind(*dbConn, L"SELECT UserAccount.UserId, UserStatus.GoldAmount "
			L"FROM UserAccount "
			L"JOIN UserStatus ON UserAccount.UserId = UserStatus.UserId "
			L"WHERE UserAccount.Username = ? AND UserAccount.Password = ?");
		dbBind.BindParam(0, username.c_str());
		dbBind.BindParam(1, password.c_str());
		dbBind.BindCol(0, OUT userId);
		dbBind.BindCol(1, OUT goldAmount);


		success = dbBind.Execute() && dbConn->Fetch();
	}
	catch (const std::exception& ex) {
		std::wcerr << L"Error fetching user info: " << ex.what() << std::endl;
		success = false;
	}

	GDBConnectionPool->Push(dbConn);
	return success;
}


bool DB_AccountService::ChangeGold(DBConnection* dbConn, int32 userId, int32 amount, shared_ptr<Room> room)
{
	ASSERT_CRASH(dbConn != nullptr);
	LOG_EVENT(Color::WHITE, L"[DB] Changing gold. UserId=%d, Amount=%d\n", userId, amount);

	DBBind<3, 0> dbBind(*dbConn, L"UPDATE UserStatus SET GoldAmount = GoldAmount + ? WHERE UserId = ? AND GoldAmount + ? >= 0");
	dbBind.BindParam(0, amount);
	dbBind.BindParam(1, userId);
	dbBind.BindParam(2, amount);

	bool success = dbBind.Execute();

	if (success) {
		auto userStatus = GDBManager->GetUserStatus(userId);
		shared_ptr<Player> player = nullptr;

		for (const auto& [id, playerIn] : room->GetPlayers()) {
			if (playerIn->GetSession().lock()->GetUserStatus()->GetUserId() == userId) {
				player = playerIn;
				break;
			}
		}

		if (player == nullptr) {
			return success; // 연결 반환 필요 없음
		}

		int32 newGoldAmount = userStatus->GetGoldAmount() + amount;
		userStatus->SetGoldAmount(newGoldAmount);

		Protocol::S_CHANGE_CURRENCY packet;
		{
			packet.set_changeamount(amount);
			packet.set_currencytype(Protocol::CurrencyType::GOLD);
			packet.set_totalamount(newGoldAmount);
		}
		if (auto session = player->GetSession().lock()) {
			SEND_PACKET(packet);
		}
	}

	return success;
}


std::shared_ptr<UserStatus> DB_AccountService::LoginUser(int32 userDbId, bool isLoggedIn) {
	DBConnection* dbConn = GDBConnectionPool->Pop();
	ASSERT_CRASH(dbConn != nullptr);
	LOG_EVENT(Color::WHITE, L"[DB] Logging in user. UserId=%d, IsLoggedIn=%s\n", userDbId, isLoggedIn ? L"true" : L"false");

	DBBind<2, 0> dbBind(*dbConn, L"UPDATE UserStatus SET IsLoggedIn = ? WHERE UserId = ?");
	dbBind.BindParam(0, isLoggedIn);
	dbBind.BindParam(1, userDbId);

	ASSERT_CRASH(dbBind.Execute());
	GDBConnectionPool->Push(dbConn);

	// Fetch or create the UserStatus object
	auto userStatus = GDBManager->GetUserStatus(userDbId);
	if (userStatus) {
		userStatus->SetLoggedIn(isLoggedIn);
	}
	else {
		// If the user status is not cached, query it from the database
		DBBind<3, 1> fetchBind(*dbConn, L"SELECT UserId, GoldAmount, IsLoggedIn FROM UserStatus WHERE UserId = ?");
		fetchBind.BindParam(0, userDbId);

		int32 dbUserId = 0;
		int32 goldAmount = 0;
		bool dbIsLoggedIn = false;

		fetchBind.BindCol(0, OUT dbUserId);
		fetchBind.BindCol(1, OUT goldAmount);
		fetchBind.BindCol(2, OUT dbIsLoggedIn);

		if (fetchBind.Execute() && dbConn->Fetch()) {
			userStatus = std::make_shared<UserStatus>();
			userStatus->SetUserId(dbUserId);
			userStatus->SetGoldAmount(goldAmount);
			userStatus->SetLoggedIn(dbIsLoggedIn);
			GDBManager->AddUserStatus(userStatus);
		}
		else {
			// Handle case where the user doesn't exist in the database
			std::cerr << "Error: UserStatus not found for UserId " << userDbId << std::endl;
			return nullptr;
		}
	}

	return userStatus;
}

bool DB_AccountService::RegisterUser(std::wstring username, std::wstring password, int32& userDbId) {
	DBConnection* dbConn = GDBConnectionPool->Pop();
	ASSERT_CRASH(dbConn != nullptr);
	LOG_EVENT(Color::WHITE, L"[DB] Registering user. Username=%s\n", username.c_str());

	if (!dbConn->BeginTransaction()) {
		GDBConnectionPool->Push(dbConn);
		return false;
	}

	try {
		int32 userId = 0;

		// UserAccount 테이블에 데이터 삽입 및 UserId 가져오기
		DBBind<2, 1> accountBind(*dbConn, L"INSERT INTO UserAccount (Username, Password) OUTPUT INSERTED.UserId VALUES (?, ?)");
		accountBind.BindParam(0, username.c_str());
		accountBind.BindParam(1, password.c_str());
		accountBind.BindCol(0, OUT userId);

		if (!accountBind.Execute() || !dbConn->Fetch()) {
			throw std::runtime_error("Failed to insert into UserAccount or retrieve UserId.");
		}

		// UserStatus 테이블에 데이터 삽입
		DBBind<2, 0> statusBind(*dbConn, L"INSERT INTO UserStatus (UserId, Username, GoldAmount) VALUES (?, ?, 0)");
		statusBind.BindParam(0, userId);
		statusBind.BindParam(1, username.c_str());

		if (!statusBind.Execute()) {
			throw std::runtime_error("Failed to insert into UserStatus.");
		}

		// 트랜잭션 커밋
		if (!dbConn->CommitTransaction()) {
			throw std::runtime_error("Failed to commit transaction.");
		}

		// 캐싱된 데이터 업데이트
		userDbId = userId; // userId 반환
		auto userStatus = std::make_shared<UserStatus>();
		userStatus->SetUserId(userId);
		userStatus->SetUsername(username);
		userStatus->SetGoldAmount(0);
		GDBManager->AddUserStatus(userStatus);

	}
	catch (const std::exception& ex) {
		dbConn->RollbackTransaction();
		std::wcerr << L"RegisterUser Error: " << ex.what() << std::endl;
		GDBConnectionPool->Push(dbConn);
		return false;
	}

	GDBConnectionPool->Push(dbConn);
	return true;
}

