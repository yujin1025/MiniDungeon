#include "pch.h"
#include "Monster.h"
#include "Player.h"
#include "BehaviourTree.h"
#include "CompositeNode.h"
#include "DecoratorNode.h"
#include "ServiceNode.h"
#include "ActionNode.h"

Monster::Monster()
{
	monsterInfo = new Protocol::MonsterInfo();
    Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
    objectInfo->set_creature_type(Protocol::CreatureType::CREATURE_TYPE_MONSTER);
    obj_info->CopyFrom(*objectInfo);

    monsterInfo->set_allocated_object_info(obj_info);
    monsterInfo->set_monster_hp(MaxHp);

    blackboard = make_shared<MonsterBlackboard>();
    behaviourTree = make_shared<BehaviourTree>();

    // 루트 노드 생성
    auto root = make_shared<RootNode>();

    // 자식 노드 생성
    auto selector = make_shared<SelectorNode>();
}

Monster::~Monster()
{
	delete monsterInfo;
    monsterInfo = nullptr;
}

void Monster::CalcDist()
{
    Protocol::STC_MONSTERINFO monsterInfoPkt;
    Protocol::STC_STANDARD_MONSTER standardMonsterPkt;

    auto _room = room.load().lock(); // Room 객체 가져오기

    auto targetPlayer = TargetPlayer.load();
    if (targetPlayer)
    {
        // 플레이어 감지
        for (auto& pair : _room->_objects)
        {
            if (auto player = dynamic_pointer_cast<Player>(pair.second))
            {
                // 보스가 플레이어와의 거리를 계산하여 타겟 플레이어를 설정
                float distance = DistanceTo(player->GetPosInfo());
                if (distance < BossSight)
                {
                    TargetPlayer.store(player);
                    IsFindPlayer = true;
                    monsterInfo->set_isfindplayer(IsFindPlayer);

                    standardMonsterPkt.set_object_id(player->GetObjectInfo().object_id());
                    standardMonsterPkt.set_isstandard(true);

                    // 어그로 플레이어에게 보스 정보 전송
                    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(standardMonsterPkt);
                    player->session.lock()->Send(sendBuffer);
                }
            }
        }
    }
    else
    {
        if (_room->_objects.find(targetPlayer->GetObjectInfo().object_id()) == _room->_objects.end())
        {
            TargetPlayer.store(nullptr);
            monsterInfo->set_isfindplayer(false);
            monsterInfo->set_targetplayer_id(-1);
            monsterInfo->set_calcdist(0.f);

            Protocol::MonsterInfo* monsterInfo = new Protocol::MonsterInfo();
            monsterInfo->CopyFrom(GetMonsterInfo());

            monsterInfoPkt.set_allocated_info(monsterInfo);
            SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(monsterInfoPkt);
            _room->Broadcast(sendBuffer);
            //monsterInfoPkt.release_info();

            return;
        }
        
        float distance = DistanceTo(TargetPlayer.load()->GetPosInfo());
        CanAttack();

        monsterInfo->set_targetplayer_id(TargetPlayer.load()->GetObjectInfo().object_id());
        monsterInfo->set_monster_hp(CurrentHp);
        monsterInfo->set_calcdist(distance);

        monsterInfoPkt.set_allocated_info(monsterInfo);
        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(monsterInfoPkt);
        _room->Broadcast(sendBuffer);
        //monsterInfoPkt.release_info();
    }
}

void Monster::CanAttack()
{
}

float Monster::DistanceTo(const Protocol::PosInfo& targetPos)
{
    // 보스 위치 (posInfo)와 타겟 플레이어 위치 (targetPos) 간의 거리 계산
    float dx = GetPosInfo().x() - targetPos.x();
    float dy = GetPosInfo().y() - targetPos.y();

    return sqrt(dx * dx + dy * dy);
}

void Monster::SetObjectInfo(const Protocol::ObjectInfo& obj_Info)
{
	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	obj_info->CopyFrom(obj_Info);

	monsterInfo->set_allocated_object_info(obj_info);
    objectInfo->CopyFrom(monsterInfo->object_info());
}

void Monster::SetPosInfo(const Protocol::PosInfo& pos_Info)
{
    Protocol::PosInfo* pos_info = new Protocol::PosInfo();
	pos_info->CopyFrom(pos_Info);

	Protocol::ObjectInfo* obj_info = new Protocol::ObjectInfo();
	obj_info->CopyFrom(monsterInfo->object_info());
	obj_info->set_allocated_pos_info(pos_info);

	monsterInfo->set_allocated_object_info(obj_info);
    objectInfo->CopyFrom(monsterInfo->object_info());
}
