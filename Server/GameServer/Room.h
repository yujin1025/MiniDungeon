#pragma once
#include "JobQueue.h"
#include "Struct.pb.h"

class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

public:
	virtual bool Enter(PlayerRef player);
	virtual bool Leave(PlayerRef player);
	//bool EnterRoom(ObjectRef object);

	bool EnterRoom(PlayerRef player, bool isHost = true);
	bool LeaveRoom(PlayerRef player, bool isExitGame = false);
	bool ChangeCharacter(uint64 playerIndex, const Protocol::PlayerType characterType);

	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(uint64 playerIndex, bool isExitGame = false);
	void HandleStartGame();

	bool HandleChangeCharacter(uint64 playerIndex, const Protocol::PlayerType characterType);
	//void HandleMove(Protocol::CTS_MOVE pkt);
	void HandleMove(const Protocol::PosInfo &info);

	void HandleMoveMonster(const Protocol::PosInfo &info);

	void HandleAttack(const Protocol::CTS_ATTACK& pkt);

	void HandleMonsterAttackFinished(uint64 monster_object_id);

	void HandleAttacked(uint64 player_object_id, const Protocol::CTS_ATTACKED& pkt);

	void HandleMonsterCleared();

	void HandleDead(uint64 player_object_id, uint64 objectId);

	uint64 GetRoomIndex() const { return _roomIndex; }
	void SetRoomIndex(uint64 roomIndex);

	void ReleaseThisRoom();

	void SpawnMonsters();

	void SpawnMonster(const Protocol::PosInfo& pos_Info);

	void Spawn(const Protocol::CreatureType creatureType, const Protocol::PosInfo &info);

public:
	void UpdateTick();

	RoomRef GetRoomRef();

private:
	bool AddObject(ObjectRef object);
	bool RemoveObject(uint64 objectId);

	bool AddPlayer(PlayerRef player);
	bool RemovePlayer(PlayerRef player, bool isExitGame = false);

	bool AddMonster(MonsterRef monster, const Protocol::PosInfo& pos_Info = Protocol::PosInfo::default_instance());

	bool AddBoss(BossRef boss, const Protocol::PosInfo& pos_Info = Protocol::PosInfo::default_instance());

	bool RemoveMonster(uint64 monsterId);
public:
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);

	void BroadcastToPlayer(SendBufferRef sendBuffer, uint64 exceptId = 0);

public:
	unordered_map<uint64, PlayerRef> _players;
	unordered_map<uint64, ObjectRef> _objects;
	unordered_map<uint64, MonsterRef> _monsters;
	BossRef _boss;
	weak_ptr<class Lobby> _lobby;
	uint64 _roomIndex = 0;

protected:
	Protocol::RoomInfo* info;

public:
	const Protocol::RoomInfo* GetRoomInfo() { return info; }

	void SetRoomInfo(const Protocol::RoomInfo& info) { this->info->CopyFrom(info); }

	void SetLobby(LobbyRef lobby) { this->_lobby = lobby; }
};