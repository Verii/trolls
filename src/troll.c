#include "troll.h"
#include "game.h"

// Troll AI/movement function
void
trolls_update(const struct maze* maze, struct entity* troll)
{
  // Check if we already have a defined path and follow it
  if (entity_follow_path(maze, troll))
    return;

  // Else
  // If we've failed to follow the path for any reason, try to calculate the
  // new path to a random empty location on the maze
  if (entity_new_path(maze, troll, maze_find_empty_location(maze)))
    return;

  // Else
  // Try to move in the direction we're already facing
  // This only fails if we can't continue moving the direction we're facing
  entity_move(maze, troll, troll->face);
}
