#include <stdlib.h>

#include "troll.h"

void
update_trolls(struct maze *maze, struct entity *trolls, size_t num_trolls) {

	for (uint8_t i = 0; i < num_trolls; i++) {

		struct entity *troll = &(trolls[i]);

		if (entity_move(maze, troll, troll->face))
			continue;

		troll->face = (rand() % 4);
	}
}
