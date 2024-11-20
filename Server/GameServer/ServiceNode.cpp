#include "pch.h"
#include "ServiceNode.h"
#include "Monster.h"
#include "Player.h"
#include "Room.h"

void ServiceNode::OnStart()
{
	lastUpdateTime = static_cast<float>(GetTickCount64()) / 1000.0f; // Initialize time in seconds
}

void ServiceNode::OnStop()
{
}

ENodeState ServiceNode::OnUpdate()
{
    LOG_INFO();
    float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;

    // Check if the update interval has passed
    if (currentTime - lastUpdateTime >= updateInterval)
    {
        lastUpdateTime = currentTime;
        OnUpdateService(); // Perform Service-specific logic
    }

    // Always return Running, as ServiceNode is not a terminal node
    return ENodeState::Running;
}

void DetectionService::OnStart()
{
}

void DetectionService::OnStop()
{
}

ENodeState DetectionService::OnUpdate()
{
    return ENodeState();
}

void DetectionService::OnUpdateService()
{
    LOG_INFO();
    auto ownerMonster = tree.lock()->owner.lock();
    if (!ownerMonster)
		return;

    auto target = ownerMonster->TargetPlayer.load();
    if (!target)
    {
        auto currentRoom = ownerMonster->room.load().lock();
        if (!currentRoom)
			return;

        auto players = currentRoom->_players;
        if (players.empty())
        {
            ownerMonster->TargetPlayer.store(nullptr);
			return;
        }
        else
        {
            float minDistance = FLT_MAX;
            PlayerRef closestPlayer = nullptr;

            for (auto player : players)
            {
                float distance = ownerMonster->DistanceTo(player.second->GetPosInfo());
                if (distance < detectRange && distance < minDistance)
                {
                    minDistance = distance;
                    closestPlayer = player.second;
                }
            }

            ownerMonster->TargetPlayer.store(closestPlayer);
        }
    }
}
