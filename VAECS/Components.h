#pragma once
#include "Common.h"
#include "utils.h"
 
	using namespace utils::ComponentHelpers;

#pragma region Base
	struct ComponentCounter
	{
		static int counter;
	};

	inline int ComponentCounter::counter = 0;

	/// All components must inherit from base class, otherwise will assert
	template<typename C>
	struct BaseComponent
	{
		/// Returns component unique identifier
		static inline int family()
		{
			static int family = ComponentCounter::counter++;
			return family;
		}

	};

	/// Used to identify the index of the component manager 
	/// in the component manager vector in the world
	template<typename C>
	static size_t GetComponentFamily()
	{
		return BaseComponent<typename std::remove_const<C>::type>::family();
	}
#pragma endregion

	/// IMPORTANT: Components must follow the provided template below for ecs to work properly

	struct Transform : BaseComponent<Transform>
	{
		Transform(float x, float y, float w, float h)
			: x(x)
			, y(y)
			, w(w)
			, h(h)
		{}
		/// Data
		float x = {};
		float y = {};
		float w = {};
		float h = {};
	};

	template<> struct GetMemberCount<Transform>
	{
		static const size_t count = 4;
	};

	template<> struct GetType<Transform, 0>
	{
		using Type = float;
	};

	template<> struct GetType<Transform, 1>
	{
		using Type = float;
	};

	template<> struct GetType<Transform, 2>
	{
		using Type = float;
	};

	template<> struct GetType<Transform, 3>
	{
		using Type = float;
	};

	template<> typename GetPointerToMemeberType<Transform, 0>::Type utils::ComponentHelpers::GetPointerToMemeber<Transform, 0>() { return &Transform::x; }
	template<> typename GetPointerToMemeberType<Transform, 1>::Type utils::ComponentHelpers::GetPointerToMemeber<Transform, 1>() { return &Transform::y; }
	template<> typename GetPointerToMemeberType<Transform, 2>::Type utils::ComponentHelpers::GetPointerToMemeber<Transform, 2>() { return &Transform::w; }
	template<> typename GetPointerToMemeberType<Transform, 3>::Type utils::ComponentHelpers::GetPointerToMemeber<Transform, 3>() { return &Transform::h; }


	struct Velocity : BaseComponent<Velocity>
	{
		Velocity(float x, float y)
			: x(x)
			, y(y)
		{}
		/// Data
		float x = {};
		float y = {};
	};

	template<> struct GetMemberCount<Velocity>
	{
		static const size_t count = 2;
	};

	template<> struct GetType<Velocity, 0>
	{
		using Type = float;
	};

	template<> struct GetType<Velocity, 1>
	{
		using Type = float;
	};

	template<> typename GetPointerToMemeberType<Velocity, 0>::Type utils::ComponentHelpers::GetPointerToMemeber<Velocity, 0>() { return &Velocity::x; }
	template<> typename GetPointerToMemeberType<Velocity, 1>::Type utils::ComponentHelpers::GetPointerToMemeber<Velocity, 1>() { return &Velocity::y; }
	

	struct Color : public BaseComponent<Color>
	{
		Color(char r, char g, char b, char a)
			: r(r)
			, g(g)
			, b(b)
			, a(a)
		{}
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

	template<> typename GetPointerToMemeberType<Color, 0>::Type utils::ComponentHelpers::GetPointerToMemeber<Color, 0>() { return &Color::r; }
	template<> typename GetPointerToMemeberType<Color, 1>::Type utils::ComponentHelpers::GetPointerToMemeber<Color, 1>() { return &Color::g; }
	template<> typename GetPointerToMemeberType<Color, 2>::Type utils::ComponentHelpers::GetPointerToMemeber<Color, 2>() { return &Color::b; }
	template<> typename GetPointerToMemeberType<Color, 3>::Type utils::ComponentHelpers::GetPointerToMemeber<Color, 3>() { return &Color::a; }


