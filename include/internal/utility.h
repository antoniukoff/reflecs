#pragma once

namespace reflecs
{
	namespace constexpr_loop
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

	namespace component_reflection
	{
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
			using type = typename get_type<T, N>::type T::*;// pointer to member
		};

		/// Used to get the handle to the member within the pool
		template<typename T, size_t N>
		typename get_pointer_to_member_type<T, N>::type get_pointer_to_member() {};
	}
	namespace type_utils
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

		/**
	 * @brief Returns the index of the component in the component list
	 * @tparam C Component
	 * @tparam Head First component in the list
	 * @tparam Tail Remaining components
	 * @param index Index of the component
	*/
		template<typename C, typename Head, typename ... Tail>
		constexpr size_t get_component_type_id(size_t index = 0)
		{
			if constexpr (std::is_same<C, Head>::value)
			{
				return index;
			}
			else
			{
				if constexpr (sizeof...(Tail) > 0)
				{
					return get_component_type_id<C, Tail...>(++index);
				}
				else
				{
					return -1; /// compile-error
				}
			}
		}
	}
}

#define ANNOTATE(ComponentName, MemberCount, ...)											\
																								\
		template<> struct reflecs::component_reflection::get_member_count<ComponentName>			\
		{																						\
			static const size_t count = MemberCount;											\
		};																						\
																								\
		__VA_ARGS__																				\
																								\

#define DEFINE_COMPONENT_MEMBER(ComponentName, Index, MemberType, MemberName)				\
		template<> struct reflecs::component_reflection::get_type<ComponentName, Index>			\
		{																						\
			using type = MemberType;															\
		};																						\
																								\
	template<> inline typename reflecs::component_reflection::get_pointer_to_member_type<ComponentName, Index>::type   \
							   reflecs::component_reflection::get_pointer_to_member<ComponentName, Index>()   \
							   {																		    \
									return &ComponentName::MemberName;									    \
							   }																		    \
																											\


#define COMPONENT_HANDLE_ACCESSOR(index, type, name) \
		inline type& name() { return mgr.get_member_buffer<index>(e_id); }\


#define DEFINE_COMPONENT_HANDLE(ComponentType, ...)					   \
	template<>                                                         \
	class component_handle<ComponentType>                              \
	{                                                                  \
	public:                                                            \
		component_manager<ComponentType>& mgr;                         \
		entity_id e_id;	     										   \
																	   \
	public:                                                            \
		component_handle() = default;                                  \
																	   \
		component_handle(component_manager<ComponentType>& mgr,        \
						entity_id e_id)								   \
			: mgr(mgr), e_id(e_id) {}								   \
																	   \
																	   \
		__VA_ARGS__                                                    \
	};

