#include <stdlib.h>     // for atexit, exit
#include <sys/types.h>  // for int32_t
#include "draw.h"       // for draw_getch, draw_init, draw_maze, draw_player
#include "game.h"       // for game, entity_move, direction::EAST, direction...
#include "troll.h"      // for update_trolls

void update_player(struct game *, int32_t);
int main(void);

void
update_player(struct game *game, int32_t key) {
	struct entity *player = &(game->player);

	switch (key) {
		case 'w': case 'W': entity_move(&game->maze, player, NORTH); break;
		case 'a': case 'A': entity_move(&game->maze, player, WEST); break;
		case 's': case 'S': entity_move(&game->maze, player, SOUTH); break;
		case 'd': case 'D': entity_move(&game->maze, player, EAST); break;
	}
}

int
main(void) {
	draw_init();
	atexit(draw_cleanup);

	struct game *game = game_new();
	if (!game) exit(1);

	// Main loop
	while (1) {
		draw_maze(&game->maze);
		draw_player(&game->player);
		draw_trolls(game->trolls, game->num_trolls);

		// wait for user input
		int key = draw_getch();
		update_player(game, key);
		trolls_update(&game->maze, game->trolls, game->num_trolls);

		// Check game state (i.e. win or lose)
		game_update(game);

		if (game->state == GAME_WIN ||
				game->state == GAME_LOSE)
			break;
	}

	if (game->state == GAME_LOSE)
		draw_game_over();

	game_delete(game);

	return 0;
}
