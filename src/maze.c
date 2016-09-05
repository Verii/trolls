#include <stdlib.h>
#include <string.h>

#include "game.h"

int
maze_load(struct maze *maze, const char *data, size_t datalen) {

	maze->maze = malloc(datalen);
	if (!maze->maze)
		return 0;

	memcpy(maze->maze, data, datalen);

	return 1;
}

bool
maze_is_empty_space(struct maze *maze, struct entity *entity, enum direction dir) {

	uint16_t x, y;
	x = entity->loc.x;
	y = entity->loc.y;

	if (dir == NORTH) {
			if (maze_check_bound(maze, y -1, dir) && MAZE_XY(maze, x, y -1) != '#')
				return true;

	} else if (dir == SOUTH) {
			if (maze_check_bound(maze, y +1, dir) && MAZE_XY(maze, x, y +1) != '#')
				return true;

	} else if (dir == EAST) {
			if (maze_check_bound(maze, x +1, dir) && MAZE_XY(maze, x +1, y) != '#')
				return true;

	} else if (dir == WEST) {
			if (maze_check_bound(maze, x -1, dir) && MAZE_XY(maze, x -1, y) != '#')
				return true;
	}

	return false;
}

/* Pick a random empty entity for the entity */
int
maze_random_spawn(struct maze *maze, struct entity *entity) {

	// random (x, y) entity
	uint16_t check_x = 0, check_y = 0;

	bool found = false;
	do {

		// pick a random y value
		check_x = 0;
		check_y = rand() % maze->maze_height;

		// count the number of empty columns on the row
		uint8_t empty_columns = 1;
		for (uint8_t i = 0; i < maze->maze_width; i++)
			if (MAZE_XY(maze, i, check_y) != '#')
				empty_columns++;

		// pick a random available column
		uint8_t get_col = rand() % empty_columns;

		// pick the (get_col) empty column
		for (uint8_t i = 0; i < maze->maze_width && get_col; i++) {
			// decrement get_col if we find an empty space
			if (MAZE_XY(maze, i, check_y) != '#')
				get_col--;

			if (!get_col)
				check_x = i;
		}

		if (MAZE_XY(maze, check_x, check_y) == ' ')
			found = true;

	} while (!found);
	
	if (!maze_check_bound(maze, check_x, EAST) ||
			!maze_check_bound(maze, check_y, SOUTH))
		return 0;

	entity->face = rand() % 4;
	entity->loc.x = check_x;
	entity->loc.y = check_y;

	return 1;
}

// Make sure we're in bounds
bool
maze_check_bound(struct maze *maze, uint16_t value, enum direction dir)
{
	switch (dir) {
		case NORTH:
		case SOUTH:
			if (value >= maze->maze_height || value < 0)
				return false;
			break;
		case EAST:
		case WEST:
			if (value >= maze->maze_width || value < 0)
				return false;
			break;
	}

	return true;
}
