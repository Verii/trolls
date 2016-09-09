#include <stdlib.h>

#include "game.h"
#include "path.h"

struct entity*
entity_new(void)
{
  struct entity* ret = calloc(1, sizeof(*ret));
  if (!ret)
    exit(1);

  return ret;
}

void
entity_delete(struct entity** entity)
{
  struct entity* e;
  if (!entity || !(*entity))
    return;

  e = *entity;

  if (e->path) {
    free(e->path->steps);
    free(e->path);
    e->path = NULL;
  }

  free(e);
  *entity = NULL;
}

// Move the entity in the direction (dir)
// We make sure the space is empty, and return 1 if we've moved
// Return 2 if we haven't moved but did change our direction
// Return 0 if we couldn't move, but we are facing the correct direction
int
entity_move(const struct maze* maze, struct entity* entity, enum direction dir)
{
  const uint16_t x = entity->loc.x;
  const uint16_t y = entity->loc.y;

  if (entity->face != dir) {
    entity->face = dir;
    return 2;
  }

  switch (dir) {
    case NORTH:
      if (maze_check_bound(maze, y - 1, dir) &&
          maze_is_empty_space(maze, entity, dir))
        entity->loc.y--;
      else
        return 0;
      break;

    case SOUTH:
      if (maze_check_bound(maze, y + 1, dir) &&
          maze_is_empty_space(maze, entity, dir))
        entity->loc.y++;
      else
        return 0;
      break;

    case EAST:
      if (maze_check_bound(maze, x + 1, dir) &&
          maze_is_empty_space(maze, entity, dir))
        entity->loc.x++;
      else
        return 0;
      break;

    case WEST:
      if (maze_check_bound(maze, x - 1, dir) &&
          maze_is_empty_space(maze, entity, dir))
        entity->loc.x--;
      else
        return 0;
      break;
  }

  return 1;
}

// Attempt to follow a pre-calculated path
//
// Returns non-zero on all of the following: &&
//  - a path was found for this entity
//  - we could move along it
//
// Return 0 on any of the following: ||
//  - no path was defined
//  - no path has been calculated
//  - we cannot continue the path
int
entity_follow_path(const struct maze* maze, struct entity* entity)
{
  struct path* path;
  path = entity->path;

  // No path struct found for this entity; or
  // no path structs allocated; or
  // the path is not defined
  if (!path)
    return 0;

  // We've already stepped through the path
  if (path->next >= path->num_steps) {
    free(path->steps);
    free(path);
    entity->path = NULL;
    return 0;
  }

  int try_move = entity_move(maze, entity, path->steps[path->next]);

  if (try_move == 0) {
    // Cannot follow path or it doesn't exist
    free(path->steps);
    free(path);
    entity->path = NULL;

  } else if (try_move == 1) {
    // Success
    path->next++;

  } else if (try_move == 2) {
    // We couldn't move but we did turn in the correct direction
  }

  return try_move;
}

// Construct a new path to (target) location for the entity.
//
// Return 0 if the path cannot be found
// Return 1 if the path was found
int
entity_new_path(const struct maze* maze, struct entity* entity,
                struct location target)
{
  if (entity->path) {
    free(entity->path->steps);
    free(entity->path);
    entity->path = NULL;
  }

  struct path* new_path = path_find(maze, entity->loc, target);

  if (new_path == NULL)
    return 0;

  entity->path = new_path;
  return 1;
}

// Returns the number of blocks between the entity and the wall.
// Looks in direction (dir) from the position of the entity (entity) until a
// wall is hit.
// If the entity is standing next to a wall in direction (dir): return 0
int
entity_look(const struct maze* maze, const struct entity* entity,
            enum direction dir)
{
  uint16_t x = entity->loc.x;
  uint16_t y = entity->loc.y;

  switch (dir) {
    case NORTH:
      while (maze_check_bound(maze, --y, NORTH) && MAZE_XY(maze, x, y) != '#')
        ;
      return entity->loc.y - y - 1;

    case SOUTH:
      while (maze_check_bound(maze, ++y, SOUTH) && MAZE_XY(maze, x, y) != '#')
        ;
      return y - entity->loc.y - 1;

    case EAST:
      while (maze_check_bound(maze, ++x, EAST) && MAZE_XY(maze, x, y) != '#')
        ;
      return x - entity->loc.x - 1;

    case WEST:
      while (maze_check_bound(maze, --x, WEST) && MAZE_XY(maze, x, y) != '#')
        ;
      return entity->loc.x - x - 1;
  }

  // Not reached
  return 0;
}
