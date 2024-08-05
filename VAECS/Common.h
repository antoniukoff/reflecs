#pragma once
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <assert.h>
#include <functional>
#include "utils.h"

using EntityID = std::size_t;
using ComponentInstance = std::size_t;

constexpr size_t MAX_COMPONENTS_SIZE = 32;
constexpr size_t MAX_ENTITIES = 5000;
constexpr size_t CONTAINER_SIZE = MAX_ENTITIES + 1;