#pragma once
#include <string>
#include <iostream>
#include "common.h"

namespace utils
{
	namespace compile_loop
	{
		template<size_t Iterations, template<size_t Index> typename FunctionToExecuteWrapperClass, typename Parent, typename ... Args>
		struct for_each
		{
			template<size_t Index>
			static void loop(Parent* parent, Args&& ... args)
			{
				if constexpr (Index >= Iterations)
				{
					return;
				}
				else
				{
					FunctionToExecuteWrapperClass<Index> wrapper = FunctionToExecuteWrapperClass<Index>();

					wrapper(parent, args...);

					for_each<Iterations, FunctionToExecuteWrapperClass, Parent, Args ...>::template loop<Index + 1>(parent, args...);
				}
			}
		};

		template<size_t Iterations, template<size_t N> typename FunctionToExecuteWrapperClass, typename Parent, typename ... Args>
		void execute(Parent* parent, Args&& ... args)
		{
			for_each<Iterations, FunctionToExecuteWrapperClass, Parent, Args...>::template loop<0>(parent, args...); // Index of starting Pos?
		}
	}

	namespace component_helpers
	{
		template<typename... Ts>
		struct type_list {};

		template<size_t Index, typename List>
		struct get_type_at_index;

		/// Recursively get the desired type based on the index until it hits the 0th base case
		template<size_t Index, typename Head, typename... Tail>
		struct get_type_at_index<Index, type_list<Head, Tail...>>
		{
			using type = typename get_type_at_index<Index - 1, type_list<Tail...>>::type;
		};

		/// Base case
		template<typename Head, typename... Tail>
		struct get_type_at_index<0, type_list<Head, Tail...>>
		{
			using type = Head;
		};

		/// Get type of component at index
		template<size_t Index, typename ... Ts>
		using component_type_at_index = typename get_type_at_index<Index, type_list<Ts...>>::type;

		/// Compile-Time field count
		template<typename ComponentType>
		struct get_member_count;

		/// Compile-Time field type based on its position within the struct
		template<typename ComponentType, size_t N>
		struct get_type;

		/// Helps identify the correct type of the pointer of the field within a class
		template<typename T, size_t N>
		struct get_pointer_to_member_type
		{
			using type = get_type<T, N>::type T::*;// pointer to member
		};

		/// Used to get the handle to the member within the pool
		template<typename T, size_t N>
		typename get_pointer_to_member_type<T, N>::type get_pointer_to_member() {};
	}
}