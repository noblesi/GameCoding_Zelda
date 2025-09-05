#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player();
	virtual ~Player();

public:
	GameSessionRef session;
	int32 invalidMoveCount = 0; // 연속하여 잘못 이동한 횟수
};