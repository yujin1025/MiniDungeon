#include "pch.h"
#include "ServiceNode.h"
#include "Monster.h"
#include "Player.h"
#include "Room.h"

ServiceNode::~ServiceNode()
{
    child.reset();
}

void ServiceNode::OnStart()
{
	lastUpdateTime = static_cast<float>(GetTickCount64()) / 1000.0f; // Initialize time in seconds
}

void ServiceNode::OnStop()
{
}

ENodeState ServiceNode::OnUpdate()
{
    float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;

    // Check if the update interval has passed
    if (currentTime - lastUpdateTime >= updateInterval)
    {
        lastUpdateTime = currentTime;
        OnUpdateService(); // Perform Service-specific logic
    }

    if(child != nullptr)
        child->Update(); // Update child node

    // Always return Running, as ServiceNode is not a terminal node
    return ENodeState::Running;
}

void DetectionService::OnUpdateService()
{

    auto bt = tree.lock();
    if (bt == nullptr)
    {
        LOG("DetectionService : BehaviourTree is nullptr");
		return;
    }

    auto ownerMonster = bt->owner.lock();
    if (ownerMonster == nullptr)
    {
        LOG("DetectionService : ownerMonster is nullptr");
        return;
    }


    auto currentRoom = ownerMonster->room.load().lock();
    if (currentRoom == nullptr)
    {
        LOG("DetectionService : currentRoom is nullptr");
        return;
    }

    auto players = currentRoom->_players;
    if (players.empty())
    {
        LOG("DetectionService : No players in the room");
        ownerMonster->TargetPlayer.store(nullptr);
        blackboard->SetData(EBlackboardKey::Target, 0);
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

        shared_ptr<Player> expected = ownerMonster->TargetPlayer.load();
        uint64 target_id = 0;
        if (ownerMonster->TargetPlayer.compare_exchange_strong(expected, closestPlayer))
        {
            if (closestPlayer != nullptr)
            {
                target_id = closestPlayer->GetObjectInfo().object_id();
            }

            blackboard->SetData(EBlackboardKey::Target, target_id);
            LOG("DetectionService : id %d", target_id);
        }
        else
        {
            if(expected != nullptr)
				target_id = expected->GetObjectInfo().object_id();

            blackboard->SetData(EBlackboardKey::Target, target_id);
            LOG("DetectionService : id %d", target_id);
        }
    }

}
