#pragma once

class Actor;
class Creature;

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Init() abstract;
	virtual void Update() abstract;
	virtual void Render(HDC hdc) abstract;

	virtual void AddActor(std::shared_ptr<Actor> actor);
	virtual void RemoveActor(std::shared_ptr<Actor> actor);

	Creature* GetCreatureAt(Vec2Int cellPos);

public:
	std::vector<std::shared_ptr<Actor>> _actors[LAYER_MAXCOUNT];
	std::vector<std::shared_ptr<Actor>> _pendingRemoveActors;
};

