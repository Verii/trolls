#pragma once

#include <stddef.h>

struct entity;
struct maze;

// Initialize nCurses context
void draw_init(void);

// Destroy nCurses context
void draw_cleanup(void);

// FIXME, game over screen
void draw_game_over(void);

// Simple wrapper to keep the nCurses functions in the draw.c file
// Returns the next character from the user
int draw_getch(void);

/* Draw functions
 * These should be called in this order to update the screen
 *
 * Be sure to initialize the screen with draw_init() before using these
 * functions
 */

// Draw the maze (p_maze) to the screen
void draw_maze(const struct maze* p_maze);

// Draw the trolls to the screen
// Takes pointer to entity (trolls)
void draw_trolls(const struct entity* troll);

// Draw the player to the screen
// Takes a pointer (p_player) to the player entity
void draw_player(const struct entity* p_player);
