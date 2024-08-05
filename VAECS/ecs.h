#pragma once
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <assert.h>
#include <functional>
#include <SDL.h>
#include <type_traits>

using EntityID = std::size_t;
using ComponentInstance = std::size_t;

constexpr size_t MAX_COMPONENTS_SIZE = 32;
constexpr size_t MAX_ENTITIES = 5000;
constexpr size_t CONTAINER_SIZE = MAX_ENTITIES + 1;

template<typename ComponentType>
struct GetMemberCount;

template<typename ComponentType, size_t N>
struct GetType;

template<typename T, size_t N>
struct GetPointerToMemeberType
{
	using Type = GetType<T, N>::Type T::*;// pointer to member
};

template<typename T, size_t N>
static typename GetPointerToMemeberType<T, N>::Type getPointerToMemeber() {};
	
/////////////////////////////////
//  COMPONENT DEFINITION
/////////////////////////////////

struct ComponentCounter
{
	static int counter;
};

int ComponentCounter::counter = 0;

template<typename C>
struct BaseComponent
{
	static inline int family()
	{
		static int family = ComponentCounter::counter++;
		return family;
	}
};

template<typename C>
static int getComponentFamily()
{
	return BaseComponent<typename std::remove_const<C>::type>::family();
}

struct Component 
{
	/// Data
	int x = {};
	int y = {};
	int w = {};
	int h = {};
};

template<> struct GetMemberCount<Component>
{
	static const size_t count = 4;
};

