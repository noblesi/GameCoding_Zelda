#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Sprite.h"
#include "Flipbook.h"
#include "Tilemap.h"
#include "Sound.h"

ResourceManager::~ResourceManager()
{
	Clear();
}

void ResourceManager::Init(HWND hwnd, fs::path resourcePath)
{
	_hwnd = hwnd;
	_resourcePath = resourcePath;

	//fs::current_path();
	//_resourcePath.relative_path();
	//fs::absolute(_resourcePath);
}

void ResourceManager::Clear()
{
	_textures.clear();
	_sprites.clear();
	_flipbooks.clear();
	_tilemaps.clear();
	_sounds.clear();
}

Texture* ResourceManager::GetTexture(const wstring& key)
{
	auto it = _textures.find(key);
	return it != _textures.end() ? it->second.get() : nullptr;
}

Texture* ResourceManager::LoadTexture(const wstring& key, const wstring& path, uint32 transparent /*= RGB(255, 0, 255)*/)
{
	if (_textures.find(key) != _textures.end())
		return _textures[key].get();

	fs::path fullPath = _resourcePath / path;

	auto texture = make_unique<Texture>();
	texture->LoadBmp(_hwnd, fullPath.c_str());
	texture->SetTransparent(transparent);
	Texture* ptr = texture.get();
	_textures[key] = move(texture);

	return ptr;
}

Sprite* ResourceManager::GetSprite(const wstring& key)
{
	auto it = _sprites.find(key);
	return it != _sprites.end() ? it->second.get() : nullptr;
}

Sprite* ResourceManager::CreateSprite(const wstring& key, Texture* texture, int32 x, int32 y, int32 cx, int32 cy)
{
	if (_sprites.find(key) != _sprites.end())
		return _sprites[key].get();

	if (cx == 0)
		cx = texture->GetSize().x;

	if (cy == 0)
		cy = texture->GetSize().y;

	auto sprite = make_unique<Sprite>(texture, x, y, cx, cy);
	Sprite* ptr = sprite.get();
	_sprites[key] = move(sprite);

	return ptr;
}

Flipbook* ResourceManager::GetFlipbook(const wstring& key)
{
	auto it = _flipbooks.find(key);
	return it != _flipbooks.end() ? it->second.get() : nullptr;
}

Flipbook* ResourceManager::CreateFlipbook(const wstring& key)
{
	if (_flipbooks.find(key) != _flipbooks.end())
		return _flipbooks[key].get();

	auto fb = make_unique<Flipbook>();
	Flipbook* ptr = fb.get();
	_flipbooks[key] = move(fb);

	return ptr;
}

Tilemap* ResourceManager::GetTilemap(const wstring& key)
{
	auto it = _tilemaps.find(key);
	return it != _tilemaps.end() ? it->second.get() : nullptr;
}

Tilemap* ResourceManager::CreateTilemap(const wstring& key)
{
	if (_tilemaps.find(key) != _tilemaps.end())
		return _tilemaps[key].get();

	auto tm = make_unique<Tilemap>();
	Tilemap* ptr = tm.get();
	_tilemaps[key] = move(tm);

	return ptr;
}

void ResourceManager::SaveTilemap(const wstring& key, const wstring& path)
{
	Tilemap* tilemap = GetTilemap(key);

	fs::path fullPath = _resourcePath / path;
	tilemap->SaveFile(fullPath);
}

Tilemap* ResourceManager::LoadTilemap(const wstring& key, const wstring& path)
{
	Tilemap* tilemap = nullptr;

	if (_tilemaps.find(key) == _tilemaps.end())
		_tilemaps[key] = make_unique<Tilemap>();
		
	tilemap = _tilemaps[key].get();

	fs::path fullPath = _resourcePath / path;
	tilemap->LoadFile(fullPath);

	return tilemap;
}

Sound* ResourceManager::GetSound(const wstring& key)
{
	auto it = _sounds.find(key);
	return it != _sounds.end() ? it->second.get() : nullptr;
}

Sound* ResourceManager::LoadSound(const wstring& key, const wstring& path)
{
	if (_sounds.find(key) != _sounds.end())
		return _sounds[key].get();

	fs::path fullPath = _resourcePath / path;

	auto sound = make_unique<Sound>();
	sound->LoadWave(fullPath);
	Sound* ptr = sound.get();
	_sounds[key] = move(sound);

	return ptr;
}
