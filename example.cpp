#include <random>
#include "include/reflecs.h"
#include <algorithm>
#include <string>
#include <thread>

#pragma region HOW TO DEFINE A COMPONENT

struct vec2
{
    float x = 0.0f;
    float y = 0.0f;
};

/// Define a component struct
struct transform
{
    float x, y, w, h;
};

ANNOTATE(transform,
    4,
    DEFINE_COMPONENT_MEMBER(transform, 0, float, x)
    DEFINE_COMPONENT_MEMBER(transform, 1, float, y)
    DEFINE_COMPONENT_MEMBER(transform, 2, float, w)
    DEFINE_COMPONENT_MEMBER(transform, 3, float, h)
)

DEFINE_COMPONENT_HANDLE(transform,
    COMPONENT_HANDLE_ACCESSOR(0, float, x)
    COMPONENT_HANDLE_ACCESSOR(1, float, y)
    COMPONENT_HANDLE_ACCESSOR(2, float, w)
    COMPONENT_HANDLE_ACCESSOR(3, float, h)
)
/// Other components can be defined in a similar way
struct velocity
{
    float x, y;
};

ANNOTATE(velocity,
    2,
    DEFINE_COMPONENT_MEMBER(velocity, 0, float, x)
    DEFINE_COMPONENT_MEMBER(velocity, 1, float, y)
)

DEFINE_COMPONENT_HANDLE(velocity,
    COMPONENT_HANDLE_ACCESSOR(0, float, x)
    COMPONENT_HANDLE_ACCESSOR(1, float, y)
)

struct tag
{
    std::string tag;
};

ANNOTATE(tag,
    1,
    DEFINE_COMPONENT_MEMBER(tag, 0, std::string, tag)
)

DEFINE_COMPONENT_HANDLE(tag,
    COMPONENT_HANDLE_ACCESSOR(0, std::string, tag)
)

#pragma endregion

#pragma region ECS USAGE

int main(int argc, char* argv[])
{
    //////////////////////////
    /// Creating a registry
    //////////////////////////

    using scene_registry = registry<transform, velocity, tag>;
    scene_registry registry;

    {
        std::random_device randomEngine;
        std::uniform_real_distribution<float> randomGenerator(0.0f, 800.0f);

        for (size_t i = 0; i < g_max_entities; ++i)
        {
            /// Random values to populate components
            float randPosX = std::clamp(randomGenerator(randomEngine), 0.0f, 800.0f - 50.0f);
            float randPosY = std::clamp(randomGenerator(randomEngine), 0.0f, 600.0f - 50.0f);

            float randVelX = std::clamp(randomGenerator(randomEngine), 5.0f, 10.0f);
            float randVelY = std::clamp(randomGenerator(randomEngine), 5.0f, 10.0f);

            //////////////////////////////////
            /// Creating entity ID
            //////////////////////////////////

            auto eID = registry.create_entity();

            //////////////////////////////////
            /// Adding Component to the entity
            //////////////////////////////////

            registry.add<transform>(eID, randPosX, randPosY, 50.0f, 50.0f); /// Transform(x - randPosX, y = randPosY, w = 50.0f, h - 50.0f)
            registry.add<velocity>(eID, randVelX, randVelY); /// Velocity(x - randPosX, y - randPosY)
            registry.add<tag>(eID, std::string("Entity - " + std::to_string(eID))); /// Tag(Entity - i)

        }
    }
    auto print_stats = [](
        component_handle<transform>& transform,
        component_handle<velocity>& velocity,
        component_handle<tag>& tag) -> void
        {
            printf("%s's components\n", tag.tag().c_str());
            printf("    Transform component(x = %f, y = %f, w = %f, h = %f)\n", transform.x(), transform.y(), transform.w(), transform.h());
            printf("    Velocity component(x = %f, y = %f)\n", velocity.x(), velocity.y());
        };

    bool running = true;

    while (running)
    {
        /////////////////////////////////
        ///// Batch processing
        /////////////////////////////////

        registry.for_each<transform, velocity, tag>([print_stats](
            entity_id _,
            component_handle<transform> transform,
            component_handle<velocity> velocity,
            component_handle<tag> tag)
            {
                print_stats(transform, velocity, tag);

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
                printf("\n");

            });

        printf("\n\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
        /// Your rendering code here
    }
    return 0;
}

#pragma endregion
