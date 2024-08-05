#pragma once
#include "Common.h"

#pragma region ComponentHelpers

	/// Compile-Time field count
	template<typename ComponentType>
	struct GetMemberCount;

	/// Compile-Time field type based on its position within the struct
	template<typename ComponentType, size_t N>
	struct GetType;

	/// Helps identify the correct type of the pointer of the field within a class
	template<typename T, size_t N>
	struct GetPointerToMemeberType
	{
		using Type = GetType<T, N>::Type T::*;// pointer to member
	};
	
	/// Used to get the handle to the member within the pool
	template<typename T, size_t N>
	static typename GetPointerToMemeberType<T, N>::Type getPointerToMemeber() {};

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
	static int getComponentFamily()
	{
		return BaseComponent<typename std::remove_const<C>::type>::family();
	}

#pragma endregion

#pragma region Components

	/// <summary>
	/// Components must follow the provided template below for ecs to work properly
	/// </summary>

	struct Transform : BaseComponent<Transform>
	{
		Transform(int x, int y, int w, int h)
			: x(x)
			, y(y)
			, w(w)
			, h(h)
		{}
		/// Data
		int x = {};
		int y = {};
		int w = {};
		int h = {};
	};

	template<> struct GetMemberCount<Transform>
	{
		static const size_t count = 4;
	};

	template<> struct GetType<Transform, 0>
	{
		using Type = int;
	};

	template<> struct GetType<Transform, 1>
	{
		using Type = int;
	};

	template<> struct GetType<Transform, 2>
	{
		using Type = int;
	};

	template<> struct GetType<Transform, 3>
	{
		using Type = int;
	};

	template<> typename GetPointerToMemeberType<Transform, 0>::Type getPointerToMemeber<Transform, 0>() { return &Transform::x; }
	template<> typename GetPointerToMemeberType<Transform, 1>::Type getPointerToMemeber<Transform, 1>() { return &Transform::y; }
	template<> typename GetPointerToMemeberType<Transform, 2>::Type getPointerToMemeber<Transform, 2>() { return &Transform::w; }
	template<> typename GetPointerToMemeberType<Transform, 3>::Type getPointerToMemeber<Transform, 3>() { return &Transform::h; }

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

	template<> typename GetPointerToMemeberType<Color, 0>::Type getPointerToMemeber<Color, 0>() { return &Color::r; }
	template<> typename GetPointerToMemeberType<Color, 1>::Type getPointerToMemeber<Color, 1>() { return &Color::g; }
	template<> typename GetPointerToMemeberType<Color, 2>::Type getPointerToMemeber<Color, 2>() { return &Color::b; }
	template<> typename GetPointerToMemeberType<Color, 3>::Type getPointerToMemeber<Color, 3>() { return &Color::a; }

#pragma endregion