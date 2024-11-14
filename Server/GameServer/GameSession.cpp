#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "Lobby.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
    // Step 1: Session에서 Player 접근
    shared_ptr<Player> currentPlayer = player.load();
    if (!currentPlayer)
    {
        // Player가 이미 존재하지 않으면 종료
        return;
    }

    // Step 2: Player에서 Room 접근
    weak_ptr<Room> weakRoom = currentPlayer->room.load();
    shared_ptr<Room> currentRoom = weakRoom.lock();

    // Step 3: Room에서 LeaveRoom 호출
    if (currentRoom)
    {
        currentRoom->LeaveRoom(currentPlayer);
    }

    // Step 4: Player에서 Lobby 접근
    weak_ptr<Lobby> weakLobby = currentPlayer->lobby.load();
    shared_ptr<Lobby> currentLobby = weakLobby.lock();

    // Step 5: Lobby에서 LeaveLobby 호출
    if (currentLobby)
	{
		currentLobby->LeaveLobby(currentPlayer);
	}

    // Step 6: GameSessionManager에서 Remove 호출
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{
}