#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "game.h"

double
location_distance(struct location loc1, struct location loc2) {
	int32_t diff_x, diff_y;
	diff_x = ((int32_t) loc2.x - (int32_t) loc1.x);
	diff_y = ((int32_t) loc2.y - (int32_t) loc1.y);

	return sqrt(pow(diff_x, 2) + pow(diff_y, 2));
}
}
