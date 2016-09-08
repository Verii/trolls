#include "draw.h"
#include "game.h"    // for entity, location, maze
#include <curses.h>  // for mvprintw, chtype, nodelay, stdscr, attrset, A_BOLD
#include <locale.h>  // for setlocale, LC_ALL, NULL
#include <stdbool.h> // for false, true
#include <stdint.h>  // for uint8_t, uint16_t
#include <stdlib.h>  // for srand, size_t
#include <time.h>    // for time

static const uint8_t X_OFF = 5;
static const uint8_t Y_OFF = 5;

static const enum draw_colors {
  DRAW_WHITE = 0,
  DRAW_RED,
  DRAW_GREEN,
  DRAW_YELLOW,
  DRAW_BLUE,
  DRAW_MAGENTA,
  DRAW_CYAN,
} colors[] = {
    [DRAW_WHITE] = COLOR_WHITE, [DRAW_RED] = COLOR_RED,
    [DRAW_GREEN] = COLOR_GREEN, [DRAW_YELLOW] = COLOR_YELLOW,
    [DRAW_BLUE] = COLOR_BLUE,   [DRAW_MAGENTA] = COLOR_MAGENTA,
    [DRAW_CYAN] = COLOR_CYAN,
};

void
draw_init()
{
  srand((unsigned int)time(NULL));
  setlocale(LC_ALL, "");

  initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  curs_set(0);

  start_color();
  for (uint8_t i = 0; i < LEN(colors); i++)
    init_pair(i + 1, colors[i], COLOR_BLACK);

  refresh();
}

void
draw_cleanup()
{
  nodelay(stdscr, true); // make getch() non-blocking
  while (getch() != ERR)
    ; // drop all user input
  nodelay(stdscr, false);

  refresh();
  endwin();
}

int
draw_getch(void)
{
  return getch();
}

void
draw_game_over(void)
{
  attrset(COLOR_PAIR(colors[DRAW_RED]) | A_BOLD);
  mvprintw(1, 0, "GAME OVER");
}

void
draw_maze(const struct maze* maze)
{
  clear();
  attrset(A_NORMAL);

  for (uint8_t y = 0; y < maze->maze_height; y++) {
    for (uint8_t x = 0; x < maze->maze_width; x++) {
      // FIXME
      // Needs player
      // struct location new_xy = { .x = x, .y = y };
      // if (location_distance(player.loc, new_xy) < game->player_vision)
      mvprintw(Y_OFF + y, X_OFF + x, "%c", MAZE_XY(maze, x, y));
    }
  }
}

void
draw_player(const struct entity* player)
{
  attrset(COLOR_PAIR(colors[DRAW_MAGENTA]) | A_BOLD);
  mvprintw(0, 0, "Player: (%.2d, %.2d)", player->loc.x, player->loc.y);
  mvprintw(Y_OFF + player->loc.y, X_OFF + player->loc.x, "%c", 'P');
}

void
draw_trolls(const struct entity* trolls, size_t num_trolls)
{
  attrset(COLOR_PAIR(colors[DRAW_BLUE]) | A_BOLD);

  for (uint8_t i = 0; i < num_trolls; i++) {

    const struct entity* troll = &(trolls[i]);

    // FIXME
    // Needs player
    // int32_t dist = location_distance(game->player.loc, game->trolls[i].loc);
    // if (dist < game->player_vision)
    mvprintw(Y_OFF + troll->loc.y, X_OFF + troll->loc.x, "%c", 'T');
    mvprintw(i, 20, "Troll %d: (%.2d, %.2d)", i + 1, troll->loc.x,
             troll->loc.y);
  }
}
