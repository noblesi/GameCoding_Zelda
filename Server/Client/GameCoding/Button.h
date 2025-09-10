#pragma once

#include "Actor.h"

class Sprite;

enum ButtonState
{
    BS_Default,
    BS_Pressed,
    BS_Clicked,
    BS_MaxCount
};

class Button : public Actor
{
    using Super = Actor;
public:
    Button();
    virtual ~Button() override;

    virtual void BeginPlay() override;
    virtual void Tick() override;
    virtual void Render(HDC hdc) override;

    void    SetSize(Vec2Int size) { _size = size; }
    Sprite* GetSprite(ButtonState state) { return _sprites[state]; }

    void SetCurrentSprite(Sprite* sprite) { _currentSprite = sprite; }
    void SetSprite(Sprite* sprite, ButtonState state) { _sprites[state] = sprite; }
    void SetButtonState(ButtonState state);

    template<typename T>
    void AddOnClickDelegate(T* owner, void(T::* func)())
    {
        _onClick = [owner, func]()
            {
                (owner->*func)();
            };
    }

private:
    RECT GetRect() const;
    bool IsMouseInRect() const;

protected:
    Sprite* _currentSprite = nullptr;
    Sprite* _sprites[BS_MaxCount] = {};
    ButtonState _state = BS_Default;
    float _sumTime = 0.f;
    Vec2Int _size = { 150, 150 };

public:
    std::function<void(void)> _onClick = nullptr;
};

