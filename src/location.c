#include <stdlib.h>
#include <math.h>

#include "game.h"

uint32_t
location_distance(struct location loc1, struct location loc2) {
	int32_t diff_x, diff_y;
	diff_x = abs(((int32_t) loc2.x - (int32_t) loc1.x));
	diff_y = abs(((int32_t) loc2.y - (int32_t) loc1.y));

	double dist = sqrt(pow(diff_x, 2) + pow(diff_y, 2));
	return (dist < 0) ? (uint32_t) -dist : (uint32_t) dist;
}
