#pragma once

class Actor;
class Creature;
class UI;

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Init() abstract;
	virtual void Update() abstract;
	virtual void Render(HDC hdc) abstract;

	virtual void AddActor(shared_ptr<Actor> actor);
	virtual void RemoveActor(shared_ptr<Actor> actor);

	Creature* GetCreatureAt(Vec2Int cellPos);

public:
	vector<shared_ptr<Actor>> _actors[LAYER_MAXCOUNT];
	vector<UI*> _uis;
};

