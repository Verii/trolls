#include "game.h"   // for game, entity_move, direction::EAST, direction...
#include "troll.h"  // for update_trolls
#include <stdint.h> // for int32_t

#if defined(BENCH_PATH_QUEUE)
# include "src/path-queue.c"
#elif defined(BENCH_PATH_BHEAP)
# include "src/path-bheap.c"
#endif

int main(void);

int
main(void)
{
  struct game* game = game_new();

  // Start at the bottom left of the default maze
  game->trolls[0]->loc.x = 1;
  game->trolls[0]->loc.y = 21;

  // Move to the top right of the maze(longest path)
  // Calculate the same path 10 000 times
  for (size_t count = 0; count < 10000; count++) {
    entity_new_path(&game->maze, game->trolls[0],
        (struct location) { .x = 35, .y = 1 });
  }

  return 0;
}
