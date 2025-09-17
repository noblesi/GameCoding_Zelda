#include "pch.h"
#include "Player.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "ResourceManager.h"
#include "Flipbook.h"
#include "CameraComponent.h"
#include "SceneManager.h"
#include "DevScene.h"
#include "Arrow.h"
#include "Staff.h"
#include "HitEffect.h"

namespace
{
	using WeaponStatArray = std::array<Stat, 3>;

	const WeaponStatArray kWeaponBaseStats =
	{
	Stat{ 100, 100, 15, 5, 200.f },
	Stat{ 100, 100, 12, 3, 220.f },
	Stat{ 100, 100, 18, 2, 180.f }
	};

	size_t ToIndex(WeaponType type)
	{
		return static_cast<size_t>(type);
	}

	const Stat& GetWeaponBaseStat(WeaponType type)
	{
		return kWeaponBaseStats[ToIndex(type)];
	}

	WeaponType ResolveWeaponType(int32 attack, WeaponType current)
	{
		for (size_t i = 0; i < kWeaponBaseStats.size(); ++i)
		{
			if (kWeaponBaseStats[i].attack == attack)
				return static_cast<WeaponType>(i);
		}

		return current;
	}

	bool SyncInfoWithStat(Protocol::ObjectInfo& info, const Stat& stat)
	{
		bool changed = false;

		if (info.hp() != stat.hp)
		{
			info.set_hp(stat.hp);
			changed = true;
		}
		if (info.maxhp() != stat.maxHp)
		{
			info.set_maxhp(stat.maxHp);
			changed = true;
		}
		if (info.attack() != stat.attack)
		{
			info.set_attack(stat.attack);
			changed = true;
		}
		if (info.defence() != stat.defence)
		{
			info.set_defence(stat.defence);
			changed = true;
		}

		return changed;
	}
}


Player::Player()
{
	_flipbookIdle[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_IdleUp");
	_flipbookIdle[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_IdleDown");
	_flipbookIdle[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_IdleLeft");
	_flipbookIdle[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_IdleRight");
	
	_flipbookMove[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_MoveUp");
	_flipbookMove[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_MoveDown");
	_flipbookMove[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_MoveLeft");
	_flipbookMove[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_MoveRight");

	_flipbookAttack[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_AttackUp");
	_flipbookAttack[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_AttackDown");
	_flipbookAttack[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_AttackLeft");
	_flipbookAttack[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_AttackRight");

	_flipbookBow[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_BowUp");
	_flipbookBow[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_BowDown");
	_flipbookBow[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_BowLeft");
	_flipbookBow[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_BowRight");

	_flipbookStaff[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_StaffUp");
	_flipbookStaff[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_StaffDown");
	_flipbookStaff[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_StaffLeft");
	_flipbookStaff[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_StaffRight");

	_stat = GetWeaponBaseStat(_weaponType);
	SyncInfoWithStat(info, _stat);
}

Player::~Player()
{

}

void Player::BeginPlay()
{
	Super::BeginPlay();

	SetState(MOVE);
	SetState(IDLE);
}

void Player::Tick()
{
	Super::Tick();
}

void Player::Render(HDC hdc)
{
	Super::Render(hdc);

}

void Player::TickIdle()
{
	
}

void Player::TickMove()
{
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();
	float moveSpeed = (_stat.speed < 0.f) ? 0.f : _stat.speed;

	Vec2 dir = (_destPos - _pos);
	if (dir.Length() < 1.f)
	{
		SetState(IDLE);
		_pos = _destPos;
	}
	else
	{
		switch (info.dir())
		{
		case DIR_UP:
			_pos.y -= moveSpeed * deltaTime;
			break;
		case DIR_DOWN:
			_pos.y += moveSpeed * deltaTime;
			break;
		case DIR_LEFT:
			_pos.x -= moveSpeed * deltaTime;
			break;
		case DIR_RIGHT:
			_pos.x += moveSpeed * deltaTime;
			break;
		}
	}
}

void Player::TickSkill()
{
	if (_flipbook == nullptr)
		return;

	// TODO : Damage?
	if (IsAnimationEnded())
	{
		DevScene* scene = dynamic_cast<DevScene*>(GET_SINGLE(SceneManager)->GetCurrentScene());
		if (scene == nullptr)
			return;

		if (_weaponType == WeaponType::Sword)
		{
			Creature* creature = scene->GetCreatureAt(GetFrontCellPos());
			if (creature)
			{
				scene->SpawnObject<HitEffect>(GetFrontCellPos());
				creature->OnDamaged(this);
			}
		}
		else if (_weaponType == WeaponType::Bow)
		{
			Arrow* arrow = scene->SpawnObject<Arrow>(GetCellPos());
			arrow->SetDir(info.dir());	
		}
		else if (_weaponType == WeaponType::Staff)
		{
			Staff* staff = scene->SpawnObject<Staff>(GetCellPos());
			staff->SetDir(info.dir());
		}

		SetState(IDLE);
	}
}

void Player::UpdateAnimation()
{
	switch (info.state())
	{
	case IDLE:
		SetFlipbook(_flipbookIdle[info.dir()]);
		break;
	case MOVE:
		SetFlipbook(_flipbookMove[info.dir()]);
		break;
	case SKILL:
		if (_weaponType == WeaponType::Sword)
			SetFlipbook(_flipbookAttack[info.dir()]);
		else if (_weaponType == WeaponType::Bow)
			SetFlipbook(_flipbookBow[info.dir()]);
		else
			SetFlipbook(_flipbookStaff[info.dir()]);
		break;
	}
}

void Player::SetWeaponType(WeaponType weaponType)
{
	const Stat& baseStat = GetWeaponBaseStat(weaponType);

	Stat nextStat = baseStat;
	nextStat.hp = _stat.hp;
	nextStat.maxHp = _stat.maxHp;

	bool dirty = (_weaponType != weaponType) ||
		(_stat.attack != nextStat.attack) ||
		(_stat.defence != nextStat.defence) ||
		(_stat.speed != nextStat.speed);

	_weaponType = weaponType;
	_stat = nextStat;
	if (SyncInfoWithStat(info, _stat))
		dirty = true;

	if (dirty)
		_dirtyFlag = true;
}

void Player::RefreshStatFromInfo()
{
	WeaponType resolvedType = ResolveWeaponType(info.attack(), _weaponType);
	const Stat& baseStat = GetWeaponBaseStat(resolvedType);

	Stat stat = baseStat;

	int32 maxHp = info.maxhp();
	if (maxHp <= 0)
		maxHp = baseStat.maxHp;
	stat.maxHp = maxHp;

	int32 hp = info.hp();
	if (hp < 0)
		hp = 0;
	if (hp == 0 && info.maxhp() == 0)
		hp = stat.maxHp;
	if (hp > stat.maxHp)
		hp = stat.maxHp;
	stat.hp = hp;

	int32 attack = info.attack();
	if (attack == 0)
		attack = baseStat.attack;
	stat.attack = attack;

	int32 defence = info.defence();
	if (defence == 0)
		defence = baseStat.defence;
	stat.defence = defence;

	stat.speed = baseStat.speed;

	_weaponType = resolvedType;
	_stat = stat;

	SyncInfoWithStat(info, _stat);
}