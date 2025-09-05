#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
        Player();
        virtual ~Player();

public:
        GameSessionRef session;
        int32 invalidMoveCount = 0;
};

