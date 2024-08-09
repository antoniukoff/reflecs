#include <bitset>
#include "Components.h"
class ComponentMask
{
	std::bitset<2> mask;

public:
	template<typename T>
	void addComponent()
	{
		mask.set(GetComponentFamily<T>(), true);
	}

	template<typename T>
	void removeComponent()
	{
		mask.set(GetComponentFamily<T>(), false);
	}

	void reset()
	{
		mask.reset();
	}

};



