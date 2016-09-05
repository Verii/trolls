#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <locale.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include <event2/event.h>

#include "troll.h"

enum direction {
	NORTH,
	SOUTH,
	EAST,
	WEST,
};

struct location {
	uint8_t x, y;
};

struct entity {
	enum direction face;
	struct location loc;
};

struct game {
	uint8_t maze_width;
	uint8_t maze_height;

	uint8_t player_vision;
	struct entity player;

	uint8_t num_trolls;
	struct entity *trolls;

	char *maze;
};

static const short colors[] = {COLOR_WHITE,  COLOR_RED,  COLOR_GREEN,
												COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA,
                        COLOR_CYAN};

static const char *default_maze =
"#####################################"
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


void init(void);
void cleanup(void);

int load_maze(struct game *, const char *, size_t );
uint8_t get_distance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
bool is_empty_space(struct game *, struct entity *, enum direction);

int random_spawn(struct game *, struct entity *);
struct game *new_game(void);

void draw_game(struct game *);
int move_entity(struct game *, struct entity *, enum direction);
void update_player(struct game *, int32_t);
void update_trolls(struct game *);

int main(void);



void
init(void) {

	srand((unsigned int) time(NULL));
	setlocale(LC_ALL, "");

	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	curs_set(0);

	start_color();

	for (uint8_t i = 0; i < sizeof(colors)/sizeof(*colors); i++)
		init_pair((uint8_t) (i + 1), colors[i], COLOR_BLACK);

	refresh();
}

void
cleanup(void) {
	// make getch() non-blocking
	nodelay(stdscr, true);
	// drop all user input
	while (getch() != ERR);
	nodelay(stdscr, false);

	refresh();
	endwin();
}

int
load_maze(struct game *game, const char *maze, size_t len) {

	game->maze = malloc(len);
	if (!game->maze)
		return 0;

	memcpy(game->maze, maze, len);

	return 1;
}

/* Pick a random empty entity for the entity */
int
random_spawn(struct game *game, struct entity *entity) {

	if (!game || !entity)
		return 0;

	// random (x, y) entity
	uint16_t check_x = 0, check_y = 0;

	bool found = false;
	do {

		// pick a random y value
		check_y = (uint16_t) (rand() % game->maze_height);

		// find the first index of the row at check_y
		uint16_t idx_y = (uint16_t) (check_y*game->maze_width);

		// count the number of empty columns on the row
		uint8_t empty_columns = 1;
		for (uint8_t i = 0; i < game->maze_width; i++)
			if (game->maze[i + idx_y] != '#')
				empty_columns++;

		// pick a random available column
		uint8_t get_col = (uint8_t) (rand() % empty_columns);

		// pick the (get_col) empty column
		for (uint8_t i = 0; i < game->maze_width && get_col; i++) {
			// decrement get_col if we find an empty space
			if (game->maze[i + idx_y] != ' ')
				get_col--;

			if (!get_col)
				check_x = i;
		}

		if (game->maze[idx_y + check_x] == ' ')
			found = true;

	} while (!found);
	
	if (check_x >= game->maze_width
			|| check_y >= game->maze_height)
		return 0;

	entity->face = rand() % 4;
	entity->x = (uint8_t) check_x;
	entity->y = (uint8_t) check_y;

	return 1;
}

// Initialize a new game
struct game *
new_game(void) {
	struct game *new_game;
	new_game = malloc(sizeof(*new_game));

	if (!new_game) {
		exit(1);
	}

	new_game->num_trolls = 4;

	new_game->trolls = malloc(sizeof(*new_game->trolls) * new_game->num_trolls);

	new_game->maze_width = 37;
	new_game->maze_height = 23;
	new_game->player_vision = 10;

	// put the default maze into the game struct
	if (load_maze(new_game, default_maze, strlen(default_maze)) != 1) {
		free(new_game->trolls);
		free(new_game);
		exit(1);
	}

	for (uint8_t i = 0; i < new_game->num_trolls; i++) {
		if (random_spawn(new_game, &(new_game->trolls[i])) != 1) {
			free(new_game->trolls);
			free(new_game);
			exit(1);
		}
	}

	// give the player a random spawn
	if (random_spawn(new_game, &(new_game->player)) != 1) {
		free(new_game->trolls);
		free(new_game);
		exit(1);
	}

	return new_game;
}

