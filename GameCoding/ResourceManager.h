#pragma once

class ResourceBase;

class ResourceManager
{
public:
	DECLARE_SINGLE(ResourceManager);

	~ResourceManager();

public:
	void Init();
	void Clear();
private:

};

