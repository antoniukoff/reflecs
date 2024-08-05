#include <random>
#include <SDL.h>
#include "World.h"

void drawRectangle(SDL_Renderer* renderer, EntityID eID, World<Transform, Color>& world)
{
	auto [transorm, color] = world.unpack<Transform, Color>(eID);

	SDL_Rect rect{
		.x = transorm.x,
		.y = transorm.y,
		.w = transorm.w,
		.h = transorm.h
	};

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

EntityID generateEntityWithRectangle(World<Transform, Color>& world)
{
	EntityID eID = world.createEntity();
	static std::random_device randomEngine;
	static std::uniform_real_distribution<float> randomGenerator(0, 800);

	static std::random_device randomEngine1;
	static std::uniform_int_distribution<int> randomGenerator1(0, 255);

	int randPosX = randomGenerator(randomEngine);
	int randPosY = randomGenerator(randomEngine);

	world.addComponent<Transform>(eID, randPosX, randPosY > 600 ? 600 : randPosY, 50, 50);
	world.addComponent<Color>(eID, randomGenerator1(randomEngine1), randomGenerator1(randomEngine1), randomGenerator1(randomEngine1), randomGenerator1(randomEngine1));
	return eID;
}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("VAECS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	World<Transform, Color> world;

	std::vector<EntityID> entityVec;

	entityVec.push_back(generateEntityWithRectangle(world));
	entityVec.push_back(generateEntityWithRectangle(world));
	entityVec.push_back(generateEntityWithRectangle(world));
	entityVec.push_back(generateEntityWithRectangle(world));
	entityVec.push_back(generateEntityWithRectangle(world));

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
				entityVec.push_back(generateEntityWithRectangle(world));
			}
			if (event.key.keysym.sym == SDLK_2)
			{
				//world.removeComponent<Color>(entityVec.size() - 1);
			}
		}

		SDL_RenderClear(renderer);

		for (auto entity : entityVec)
		{
			drawRectangle(renderer, entity, world);
		}
		SDL_SetRenderDrawColor(renderer, 54, 136, 177, 255);

		SDL_RenderPresent(renderer);
	}

	return 0;
}