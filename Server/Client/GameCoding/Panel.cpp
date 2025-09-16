#include "pch.h"
#include "Panel.h"

Panel::Panel()
{
}

Panel::~Panel()
{
}

void Panel::BeginPlay()
{
    Super::BeginPlay();
    for (auto& child : _children)
        child->BeginPlay();
}

void Panel::Tick()
{
    Super::Tick();
    for (auto& child : _children)
        child->Tick();
}

void Panel::Render(HDC hdc)
{
    Super::Render(hdc);
    for (auto& child : _children)
        child->Render(hdc);
}

void Panel::AddChild(unique_ptr<Actor> child)
{
    if (child)
        _children.emplace_back(move(child));
}
