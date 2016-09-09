#pragma once

#include <stdbool.h> // for bool
#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint32_t, uint8_t

#define LEN(X) (sizeof(X) / sizeof(*X))

enum direction
{
  NORTH = 0,
  SOUTH,
  EAST,
  WEST,
};

struct location
{
  uint32_t x, y;
};

struct entity
{
  enum direction face;
  struct location loc;
};

#define MAZE_XY(M, X, Y) ((M)->maze[(M)->maze_width * (Y) + (X)])
struct maze
{
  char* maze;

  uint32_t maze_width;
  uint32_t maze_height;
};

enum game_state
{
  GAME_NONE = 0,
  GAME_WIN,
  GAME_LOSE,
};

struct game
{
  uint8_t player_vision;
  struct entity player;

  uint8_t num_trolls;
  struct entity* trolls;

  struct maze maze;
  enum game_state state;
};

// Returns the real distance between the two locations
double location_distance(struct location, struct location);

// Returns true if the locations are adjacent to each other
bool location_adjacent(struct location, struct location);

// Returns true if (l2) is adjacent to (l1)
//  - Also returns the relative direction of (l2) from (l1)
// Returns false If (l2) is not adjacent to (l1)
bool location_relative(struct location l1, struct location l2, enum direction*);

// Move the entity in the specified direction
int entity_move(const struct maze*, struct entity*, enum direction);

// Returns the number of empty space in the given direction
int entity_look(const struct maze*, const struct entity*, enum direction);

// Load the maze in (str) of length (len) into memory
int maze_load(struct maze*, const char* str, size_t len);

// spawn the entity randomly on an empty location in the maze
int maze_random_spawn(const struct maze*, struct entity*);

// Returns true if the specified direction from the entity is empty
bool maze_is_empty_space(const struct maze*, struct entity*, enum direction);

// Returns true if the value (val) in direction is within range
bool maze_check_bound(const struct maze*, uint16_t val, enum direction);

// Returns true if the coordinate in location is empty space
bool maze_is_empty_space_loc(const struct maze*, struct location);

// Returns true if the coordinate in location is within bounds
bool maze_check_bound_loc(const struct maze*, struct location);

// Allocate memory and initialize new game structure
struct game* game_new(void);

// Free game memory
void game_delete(struct game*);

// Return the status of the game, i.e. WIN, LOSE, NONE
// This is written to (struct game)->state
void game_update(struct game*);