#pragma once
#include <string>
#include <iostream>
#include "Common.h"
namespace utils
{
	namespace CompileLoop
	{
		template<size_t numberofiterations, template<size_t index> typename FunctionToExecuteWrapperClass, typename Parent, typename ... Args>
		struct ForEach
		{
			template<size_t index>
			static void loop(Parent* parent, Args&& ... args)
			{
				if constexpr (index >= numberofiterations)
				{
					return;
				}
				else
				{
					FunctionToExecuteWrapperClass<index> wrapper = FunctionToExecuteWrapperClass<index>();

					wrapper(parent, args...);

					ForEach<numberofiterations, FunctionToExecuteWrapperClass, Parent, Args ...>::template loop<index + 1>(parent, args...);
				}
			}
		};

		template<size_t numberofiterations, template<size_t N> typename FunctionToExecuteWrapperClass, typename Parent, typename ... Args>
		void execute(Parent* parent, Args&& ... args)
		{
			ForEach<numberofiterations, FunctionToExecuteWrapperClass, Parent, Args...>::template loop<0>(parent, args...); // index of starting Pos?
		}
	}

	namespace ComponentHelpers
	{
		template<typename... Ts>
		struct TypeList {};

		template<size_t Index, typename List>
		struct GetTypeAtIndex;

		/// Recursively get the desired type based on the index until it hits the 0th base case
		template<size_t Index, typename Head, typename... Tail>
		struct GetTypeAtIndex<Index, TypeList<Head, Tail...>>
		{
			using Type = typename GetTypeAtIndex<Index - 1, TypeList<Tail...>>::Type;
		};

		/// Base case
		template<typename Head, typename... Tail>
		struct GetTypeAtIndex<0, TypeList<Head, Tail...>>
		{
			using Type = Head;
		};

		/// Get type of component at index
		template<size_t Index, typename ... Ts>
		using ComponentTypeAtIndex = typename GetTypeAtIndex<Index, TypeList<Ts...>>::Type;

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
		typename GetPointerToMemeberType<T, N>::Type GetPointerToMemeber() {};
	}
}


///////////////////////////////////////////////////////
///						TESTS
///////////////////////////////////////////////////////

struct PrintMessages
{
private:
	std::string message = "Hello";

	void printMassage(std::string customMessage)
	{
		std::cout << message + customMessage << std::endl;
	}

	template<size_t N>
	struct PrintMessagesWrapper
	{
		void operator()(PrintMessages* parent, std::string message)
		{
			parent->printMassage(message);
		}
	};

	void printNumber(size_t number)
	{
		std::cout << message + " " + std::to_string(number) << std::endl;
	}

	template<size_t N>
	struct PrintNumberWrapper
	{
		void operator()(PrintMessages* parent, size_t number)
		{
			parent->printNumber(number);
		}
	};

public:

	template<size_t numofIter>
	void printMessages(std::string messageToPrint)
	{
		utils::CompileLoop::execute<numofIter, PrintMessagesWrapper>(this, messageToPrint);
	}

	template<size_t numofIter>
	void printNumbers(size_t numbers)
	{
		utils::CompileLoop::execute<numofIter, PrintNumberWrapper>(this, numbers);
	}
};

///////////////////////////////////////////////////////
///						TESTS
///////////////////////////////////////////////////////
