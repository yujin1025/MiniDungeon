#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "GameSession.h"

atomic<uint64> ObjectUtils::s_idGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session)
{
	PlayerRef player = make_shared<Player>();
	player->session = session;
	session->player.store(player);

	return player;
}

const uint64 ObjectUtils::GetNewObjectID()
{
	return s_idGenerator.fetch_add(1);
}
