#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "GameSession.h"

atomic<int64> ObjectUtils::s_idGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session)
{
	// ID ������
	const int64 newId = s_idGenerator.fetch_add(1);

	PlayerRef player = make_shared<Player>();
	player->GetObjectInfo()->set_object_id(newId);
	player->GetPosInfo()->set_object_id(newId);
	player->GetPlayerInfo()->set_player_id(newId);
	player->session = session;
	//player->objectInfo->set_object_id(newId);
	//player->posInfo->set_object_id(newId);
	//player->session = session;
	//session->player.store(player);

	session->player = player;

	return player;
}
