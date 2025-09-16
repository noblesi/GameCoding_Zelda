#include "pch.h"
#include "Tilemap.h"
#include <iostream>
#include <fstream>

Tilemap::Tilemap()
{

}

Tilemap::~Tilemap()
{

}

void Tilemap::LoadFile(const wstring& path)
{
	if (false)
	{
		FILE* file = nullptr;

		::_wfopen_s(&file, path.c_str(), L"rb");
		assert(file);

		::fread(&_mapSize.x, sizeof(_mapSize.x), 1, file);
		::fread(&_mapSize.y, sizeof(_mapSize.y), 1, file);

		SetMapSize(_mapSize);

		for (int32 y = 0; y < _mapSize.y; y++)
		{
			for (int32 x = 0; x < _mapSize.x; x++)
			{
				int32 value = -1;
				::fread(&value, sizeof(value), 1, file);
				_tiles[y][x].value = value;

				bool walkable = false;
				if (::fread(&walkable, sizeof(walkable), 1, file) == 1)
				{
					_tiles[y][x].walkable = walkable;
				}
				else
				{
					_tiles[y][x].walkable = (_tiles[y][x].value == 0);
				}
			}
		}

		::fclose(file);
		return;
	}


	{
		wifstream ifs;

		ifs.open(path);
		if (ifs.is_open() == false)
			return;

		ifs >> _mapSize.x >> _mapSize.y;

		SetMapSize(_mapSize);

		const size_t totalTiles = static_cast<size_t>(_mapSize.x) * static_cast<size_t>(_mapSize.y);
		vector<int32> rawValues;
		rawValues.reserve(totalTiles * 2);

		wchar_t ch = 0;
		while (ifs.get(ch))
		{
			if (ch >= L'0' && ch <= L'9')
				rawValues.push_back(static_cast<int32>(ch - L'0'));
		}

		for (int32 y = 0; y < _mapSize.y; y++)
		{
			for (int32 x = 0; x < _mapSize.x; x++)
			{
				const size_t index = static_cast<size_t>(y) * static_cast<size_t>(_mapSize.x) + static_cast<size_t>(x);
				if (index < rawValues.size())
				{
					_tiles[y][x].value = rawValues[index];
				}
				else
				{
					_tiles[y][x].value = 0;
				}

				const size_t walkableIndex = totalTiles + index;
				if (walkableIndex < rawValues.size())
				{
					_tiles[y][x].walkable = (rawValues[walkableIndex] != 0);
				}
				else
				{
					_tiles[y][x].walkable = (_tiles[y][x].value == 0);
				}
			}
		}

		ifs.close();
	}
}

Tile* Tilemap::GetTileAt(Vec2Int pos)
{
	if (pos.x < 0 || pos.x >= _mapSize.x || pos.y < 0 || pos.y >= _mapSize.y)
		return nullptr;

	return &_tiles[pos.y][pos.x];
}

void Tilemap::SetMapSize(Vec2Int size)
{
	_mapSize = size;

	_tiles = vector<vector<Tile>>(size.y, vector<Tile>(size.x));

	for (int32 y = 0; y < size.y; y++)
	{
		for (int32 x = 0; x < size.x; x++)
		{
			_tiles[y][x] = Tile{ 0, true };
		}
	}
}

void Tilemap::SetTileSize(int32 size)
{
	_tileSize = size;
}
