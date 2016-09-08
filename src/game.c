#include "game.h"

#include <stdlib.h>
#include <string.h>

static const char* default_maze = "#####################################"
                                  "# #       #       #     #         # #"
                                  "# # ##### # ### ##### ### ### ### # #"
                                  "#       #   # #     #     # # #   # #"
                                  "##### # ##### ##### ### # # # ##### #"
                                  "#   # #       #     # # # # #     # #"
                                  "# # ####### # # ##### ### # ##### # #"
                                  "# #       # # #   #     #     #   # #"
                                  "# ####### ### ### # ### ##### # ### #"
                                  "#     #   # #   # #   #     # #     #"
                                  "# ### ### # ### # ##### # # # #######"
                                  "#   #   # # #   #   #   # # #   #   #"
                                  "####### # # # ##### # ### # ### ### #"
                                  "#     # #     #   # #   # #   #     #"
                                  "# ### # ##### ### # ### ### ####### #"
                                  "# #   #     #     #   # # #       # #"
                                  "# # ##### # ### ##### # # ####### # #"
                                  "# #     # # # # #     #       # #   #"
                                  "# ##### # # # ### ##### ##### # #####"
                                  "# #   # # #     #     # #   #       #"
                                  "# # ### ### ### ##### ### # ##### # #"
                                  "# #         #     #       #       # #"
                                  "#X###################################";

// Initialize a new game
struct game*
game_new()
{

  struct game* new_game;
  new_game = calloc(1, sizeof(*new_game));
  if (!new_game)
    exit(1);

  new_game->num_trolls = 4;
  new_game->trolls = malloc(sizeof(*new_game->trolls) * new_game->num_trolls);
  if (!new_game->trolls)
    exit(1);

  new_game->state = GAME_NONE;
  new_game->player_vision = 10;
  new_game->maze.maze_width = 37;
  new_game->maze.maze_height = 23;

  // put the default maze into the game struct
  if (maze_load(&new_game->maze, default_maze, strlen(default_maze)) != 1)
    exit(1);

  for (uint8_t i = 0; i < new_game->num_trolls; i++)
    if (maze_random_spawn(&new_game->maze, &(new_game->trolls[i])) != 1)
      exit(1);

  // give the player a random spawn
  if (maze_random_spawn(&new_game->maze, &(new_game->player)) != 1)
    exit(1);

  return new_game;
}

void
game_delete(struct game* game)
{
  free(game->trolls);
  free(game);
}

void
game_update(struct game* game)
{
  if (MAZE_XY(&game->maze, game->player.loc.x, game->player.loc.y) == 'X')
    game->state = GAME_WIN;

  // FIXME
  else if (0)
    game->state = GAME_NONE;
}
