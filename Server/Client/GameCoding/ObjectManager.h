#pragma once
#include "GameObject.h"

class ObjectManager
{
	DECLARE_SINGLE(ObjectManager)

public:
	template<typename T>
	T* AddObject()
	{
		auto object = make_unique<T>();

		int64 id = _idGenerator++;
		object->SetObjectID(id);
		T* raw = object.get();
		_objects[id] = move(object);

		return raw;
	}

	void RemoveObject(int64 id)
	{
		auto findIt = _objects.find(id);
		if (findIt == _objects.end())
			return;

		_objects.erase(id);
	}

private:
	int64 _idGenerator = 1;
	unordered_map<int64, unique_ptr<GameObject>> _objects;
};

