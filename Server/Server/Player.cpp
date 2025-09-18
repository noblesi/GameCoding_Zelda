#include "pch.h"
#include "Player.h"
#include "GameRoom.h"

Player::Player()
{

}

Player::~Player()
{

}

void Player::ProcessSkill()
{
    if (room == nullptr)
        return;

    Vec2Int targetCell = GetFrontCellPos();
    GameObjectRef target = room->GetGameObjectAt(targetCell);
    if (target == nullptr)
        return;

    if (target->info.objecttype() != Protocol::OBJECT_TYPE_MONSTER)
        return;

    if (target->room.get() != room.get())
        return;

    GameObjectRef self = room->FindObject(info.objectid());
    if (self == nullptr)
        return;

    room->ApplyDamage(self, target);
}
