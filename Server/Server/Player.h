#pragma once
#include "GameObject.h"

class Player : public GameObject
{
	using Super = GameObject;
public:
	Player();
	virtual ~Player();

	virtual void Update() override;
	void OnDamaged(int32 damage);

private:
	void UpdateIdle();
	void UpdateMove();
	void UpdateSkill();

public:
	GameSessionRef session;
	int32 invalidMoveCount = 0; // 연속하여 잘못 이동한 횟수

private:
	uint64 _waitUntil = 0;
};