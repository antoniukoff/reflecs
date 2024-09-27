
# Reflecs

## Overview
Reflecs is my personal take on building a fast and efficient ECS (Entity Component System) header-only framework for C++. 

## Why I Built This
I built Reflecs primarily as a learning project to better understand how data layout affects application performance. I aimed to create something fast and lightweight that met my needs while avoiding the overhead of runtime polymorphism and data scattering. The end goal was to maximize cache efficiency and provide a simple, user-friendly interface for future projects.

## Key Features
- **Header-Only Library**: Just include the headers and you’re ready to go.
- **Compile-Time Reflection**: Components are analyzed and managed at compile time leveraging C++ 17 features and metaprogramming.
- **Sparse Sets**: O(1) data unpacking, addition, and removal, even with a large number of entities.
- **Field-Level SOA**: Component's fields are stored in their own contiguous memory pool, which improves cache locality and performance when accessing components.
- **No Run-Time:** The framework has no run-time.
## Requirements

- **C++17 or later**: Reflecs requires a modern C++ compiler.
- **No external dependencies**: Just drop it into your project.


## Installation

To integrate Reflecs into your project, clone the repository into your project directory:

```bash
git clone https://github.com/antoniukoff/reflecs.git
```
Or simply download it from under the ```Releases``` section.

Then include the main header in your project:

```cpp
#include "reflecs(-main)/include/registry.h"
```
## Usage

### Defining Components

Here's an example of a simple `health_component` with two fields: `health` and `max_health`.

```cpp
// components.cpp
struct health_component
{
    int health;
    int max_health;

    health_component(int h, int max_h) : health(h), max_health(max_h) {}
};

// Specialize get_member_count for reflection
template<> struct get_member_count<health_component> { static const int count = 2; };

// Specialize get_type for each field
template<> struct get_type<health_component, 0> { using type = int; };
template<> struct get_type<health_component, 1> { using type = int; };

// Provide pointers to the component's members
template<> inline typename get_pointer_to_member_type<movement_component, 0>::type get_pointer_to_member<health_component, 0>() { return &health_component::health; }
template<> inline typename get_pointer_to_member_type<movement_component, 1>::type get_pointer_to_member<health_component, 1>() { return &health_component::max_health; }
```
> **Note:** Dynamic types such as `std::vector`, `std::string`, or other heap-allocated types are not directly supported. However, you can use pointers to dynamic types within components if necessary.

### Managing Entities and Components

Registry provides the following methods to manage entities:

- **`create`**: Creates an entity.
- **`add`**: Adds a component to an entity.
- **`remove`**: Removes a component from an entity.
- **`destroy`**: Deletes an entity and all its components.
- **`unpack`**: Unpacks multiple components for an entity using tuple-like syntax.

#### Creating an entity

To ```create``` an entity simply follow:

```cpp
auto entity = registry.create_entity(); // Returns a new entity id
```

> **Note:** You can set maximum number entities in the ```common.h``` file for large scale environments

#### Adding Components

To ```add``` a component to an entity:

```cpp
registry.add<health_component>(entity, 100, 100);  // Adds a health_component with 100 health and 100 max_health
```

#### Removing Components

To ```remove``` a component from an entity:

```cpp
registry.remove<health_component>(entity);  // Removes the health_component from the entity

```

#### Destroying Entities

To ```delete``` an entity and all its associated components:

```cpp
registry.destroy(entity);  // Destroys the entity and all its components
```

#### Unpacking Components

You can ```unpack``` multiple components from a single entity using the tuple-like syntax:

```cpp
auto [hc, vc] = registry.unpack<health_component, velocity_component>(entity);

// Modify component values
hc.health() -= 10;
vc.x() += 1.0f;

```

#### Processing Entities

The ```for_each``` method lets you iterate over entities that have at least the specified set of components. Here’s an example where we reduce the health of each entity:

```cpp
registry.for_each<health_component>([](entity_id id, component_handle<health_component> hc)
{
    hc.health() -= 10;  // Reduce health by 10
    if (hc.health() <= 0)
    {
        // Handle entity death
    }
});
```




