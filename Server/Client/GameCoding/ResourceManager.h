#pragma once

class ResourceBase;
class Texture;
class Sprite;
class Flipbook;
class Tilemap;
class Sound;

class ResourceManager
{
public:
	DECLARE_SINGLE(ResourceManager);

	~ResourceManager();

public:
	void Init(HWND hwnd, fs::path resourcePath);
	void Clear();

	const fs::path& GetResourcePath() { return _resourcePath; }

	Texture* GetTexture(const wstring& key);
	Texture* LoadTexture(const wstring& key, const wstring& path, uint32 transparent = RGB(255, 0, 255));
	
	Sprite* GetSprite(const wstring& key);
	Sprite* CreateSprite(const wstring& key, Texture* texture, int32 x = 0, int32 y = 0, int32 cx = 0, int32 cy = 0);

	Flipbook* GetFlipbook(const wstring& key);
	Flipbook* CreateFlipbook(const wstring& key);

	Tilemap* GetTilemap(const wstring& key);
	Tilemap* CreateTilemap(const wstring& key);
	void SaveTilemap(const wstring& key, const wstring& path);
	Tilemap* LoadTilemap(const wstring& key, const wstring& path);
		
	Sound* GetSound(const wstring& key);
	Sound* LoadSound(const wstring& key, const wstring& path);

private:
	HWND _hwnd;
	fs::path _resourcePath;

	unordered_map<wstring, unique_ptr<Texture>> _textures;
	unordered_map<wstring, unique_ptr<Sprite>> _sprites;
	unordered_map<wstring, unique_ptr<Flipbook>> _flipbooks;
	unordered_map<wstring, unique_ptr<Tilemap>> _tilemaps;
	unordered_map<wstring, unique_ptr<Sound>> _sounds;
};

