#pragma once

class ObjectUtils
{
public:
	static PlayerRef CreatePlayer(GameSessionRef session);

	static const uint64 GetNewObjectID();

private:
	static atomic<uint64> s_idGenerator;
};

