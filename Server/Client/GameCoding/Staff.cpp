#include "pch.h"
#include "Staff.h"
#include "TimeManager.h"
#include "ResourceManager.h"
#include "Flipbook.h"
#include "SceneManager.h"
#include "DevScene.h"
#include "Creature.h"
#include "HitEffect.h"

Staff::Staff()
{
	_flipbookMove[DIR_UP] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_StaffProjectileUp");
	_flipbookMove[DIR_DOWN] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_StaffProjectileDown");
	_flipbookMove[DIR_LEFT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_StaffProjectileLeft");
	_flipbookMove[DIR_RIGHT] = GET_SINGLE(ResourceManager)->GetFlipbook(L"FB_StaffProjectileRight");
}

Staff::~Staff()
{
}

void Staff::BeginPlay()
{
	Super::BeginPlay();
	UpdateAnimation();
}

void Staff::Tick()
{
	Super::Tick();
}

void Staff::Render(HDC hdc)
{
	Super::Render(hdc);
}

void Staff::TickIdle()
{
	DevScene* scene = dynamic_cast<DevScene*>(GET_SINGLE(SceneManager)->GetCurrentScene());
	if (scene == nullptr)
		return;

	Vec2Int deltaXY[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
	Vec2Int nextPos = GetCellPos() + deltaXY[info.dir()];

	if (CanGo(nextPos))
	{
		SetCellPos(nextPos);
		SetState(MOVE);
	}
	else
	{
		Creature* creature = scene->GetCreatureAt(nextPos);
		if (creature)
		{
			scene->SpawnObject<HitEffect>(nextPos);
			//creature->OnDamaged(this);
		}

		scene->RemoveActor(this);
	}
}

void Staff::TickMove()
{
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	Vec2 dir = (_destPos - _pos);
	if (dir.Length() < 5.f)
	{
		SetState(IDLE);
		_pos = _destPos;
	}
	else
	{
		switch (info.dir())
		{
		case DIR_UP:
			_pos.y -= 600 * deltaTime;
			break;
		case DIR_DOWN:
			_pos.y += 600 * deltaTime;
			break;
		case DIR_LEFT:
			_pos.x -= 600 * deltaTime;
			break;
		case DIR_RIGHT:
			_pos.x += 600 * deltaTime;
			break;
		}
	}
}

void Staff::UpdateAnimation()
{
	SetFlipbook(_flipbookMove[info.dir()]);
}
