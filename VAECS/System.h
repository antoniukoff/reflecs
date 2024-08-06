#pragma once

#include "Common.h"

class System
{
public:
	virtual void init() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	
	void registerEntity(EntityID entity)
	{
		registeredEntities.push_back(entity);
	}
	void deregisterEntity(const EntityID entity)
	{
		std::erase_if(registeredEntities, [entity](const EntityID e)  
			{
				return e == entity;
			});
	}
private:
	std::bitset<MAX_COMPONENTS_SIZE> systemSignatures;
	std::vector<EntityID> registeredEntities;
};

