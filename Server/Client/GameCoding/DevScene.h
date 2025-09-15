#pragma once
#include "Scene.h"
#include <type_traits>

class Actor;
class Creature; 
class Player;
class GameObject;

struct PQNode
{
	PQNode(int32 cost, Vec2Int pos) : cost(cost), pos(pos) { }

	bool operator<(const PQNode& other) const { return cost < other.cost; }
	bool operator>(const PQNode& other) const { return cost > other.cost; }

	int32 cost;
	Vec2Int pos;
};

class DevScene : public Scene
{
	using Super = Scene;
public:
	DevScene();
	virtual ~DevScene() override;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;

	virtual void AddActor(std::shared_ptr<Actor> actor) override;
	virtual void RemoveActor(std::shared_ptr<Actor> actor) override;

	void LoadMap();
	void LoadPlayer();
	void LoadMonster(); 
	void LoadProjectiles();
	void LoadEffect();
	void LoadTilemap();	

	template<typename T>
	std::shared_ptr<T> SpawnObject(Vec2Int pos)
	{
		auto isGameObject = std::is_convertible_v<T*, GameObject*>;
		assert(isGameObject);

		std::shared_ptr<T> ret = std::make_shared<T>();
		ret->SetCellPos(pos, true);
		AddActor(ret);

		ret->BeginPlay();

		_ownedActors.push_back(ret);

		return ret;
	} 

	template<typename T>
	std::shared_ptr<T> SpawnObjectAtRandomPos()
	{
		Vec2Int randPos = GetRandomEmptyCellPos();
		return SpawnObject<T>(randPos);
	}

public:
	void Handle_S_AddObject(Protocol::S_AddObject& pkt);
	void Handle_S_RemoveObject(Protocol::S_RemoveObject& pkt);

public:
	std::shared_ptr<GameObject> GetObject(uint64 id);

	Player* FindClosestPlayer(Vec2Int pos);
	bool FindPath(Vec2Int src, Vec2Int dest, std::vector<Vec2Int>& path, int32 maxDepth = 10);
	bool CanGo(Vec2Int cellPos);
	Vec2 ConvertPos(Vec2Int cellPos);
	Vec2Int GetRandomEmptyCellPos();

private:
	void TickMonsterSpawn();

	const int32 DESIRED_COUNT = 1;
	std::shared_ptr<class TilemapActor> _tilemapActor;
	int32 _monsterCount = 0;
	std::vector<std::shared_ptr<Actor>> _ownedActors;
};

