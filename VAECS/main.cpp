#include <random>
#include <SDL.h>
#include "Registry.h"

#pragma region Test1

void generateEntityWithRectangle(Registry<Transform, Velocity, Color>& registry)
{
    static std::random_device randomEngine;
    static std::uniform_real_distribution<float> randomGenerator(0, 800);

    static std::random_device randomEngine1;
    static std::uniform_int_distribution<int> randomGenerator1(0, 255);

    int randPosX = randomGenerator(randomEngine);
    int randPosY = randomGenerator(randomEngine);

    EntityID eID = registry.createEntity();
    registry.addComponent<Transform>(eID, randPosX, randPosY > 600 ? 600 : randPosY, 50, 50);
    registry.addComponent<Color>(eID, randomGenerator1(randomEngine1), randomGenerator1(randomEngine1), randomGenerator1(randomEngine1), randomGenerator1(randomEngine1));
    registry.addComponent<Velocity>(eID, randomGenerator1(randomEngine1), randomGenerator1(randomEngine1));
}


#include <chrono>
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("VAECS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Registry<Transform, Velocity, Color> registry;

    for (size_t i = 0; i < MAX_ENTITIES; ++i) {
        generateEntityWithRectangle(registry);
    }
    
    size_t size = MAX_ENTITIES;
    bool running = true;
#pragma region fpscount
    int frameCount = 0;
    double totalElapsedTime = 0.0;

    using Clock = std::chrono::high_resolution_clock;
    auto startTime = Clock::now();
    auto lastTime = Clock::now();
#pragma endregion

    while (running) 
    {
#pragma region fpscount
        auto currentTime = Clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;
        lastTime = currentTime;

        frameCount++;
        totalElapsedTime += elapsed.count();

        double fps = 1.0 / elapsed.count();
        std::cout << "FPS: " << fps << '\n';
#pragma endregion

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
            {
                running = false;
            }
            if (event.key.keysym.sym == SDLK_1)
            {
                for (size_t i = size; i > size - 500; i--)
                {
                    registry.destroyEntity(i - 1);
                }
                size -= 50;
            }
            if (event.key.keysym.sym == SDLK_2)
            {
                generateEntityWithRectangle(registry);
            }
        }

        registry.ForEach<Transform, Velocity>([&registry](ComponentHandle<Transform> transform, ComponentHandle<Velocity> velocity)
            {
                velocity.x() *= 0.98f;
                velocity.y() *= 0.98f;
                transform.x() += velocity.x();
                transform.y() += velocity.y();

                // Reverse direction if the entity hits the left or right bounds
                if (transform.x() < 0) {
                    transform.x() = 0;
                    velocity.x() = -velocity.x(); // Reverse horizontal direction
                }
                else if (transform.x() + transform.w() > 800) {
                    transform.x() = 800 - transform.w();
                    velocity.x() = -velocity.x(); // Reverse horizontal direction
                }

                // Reverse direction if the entity hits the top or bottom bounds
                if (transform.y() < 0) {
                    transform.y() = 0;
                    velocity.y() = -velocity.y(); // Reverse vertical direction
                }
                else if (transform.y() + transform.h() > 600) {
                    transform.y() = 600 - transform.h();
                    velocity.y() = -velocity.y(); // Reverse vertical direction
                }
            });

        SDL_RenderClear(renderer);

        registry.ForEach<Transform, Color>([renderer](ComponentHandle<Transform> transform, ComponentHandle<Color> color)
            {
                SDL_FRect rect{
                    .x = transform.x(),
                    .y = transform.y(),
                    .w = transform.w(),
                    .h = transform.h()
                };

                SDL_SetRenderDrawColor(renderer, color.r(),
                                                 color.g(),
                                                 color.b(),
                                                 color.a());

                SDL_RenderFillRectF(renderer, &rect);

            });
        

        SDL_SetRenderDrawColor(renderer, 54, 136, 177, 255);
        SDL_RenderPresent(renderer);
    }
#pragma region fpscount

    auto endTime = Clock::now();
    std::chrono::duration<double> totalRunTime = endTime - startTime;
    double averageFPS = frameCount / totalRunTime.count();

    std::cout << "Average FPS: " << averageFPS << '\n';
