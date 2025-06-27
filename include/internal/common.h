#pragma once
#include <unordered_map>
#include <assert.h>
#include <bitset>
#include <vector>

using entity_id = std::size_t;
using component_instance = std::size_t;

extern const size_t g_max_entities;
inline static size_t g_container_size = g_max_entities + 1;

