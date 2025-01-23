#include "pch.h"

#include "Player.h"

Player::Player()  {
	SetObjectType(Protocol::ObjectType::PLAYER);

	Protocol::ObjectInfo objInfo;
	Protocol::PositionInfo* posInfo = objInfo.mutable_posinfo();
	posInfo->set_posx(0);
	posInfo->set_posz(0);
	posInfo->set_state(Protocol::CreatureState::IDLE);

	Protocol::StatInfo statInfo;
	statInfo = DataManager::GetStatDict().at(1);
	SetObjectInfo(objInfo);
	SetStat(statInfo);

}

Player::~Player() {}
