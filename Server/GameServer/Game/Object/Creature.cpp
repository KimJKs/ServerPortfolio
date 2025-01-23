#pragma once
#include "pch.h"
#include "Creature.h"
#include "Room.h"
#include "AStar.h"

Creature::Creature() {

}

Creature::~Creature() {}

void Creature::OnDamaged(int32 damage, std::shared_ptr<Object> attacker) {

	Protocol::StatInfo* statInfo = _objectInfo.mutable_statinfo();
	int32 finalDamage = max(0, damage - statInfo->defense());

	// 체력 감소
	statInfo->set_hp(max(0, statInfo->hp() - finalDamage));

	Protocol::S_UPDATE_RESOURCE packet;
	{
		packet.set_changevalue(-finalDamage);
		packet.set_objectid(GetId());
		packet.set_nowvalue(statInfo->hp());
		packet.set_type(Protocol::ResourceType::HP);
	}

	if (auto room = GetRoom())
	{
		
		room->Broadcast(packet, std::dynamic_pointer_cast<Creature>(shared_from_this()));
	}

	if (IsDead()) {
		OnDead(attacker);
	}
}

void Creature::OnDead(std::shared_ptr<Object> attacker) {
	std::shared_ptr<Room> room = GetRoom();
	if (!room) {
		std::cerr << "Error: Creature's room is null in OnDead!" << std::endl;
		return;
	}

	Protocol::S_DIE diePacket;
	diePacket.set_objectid(GetId());
	diePacket.set_attackerid(attacker ? attacker->GetId() : 0);

	
	room->Broadcast(diePacket, std::dynamic_pointer_cast<Creature>(shared_from_this()));

}

bool Creature::IsDead() const {
	return GetStat().hp() <= 0;
}

void Creature::ReduceMana(int32 amount)
{

	Protocol::StatInfo* statInfo = _objectInfo.mutable_statinfo();

	statInfo->set_mp(max(0, statInfo->mp() - amount));

	if (auto player = std::dynamic_pointer_cast<Player>(shared_from_this()))
	{
		Protocol::S_UPDATE_RESOURCE packet;
		{
			packet.set_changevalue(-amount);
			packet.set_objectid(GetId());
			packet.set_nowvalue(statInfo->mp());
			packet.set_type(Protocol::ResourceType::MP);
		}

		if (auto session = player->GetSession().lock()) {
			SEND_PACKET(packet);
		}
	}

}
