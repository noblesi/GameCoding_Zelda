#include "pch.h"
#include "GameRoom.h"
#include "Player.h"
#include "Monster.h"
#include "GameSession.h"
#include <filesystem>

GameRoomRef GRoom = make_shared<GameRoom>();

GameRoom::GameRoom()
{
	
}

GameRoom::~GameRoom()
{

}

void GameRoom::Init()
{
	MonsterRef monster = GameObject::CreateMonster();
	monster->info.set_posx(8);
	monster->info.set_posy(8);
	AddObject(monster);

	std::filesystem::path tilemapPath = std::filesystem::current_path() / "Resources" / "Tilemap" / "Tilemap_01.txt";
	_tilemap.LoadFile(tilemapPath.wstring());
}

void GameRoom::Update()
{
	ProcessMoveRequests();

	vector<uint64> removeIds;

	for (auto& item : _players)
	{
		item.second->Update();

		if (item.second->info.hp() <= 0)
			removeIds.push_back(item.first);
	}

	for (auto& item : _monsters)
	{
		item.second->Update();

		if (item.second->info.hp() <= 0)
			removeIds.push_back(item.first);
	}

	for (uint64 id : removeIds)
		RemoveObject(id);
}

void GameRoom::EnterRoom(GameSessionRef session)
{
	PlayerRef player = GameObject::CreatePlayer();

	// 서로의 존재를 연결
	session->gameRoom = GetRoomRef();
	session->player = player;
	player->session = session;

	// TEMP
	player->info.set_posx(5);
	player->info.set_posy(5);

	// 입장한 클라에게 정보를 보내주기
	{
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_MyPlayer(player->info);
		session->Send(sendBuffer);
	}

	AddObject(player);

	// 모든 오브젝트 정보 전송
	{
		Protocol::S_AddObject pkt;

		for (auto& item : _players)
		{
			if (item.first == player->info.objectid())
				continue;
			Protocol::ObjectInfo* info = pkt.add_objects();
			*info = item.second->info;
		}

		for (auto& item : _monsters)
		{
			Protocol::ObjectInfo* info = pkt.add_objects();
			*info = item.second->info;
		}

		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_AddObject(pkt);
		session->Send(sendBuffer);
	}

	
}

void GameRoom::LeaveRoom(GameSessionRef session)
{
	if (session == nullptr)
		return;
	if (session->player.lock() == nullptr)
		return;

	uint64 id = session->player.lock()->info.objectid();
	RemoveObject(id);
}

GameObjectRef GameRoom::FindObject(uint64 id)
{
	{
		auto findIt = _players.find(id);
		if (findIt != _players.end())
			return findIt->second;
	}
	{
		auto findIt = _monsters.find(id);
		if (findIt != _monsters.end())
			return findIt->second;
	}

	return nullptr;
}

void GameRoom::Handle_C_Move(Protocol::C_Move& pkt)
{
	uint64 id = pkt.info().objectid();
	GameObjectRef gameObject = FindObject(id);
	if (gameObject == nullptr)
		return;

	PlayerRef player = dynamic_pointer_cast<Player>(gameObject);
	if (player == nullptr)
		return;

	if (pkt.seq() <= player->lastMoveSeq)
		return;
	player->lastMoveSeq = pkt.seq();

	Vec2Int curPos = gameObject->GetCellPos();
	Vec2Int targetPos{ pkt.info().posx(), pkt.info().posy() };
	Vec2Int delta = targetPos - curPos; // 목표 지점과 현재 위치의 차이 계산

	auto sendCorrection = [&]()
		{
			// 서버기준 위치로 수정 패킷 전송
			SendBufferRef sendBuffer = ServerPacketHandler::Make_S_Move(gameObject->info, player->lastMoveSeq);

			if (player->session)
				player->session->Send(sendBuffer);
		};

	bool invalid = false; // 검증 실패 여부

	if (delta.LengthSquared() > 1)
	{
		// delta 방향을 따라 한 칸씩 이동 가능 여부 확인
		if (delta.x != 0 && delta.y != 0)
		{
			// 대각선 이동은 허용하지 않음
			invalid = true;
		}
		else
		{
			Vec2Int stepDir
			{
					(delta.x > 0) ? 1 : (delta.x < 0 ? -1 : 0),
					(delta.y > 0) ? 1 : (delta.y < 0 ? -1 : 0)
			};

			int32 steps = abs(delta.x) + abs(delta.y);
			Vec2Int nextPos = curPos;
			for (int32 i = 0; i < steps; ++i)
			{
				nextPos += stepDir;
				if (CanGo(nextPos) == false)
				{
					invalid = true;
					break;
				}
			}

			if (invalid == false)
			{
				targetPos = nextPos;

				Dir expectedDir = DIR_UP;
				if (stepDir.x == 1 && stepDir.y == 0) expectedDir = DIR_RIGHT;
				else if (stepDir.x == -1 && stepDir.y == 0) expectedDir = DIR_LEFT;
				else if (stepDir.x == 0 && stepDir.y == 1) expectedDir = DIR_DOWN;
				else if (stepDir.x == 0 && stepDir.y == -1) expectedDir = DIR_UP;

				if (pkt.info().dir() != expectedDir)
					invalid = true;
			}
		}
	}

	if (pkt.info().state() == MOVE && delta.LengthSquared() == 0)
		invalid = true; // 이동상태인데 좌표변화가 없으면 무효

	if (delta.LengthSquared() == 1)
	{
		// 이동방향이 맞는지 확인
		Dir expectedDir = DIR_UP;
		if (delta.x == 1 && delta.y == 0) expectedDir = DIR_RIGHT;
		else if (delta.x == -1 && delta.y == 0) expectedDir = DIR_LEFT;
		else if (delta.x == 0 && delta.y == 1) expectedDir = DIR_DOWN;
		else if (delta.x == 0 && delta.y == -1) expectedDir = DIR_UP;

		if (pkt.info().dir() != expectedDir)
			invalid = true; // 방향 불일치
	}

	if (delta.LengthSquared() != 0 && CanGo(targetPos, id) == false)
		invalid = true; // 벽 또는 다른 오브젝트와 충돌

	if (invalid)
	{
		cout << "Invalid move packet from object" << id << endl; // 서버 로그 출력
		player->invalidMoveCount++;
		if (player->invalidMoveCount >= 5)
		{
			// 반복 위반시 세션 종료
			if (player->session)
				player->session->Disconnect(L"Invalid Move Packet");
		}
		else
		{
			sendCorrection(); // 위치 수정 패킷 전송
		}

		return;
	}

	_moveRequests.push_back({ player, id, targetPos, pkt.info().state(), pkt.info().dir() });
}

