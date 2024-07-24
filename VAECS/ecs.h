#pragma once
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <assert.h>
#include <functional>
#include <format>

using EntityID = std::size_t;
using ComponentInstance = std::size_t;

constexpr size_t MAX_COMPONENTS_SIZE = 32;
constexpr size_t MAX_ENTITIES = 10;

/////////////////////////////////
//  COMPONENT DEFINITION
/////////////////////////////////

template<typename ComponentType>
struct GetMemberCount;

template<typename ComponentType, size_t N>
struct GetType;

struct Component
{
	/// Data
	size_t z = 0;
	int x, y = 0;
	int h = 0;
};

template<> struct GetMemberCount<Component>
{
	static const size_t count = 4;
};

template<> struct GetType<Component, 0> 
{ 
	using Type = size_t;
};
template<> struct GetType<Component, 1>
{ 
	using Type = int;
};

template<> struct GetType<Component, 2>
{
	using Type = int;
};

template<> struct GetType<Component, 3>
{
	using Type = int;
};

////////////////////////////////////




template<typename C, size_t elements>
struct ComponentData
{
	size_t size = 1; //  first available element in the array starts at 1; 0 reserved for error handling
	void* buffer[elements] ;
};

/// <summary>
/// This class will hold the component pool and manage
/// the assignment of an entity to the available slot as well as its removal from the pool
/// </summary>
/// <typeparam name="C">Component Type</typeparam>
/// 
template<typename C>
class ComponentManager;

//template<typename C>
//class ComponentHandle
//{
//	EntityID owner;
//	C& component;
//	ComponentManager<C>& mgr;
//
//	ComponentHandle(ComponentManager<C>& mgr, ComponentInstance inst, EntityID eID)
//		: owner(eID)
//		, component(mgr.getComponent(eID))
//		, mgr(mgr)
//	{}
//
//	void destroy()
//	{
//		mgr->removeComponent(owner);
//	}
//};


template <size_t index, size_t count, typename ComponentType>
size_t countComponentSize(size_t& bytes)
{
	if constexpr (index >= count)
	{
		return bytes;
	}
	else
	{
		bytes += sizeof(typename GetType<ComponentType, index>::Type);
		return countComponentSize<index + 1, count, ComponentType>(bytes);
	}
}

template<size_t count, typename ComponentType>
size_t countComponentSizeLoop()
{
	size_t bytes = 0;
	
	return countComponentSize<0, count, ComponentType>(bytes);
}

//template <size_t index, size_t count, typename ComponentType>
//void generateBuffers(void* buffer[], size_t numElements)
//{
//	if constexpr (index >= count - 1)
//	{
//		return;
//	}
//	else
//	{
//		using DataType		= GetType<ComponentType, index>::Type;																										
//		buffer[index + 1]	= static_cast<DataType*>(buffer[index]) + numElements;
//		ptrdiff_t byteDiff  = (char*)buffer[index + 1] - (char*)buffer[index];
//		generateBuffers<index + 1, count, ComponentType>(buffer, numElements);
//	}
//}

#include "utils.h"

template<typename C>
class ComponentManager
{
public:

	ComponentManager()
	{
		size_t packed_component_size = 0;
		CompileLoop::loop<MEMBER_COUNT, CountComponentSizeWrapper>(this, packed_component_size);
		
		size_t bytes = MAX_ENTITIES * packed_component_size;
		m_component_pool.buffer[0] = malloc(bytes);


		CompileLoop::loop<MEMBER_COUNT - 1, GenerateBuffersWrapper>(this, m_component_pool.buffer, MAX_ENTITIES);
		//generateBuffers<0, MEMBER_COUNT, C>(m_component_pool.buffer, MAX_ENTITIES);
	}

private:

	template<size_t index>
	void generateBuffers(void* buffer[], size_t numElements)
	{
		using DataType = GetType<C, index>::Type;
		buffer[index + 1] = static_cast<DataType*>(buffer[index]) + numElements;
		ptrdiff_t byteDiff = (char*)buffer[index + 1] - (char*)buffer[index];
	}

