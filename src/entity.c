#include "game.h"

// Move the entity in the direction (dir)
// We make sure the space is empty, and return 1 if we've moved
// 0 otherwise
int entity_move(struct maze *maze, struct entity *entity, enum direction dir)
{
  switch (dir) {
	case NORTH:
    if (maze_check_bound(maze, entity->loc.y -1, dir) && 
				maze_is_empty_space(maze, entity, dir))
      entity->loc.y--;
    else
      return 0;
    break;

  case SOUTH:
    if (maze_check_bound(maze, entity->loc.y +1, dir) &&
				maze_is_empty_space(maze, entity, dir))
      entity->loc.y++;
    else
      return 0;
    break;

  case EAST:
    if (maze_check_bound(maze, entity->loc.x +1, dir) &&
				maze_is_empty_space(maze, entity, dir))
      entity->loc.x++;
    else
      return 0;
    break;

  case WEST:
    if (maze_check_bound(maze, entity->loc.x -1, dir) &&
				maze_is_empty_space(maze, entity, dir))
      entity->loc.x--;
    else
      return 0;
    break;
  }

  return 1;
}

// Looks in (direction) from the position of the entity up to (count) blocks
// until a wall is hit.
// Returns the number of blocks between the entity and the wall.
// If the entity is standing next to a wall in (direction): return 0
int entity_look(struct maze *maze, struct entity *entity, enum direction dir) {
	uint16_t x, y;
	x = entity->loc.x;
	y = entity->loc.y;

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
