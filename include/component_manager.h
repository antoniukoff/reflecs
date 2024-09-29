#pragma once

#include "Common.h"
#include "utility.h"


/**
 * @class component_pool
 *
 * @brief Represents the compotent pool data
 *
 * @tparam C Component type
 * @tparam elements Number of elements in the component
 */
template<typename C, size_t elements>
struct component_pool
{
	size_t size = 1; // first available element in the array starts at 1; 0 reserved for error handling
	void* buffer[elements];
	~component_pool()
	{
		delete buffer[0];
	}
};

/**
 * @class component_handle
 *
 * @brief This class is responsible for accessing the component fields
 *
 * @tparam C Component type
 */
template<typename C>
class component_handle;

/**
 * @class component_manager
 *
 * @brief This class is responsible for pooling the fields of an component,
 *		  assigning a free component instance to an entity and also removing
 *		  it from this instance
 *
 * @tparam C Component type
 */
template<typename C>
class component_manager
{
private:
	static constexpr size_t member_count = reflecs::component_reflection::get_member_count<C>::count;
	component_pool<C, member_count> m_component_pool;
	std::vector<component_instance> m_entities_to_components;

public:

	component_manager()
		: m_entities_to_components(g_max_entities)
	{
		size_t packed_component_size = 0;
		reflecs::constexpr_loop::execute<member_count, count_component_size_wrapper>(this, packed_component_size);

		size_t bytes = g_container_size * packed_component_size;
		m_component_pool.buffer[0] = malloc(bytes);
		reflecs::constexpr_loop::execute<member_count - 1, generate_buffers_wrapper>(this, m_component_pool.buffer, g_container_size);
	}

	/*
	* @brief Adds the component to the field pools
	*
	* @tparam ...Args Arguments to pass to the constructor of the component
	* @param e_id Entity ID
	* @param args Arguments to pass to the constructor of the component
	*/
	template<typename ... Args>
	component_instance& add(entity_id e_id, Args&& ... args)
	{
		/// Get the available instance in the pool
		component_instance instance_to_add = m_entities_to_components[e_id];
		if (instance_to_add == 0)
		{
			instance_to_add = m_component_pool.size;
			m_entities_to_components[e_id] = instance_to_add;
			m_component_pool.size++;

		}
		C component = C(std::forward<Args>(args)...);

		/// Add the component data to the member pools at their new instance
		reflecs::constexpr_loop::execute<member_count, add_component_data_wrappper>(this, instance_to_add, component);

		return instance_to_add;
	}

	/**
	 * @brief Maps the entity to the component instance
	 * @param e_id Entity ID
	 * @return Component instance
	*/
	component_instance look_up(entity_id e_id)
	{
		return m_entities_to_components[e_id];
	}

	/**
	* @brief Retrieves the component handle that accesses the component fields
	*
	* @param e_id Entity ID
	*/
	component_handle<C> retrieve(entity_id e_id)
	{
		return component_handle<C>(*this, look_up(e_id));
	}

	/*
	* @brief Returns the fields buffer
	*
	* @tparam index Index of the member in the component by order
	* @param component_instance instance of the component
	*/
	template<size_t index>
	auto& get_member_buffer(entity_id component_instance)
	{
		using data_type = typename reflecs::component_reflection::get_type<C, index>::type;

		std::array<data_type, g_container_size>& arr = *static_cast<std::array<data_type, g_container_size>*>(m_component_pool.buffer[index]);
		return arr[component_instance];
	}

