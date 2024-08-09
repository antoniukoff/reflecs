#pragma once
#include <string>
#include <iostream>
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
		CompileLoop::execute<numofIter, PrintMessagesWrapper>(this, messageToPrint);
	}

	template<size_t numofIter>
	void printNumbers(size_t numbers)
	{
		CompileLoop::execute<numofIter, PrintNumberWrapper>(this, numbers);
	}
};

///////////////////////////////////////////////////////
///						TESTS
///////////////////////////////////////////////////////
