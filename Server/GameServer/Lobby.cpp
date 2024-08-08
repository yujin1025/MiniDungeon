#include "pch.h"
#include "Lobby.h"

LobbyRef GLobby = make_shared<Lobby>();

Lobby::Lobby()
{

}

Lobby::~Lobby()
{

}

bool Lobby::EnterLobby(PlayerRef player)
{
	return false;
}


