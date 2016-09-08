#include "game.h"

// Move the entity in the direction (dir)
// We make sure the space is empty, and return 1 if we've moved
// 0 otherwise
int entity_move(const struct maze *maze, struct entity *entity, enum direction dir)
{
	const uint16_t x = entity->loc.x;
  const uint16_t y = entity->loc.y;

  switch (dir) {
	case NORTH:
    if (maze_check_bound(maze, y -1, dir) && 
				maze_is_empty_space(maze, entity, dir))
      entity->loc.y--;
    else
      return 0;
    break;

  case SOUTH:
    if (maze_check_bound(maze, y +1, dir) &&
				maze_is_empty_space(maze, entity, dir))
      entity->loc.y++;
    else
      return 0;
    break;

  case EAST:
    if (maze_check_bound(maze, x +1, dir) &&
				maze_is_empty_space(maze, entity, dir))
      entity->loc.x++;
    else
      return 0;
    break;

  case WEST:
    if (maze_check_bound(maze, x -1, dir) &&
				maze_is_empty_space(maze, entity, dir))
      entity->loc.x--;
    else
      return 0;
    break;
  }

  return 1;
}

// Returns the number of blocks between the entity and the wall.
// Looks in direction (dir) from the position of the entity (entity) until a
// wall is hit.
// If the entity is standing next to a wall in direction (dir): return 0
int entity_look(const struct maze *maze, const struct entity *entity, enum direction dir) {
	uint16_t x = entity->loc.x;
	uint16_t y = entity->loc.y;

	switch (dir) {
		case NORTH:
			while (maze_check_bound(maze, --y, NORTH) && MAZE_XY(maze, x, y) != '#')
				;
			return entity->loc.y - y -1;

		case SOUTH:
			while (maze_check_bound(maze, ++y, SOUTH) && MAZE_XY(maze, x, y) != '#')
				;
			return y - entity->loc.y -1;

		case EAST:
			while (maze_check_bound(maze, ++x, EAST) && MAZE_XY(maze, x, y) != '#')
				;
			return x - entity->loc.x -1;

		case WEST:
			while (maze_check_bound(maze, --x, WEST) && MAZE_XY(maze, x, y) != '#')
				;
			return entity->loc.x - x -1;
	}

	// Not reached
	return 0;
}
