#pragma once

#include "GameObject.h"

class Flipbook;
class Collider;
class BoxCollider;

struct Stat
{
	int32 hp = 100;
	int32 maxHp = 100;
	int32 attack = 10;
	int32 defence = 0;
	float speed = 0;
};

class Creature : public GameObject
{
	using Super = GameObject;
public:
	Creature();
	virtual ~Creature() override;

	virtual void BeginPlay() override;
	virtual void Tick() override;
	virtual void Render(HDC hdc) override;

	virtual void TickIdle() override {}
	virtual void TickMove() override {}
	virtual void TickSkill() override {}
	virtual void UpdateAnimation() override {}

	virtual void OnDamaged(Creature* attacker);

	bool IsDead() { return _stat.hp <= 0; }

	void SetStat(Stat stat) { _stat = stat;}
	Stat& GetStat() { return _stat;}
	bool HasPendingHp() const { return _hasPendingHp; }
	int32 GetPendingHp() const { return _pendingHp; }
	bool IsPendingHpHoldActive() const { return _hasPendingHp && _pendingHpHoldTime > 0.f; }
	bool HasPendingHpTimedOut() const { return _hasPendingHp && _pendingHpHoldElapsed >= _pendingHpHoldTimeout; }
	void ClearPendingHp();
	void RefreshPendingHpHold();

protected:
	Stat _stat;
	bool _hasPendingHp = false;
	int32 _pendingHp = 0;
	float _pendingHpHoldTime = 0.f;
	float _pendingHpHoldElapsed = 0.f;
	float _pendingHpHoldTimeout = 0.f;

private:
	void StartPendingHp(int32 hp);
};

