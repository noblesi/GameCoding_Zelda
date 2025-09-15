#pragma once
#include "Sound.h"
#include "Texture.h"
#include "Sprite.h"
#include "Tilemap.h"
#include "Flipbook.h"

class ResourceBase;

class ResourceManager
{
public:
	DECLARE_SINGLE(ResourceManager);

	~ResourceManager();

public:
	void Init(HWND hwnd, fs::path resourcePath);
	void Clear();

	const fs::path& GetResourcePath() { return _resourcePath; }

	Texture* GetTexture(const std::wstring& key);
	Texture* LoadTexture(const std::wstring& key, const std::wstring& path, uint32 transparent = RGB(255, 0, 255));
	
	Sprite* GetSprite(const std::wstring& key);
	Sprite* CreateSprite(const std::wstring& key, Texture* texture, int32 x = 0, int32 y = 0, int32 cx = 0, int32 cy = 0);

	Flipbook* GetFlipbook(const std::wstring& key);
	Flipbook* CreateFlipbook(const std::wstring& key);

	Tilemap* GetTilemap(const std::wstring& key);
	Tilemap* CreateTilemap(const std::wstring& key);
	void SaveTilemap(const std::wstring& key, const std::wstring& path);
	Tilemap* LoadTilemap(const std::wstring& key, const std::wstring& path);
		
	Sound* GetSound(const std::wstring& key);
	Sound* LoadSound(const std::wstring& key, const std::wstring& path);

private:
	HWND _hwnd;
	fs::path _resourcePath;

	std::unordered_map<std::wstring, std::unique_ptr<Texture>> _textures;
	std::unordered_map<std::wstring, std::unique_ptr<Sprite>> _sprites;
	std::unordered_map<std::wstring, std::unique_ptr<Flipbook>> _flipbooks;
	std::unordered_map<std::wstring, std::unique_ptr<Tilemap>> _tilemaps;
	std::unordered_map<std::wstring, std::unique_ptr<Sound>> _sounds;
};

