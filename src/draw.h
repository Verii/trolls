#pragma once

#include <stddef.h>
struct entity;
struct maze;

enum draw_colors {
	DRAW_WHITE,
	DRAW_RED,
	DRAW_GREEN,
	DRAW_YELLOW,
	DRAW_BLUE,
	DRAW_MAGENTA,
	DRAW_CYAN,
};

void draw_init(void);
void draw_cleanup(void);
void draw_game_over(void);

void draw_maze(const struct maze *);
void draw_player(const struct entity *);
void draw_trolls(const struct entity *, size_t);
int draw_getch(void);
