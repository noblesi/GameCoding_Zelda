#include "pch.h"
#include "SceneManager.h"
#include "DevScene.h"
#include "EditScene.h"
#include "MyPlayer.h"

void SceneManager::Init()
{

}

void SceneManager::Update()
{
	if (_scene)
		_scene->Update();		
}

void SceneManager::Render(HDC hdc)
{
	if (_scene)
		_scene->Render(hdc);
}

void SceneManager::Clear()
{
	_scene.reset();
}

void SceneManager::ChangeScene(SceneType sceneType)
{
	if (_sceneType == sceneType)
		return;

	unique_ptr<Scene> newScene;

	switch (sceneType)
	{
		case SceneType::DevScene:
			newScene = make_unique<DevScene>();
			break;
		case SceneType::EditScene:
			newScene = make_unique<EditScene>();
			break;
	}

	_scene = move(newScene);
	_sceneType = sceneType;

	_scene->Init();
}

class DevScene* SceneManager::GetDevScene()
{
	return dynamic_cast<DevScene*>(GetCurrentScene());
}

uint64 SceneManager::GetMyPlayerId()
{
	return _myPlayer ? _myPlayer->info.objectid() : 0;
}
