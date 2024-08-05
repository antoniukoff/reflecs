//#pragma once
//#include "ecs.h"
//
//class World
//{
//public:
//	void init()
//	{
//
//	}
//	EntityHandle createEntity()
//	{
//
//	}
//	void addSystem(class System* system)
//	{
//
//	}
//	void destroyEntity(EntityID e)
//	{
//
//	}
//	
//	template<typename C, typename ... Cs>
//	void unpack(EntityID eID, ComponentHandle<C>& componentHandle, ComponentHandle<Cs>& ... remainingHandles)
//	{
//		using ComponentManagerType = ComponentManager<C>;
//		ComponentManagerType* mgr = static_cast<ComponentManagerType*>(componentManagers[getComponentFamily<C>()]);
//		componentHandle = mgr->getComponent(eID);
//
//		unpack<Cs...>(eID, remainingHandles ...);
//	}
//
//	template<typename C, typename ... Args>
//	void addComponent(EntityID eID, Args&& ... args)
//	{
//	}
//
//	template<typename C>
//	void removeComponent(EntityID eID)
//	{
//	}
//
//private:
//	std::vector<System*> systems;
//	std::vector<BaseComponentManager*> componentManagers;
//	//EntityManager* entityManager;
//};