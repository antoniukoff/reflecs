#pragma once

#include "Components.h"
#include "ComponentPool.h"

template<typename C>
class ComponentHandle;

template<>
class ComponentHandle<Transform>
{
public:
	ComponentPool<Transform>& mgr;
	ComponentInstance instance;

public:
	ComponentHandle() = default;

	ComponentHandle(ComponentPool<Transform>& mgr, ComponentInstance instance)
		: mgr(mgr)
		, instance(instance)
	{}

	inline float& x() { return mgr.getMemberBuffer<0>(instance); }
	inline float& y() { return mgr.getMemberBuffer<1>(instance); }
	inline float& w() { return mgr.getMemberBuffer<2>(instance); }
	inline float& h() { return mgr.getMemberBuffer<3>(instance); }

};

template<>
class ComponentHandle<Color>
{
public:
	ComponentPool<Color>& mgr;
	ComponentInstance instance;

public:
	ComponentHandle() = default;

	ComponentHandle(ComponentPool<Color>& mgr, ComponentInstance instance)
		: mgr(mgr)
		, instance(instance)
	{}

	inline char& r() { return mgr.getMemberBuffer<0>(instance); }
	inline char& g() { return mgr.getMemberBuffer<1>(instance); }
	inline char& b() { return mgr.getMemberBuffer<2>(instance); }
	inline char& a() { return mgr.getMemberBuffer<3>(instance); }
};

template<>
class ComponentHandle<Velocity>
{
public:
	ComponentPool<Velocity>& mgr;
	ComponentInstance instance;

public:
	ComponentHandle() = default;

	ComponentHandle(ComponentPool<Velocity>& mgr, ComponentInstance instance)
		: mgr(mgr)
		, instance(instance)
	{}

	inline float& x() { return mgr.getMemberBuffer<0>(instance); }
	inline float& y() { return mgr.getMemberBuffer<1>(instance); }
};
