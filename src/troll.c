#include <stdbool.h>
#include <stdlib.h>

#include "game.h"
#include "troll.h"

struct path {
	bool defined; // Have we already found a path

	size_t next; // index into the steps array indicating the next move
	enum direction steps[20];	// Each direction necessary to get to the destination
};

struct entity_path {
	struct entity *troll;
	struct path path;
};

// static global entity_paths
// Array of size num_trolls
static size_t num_paths = 0;
static struct entity_path *entity_paths = NULL;

// Turn in the direction of the longest hallway
static void
_longest_hallway(const struct maze *maze, struct entity *troll)
{
	// Each of the 4 directions
	struct {
		enum direction dir;
		size_t len; // length to the nearest wall
	} faces[4];

	enum direction newface = troll->face;

	// Find the length of each direction
	for (int dir = 0; dir < 4; dir++) {
		newface = (troll->face +dir) % 4;

		faces[dir].dir = newface;
		faces[dir].len = entity_look(maze, troll, newface);
	}

	uint8_t max_direction = 0;

	for (int dir = 1; dir < 4; dir++) {
		if (faces[dir].len > faces[max_direction].len)
			max_direction = dir;
	}

	troll->face = faces[max_direction].dir;
}

// FIXME
// TODO
static void
_shortest_hallway(const struct maze *maze, struct entity *troll)
{
	(void) maze;
	(void) troll;
}

// Turn in a random direction
static void
_random_hallway(struct entity *troll)
{
		troll->face = (rand() % 4);
}

// Initialize the memory for the path data on each troll
// Only run once, immediately returns if memory is already allocated
static void
_init_paths(struct entity *trolls, size_t num_trolls)
{
	// Don't re-initialize
	// MUST pass the same number of trolls each time
	if (entity_paths != NULL || num_paths > 0 ||
			num_trolls != num_paths)
		return;

	// Allocate each troll path

	num_paths = num_trolls;
	entity_paths = calloc(num_trolls, sizeof(*entity_paths));

	// OOM
	if (!entity_paths)
		exit(1);

	// Assign each troll to a troll path
	for (uint8_t i = 0; i < num_trolls; i++) {
		entity_paths[i].troll = &(trolls[i]);
	}

	return;
}

// Finds the troll_path structure corresponding to the given troll
// Returns a pointer to the troll_path struct if it's found
// If no match is found, return NULL
static struct entity_path *
_get_path(const struct entity *troll)
{
	struct entity_path *troll_path = NULL;

	// Find the troll path, if it exists
	for (uint8_t i = 0; i < num_paths; i++) {
		if (entity_paths[i].troll == troll)
			troll_path = &(entity_paths[i]);
	}

	return troll_path;
}

// FIXME
// TODO
// Calculate the shortest route to the destination (dest)
//
// Returns non-zero on all of the following: &&
//  - a path structure was found for this troll
//  - the destination is valid and within range
//  - an actual path was found to reach the destination
//
// Returns zero on any of the following: ||
//  - no path structure has been allocated for this troll
//  - destination is invalid
//  - we cannot calculate a path to the destination
static int
_calculate_path(const struct maze *maze, struct entity *troll, struct location dest)
{
	struct entity_path *troll_path =
		_get_path(troll);

	if (!troll_path) {
		return 0;
	}

	// FIXME
	(void) maze;
	(void) dest;

	return 0;
}

// Attempt to follow a pre-calculated path
//
// Returns non-zero on all of the following: &&
//  - a path was found for this troll
//  - we could move along it
//
// Return 0 on any of the following: ||
//  - no path was defined
//  - no path has been calculated
//  - we cannot continue the path
static int
_follow_path(const struct maze *maze, struct entity *troll)
{
	struct entity_path *troll_path =
		_get_path(troll);

	// No path struct found for this troll; or
	// no path structs allocated
	if (troll_path == NULL)
		return 0;

	// undefined path for this troll
	if (troll_path->path.defined == false)
		return 0;

	// We've already stepped through the array
	if (troll_path->path.next >= LEN(troll_path->path.steps))
		return 0;

	// Move along the calculated path
	int did_we_move = entity_move(maze, troll,
			troll_path->path.steps[troll_path->path.next++]);

	if (!did_we_move) {
		// We failed to move in the requested direction so our path is invalid
		troll_path->path.defined = false;
		return 0;
	}

	return 1;
}

// Troll AI/movement function
void
trolls_update(const struct maze *maze, struct entity *trolls, size_t num_trolls) {

	// Initialize the pathfinding/memory for the trolls
	// (only ran once)
	_init_paths(trolls, num_trolls);

	// Loop through each troll
	for (uint8_t i = 0; i < num_trolls; i++) {

		struct entity *troll = &(trolls[i]);

		// Check if we already have a defined path and follow it
		if (_follow_path(maze, troll))
			continue;

		// Else
		// If we've failed to follow the path for any reason, try to calculate the
		// new path
		// FIXME change to player location
		if (_calculate_path(maze, troll, (struct location){.x = 0, .y = 0}))
			continue;

		// Else
		// Try to move in the direction we're already facing
		// This only fails if we can't continue moving the direction we're facing
		if (entity_move(maze, troll, troll->face))
			continue;

		// Else
		// Pick a random algorithm for determining where to move next
		switch (rand() % 3) {
			case 0:
				_longest_hallway(maze, troll);
				break;
			case 1:
				_shortest_hallway(maze, troll);
				break;
			case 2:
				_random_hallway(troll);
				break;
		}
	}
}
