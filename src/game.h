#pragma once

#include <stdbool.h>  // for bool
#include <stddef.h>   // for size_t
#include <stdint.h>   // for uint32_t, uint8_t

enum direction {
	NORTH,
	SOUTH,
	EAST,
	WEST,
};

struct location {
	uint32_t x, y;
};

struct entity {
  enum direction face;
  struct location loc;
};

#define MAZE_XY(M, X, Y) ((M)->maze[(M)->maze_width*(Y) + (X)])
struct maze {
	char *maze;

	uint32_t maze_width;
	uint32_t maze_height;
};

enum game_state {
	GAME_NONE = 0,
	GAME_WIN,
	GAME_LOSE,
};

struct game {
	uint8_t player_vision;
	struct entity player;

	uint8_t num_trolls;
	struct entity *trolls;

	struct maze maze;
	enum game_state state;
};

uint32_t location_distance(struct location, struct location);

int entity_move(const struct maze *, struct entity *, enum direction);
int entity_look(const struct maze *, struct entity *, enum direction);

int maze_load(struct maze *, const char *, size_t);
bool maze_is_empty_space(const struct maze *, struct entity *, enum direction);
int maze_random_spawn(const struct maze *, struct entity *);
bool maze_check_bound(const struct maze *, uint16_t val, enum direction);

struct game *game_new(void);
void game_delete(struct game *);
void game_update(struct game *);
