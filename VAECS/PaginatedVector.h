#include "Common.h"

template<typename T>
class PaginatedVector
{
public:
	PaginatedVector() = default;

	T& operator[](size_t index)
	{
		size_t pageIndex = index / PAGE_SIZE;
		size_t offset = index % PAGE_SIZE;

		if (pageIndex >= pages.size())
		{
			pages.resize(pageIndex + 1);
		}

		return pages[pageIndex][offset];
	}

	bool exists(size_t index) const
	{
		size_t pageIndex = index / PAGE_SIZE;
		size_t offset = index % PAGE_SIZE;

		if (pageIndex >= pages.size())
		{
			return false;
		}

		return true;
	}

	void remove(size_t index)
	{
		size_t pageIndex = index / PAGE_SIZE;
		size_t offset = index % PAGE_SIZE;

		if (pageIndex < pages.size())
		{
			pages[pageIndex][offset] = T{};
		}
	}

private:
	std::vector<std::array<T, PAGE_SIZE>> pages;
};