void GameRoom::AddObject(GameObjectRef gameObject)
{
	uint64 id = gameObject->info.objectid();

	auto objectType = gameObject->info.objecttype();

	switch (objectType)
	{
		case Protocol::OBJECT_TYPE_PLAYER:
			_players[id] = static_pointer_cast<Player>(gameObject);
			break;
		case Protocol::OBJECT_TYPE_MONSTER:
			_monsters[id] = static_pointer_cast<Monster>(gameObject);
			break;
		default:
			return;
	}

	gameObject->room = GetRoomRef();

	// 신규 오브젝트 정보 전송
	{
		Protocol::S_AddObject pkt;

		Protocol::ObjectInfo* info = pkt.add_objects();
		*info = gameObject->info;
	
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_AddObject(pkt);
		Broadcast(sendBuffer);
	}
}

void GameRoom::RemoveObject(uint64 id)
{
	GameObjectRef gameObject = FindObject(id);
	if (gameObject == nullptr)
		return;

	switch (gameObject->info.objecttype())
	{
	case Protocol::OBJECT_TYPE_PLAYER:
		_players.erase(id);
		break;
	case Protocol::OBJECT_TYPE_MONSTER:
		_monsters.erase(id);
		break;
	default:
		return;
	}

	gameObject->room = nullptr;

	// 오브젝트 삭제 전송
	{
		Protocol::S_RemoveObject pkt;
		pkt.add_ids(id);

		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_RemoveObject(pkt);
		Broadcast(sendBuffer);
	}
}

void GameRoom::Broadcast(SendBufferRef& sendBuffer)
{
	for (auto& item : _players)
	{
		item.second->session->Send(sendBuffer);
	}
}

PlayerRef GameRoom::FindClosestPlayer(Vec2Int pos)
{
	float best = FLT_MAX;
	PlayerRef ret = nullptr;

	for (auto& item : _players)
	{
		PlayerRef player = item.second;
		if (player)
		{
			Vec2Int dir = pos - player->GetCellPos();
			float dist = dir.LengthSquared();
			if (dist < best)
			{
				best = dist;
				ret = player;
			}
		}
	}

	return ret;
}

