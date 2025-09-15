#pragma once

#include <optional>
#include <functional>

struct Tile
{
	// TODO
	int32 value = 0;
};

class Tilemap
{
public:
	Tilemap();
	virtual ~Tilemap();

	virtual void LoadFile(const std::wstring& path);
	
	Vec2Int GetMapSize() { return _mapSize; }
	int32 GetTileSize() { return _tileSize; }
	std::optional<std::reference_wrapper<Tile>> GetTileAt(Vec2Int pos);
	std::vector<std::vector<Tile>>& GetTiles() { return _tiles; };

	void SetMapSize(Vec2Int size);
	void SetTileSize(int32 size);

private:
	Vec2Int _mapSize = {};
	int32 _tileSize = {};
	std::vector<std::vector<Tile>> _tiles;
};

