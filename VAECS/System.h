#pragma once

#include "Common.h"
#include <SDL.h>
#include "utils.h"

template<typename ... Cs>
class Registry;

template<typename ... Ts>
class System
{
public:
	System(Registry<Ts...>& registry, Signature signature)
		: registry(registry)
		, systemSignature(signature)
	{}

	void init()
	{
		for (auto& [bitset, entityVec] : registry.m_entities)
		{
			if ((bitset & systemSignature) == systemSignature)
			{
				registeredEntities.push_back(&entityVec);
			}
		}
	}
	
	virtual void update() {};
	virtual void render(SDL_Renderer* renderer) {};

protected:
	Signature		 systemSignature;
	Registry<Ts...>& registry;
	std::vector<std::vector<EntityID>*> registeredEntities;
};

template<typename ... Ts>
class RenderSystem : public System<Ts...>
{
private:
public:
	RenderSystem(Registry<Ts...>& registry)
		: System<Ts...>(registry, utils::createSystemSignatures<Transform, Color>())
	{}

	void render(SDL_Renderer* renderer) override
	{
		for (auto& vec : this->registeredEntities)
		{
			for (auto& eID : *vec)
			{
				auto [transform, color] = this->registry.template unpack<Transform, Color>(eID);

				SDL_Rect rect{
					.x = transform.x(),
					.y = transform.y(),
					.w = transform.w(),
					.h = transform.h()
				};

				SDL_SetRenderDrawColor(renderer, color.r(),
					color.g(),
					color.b(),
					color.a());


				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
};