	template<size_t index>
	struct GenerateBuffersWrapper
	{
		void operator()(ComponentManager<C>* manager, void* buffer[], size_t numElements)
		{
			manager->generateBuffers<index>(buffer, numElements);
		}
	};

	template<size_t index>
	size_t countComponentSize(size_t& bytes)
	{
		bytes += sizeof(typename GetType<C, index>::Type);
		return bytes;
	}

	template<size_t index>
	struct CountComponentSizeWrapper
	{
		void operator()(ComponentManager<C>* manager, size_t& bytes)
		{
			manager->countComponentSize<index>(bytes);
		}
	};


#pragma region 
	//template<typename C, typename ... Args>
	//ComponentInstance& addComponent(EntityID eID, Args&& ... args)
	//{
	//	ComponentInstance new_instance = m_component_pool.size;
	//	m_component_pool.component_data[new_instance] = C(std::forward<Args>(args)...);
	//	m_entities_to_components[eID] = new_instance;
	//	m_component_pool.size++;
	//	return new_instance;	
	//}

	//C& getComponent(EntityID eID)
	//{
	//	ComponentInstance instance = m_entities_to_components[eID];
	//	return m_component_pool[instance];
	//}

	//void removeComponent(EntityID eID)
	//{
	//	ComponentInstance instance = m_entities_to_components[eID];
	//	assert(instance > 0 && "Entity is not assigned to this component");
	//	assert(instance < MAX_ENTITIES && "Instance is out of range");

	//	ComponentInstance lastInstance = m_component_pool.size - 1;
	//	m_component_pool.component_data[instance] = m_component_pool.component_data[lastInstance];
	//	m_entities_to_components.erase(eID);
	//	m_component_pool.size--;

	//	if (instance == lastInstance)
	//	{
	//		return;
	//	}

	//	auto it = std::find_if(m_entities_to_components.begin(), m_entities_to_components.end(), [lastInstance](const auto& val)
	//		{
	//			return val.second == lastInstance;
	//		});
	//	
	//	m_entities_to_components[it->first] = instance;
	//}

	//void iterateAll(std::function<void(C)> lambda)
	//{
	//	for (int i = 1; i < m_component_pool.size; i++)
	//	{
	//		lambda(m_component_pool.component_data[i]);
	//	}
	//}
private:
	static const size_t MEMBER_COUNT = GetMemberCount<C>::count;
	std::unordered_map<EntityID, ComponentInstance> m_entities_to_components;
	ComponentData<C, MEMBER_COUNT> m_component_pool;
#pragma endregion
};


//
//class EntityManager
//{
//};
//
//class World
//{
//public:
//
//	void destroy(EntityID eID)
//	{
//
//	}
//
//	template<typename C>
//	void addComponent(EntityID eID, C component)
//	{
//		ComponentManager<C>& manager = getComponentManager<C>();
//		manager.addComponent(eID, component);
//	}
//
//	template<typename C>
//	void removeComponent(EntityID eID)
//	{
//		ComponentManager<C>& manager = getComponentManager<C>();
//		manager.removeComponent(eID);
//	}
//
//private:
//	template<typename C>
//	ComponentManager<C>& getComponentManager() const
//	{
//
//	}
//
//};
//
EntityID generateID()
{
	static size_t new_id = 0;
	return new_id++;
}
//
//struct EntityHandle
//{
//	World* world;
//	EntityID e_id;
//
//	void destroy()
//	{
//		world->destroy(e_id);
//	}
//
//	template<typename C>
//	void addComponent(C component)
//	{
//		world->addComponent<C>(e_id, component);
//	}
//
//	template<typename C>
//	void removeComponent()
//	{
//		world->removeComponent<C>(e_id);
//	}
//
//};

int main()
{
	ComponentManager<Component> c_manager;
	//EntityID e1 = generateID();
	//EntityID e2 = generateID();
	//EntityID e3 = generateID();
	//EntityID e4 = generateID();
	
	/// TESTS

	//c_manager.addComponent<Component>(e1, 1, 0);
	////cManager.removeComponent(e4);
	//c_manager.addComponent<Component>(e2, 4, 6);

	///

	return 0;
}