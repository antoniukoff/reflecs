#pragma once
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <assert.h>
#include <functional>
#include <bitset>
#include <vector>
#include <optional>

using entity_id = std::size_t;
using component_instance = std::size_t;

constexpr size_t g_max_entities = 50000;
constexpr size_t g_container_size = g_max_entities + 1;

