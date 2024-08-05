//#pragma once
//#include <bitset>
//#include <vector>
//#include <algorithm>
//
//class Entity
//{
//public:
//	bool operator == (const Entity& otherE) const { return this == &otherE; }
//};
//
//class System
//{
//public:
//	virtual void init() = 0;
//	virtual void update() = 0;
//	virtual void render() = 0;
//	void registerWorld(World* world)
//	{
//		parentWorld = world;
//	}
//	void registerEntity(Entity entity)
//	{
//		registeredEntities.push_back(entity);
//	}
//	void deregisterEntity(const Entity entity)
//	{
//		std::erase_if(registeredEntities, [=](const Entity e)
//			{
//				return e == entity;
//			});
//	}
//private:
//	std::bitset<32> systemSignatures;
//	std::vector<Entity> registeredEntities;
//	World* parentWorld;
//};
//
