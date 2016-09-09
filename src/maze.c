#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"

// Load the maze into memory
int
maze_load(struct maze* maze, const char* data, size_t datalen)
{
  maze->maze = malloc(datalen);
  if (!maze->maze)
    return 0;

  memcpy(maze->maze, data, datalen);

  return 1;
}

void
maze_destroy(struct maze* maze)
{
  free(maze->maze);
  maze->maze = NULL;
}

// Find a random empty location on the maze
struct location
maze_find_empty_location(const struct maze* maze)
{
  // random (x, y) location
  uint16_t check_x = 0, check_y = 0;
  bool spot_found = false;

  do {
    // pick a random y value
    check_y = rand() % maze->maze_height;
    check_x = 0;

    // count the number of empty columns on the row
    uint8_t empty_columns = 1;
    for (uint8_t i = 0; i < maze->maze_width; i++)
      if (MAZE_XY(maze, i, check_y) == ' ')
        empty_columns++;

    // pick a random available column
    uint8_t get_col = rand() % empty_columns;

    // pick the (get_col) empty column
    for (uint8_t i = 0; i < maze->maze_width && get_col; i++) {
      // decrement get_col if we find an empty space
      if (MAZE_XY(maze, i, check_y) == ' ')
        get_col--;

      if (!get_col)
        check_x = i;
    }

    if (MAZE_XY(maze, check_x, check_y) == ' ')
      spot_found = true;

  } while (!spot_found);

  return (struct location) { .x = check_x, .y = check_y  };
}

/* Pick a random empty entity for the entity */
int
maze_random_spawn(const struct maze* maze, struct entity* entity)
{
  entity->face = rand() % 4;
  entity->loc = maze_find_empty_location(maze);
  return 1;
}

bool
maze_is_empty_space(const struct maze* maze, struct entity* entity,
                    enum direction dir)
{
  uint16_t x, y;
  x = entity->loc.x;
  y = entity->loc.y;

  if (dir == NORTH) {
    if (maze_check_bound(maze, y - 1, dir) && MAZE_XY(maze, x, y - 1) != '#')
      return true;

  } else if (dir == SOUTH) {
    if (maze_check_bound(maze, y + 1, dir) && MAZE_XY(maze, x, y + 1) != '#')
      return true;

  } else if (dir == EAST) {
    if (maze_check_bound(maze, x + 1, dir) && MAZE_XY(maze, x + 1, y) != '#')
      return true;

  } else if (dir == WEST) {
    if (maze_check_bound(maze, x - 1, dir) && MAZE_XY(maze, x - 1, y) != '#')
      return true;
  }

  return false;
}

bool
maze_is_empty_space_loc(const struct maze* maze, struct location loc)
{
  return (maze_check_bound_loc(maze, loc) &&
          MAZE_XY(maze, loc.x, loc.y) == ' ');
}

// Make sure we're in bounds
bool
maze_check_bound(const struct maze* maze, uint16_t value, enum direction dir)
{
  switch (dir) {
    case NORTH:
    case SOUTH:
      if (value >= maze->maze_height
          /* || value < 0 */)
        return false;
    case EAST:
    case WEST:
      if (value >= maze->maze_width
          /* || value < 0 */)
        return false;
  }

  return true;
}

bool
maze_check_bound_loc(const struct maze* maze, struct location loc)
{
  return (loc.y < maze->maze_height && loc.x < maze->maze_width);
}
