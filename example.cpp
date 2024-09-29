#include <random>
#include "include/registry.h"

using namespace reflecs::component_reflection;

#pragma region HOW TO DEFINE A COMPONENT

    struct vec2
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    /// Define a component struct
    struct transform
    {
        transform(float x, float y, float w, float h)
            : x(x)
            , y(y)
            , w(w)
            , h(h)
        {}

        float x, y, w, h;
    };

    /// IMPORTANT: The following specializations are required for the component helpers to work

    template<> struct get_member_count<transform>
    {
        static const int count = 4; /// Number of fields in the component
    };

    /// Partialy pecialize the get_type struct for all the members of the component
	/// The first template parameter is the component type and the second is the index of the member
	/// type is the type of the member at the specified index
	/// e.g. get_type<transform, 0>::type is x which is a float

    template<> struct get_type<transform, 0>
    {
        using type = float;
    };

    template<> struct get_type<transform, 1>
    {
        using type = float;
    };

    template<> struct get_type<transform, 2>
    {
        using type = float;
    };

    template<> struct get_type<transform, 3>
    {
        using type = float;
    };

    /// Specialize the get_pointer_to_member function for all the members of the component

    template<> inline typename get_pointer_to_member_type<transform, 0>::type reflecs::component_reflection::get_pointer_to_member<transform, 0>() { return &transform::x; }
    template<> inline typename get_pointer_to_member_type<transform, 1>::type reflecs::component_reflection::get_pointer_to_member<transform, 1>() { return &transform::y; }
    template<> inline typename get_pointer_to_member_type<transform, 2>::type reflecs::component_reflection::get_pointer_to_member<transform, 2>() { return &transform::w; }
    template<> inline typename get_pointer_to_member_type<transform, 3>::type reflecs::component_reflection::get_pointer_to_member<transform, 3>() { return &transform::h; }

	/// IMPORTANT: The following specialization for the component_handle struct is required to get the member handles

    template<>
    struct component_handle<transform>
    {
        component_manager<transform>& pool;
		component_instance instance; /// The instance of the component for the entity
        
		/// NOTE: component_manager needs to be passed as a reference to the constructor
        component_handle(component_manager<transform>& transform_pool, component_instance instance)
            : pool(transform_pool)
            , instance(instance)
        {}
        
		/// Define methods to access the members of the component
		/// The get_member_buffer function is used to get the buffer of the member at the specified index
        inline float& x() { return pool.get_member_buffer<0>(instance); }
        inline float& y() { return pool.get_member_buffer<1>(instance); }
        inline float& w() { return pool.get_member_buffer<2>(instance); }
        inline float& h() { return pool.get_member_buffer<3>(instance); }
    };

	/// Other components can be defined in a similar way
    struct velocity
    {
        velocity(float x, float y)
            : x(x)
            , y(y)
        {}

        float x, y;
    };

    template<> struct get_member_count<velocity>
    {
        static const int count = 2;
    };

    template<> struct get_type<velocity, 0>
    {
        using type = float;
    };

    template<> struct get_type<velocity, 1>
    {
        using type = float;
    };

    template<> inline typename get_pointer_to_member_type<velocity, 0>::type reflecs::component_reflection::get_pointer_to_member<velocity, 0>() { return &velocity::x; }
    template<> inline typename get_pointer_to_member_type<velocity, 1>::type reflecs::component_reflection::get_pointer_to_member<velocity, 1>() { return &velocity::y; }

    template<>
    struct component_handle<velocity>
    {
        component_manager<velocity>& pool;
        component_instance instance;

        component_handle(component_manager<velocity>& transform_pool, component_instance instance)
            : pool(transform_pool)
            , instance(instance)
        {}

        inline float& x() { return pool.get_member_buffer<0>(instance); }
        inline float& y() { return pool.get_member_buffer<1>(instance); }
    };

    struct color_component
    {
        color_component(char r, char g, char b, char a)
            : r(r)
            , g(g)
            , b(b)
            , a(a)
        {}

        char r, g, b, a;
    };

    template<> struct get_member_count<color_component>
    {
        static const int count = 4;
    };

    template<> struct get_type<color_component, 0>
    {
        using type = char;
    };

    template<> struct get_type<color_component, 1>
    {
        using type = char;
    };

    template<> struct get_type<color_component, 2>
    {
        using type = char;
    };

    template<> struct get_type<color_component, 3>
    {
        using type = char;
    };

    template<> inline typename get_pointer_to_member_type<color_component, 0>::type reflecs::component_reflection::get_pointer_to_member<color_component, 0>() { return &color_component::r; }
    template<> inline typename get_pointer_to_member_type<color_component, 1>::type reflecs::component_reflection::get_pointer_to_member<color_component, 1>() { return &color_component::g; }
    template<> inline typename get_pointer_to_member_type<color_component, 2>::type reflecs::component_reflection::get_pointer_to_member<color_component, 2>() { return &color_component::b; }
    template<> inline typename get_pointer_to_member_type<color_component, 3>::type reflecs::component_reflection::get_pointer_to_member<color_component, 3>() { return &color_component::a; }

    template<>
    struct component_handle<color_component>
    {
        component_manager<color_component>& pool;
        component_instance instance;

        component_handle(component_manager<color_component>& transform_pool, component_instance instance)
            : pool(transform_pool)
            , instance(instance)
        {}

        inline char& r() { return pool.get_member_buffer<0>(instance); }
        inline char& g() { return pool.get_member_buffer<1>(instance); }
        inline char& b() { return pool.get_member_buffer<2>(instance); }
        inline char& a() { return pool.get_member_buffer<3>(instance); }
    };
#pragma endregion

void generateEntityWithRectangle(registry<transform, velocity, color_component>& registry)
{
    static std::random_device randomEngine;
    static std::uniform_real_distribution<float> randomGenerator(0, 800);

    static std::random_device randomEngine1;
    static std::uniform_int_distribution<int> randomGenerator1(0, 255);

    int randPosX = std::clamp(randomGenerator(randomEngine), 0.0f, 800.0f - 50.0f);
    int randPosY = std::clamp(randomGenerator(randomEngine), 0.0f, 600.0f - 50.0f);

    auto eID = registry.create_entity();
    registry.add<transform>(eID, randPosX, randPosY, 50, 50);
    registry.add<velocity>(eID, randPosX, randPosY);
    registry.add<color_component>(eID, randomGenerator1(randomEngine1), randomGenerator1(randomEngine1), randomGenerator1(randomEngine1), randomGenerator1(randomEngine1));
}

int main(int argc, char* argv[])
{
    registry<transform, velocity, color_component> registry;

    for (size_t i = 0; i < g_max_entities; ++i) 
    {
        generateEntityWithRectangle(registry);
    }
    
    bool running = true;

    while (running) 
    {
        registry.for_each<transform, velocity>([](entity_id _, component_handle<transform> transform, component_handle<velocity> velocity)
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

		/// Your rendering code here
    }
    return 0;
}
