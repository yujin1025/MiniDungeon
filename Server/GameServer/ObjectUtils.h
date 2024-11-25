#pragma once

class ObjectUtils
{
public:
	static PlayerRef CreatePlayer(GameSessionRef session);
	static MonsterRef CreateMonster();
	static const uint64 GetNewObjectID();
	static BossRef CreateBoss();

private:
	static atomic<uint64> s_idGenerator;
};

