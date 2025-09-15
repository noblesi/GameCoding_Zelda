#include "pch.h"
#include "Tilemap.h"
#include <fstream>

Tilemap::Tilemap()
{

}

Tilemap::~Tilemap()
{

}

void Tilemap::LoadFile(const std::wstring& path)
{
	std::wifstream ifs(path);
	if (ifs.is_open() == false)
		return;

	ifs >> _mapSize.x >> _mapSize.y;

	SetMapSize(_mapSize);

	for (int32 y = 0; y < _mapSize.y; y++)
	{
		std::wstring line;
		ifs >> line;

		for (int32 x = 0; x < _mapSize.x; x++)
		{
			_tiles[y][x].value = line[x] - L'0';
		}
	}

	

}

std::optional<std::reference_wrapper<Tile>> Tilemap::GetTileAt(Vec2Int pos)
{
	if (pos.x < 0 || pos.x >= _mapSize.x || pos.y < 0 || pos.y >= _mapSize.y)
		return std::nullopt;

	return _tiles[pos.y][pos.x];
}

void Tilemap::SetMapSize(Vec2Int size)
{
	_mapSize = size;

	_tiles = std::vector<std::vector<Tile>>(size.y, std::vector<Tile>(size.x));

	for (int32 y = 0; y < size.y; y++)
	{
		for (int32 x = 0; x < size.x; x++)
		{
			_tiles[y][x] = Tile{ 0 };
		}
	}
}

void Tilemap::SetTileSize(int32 size)
{
	_tileSize = size;
}
