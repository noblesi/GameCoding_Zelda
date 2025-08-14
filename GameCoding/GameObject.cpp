#include "pch.h"
#include "GameObject.h"
#include "Component.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::Start()
{
    for (Component* component : _components)
    {
        component->Start();
    }
}

void GameObject::Update()
{
    for (Component* component : _components)
    {
        component->Update();
    }
}

void GameObject::Render(HDC hdc)
{
    for (Component* component : _components)
    {
        component->Render(hdc);
    }
}

void GameObject::AddComponent(Component* component)
{
    _components.push_back(component);
}