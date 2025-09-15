#include "pch.h"
#include "Scene.h"
#include "Actor.h"
#include "Creature.h"
#include "TimeManager.h"
#include "SceneManager.h"

Scene::Scene()
{

}

Scene::~Scene()
{
	for (auto& layer : _actors)
		layer.clear();
}

void Scene::Init()
{
	for (const auto& actors : _actors)
		for (const std::shared_ptr<Actor>& actor : actors)
			actor->BeginPlay();
}

void Scene::Update()
{
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	for (int layer = 0; layer < LAYER_MAXCOUNT; ++layer)
	{
		auto actors = _actors[layer];
		for (const auto& actor : actors)
			actor->Tick();
	}

	if (_pendingRemoveActors.empty() == false)
	{
		for (const auto& actor : _pendingRemoveActors)
		{
			std::vector<std::shared_ptr<Actor>>& v = _actors[actor->GetLayer()];
			v.erase(std::remove(v.begin(), v.end(), actor), v.end());
		}
		_pendingRemoveActors.clear();
	}
}

void Scene::Render(HDC hdc)
{
	std::vector<std::shared_ptr<Actor>>& actors = _actors[LAYER_OBJECT];
	std::sort(actors.begin(), actors.end(), [=](const std::shared_ptr<Actor>& a, const std::shared_ptr<Actor>& b)
	{
		return a->GetPos().y < b->GetPos().y;
	});

	for (const auto& actors : _actors)
		for (const std::shared_ptr<Actor> actor : actors)
			actor->Render(hdc);
}

void Scene::AddActor(std::shared_ptr<Actor> actor)
{
	if (actor == nullptr)
		return;

	_actors[actor->GetLayer()].push_back(actor);
}

void Scene::RemoveActor(std::shared_ptr<Actor> actor)
{
	if (actor == nullptr)
		return;

	_pendingRemoveActors.push_back(actor);
}

Creature* Scene::GetCreatureAt(Vec2Int cellPos)
{
	for (const std::shared_ptr<Actor> actor : _actors[LAYER_OBJECT])
	{
		// GameObjectType
		Creature* creature = dynamic_cast<Creature*>(actor.get());
		if (creature && creature->GetCellPos() == cellPos)
			return creature;
	}

	return nullptr;
}
