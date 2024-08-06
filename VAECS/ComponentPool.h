#pragma once

#include "Common.h"
#include "Components.h"

template<typename C, size_t elements>
struct ComponentData
{
	size_t size = 1; //  first available element in the array starts at 1; 0 reserved for error handling
	void* buffer[elements];
};

class BaseComponentPool
{
public:
	virtual ~BaseComponentPool() = default;
};

template<typename C>
class ComponentHandle;

/// <summary>
/// This class will hold the component pool and manage
/// the assignment of an entity to the available slot as well as its removal from the pool
/// </summary>
/// <typeparam name="C">Component Type</typeparam>
template<typename C>
class ComponentPool : public BaseComponentPool
{
public:

	ComponentPool()
	{
		size_t packed_component_size = 0;
		CompileLoop::execute<MEMBER_COUNT, CountComponentSizeWrapper>(this, packed_component_size);
		
		size_t bytes = CONTAINER_SIZE * packed_component_size;
		m_component_pool.buffer[0] = malloc(bytes);
		CompileLoop::execute<MEMBER_COUNT - 1, GenerateBuffersWrapper>(this, m_component_pool.buffer, CONTAINER_SIZE);
	}

	~ComponentPool()
	{
		/// TODO: Fix memory leak
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
		void operator()(ComponentPool<C>* manager, void* buffer[], size_t numElements)
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
		void operator()(ComponentPool<C>* manager, size_t& bytes)
		{
			manager->countComponentSize<index>(bytes);
		}
	};

	template<size_t index>
	struct AddComponentDataWrappers
	{
		void operator()(ComponentPool<C>* mgr, ComponentInstance instanceToAdd, C& component)
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
		void operator()(ComponentPool<C>* mgr, ComponentInstance instanceToRemove)
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

	std::optional<ComponentHandle<C>> getComponent(EntityID eID)
	{
		if (lookUp(eID) == 0)
		{
			return std::nullopt;
		}
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


#pragma region ComponentHandles

template<typename C>
class ComponentHandle;

template<>
class ComponentHandle<Transform>
{
public:
	/// Data
	int& x;
	int& y;
	int& w;
	int& h;

public:
	ComponentHandle() = default;

	ComponentHandle(ComponentPool<Transform>& mgr, EntityID eID)
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
	ComponentHandle() = default;

	ComponentHandle(ComponentPool<Color>& mgr, EntityID eID)
		: r(mgr.getMemberBuffer<0>(eID))
		, g(mgr.getMemberBuffer<1>(eID))
		, b(mgr.getMemberBuffer<2>(eID))
		, a(mgr.getMemberBuffer<3>(eID))
	{}
};
#pragma endregion