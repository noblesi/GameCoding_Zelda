#pragma once
#include "Projectile.h"

class Staff : public Projectile
{
    using Super = Projectile;

public:
    Staff();
    virtual ~Staff() override;

    virtual void BeginPlay() override;
    virtual void Tick() override;
    virtual void Render(HDC hdc) override;

    virtual void TickIdle() override;
    virtual void TickMove() override;

    virtual void UpdateAnimation() override;

protected:
    Flipbook* _flipbookMove[4] = {};
};

