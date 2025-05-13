#ifndef CLIVIEW_H
#define CLIVIEW_H

#include <ncurses.h>

#include "../../brick_game/GameInfo/GameInfo.h"
#include "../../brick_game/tetris/s21_tetris.h"

// #define BOARD_HEIGHT 21
// #define BOARD_WIDTH 10
// #define HUD_PANEL_WIDTH 5

void init_gui(void);
void display_overlay(void);
void render_game(GameInfo_t game_info, GameState state);
void display_next_figure(GameInfo_t game_info);
void display_game_field(GameInfo_t game_info);
void display_rectangle(int top_y, int bottom_y, int left_x, int right_x);
void display_game_stats(GameInfo_t game_info);
UserAction_t get_user_action(int user_input);

#endif // CLIVIEW_H