bool GameRoom::FindPath(Vec2Int src, Vec2Int dest, vector<Vec2Int>& path, int32 maxDepth /*= 10*/)
{
	int32 depth = abs(src.y - dest.y) + abs(src.x - dest.x);
	if (depth >= maxDepth)
		return false;

	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pq;
	map<Vec2Int, int32> best;
	map<Vec2Int, Vec2Int> parent;

	// 초기값
	{
		int32 cost = abs(dest.y - src.y) + abs(dest.x - src.x);

		pq.push(PQNode(cost, src));
		best[src] = cost;
		parent[src] = src;
	}

	Vec2Int front[4] =
	{
		{0, -1},
		{0, 1},
		{-1, 0},
		{1, 0},
	};

	bool found = false;

	while (pq.empty() == false)
	{
		// 제일 좋은 후보를 찾는다
		PQNode node = pq.top();
		pq.pop();

		// 더 짧은 경로를 뒤늦게 찾았다면 스킵
		if (best[node.pos] < node.cost)
			continue;

		// 목적지에 도착했으면 바로 종료
		if (node.pos == dest)
		{
			found = true;
			break;
		}

		// 방문
		for (int32 dir = 0; dir < 4; dir++)
		{
			Vec2Int nextPos = node.pos + front[dir];

			if (CanGo(nextPos) == false)
				continue;

			int32 depth = abs(src.y - nextPos.y) + abs(src.x - nextPos.x);
			if (depth >= maxDepth)
				continue;

			int32 cost = abs(dest.y - nextPos.y) + abs(dest.x - nextPos.x);
			int32 bestValue = best[nextPos];
			if (bestValue != 0)
			{
				// 다른 경로에서 더 빠른 길을 찾았으면 스킵
				if (bestValue <= cost)
					continue;
			}

			// 예약 진행
			best[nextPos] = cost;
			pq.push(PQNode(cost, nextPos));
			parent[nextPos] = node.pos;
		}
	}

	if (found == false)
	{
		float bestScore = FLT_MAX;

		for (auto& item : best)
		{
			Vec2Int pos = item.first;
			int32 score = item.second;

			// 동점이라면, 최초 위치에서 가장 덜 이동하는 쪽으로
			if (bestScore == score)
			{
				int32 dist1 = abs(dest.x - src.x) + abs(dest.y - src.y);
				int32 dist2 = abs(pos.x - src.x) + abs(pos.y - src.y);
				if (dist1 > dist2)
					dest = pos;
			}
			else if (bestScore > score)
			{
				dest = pos;
				bestScore = score;
			}
		}
	}

	path.clear();
	Vec2Int pos = dest;

	while (true)
	{
		path.push_back(pos);

		// 시작점
		if (pos == parent[pos])
			break;

		pos = parent[pos];
	}

	std::reverse(path.begin(), path.end());
	return true;
}

bool GameRoom::CanGo(Vec2Int cellPos, uint64 ignoreId)
{
	auto tile = _tilemap.GetTileAt(cellPos);
	if (tile.has_value() == false)
		return false;

	// 몬스터 충돌?
	if (GetGameObjectAt(cellPos, ignoreId) != nullptr)
		return false;

	return tile->get().value != 1;
}

Vec2Int GameRoom::GetRandomEmptyCellPos()
{
	Vec2Int ret = { -1, -1 };

	Vec2Int size = _tilemap.GetMapSize();

	// 몇 번 시도?
	while (true)
	{
		int32 x = rand() % size.x;
		int32 y = rand() % size.y;
		Vec2Int cellPos{ x, y };

		if (CanGo(cellPos))
			return cellPos;
	}
}

GameObjectRef GameRoom::GetGameObjectAt(Vec2Int cellPos, uint64 ignoreId)
{
	for (auto& item : _players)
	{
		if (item.first == ignoreId)
			continue;
		if (item.second->GetCellPos() == cellPos)
			return item.second;
	}

	for (auto& item : _monsters)
	{
		if (item.first == ignoreId)
			continue;
		if (item.second->GetCellPos() == cellPos)
			return item.second;
	}

	return nullptr;
}

void GameRoom::ProcessMoveRequests()
{
	if (_moveRequests.empty())
		return;

	set<uint64> movingIds;
	map<Vec2Int, int32> destCounts;

	for (auto& req : _moveRequests)
	{
		movingIds.insert(req.id);
		destCounts[req.targetPos]++;
	}

	vector<MoveRequest> valid;

	for (auto& req : _moveRequests)
	{
		PlayerRef player = req.player;
		if (player == nullptr)
			continue;

		bool invalid = false;

		auto tile = _tilemap.GetTileAt(req.targetPos);
		if (tile.has_value() == false || tile->get().value == 1)
			invalid = true;

		GameObjectRef occupant = GetGameObjectAt(req.targetPos);
		if (occupant)
		{
			uint64 occId = occupant->info.objectid();
			if (movingIds.find(occId) == movingIds.end())
				invalid = true;
		}

		if (destCounts[req.targetPos] > 1)
			invalid = true;

		if (invalid)
		{
			cout << "Invalid move packet from object" << req.id << endl;
			player->invalidMoveCount++;
			if (player->invalidMoveCount >= 5)
			{
				if (player->session)
					player->session->Disconnect(L"Invalid Move Packet");
			}
			else
			{
				SendBufferRef sendBuffer = ServerPacketHandler::Make_S_Move(player->info);
				if (player->session)
					player->session->Send(sendBuffer);
			}
		}
		else
		{
			player->invalidMoveCount = 0;
			valid.push_back(req);
		}
	}

	for (auto& req : valid)
	{
		PlayerRef player = req.player;
		player->info.set_state(req.state);
		player->info.set_dir(req.dir);
		player->info.set_posx(req.targetPos.x);
		player->info.set_posy(req.targetPos.y);

		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_Move(player->info);
		Broadcast(sendBuffer);
	}

	_moveRequests.clear();
}
