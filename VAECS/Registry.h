#pragma once
#include "ComponentPool.h"
#include "Common.h"
#include "System.h"
#include <queue>

template<typename ... Ts>
class Registry
{

private:
	static constexpr size_t REGISTERED_COMPONENTS = sizeof...(Ts);
	std::vector<System<Ts...>*> systems;
	std::vector<std::unique_ptr<BaseComponentPool>> componentPools; // type erasure
	std::queue<EntityID> availableIDs;
	std::vector<Signature> entities_to_signatures;
public:
	std::unordered_map<Signature, std::vector<EntityID>> m_entities;

	/// Creates the component manager vector from the templated arguments
	Registry()
		:entities_to_signatures(MAX_ENTITIES)
	{
		for (size_t i = 0; i < MAX_ENTITIES; ++i)
		{
			availableIDs.push(i);
		}
		componentPools.resize(REGISTERED_COMPONENTS);
		createComponentPools<Ts...>();
	}

	void initialize()
	{
		/// Create systems and the components required to operate before init
		for (auto& system : systems)
		{
			system->init();
		}
	}

	void update()
	{
		for (auto& system : systems)
		{
			system->update();
		}
	}

	void display(SDL_Renderer* renderer)
	{
		systems.back()->render(renderer);
	}

	EntityID createEntity()
	{
		/// generate new id to be used from the queue
		size_t new_id = availableIDs.front();
		availableIDs.pop();
		return new_id;
	}

	/// To be called before init
	template<typename T>
	void registerSystem(T& system)
	{
		static_assert(std::is_base_of<System<Ts...>, T>::value == true && std::is_same<System<Ts...>, T>::value == false && "System is not inhereting from the base class");

		systems.push_back(&system);
	}

	void destroyEntity(EntityID e)
	{
		utils::CompileLoop::execute<REGISTERED_COMPONENTS, RemoveEntityWrapper>(this, e, entities_to_signatures[e]);
		availableIDs.push(e);
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

		ComponentPool<C>* mgr = retrievePool<C>();
		mgr->add(eID, std::forward<Args>(args)...);

		updateMask<C>(eID, true);
	}

	template<typename ... Cs, typename F>
	void ForEach(F&& function)
	{
		Signature targetBitset = utils::createSystemSignatures<Cs...>();
		std::vector<std::vector<EntityID>*> entityVecs;
		for (auto& [bitset, entityVec] : m_entities)
		{
			if ((bitset & targetBitset) == targetBitset)
			{
				entityVecs.push_back(&entityVec);
			}
		}
		if (entityVecs.empty()) 
		{
			return;
		}

		for (size_t i = 0; i < entityVecs.size(); ++i)
		{
			for (auto entityID : *entityVecs[i])
			{
				auto t = unpack<Cs...>(entityID);
				std::apply(function, t);
			}
		}
	}


	template<typename C>
	void removeComponent(EntityID eID)
	{
		static_assert(std::is_base_of<BaseComponent<C>, C>::value == true && "The template argument is not inhereting from BaseComponent");

		ComponentPool<C>* mgr = retrievePool<C>();
		mgr->remove(eID);

		updateMask<C>(eID, false);
	}
	
private:
	
	template<typename C>
	void updateMask(EntityID eID, bool add)
	{
		Signature s = entities_to_signatures[eID];
		std::erase(m_entities[s], eID);
		s.set(GetComponentFamily<C>(), add);
		entities_to_signatures[eID] = s;
		m_entities[s].push_back(eID);
	}

	std::vector<EntityID>* getEntityIDs(Signature bitset)
	{
		return &m_entities[bitset];
	}

	template<typename Head, typename ... Tails>
	void createComponentPools()
	{
		/// Assert that the given component is inhereting from the base
		static_assert(std::is_base_of<BaseComponent<Head>, Head>::value == true && "The template argument is not inhereting from BaseComponent");

		componentPools[GetComponentFamily<Head>()] = std::make_unique<ComponentPool<Head>>();

		if constexpr (sizeof...(Tails) < 1)
		{
			return;
		}
		else
		{
			createComponentPools<Tails...>();
		}
	}

	template<typename C>
	ComponentHandle<C> createHandle(EntityID eID)
	{
		ComponentPool<C>* mgr = retrievePool<C>();
		return mgr->retrieve(eID);
	}

	template<typename C>
	ComponentPool<C>* retrievePool()
	{
		return static_cast<ComponentPool<C>*>(componentPools[GetComponentFamily<C>()].get());
	}

	template<size_t index>
	void removeEntityFromPool(EntityID eID, Signature& bitSet)
	{
		if (bitSet[index])
		{
			removeComponent<ComponentTypeAtIndex<index, Ts...>>(eID);
		}
	}

	template<size_t index>
	struct RemoveEntityWrapper
	{
		void operator()(Registry* parent, EntityID eID, Signature& bitSet)
		{
			parent->removeEntityFromPool<index>(eID, bitSet);
		}
	};
};

