#pragma once

#include "Common.h"
#include "ComponentMask.h"
#include <SDL.h>

template<typename ... Cs>
class Registry;

class System
{
public:
	explicit System(Signature bitSet)
		: systemSignatures(bitSet)
	{}
	virtual void update() {};
	virtual void render(SDL_Renderer* renderer) {};

	void registerEntities(std::vector<EntityID>* entities)
	{
		registeredEntities = entities;
	}

	void deregisterEntity(const EntityID entity)
	{
		std::erase_if(*registeredEntities, [entity](const EntityID e)  
			{
				return e == entity;
			});
	}

	const auto& getBitset() const
	{
		return systemSignatures;
	}

	

public:
	template<typename ... Cs>
	static Signature createSystemSignatures()
	{
		Signature signatures;

		(signatures.set(GetComponentFamily<Cs>()), ...);
		return signatures;
	}

protected:
	Signature systemSignatures;
	std::vector<EntityID>* registeredEntities = nullptr;
};

template<typename ... Ts>
class RenderSystem : public System
{
private:
	Registry<Ts...>& registry;
public:

	RenderSystem(Registry<Ts...>& registry)
		: registry(registry)
		, System(createSystemSignatures<Transform, Color>())
	{}

	void render(SDL_Renderer* renderer) override
	{
		for (auto eID : *registeredEntities)
		{
			auto [transform, color] = registry.template unpack<Transform, Color>(eID);

			SDL_Rect rect{
				.x = transform.x,
				.y = transform.y,
				.w = transform.w,
				.h = transform.h
			};

			SDL_SetRenderDrawColor(renderer, color.r,
				color.g,
				color.b,
				color.a);

			SDL_RenderFillRect(renderer, &rect);
		}
	}
};