template<> struct GetType<Component, 0>
{ 
	using Type = int;
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

template<> typename GetPointerToMemeberType<Component, 0>::Type getPointerToMemeber<Component, 0>() { return &Component::x; }
template<> typename GetPointerToMemeberType<Component, 1>::Type getPointerToMemeber<Component, 1>() { return &Component::y; }
template<> typename GetPointerToMemeberType<Component, 2>::Type getPointerToMemeber<Component, 2>() { return &Component::w; }
template<> typename GetPointerToMemeberType<Component, 3>::Type getPointerToMemeber<Component, 3>() { return &Component::h; }

struct Color
{
	/// Data
	char r = {};
	char g = {};
	char b = {};
	char a = {};
};

template<> struct GetMemberCount<Color>
{
	static const size_t count = 4;
};

template<> struct GetType<Color, 0>
{
	using Type = char;
};

template<> struct GetType<Color, 1>
{
	using Type = char;
};

template<> struct GetType<Color, 2>
{
	using Type = char;
};

template<> struct GetType<Color, 3>
{
	using Type = char;
};

template<> typename GetPointerToMemeberType<Color, 0>::Type getPointerToMemeber<Color, 0>() { return &Color::r; }
template<> typename GetPointerToMemeberType<Color, 1>::Type getPointerToMemeber<Color, 1>() { return &Color::g; }
template<> typename GetPointerToMemeberType<Color, 2>::Type getPointerToMemeber<Color, 2>() { return &Color::b; }
template<> typename GetPointerToMemeberType<Color, 3>::Type getPointerToMemeber<Color, 3>() { return &Color::a; }

////////////////////////////////////



template<typename C, size_t elements>
struct ComponentData
{
	size_t size = 1; //  first available element in the array starts at 1; 0 reserved for error handling
	void* buffer[elements];
};

#include "utils.h"

/// <summary>
/// This class will hold the component pool and manage
/// the assignment of an entity to the available slot as well as its removal from the pool
/// </summary>
/// <typeparam name="C">Component Type</typeparam>

template<typename C>
class ComponentHandle;

class BaseComponentManager
{
public:
	virtual ~BaseComponentManager() = default;
};

template<typename C>
class ComponentManager : public BaseComponentManager
{
public:

	ComponentManager()
	{
		size_t packed_component_size = 0;
		CompileLoop::execute<MEMBER_COUNT, CountComponentSizeWrapper>(this, packed_component_size);
		
		size_t bytes = CONTAINER_SIZE * packed_component_size;
		m_component_pool.buffer[0] = malloc(bytes);
		CompileLoop::execute<MEMBER_COUNT - 1, GenerateBuffersWrapper>(this, m_component_pool.buffer, CONTAINER_SIZE);
	}

private:

#pragma region CompileHelpers
	template<size_t index>
	void generateBuffers(void* buffer[], size_t numElements)
	{
		using DataType = GetType<C, index>::Type;
		buffer[index + 1] = static_cast<DataType*>(buffer[index]) + numElements;

		char* previousAddress = (char*)buffer[index];
		char* currentAddress  = (char*)buffer[index + 1];

		size_t byteCount = currentAddress - previousAddress; // debugging
		assert(byteCount == sizeof(DataType) * CONTAINER_SIZE && "Check member offset");
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
	void countComponentSize(size_t& bytes)
	{
		bytes += sizeof(typename GetType<C, index>::Type);
	}

	template<size_t index>
	struct CountComponentSizeWrapper
	{
		void operator()(ComponentManager<C>* manager, size_t& bytes)
		{
			manager->countComponentSize<index>(bytes);
		}
	};

	template<size_t index>
	struct AddComponentDataWrappers
	{
		void operator()(ComponentManager<C>* mgr, ComponentInstance instanceToAdd, C& component)
		{
			mgr->addComponentData<index>(instanceToAdd, component);
		}
	};
	
	template<size_t index>
	void addComponentData(ComponentInstance instanceToAdd, C& component)
	{
		using DataType = GetType<C, index>::Type;

		std::array<DataType, CONTAINER_SIZE>& arrayHandle = *static_cast<std::array<DataType, CONTAINER_SIZE>*>(m_component_pool.buffer[index]);
		
		arrayHandle[instanceToAdd] = component.*getPointerToMemeber<C, index>();
	}

	template<size_t index>
	struct RemoveComponentDataWrapper
	{
		void operator()(ComponentManager<C>* mgr, ComponentInstance instanceToRemove)
		{
			mgr->removeComponentData<index>(instanceToRemove);
		}
	};

	template<size_t index>
	void removeComponentData(ComponentInstance instanceToRemove)
	{
		using DataType = GetType<C, index>::Type;
		std::array<DataType, CONTAINER_SIZE>& arrayHandle = *static_cast<std::array<DataType, CONTAINER_SIZE>*>(m_component_pool.buffer[index]);


		ComponentInstance lastInstance = m_component_pool.size - 1;
		arrayHandle[instanceToRemove]  = arrayHandle[lastInstance];
	}



#pragma endregion

public:
	template<typename ... Args>
	ComponentInstance& addComponent(EntityID eID, Args&& ... args)
	{
		//Step 1: Loop throght all the member
		//Step 2: Cast it to the std::array of the member type by their index
		//Step 3: Update the value in the field pool by dereferencing pointer-to-member
		//Step 4: Update the map and increase the size count

		ComponentInstance new_instance = m_component_pool.size;
		C component = C(std::forward<Args>(args)...);
		CompileLoop::execute<MEMBER_COUNT, AddComponentDataWrappers>(this, new_instance, component);

		m_entities_to_components[eID] = new_instance;
		m_component_pool.size++;
		return new_instance;	
	}

	ComponentInstance lookUp(EntityID eID)
	{
		return m_entities_to_components[eID];
	}

	ComponentHandle<C> getComponent(EntityID eID)
	{
		return ComponentHandle<C>(*this, eID);
	}

	template<size_t index>
	typename GetType<C, index>::Type& getMemberBuffer(EntityID eID)
	{
		ComponentInstance component_index = lookUp(eID);
		using DataType = GetType<C, index>::Type;

		std::array<DataType, CONTAINER_SIZE>& arr = *static_cast<std::array<DataType, CONTAINER_SIZE>*>(m_component_pool.buffer[index]);
		return arr[component_index];
	}

	void removeComponent(EntityID eID)
	{
		/// Step 1 find the component to remove in all the member pools of the component
		/// Step 2 copy the memory from the last member of the pool to the place where removed component index
		/// Step 3 remove the index form the map 
		/// Step 4 find the entity with the last index in the map and update the component index to be the updated position
		
		ComponentInstance instance = m_entities_to_components[eID];
		assert(instance > 0 && "Entity is not assigned to this component");
		assert(instance < CONTAINER_SIZE && "Instance is out of range");	

		CompileLoop::execute<MEMBER_COUNT, RemoveComponentDataWrapper>(this, instance);
		
		m_entities_to_components.erase(eID);
		m_component_pool.size--;

		ComponentInstance lastInstance = m_component_pool.size;

		if (instance == lastInstance)
		{
			return;
		}

		auto it = std::find_if(m_entities_to_components.begin(), m_entities_to_components.end(), [lastInstance](const auto& val)
			{
				return val.second == lastInstance;
			});
		
		m_entities_to_components[it->first] = instance;
	}

	void iterateAll(std::function<void(C)> lambda)
	{
		for (int i = 1; i < m_component_pool.size; i++)
		{
			lambda(m_component_pool.component_data[i]);
		}
	}

private:
	static const size_t MEMBER_COUNT = GetMemberCount<C>::count;
	ComponentData<C, MEMBER_COUNT> m_component_pool;
	std::unordered_map<EntityID, ComponentInstance> m_entities_to_components;
};

template<typename C>
class ComponentHandle;

template<>
class ComponentHandle<Component>
{
public:
	/// Data
	int& x;
	int& y;
	int& w;
	int& h;

public:
	ComponentHandle(ComponentManager<Component>& mgr, EntityID eID)
		: x(mgr.getMemberBuffer<0>(eID))
		, y(mgr.getMemberBuffer<1>(eID))
		, w(mgr.getMemberBuffer<2>(eID))
		, h(mgr.getMemberBuffer<3>(eID))
	{}
};

template<>
class ComponentHandle<Color>
{
public:
	/// Data
	char& r;
	char& g;
	char& b;
	char& a;

public:
	ComponentHandle(ComponentManager<Color>& mgr, EntityID eID)
		: r(mgr.getMemberBuffer<0>(eID))
		, g(mgr.getMemberBuffer<1>(eID))
		, b(mgr.getMemberBuffer<2>(eID))
		, a(mgr.getMemberBuffer<3>(eID))
	{}
};


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
//
//	class World* world;
//	EntityID e_id;
//
//	void destroy()
//	{
//		//world->destroy(e_id);
//	}
//
//	template<typename C, typename ... Args>
//	void addComponent(Args&& ... args)
//	{
//		//world->addComponent<C>(e_id, args...;);
//	}
//
//	template<typename C>
//	void removeComponent()
//	{
//		//world->removeComponent<C>(e_id);
//	}
//
//};

#include <random>

void drawRectangle(SDL_Renderer* renderer, EntityID eID, ComponentManager<Component>& tMgr, ComponentManager<Color>& cMgr)
{
	ComponentHandle<Component> t = tMgr.getComponent(eID);
	ComponentHandle<Color> cm = cMgr.getComponent(eID);

	SDL_Rect rect{
		.x = t.x,
		.y = t.y,
		.w = t.w,
		.h = t.h
	};


	SDL_SetRenderDrawColor(renderer, cm.r, cm.g, cm.b, cm.a);
	SDL_RenderFillRect(renderer, &rect);
}

EntityID generateEntityWithRectangle(ComponentManager<Component>& mgr, ComponentManager<Color>& cMgr)
{
	EntityID id = generateID();
	static std::random_device randomEngine;
	static std::uniform_real_distribution<float> randomGenerator(0, 800);

	static std::random_device randomEngine1;
	static std::uniform_int_distribution<int> randomGenerator1(0, 255);

	int randPosX = randomGenerator(randomEngine);
	int randPosY = randomGenerator(randomEngine);
	mgr.addComponent(id, randPosX, randPosY > 600 ? 600 : randPosY, 50, 50);
	cMgr.addComponent(id, randomGenerator1(randomEngine1), randomGenerator1(randomEngine1), randomGenerator1(randomEngine1), randomGenerator1(randomEngine1));
	return id;
}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("VAECS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	ComponentManager<Component> tManager;
	ComponentManager<Color> cManager;

	std::vector<EntityID> entityVec;

	entityVec.push_back(generateEntityWithRectangle(tManager, cManager));
	entityVec.push_back(generateEntityWithRectangle(tManager, cManager));
	entityVec.push_back(generateEntityWithRectangle(tManager, cManager));
	entityVec.push_back(generateEntityWithRectangle(tManager, cManager));
	entityVec.push_back(generateEntityWithRectangle(tManager, cManager));

	bool running = true;

	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
			{
				running = false;
			}
			if (event.key.keysym.sym == SDLK_1)
			{
				entityVec.push_back(generateEntityWithRectangle(tManager, cManager));
			}
		}

		SDL_RenderClear(renderer);

		for (auto entity : entityVec)
		{
			drawRectangle(renderer, entity, tManager, cManager);
		}
		SDL_SetRenderDrawColor(renderer, 54, 136, 177, 255);
		
		SDL_RenderPresent(renderer);
	}

	
	//EntityID e4 = generateID();
	
	/// TESTS

	/*c_manager.addComponent<Component>(e1, 1, 0);
	c_manager.addComponent<Component>(e2, 4, 6);
	auto& t = c_manager.getComponent<0>(e1);
	c_manager.removeComponent(e1);
	c_manager.addComponent<Component>(e1, 1, 0);*/

	///

	return 0;
}