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
#include "utils.h"
#include <optional>

using EntityID = std::size_t;
using ComponentInstance = std::size_t;

constexpr size_t MAX_COMPONENTS_SIZE = 2;
constexpr size_t MAX_ENTITIES = 50000;
constexpr size_t CONTAINER_SIZE = MAX_ENTITIES + 1;
constexpr size_t PAGE_SIZE = 1024;

using Signature = std::bitset<MAX_COMPONENTS_SIZE>;
