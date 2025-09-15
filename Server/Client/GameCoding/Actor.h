#pragma once

class Component;
class Collider;

class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor();
	virtual ~Actor();

	virtual void BeginPlay();
	virtual void Tick();
	virtual void Render(HDC hdc);

	void SetPos(Vec2 pos) { _pos = pos; }
	Vec2 GetPos() { return _pos; }

	void SetLayer(LAYER_TYPE layer) { _layer = layer; }
	LAYER_TYPE GetLayer() { return _layer; }

	void AddComponent(std::shared_ptr<Component> component);
	void RemoveComponent(std::shared_ptr<Component> component);

	// OnCollisionEnter2D / OnCollisionExit2D
	virtual void OnComponentBeginOverlap(Collider* collider, Collider* other);
	virtual void OnComponentEndOverlap(Collider* collider, Collider* other);

protected:
	Vec2 _pos = {0, 0};
	Vec2 _destPos = {0, 0};
	LAYER_TYPE _layer = LAYER_OBJECT;
	std::vector<std::shared_ptr<Component>> _components;
};

