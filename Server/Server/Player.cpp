#include "pch.h"
#include "Player.h"
#include "GameRoom.h"

Player::Player()
{

}

Player::~Player()
{

}

void Player::Update()
{
    switch (info.state())
    {
    case IDLE:
        UpdateIdle();
        break;
    case MOVE:
        UpdateMove();
        break;
    case SKILL:
        UpdateSkill();
        break;
    }
}

void Player::UpdateIdle()
{

}

void Player::UpdateMove()
{
    SetState(IDLE);
}

void Player::UpdateSkill()
{
    uint64 now = GetTickCount64();

    if (_waitUntil == 0)
    {
        _waitUntil = now + 500;
        std::cout << "Player " << info.objectid() << " used skill" << std::endl;

        if (room)
        {
            GameObjectRef target = room->GetGameObjectAt(GetFrontCellPos());
            if (target)
            {
                int32 damage = max(0, info.attack() - target->info.defence());
                if (damage > 0)
                {
                    int32 hp = max(0, target->info.hp() - damage);
                    target->info.set_hp(hp);
                    std::cout << "Player " << info.objectid() << " dealt" << damage << " damage to object " << target->info.objectid() << std::endl;
                    if (target->room)
                    {
                        SendBufferRef sendBuffer = ServerPacketHandler::Make_S_Move(target->info);
                        target->room->Broadcast(sendBuffer);
                    }
                }
            }
        }
        return;
    }

    if (now < _waitUntil)
        return;

    _waitUntil = 0;

    SetState(IDLE, true);
}

void Player::OnDamaged(int32 damage)
{
    damage = max(0, damage - info.defence());
    if (damage <= 0)
        return;

    int32 hp = max(0, info.hp() - damage);
    info.set_hp(hp);
    std::cout << "Player " << info.objectid() << " took " << damage << " damage. HP: " << hp << std::endl;
    if (room)
    {
        SendBufferRef sendBuffer = ServerPacketHandler::Make_S_Move(info);
        room->Broadcast(sendBuffer);
    }
}