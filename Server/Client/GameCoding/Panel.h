#pragma once

#include "Actor.h"

class Panel : public Actor
{
    using Super = Actor;
public:
    Panel();
    virtual ~Panel() override;

    virtual void BeginPlay() override;
    virtual void Tick() override;
    virtual void Render(HDC hdc) override;

    void AddChild(Actor* child);

protected:
    vector<unique_ptr<Actor>> _children;
};

