#pragma once
#include "ComponentPool.h"
#include "Common.h"
#include "System.h"

template<typename ... Ts>
class World
{
public:
	
	/// Creates the component manager vector from the templated arguments
	World()
	{
		componentPools.resize(MAX_COMPONENTS_SIZE);
		createComponentPools<Ts...>();
	}

	template<typename Head, typename ... Tails>
	void createComponentPools()
	{
		/// Assert that the given component is inhereting from the base
		static_assert(std::is_base_of<BaseComponent<Head>, Head>::value == true && "The template argument is not inhereting from BaseComponent");
		
		componentPools[getComponentFamily<Head>()] = std::make_unique<ComponentPool<Head>>();

		if constexpr (sizeof...(Tails) < 1)
		{
			return;
		}
		else
		{
			createComponentPools<Tails...>();
		}
	}

	void init()
	{
		/// Create systems and the components required to operate before init
		/// Create entities before the init 
		/// In the init once bothe are created tie the `entities to the systems based on their set of components
		/// And if they match the systems requirements
	}

	EntityID createEntity()
	{
		/// generate new id to be used from the queue
		/// increment the total number of entities created - is it really needed????
		static size_t new_id = 0;
		return new_id++;
	}

	/// To be called before init
	void addSystem(System* system)
	{
		systems.push_back(system);
	}
	void destroyEntity(EntityID e)
	{
		/// Remove from the component managers
		/// Remove from the systems
		/// Give back the ID to the enetity Managers queue
	}
	
	template<typename C>
	std::optional<ComponentHandle<C>> createHandle(EntityID eID)
	{
		ComponentPool<C>* mgr = static_cast<ComponentPool<C>*>(componentPools[getComponentFamily<C>()].get());
		return mgr->getComponent(eID);
	}

	template<typename C, typename ... Cs>
	auto unpack(EntityID eID)
	{
		if constexpr (sizeof...(Cs) < 1)
		{
			return std::make_tuple(createHandle<C>(eID));
		}
		else
		{
			return std::tuple_cat(std::make_tuple(createHandle<C>(eID)), unpack<Cs...>(eID));
		}
	}

	template<typename C, typename ... Args>
	void addComponent(EntityID eID, Args&& ... args)
	{
		static_assert(std::is_base_of<BaseComponent<C>, C>::value == true && "The template argument is not inhereting from BaseComponent");

		ComponentPool<C>* mgr = static_cast<ComponentPool<C>*>(componentPools[getComponentFamily<C>()].get());
		mgr->addComponent(eID, std::forward<Args>(args)...);

		/// add to the system that uses that set of components
	}

	template<typename C>
	void removeComponent(EntityID eID)
	{
		static_assert(std::is_base_of<BaseComponent<C>, C>::value == true && "The template argument is not inhereting from BaseComponent");

		ComponentPool<C>* mgr = static_cast<ComponentPool<C>*>(componentPools[getComponentFamily<C>()].get());
		mgr->removeComponent(eID);
	}

private:
	std::vector<System*> systems;
	std::vector<std::unique_ptr<BaseComponentPool>> componentPools;
	//EntityManager* entityManager;
};

