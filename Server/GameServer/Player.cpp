#include "pch.h"
#include "Player.h"

Player::Player()
{
	_isPlayer = true;
	playerInfo = new Protocol::PlayerInfo();
}

Player::~Player()
{
	delete playerInfo;
	playerInfo = nullptr;
}
