#pragma once

#include "game.h"

// Return an array of steps to get from source location (s) to target location
// (t). The length of the array is returned in the passes size_t pointer (l).
struct path* path_find(const struct maze* m, struct location s,
                       struct location t);
