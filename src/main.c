#include "draw.h"   // for draw_getch, draw_init, draw_maze, draw_player
#include "game.h"   // for game, entity_move, direction::EAST, direction...
#include "troll.h"  // for update_trolls
#include <stdint.h> // for int32_t
#include <stdlib.h> // for atexit, exit

void player_update(const struct maze* maze, struct entity*, int32_t);
int main(void);

void
player_update(const struct maze* maze, struct entity* player, int32_t key)
{
  switch (key) {
    case 'w':
    case 'W':
      entity_move(maze, player, NORTH);
      break;
    case 'a':
    case 'A':
      entity_move(maze, player, WEST);
      break;
    case 's':
    case 'S':
      entity_move(maze, player, SOUTH);
      break;
    case 'd':
    case 'D':
      entity_move(maze, player, EAST);
      break;
  }
}

int
main(void)
{
  draw_init();
  atexit(draw_cleanup);

  struct game* game = game_new();

  // Main loop
  while (1) {

    // Draw the game
    draw_maze(&game->maze);
    for (uint8_t i = 0; i < game->num_trolls; i++)
      draw_trolls(game->trolls[i]);
    draw_player(game->player);

    // wait for user input
    int key = draw_getch();
    player_update(&game->maze, game->player, key);

    // Update each troll
    for (uint8_t i = 0; i < game->num_trolls; i++)
      trolls_update(&game->maze, game->trolls[i]);

    // Check game state (i.e. win or lose)
    game_get_status(game);

    if (game->state == GAME_WIN || game->state == GAME_LOSE)
      break;
  }

  if (game->state == GAME_LOSE)
    draw_game_over();

  game_delete(game);

  return 0;
}