	/**
	* @brief Removes the component from the pool
	*
	* @param e_id Entity ID
	*/
	void remove(entity_id e_id)
	{
		/// Find the component instance to remove
		component_instance instance_to_remove = m_entities_to_components[e_id];
		assert(instance_to_remove > 0 && "Entity is not assigned to this component");
		assert(instance_to_remove < g_container_size && "instance is out of range");

		/// If exists, iterate over all members and reassign the last component data to the position of the removing instance  
		component_instance instance_to_reassign = m_component_pool.size - 1;
		reflecs::constexpr_loop::execute<member_count, remove_component_data_wrapper>(this, instance_to_remove, instance_to_reassign);

		/// Assign the entity's instance to 0
		m_entities_to_components[e_id] = 0;

		/// Decrease the pool size
		m_component_pool.size--;

		if (instance_to_remove == instance_to_reassign)
		{
			return;
		}

		/// Find the entity ID that has the last component instance assigned to it and change it to the removed instance
		auto it = std::find_if(m_entities_to_components.begin(), m_entities_to_components.end(), [instance_to_reassign](const auto component_instance)
			{
				return component_instance == instance_to_reassign;
			}
		);

		if (it != m_entities_to_components.end())
		{
			*it = instance_to_remove;
		}
	}

private:

#pragma region CompileHelpers
	/**
	 * @brief Generates field buffers for each member of the component
	 * @tparam index Index of the component member
	 * @param buffer Pointer to the buffer
	 * @param num_elements Size of the buffer
	 */
	template<size_t index>
	void generate_buffers(void* buffer[], size_t num_elements)
	{
		using data_type = typename reflecs::component_reflection::get_type<C, index>::type;
		buffer[index + 1] = static_cast<data_type*>(buffer[index]) + num_elements;

		char* previous_address = (char*)buffer[index];
		char* current_address = (char*)buffer[index + 1];

		size_t byte_count = current_address - previous_address; // debugging
		assert(byte_count == sizeof(data_type) * g_container_size && "Check member offset");
	}

	/**
	 * @brief Dummy struct to call the generate_buffers function
	 * @tparam index Member index in the component
	 *
	*/
	template<size_t index>
	struct generate_buffers_wrapper
	{
		void operator()(component_manager<C>* manager, void* buffer[], size_t num_elements)
		{
			manager->generate_buffers<index>(buffer, num_elements);
		}
	};

	/**
	 * @brief Counts the byte size of the component
	 * @tparam index member index in the component
	 * @param bytes Total byte size of the component
	*/
	template<size_t index>
	void count_component_size(size_t& bytes)
	{
		bytes += sizeof(typename reflecs::component_reflection::get_type<C, index>::type);
	}

	/**
	* @brief Dummy struct to call the count_component_size function
	* @tparam index Member index in the component
	*/
	template<size_t index>
	struct count_component_size_wrapper
	{
		void operator()(component_manager<C>* manager, size_t& bytes)
		{
			manager->count_component_size<index>(bytes);
		}
	};

	/**
	 * @brief Dummy struct to call the add_component_data function
	 * @tparam index Member index in the component
	*/
	template<size_t index>
	struct add_component_data_wrappper
	{
		void operator()(component_manager<C>* mgr, component_instance instance_to_add, C& component)
		{
			mgr->add_component_data<index>(instance_to_add, component);
		}
	};

	/**
	* @brief Adds the component data to the field pool
	* @tparam index Index of the member in the component
	* @param instance_to_add instance of the component
	* @param component Constructed component
	*/
	template<size_t index>
	void add_component_data(component_instance instance_to_add, C& component)
	{
		using data_type = typename reflecs::component_reflection::get_type<C, index>::type;

		std::array<data_type, g_container_size>& array_handle = *static_cast<std::array<data_type, g_container_size>*>(m_component_pool.buffer[index]);

		array_handle[instance_to_add] = component.*reflecs::component_reflection::get_pointer_to_member<C, index>();
	}

	/**
	 * @brief Dummy struct to call the removeComponentData function
	 * @tparam index Member index in the component
	*/
	template<size_t index>
	struct remove_component_data_wrapper
	{
		void operator()(component_manager<C>* mgr, component_instance instance_to_remove, component_instance replacing_instance)
		{
			mgr->remove_component_data<index>(instance_to_remove, replacing_instance);
		}
	};

	/**
	 * @brief Removes the component data from the field pool
	 * @tparam index Index of the member in the component
	 * @param instance_to_remove instance of the component
	*/
	template<size_t index>
	void remove_component_data(component_instance instance_to_remove, component_instance replacing_instance)
	{
		using data_type = typename reflecs::component_reflection::get_type<C, index>::type;
		std::array<data_type, g_container_size>& array_handle = *static_cast<std::array<data_type, g_container_size>*>(m_component_pool.buffer[index]);

		array_handle[instance_to_remove] = array_handle[replacing_instance];
	}
#pragma endregion

};

