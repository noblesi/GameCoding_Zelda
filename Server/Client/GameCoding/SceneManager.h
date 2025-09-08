#pragma once
#include "Scene.h"

class MyPlayer;

class SceneManager
{
	DECLARE_SINGLE(SceneManager)

public:
	void Init();
	void Update();
	void Render(HDC hdc);

	void Clear();

public:
	void ChangeScene(SceneType sceneType);
	Scene* GetCurrentScene() { return _scene.get();}

	class DevScene* GetDevScene();

	shared_ptr<MyPlayer> GetMyPlayer() { return _myPlayer; }
	uint64 GetMyPlayerId();
	void SetMyPlayer(shared_ptr<MyPlayer> myPlayer) { _myPlayer = myPlayer; }

private:
	unique_ptr<Scene> _scene;
	SceneType _sceneType = SceneType::None;
	shared_ptr<MyPlayer> _myPlayer = nullptr;

public:
	Vec2 GetCameraPos() { return _cameraPos; }
	void SetCameraPos(Vec2 pos) { _cameraPos = pos; }

private:
	Vec2 _cameraPos = {400, 300};
};

