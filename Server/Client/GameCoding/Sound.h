#pragma once
#include "ResourceBase.h"

enum class SoundType
{
	BGM,
	Normal,
};

class Sound : public ResourceBase
{
public:
	Sound();
	virtual ~Sound();
	
	bool LoadWave(fs::path fullPath);
	void Play(bool loop = false);
	void Stop(bool reset = false);

private:
	virtual void LoadFile(const std::wstring& path) {}
	virtual void SaveFile(const std::wstring& path) {}

private:
	LPDIRECTSOUNDBUFFER _soundBuffer = nullptr;
	DSBUFFERDESC _bufferDesc = {};
};
