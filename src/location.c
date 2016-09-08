#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "game.h"

double
location_distance(struct location loc1, struct location loc2)
{
  const int32_t diff_x = ((int32_t)loc2.x - (int32_t)loc1.x);
  const int32_t diff_y = ((int32_t)loc2.y - (int32_t)loc1.y);

  return sqrt(pow(diff_x, 2) + pow(diff_y, 2));
}

bool
location_adjacent(struct location loc1, struct location loc2)
{
  const int32_t diff_x = ((int32_t)loc2.x - (int32_t)loc1.x);
  const int32_t diff_y = ((int32_t)loc2.y - (int32_t)loc1.y);

  if (loc1.x == loc2.x && abs(diff_y) == 1)
    return true;

  if (loc1.y == loc2.y && abs(diff_x) == 1)
    return true;

  return false;
}

// Returns true if loc2 is adjacent to loc1,
// We also return the relative direction thru the (*dir) pointer
//
// If the locations are not adjacent, return false
bool
location_relative(struct location loc1, struct location loc2,
                  enum direction* dir)
{
  const int32_t diff_x = ((int32_t)loc2.x - (int32_t)loc1.x);
  const int32_t diff_y = ((int32_t)loc2.y - (int32_t)loc1.y);

  if (loc1.x == loc2.x) {
    switch (diff_y) {
      case -1:
        *dir = NORTH;
        return true;

      case 1:
        *dir = SOUTH;
        return true;
    }
  }

  if (loc1.y == loc2.y) {
    switch (diff_x) {
      case -1:
        *dir = WEST;
        return true;

      case 1:
        *dir = EAST;
        return true;
    }
  }

  return false;
}