// Get the distance between two (x, y) coordinate pairs
uint8_t
get_distance(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	double dist = sqrt(pow(abs(x2 - x1), 2) + pow(abs(y2 - y1), 2));
	return (dist < 0) ? (uint8_t) -dist : (uint8_t) dist;
}

bool
is_empty_space(struct game *game, struct entity *entity, enum direction direction) {

	uint16_t idx_y = (uint16_t) (entity->y * game->maze_width);
	uint16_t idx_x = (uint16_t) entity->x;

	if (direction == NORTH) {
			if (entity->y > 0
					&& game->maze[idx_y + idx_x - game->maze_width] != '#')
				return true;

	} else if (direction == SOUTH) {
			if (entity->y < (game->maze_height-1) 
					&& game->maze[idx_y + idx_x + game->maze_width] != '#')
				return true;

	} else if (direction == EAST) {
			if (entity->x < (game->maze_width-1)
					&& game->maze[idx_y + idx_x +1] != '#')
				return true;

	} else if (direction == WEST) {
			if (entity->x > 0
					&& game->maze[idx_y + idx_x -1] != '#')
				return true;
	}

	return false;
}

void
draw_game(struct game *game) {

	clear();
	attrset(A_NORMAL);
	mvprintw(0, 0, "(%d, %d)", game->player.x, game->player.y);

	uint8_t x_offset = 5, y_offset = 5;

	for (uint8_t y = 0; y < game->maze_height; y++) {
		for (uint8_t x = 0; x < game->maze_width; x++) {

			uint16_t pos = (uint16_t) (x + y*game->maze_width);

			if (get_distance(game->player.x, game->player.y, x, y) < game->player_vision)
				mvprintw(y_offset + y, x_offset + x, "%c", game->maze[pos]);
		}
	}

	attrset(COLOR_PAIR(COLOR_MAGENTA) | A_BOLD);
	mvprintw(y_offset + game->player.y, x_offset + game->player.x, "%c", 'P');

	for (uint8_t i = 0; i < game->num_trolls; i++) {

		attrset(COLOR_PAIR(COLOR_BLUE) | A_BOLD);
		int32_t dist = get_distance(game->player.x, game->player.y, game->trolls[i].x, game->trolls[i].y);
		if (dist < game->player_vision)
			mvprintw(y_offset + game->trolls[i].y, x_offset + game->trolls[i].x, "%c", 'T');
	}
}

int
move_entity(struct game *game, struct entity *entity, enum direction direction) {
	switch (direction) {
		case NORTH:
			if (is_empty_space(game, entity, direction)) entity->y--;
			else return 0;
			break;

		case SOUTH:
			if (is_empty_space(game, entity, direction)) entity->y++;
			else return 0;
			break;

		case EAST:
			if (is_empty_space(game, entity, direction)) entity->x++;
			else return 0;
			break;

		case WEST:
			if (is_empty_space(game, entity, direction)) entity->x--;
			else return 0;
			break;
	}

	return 1;
}

void
update_player(struct game *game, int32_t key) {
	struct entity *player = &(game->player);

	switch (key) {
		case 'w': case 'W': move_entity(game, player, NORTH); break;
		case 'a': case 'A': move_entity(game, player, WEST); break;
		case 's': case 'S': move_entity(game, player, SOUTH); break;
		case 'd': case 'D': move_entity(game, player, EAST); break;
	}
}

int
main(void) {
	init();
	atexit(cleanup);

	struct event_base *ebase = event_base_new();

	struct game *game = new_game();
	if (!game) exit(1);

	// Main loop
	while (1) {
		draw_game(game);

		// wait for user input
		int key = getch();
		update_player(game, key);

		update_trolls(game);
	}

	event_base_free(ebase);

	return 0;
}