#pragma endregion

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
#pragma endregion


/*
#pragma region Test2

#include <SDL.h>
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <chrono>

// Define an EntityID type for convenience
using EntityID = size_t;

// Struct to represent a rectangle with position and color
struct Rectangle {
    int x, y, w, h;
    int velx, vely;
    Uint8 r, g, b, a;
};

// Function to generate a rectangle with random properties
Rectangle generateRectangle() {
    static std::random_device randomEngine;
    static std::uniform_int_distribution<int> randomPos(0, 800);
    static std::uniform_int_distribution<int> randomColor(0, 255);

    int x = randomPos(randomEngine);
    int y = randomPos(randomEngine);
    int velx = randomPos(randomEngine);
    int vely = randomPos(randomEngine);
    Uint8 r = randomColor(randomEngine);
    Uint8 g = randomColor(randomEngine);
    Uint8 b = randomColor(randomEngine);
    Uint8 a = randomColor(randomEngine);

    return Rectangle{ x, y > 600 ? 600 : y, 50, 50, velx, vely, r, g, b, a };
}

// Function to draw a rectangle
void drawRectangle(SDL_Renderer* renderer, const Rectangle& rect) {
    SDL_Rect sdlRect{ rect.x, rect.y, rect.w, rect.h };
    SDL_SetRenderDrawColor(renderer, rect.r, rect.g, rect.b, rect.a);
    SDL_RenderFillRect(renderer, &sdlRect);
}


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("VAECS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    std::vector<Rectangle> rectangles;

    for (int i = 0; i < MAX_ENTITIES; ++i) {
        rectangles.push_back(generateRectangle());
    }

    bool running = true;
    bool up = false, down = false, left = false, right = false;

    int frameCount = 0;
    double totalElapsedTime = 0.0;

    using Clock = std::chrono::high_resolution_clock;
    auto startTime = Clock::now();
    auto lastTime = Clock::now();

    while (running) {
        auto currentTime = Clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;
        lastTime = currentTime;

        frameCount++;
        totalElapsedTime += elapsed.count();

        double fps = 1.0 / elapsed.count();
        std::cout << "FPS: " << fps << std::endl;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                bool isKeyDown = (event.type == SDL_KEYDOWN);
                switch (event.key.keysym.sym) {
                case SDLK_w:
                    up = isKeyDown;
                    break;
                case SDLK_s:
                    down = isKeyDown;
                    break;
                case SDLK_a:
                    left = isKeyDown;
                    break;
                case SDLK_d:
                    right = isKeyDown;
                    break;
                }
            }
        }
        for (auto& rect : rectangles)
        {
            rect.velx *= 0.98f;
            rect.vely *= 0.98f;
            rect.x += rect.velx;
            rect.y += rect.vely;

            // Reverse direction if the entity hits the left or right bounds
            if (rect.x < 0) {
                rect.x = 0;
                rect.velx = -rect.velx; // Reverse horizontal direction
            }
            else if (rect.x + rect.w > 800) {
                rect.x = 800 - rect.w;
                rect.velx = -rect.velx; // Reverse horizontal direction
            }

            // Reverse direction if the entity hits the top or bottom bounds
            if (rect.y < 0) {
                rect.y = 0;
                rect.vely = -rect.vely; // Reverse vertical direction
            }
            else if (rect.y + rect.h > 600) {
                rect.y = 600 - rect.h;
                rect.vely = -rect.vely; // Reverse vertical direction
            }
        }     




        SDL_RenderClear(renderer);

        for (const auto& rect : rectangles) {
            drawRectangle(renderer, rect);
        }

        SDL_SetRenderDrawColor(renderer, 54, 136, 177, 255);
        SDL_RenderPresent(renderer);
    }

    auto endTime = Clock::now();
    std::chrono::duration<double> totalRunTime = endTime - startTime;
    double averageFPS = frameCount / totalRunTime.count();

    std::cout << "Average FPS: " << averageFPS << '\n';

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#pragma endregion
*/