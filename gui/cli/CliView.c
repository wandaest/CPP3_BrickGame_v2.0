#include "CliView.h"

#include <ncurses.h>
#include <locale.h>
#include <time.h>

// Macros for convenient text and character display in ncurses.
#define MVPRINTW(y, x, ...) \
  mvprintw(kBoardStartRow + (y), kBoardStartRow + (x), __VA_ARGS__)
#define MVADDCH(y, x, c) \
  mvaddch(kBoardStartRow + (y), kBoardStartRow + (x), c)

// Macro to add a colored block to the screen.
#define ADD_BLOCK(w, x) \
  waddch((w), ' ' | A_REVERSE | COLOR_PAIR(x)); \
  waddch((w), ' ' | A_REVERSE | COLOR_PAIR(x))

// Macro to add an empty block to the screen.
#define ADD_EMPTY(w) \
  waddch((w), ' '); \
  waddch((w), ' ')

const int kBoardStartRow = 1;

void init_gui(void) {
  srand(time(NULL));
  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  timeout(0);
  setlocale(LC_ALL, "");
  start_color();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);  // For snake.
  init_pair(2, COLOR_RED, COLOR_BLACK);   // For apple.
  display_overlay();
}

void display_overlay(void) {
  display_rectangle(0, BOARD_HEIGHT + 1, 0, BOARD_WIDTH * 2 + 1);
  display_rectangle(0, BOARD_HEIGHT + 1, BOARD_WIDTH * 2 + 2,
                    (BOARD_WIDTH + HUD_PANEL_WIDTH) * 2 + 3);

  mvprintw(1, BOARD_WIDTH * 2 + 3, "SCORE");
  mvprintw(4, BOARD_WIDTH * 2 + 3, "HIGH-SCORE");
  mvprintw(7, BOARD_WIDTH * 2 + 3, "LEVEL");
  mvprintw(10, BOARD_WIDTH * 2 + 3, "NEXT");

  int center_x = (FIELD_COLS * 2) / 2;
  int center_y = FIELD_ROWS / 2;

  int text_height = 2;
  int start_y = center_y - (text_height / 2);

  mvprintw(start_y, center_x - strlen("Press S") / 2, "Press S");
  mvprintw(start_y + 1, center_x - strlen("to start!") / 2, "to start!");
}

void render_game(GameInfo_t game_info, GameState state) {
  clear();
  display_overlay();
  if (game_info.next != NULL) {
    display_next_figure(game_info);
  }
  display_game_field(game_info);
  display_game_stats(game_info);
  if (state == PAUSE) {
    int center_x = (FIELD_COLS * 2) / 2;
    int center_y = FIELD_ROWS / 2;
    mvprintw(center_y, center_x - strlen("PAUSED") / 2, "PAUSED");
  } else if (state == GAMEOVER) {
    int center_x = (FIELD_COLS * 2) / 2;
    int center_y = FIELD_ROWS / 2;
    mvprintw(center_y, center_x - strlen("GAME OVER!") / 2, "GAME OVER!");
  }
  refresh();
}

void display_next_figure(GameInfo_t game_info) {
  for (int i = 0; i < NEXT_FIELD_ROWS; i++) {
    wmove(stdscr, 11 + i, 24);
    for (int j = 0; j < NEXT_FIELD_COLS; j++) {
      if (game_info.next[i][j] == 1) {
        ADD_BLOCK(stdscr, game_info.next[i][j]);
      } else {
        ADD_EMPTY(stdscr);
      }
    }
  }
}

void display_game_field(GameInfo_t game_info) {
  const int kVisibleRows = 20;
  int start_row = 0;
#ifdef TETRIS
  start_row = 3;
#endif
  for (int i = 0; i < kVisibleRows; i++) {
    int field_row = start_row + i;
    wmove(stdscr, i + 1, 1);
    for (int j = 0; j < FIELD_COLS; j++) {
      if (game_info.field[field_row][j] == 1) {
        ADD_BLOCK(stdscr, 1);
      } else if (game_info.field[field_row][j] == 2) {
        ADD_BLOCK(stdscr, 2);
      } else {
        ADD_EMPTY(stdscr);
      }
    }
  }
}

void display_rectangle(int top_y, int bottom_y, int left_x, int right_x) {
  mvaddch(top_y, left_x, ACS_ULCORNER);

  int i = left_x + 1;
  for (; i < right_x; i++) {
    mvaddch(top_y, i, ACS_HLINE);
  }
  mvaddch(top_y, i, ACS_URCORNER);

  for (int i = top_y + 1; i < bottom_y; i++) {
    mvaddch(i, left_x, ACS_VLINE);
    mvaddch(i, right_x, ACS_VLINE);
  }

  mvaddch(bottom_y, left_x, ACS_LLCORNER);
  i = left_x + 1;
  for (; i < right_x; i++) {
    mvaddch(bottom_y, i, ACS_HLINE);
  }
  mvaddch(bottom_y, i, ACS_LRCORNER);
}

void display_game_stats(GameInfo_t game_info) {
  mvprintw(2, BOARD_WIDTH * 2 + 4, "%d", game_info.score);
  mvprintw(5, BOARD_WIDTH * 2 + 4, "%d", game_info.high_score);
  mvprintw(8, BOARD_WIDTH * 2 + 4, "%d", game_info.level);
  if (game_info.pause == 1) {
    mvprintw(19, BOARD_WIDTH * 2 + 3, "PAUSED");
  } else {
    mvprintw(19, BOARD_WIDTH * 2 + 3, "      ");
  }
}

UserAction_t get_user_action(int user_input) {
  UserAction_t action = NOSIG;
  switch (user_input) {
    case 's':
    case 'S':
      action = Start;
      break;
    case 'p':
    case 'P':
    case SPACE_KEY:
      action = Pause;
      break;
    case ESCAPE_KEY:
      action = Terminate;
      break;
    case KEY_LEFT:
      action = Left;
      break;
    case KEY_RIGHT:
      action = Right;
      break;
    case KEY_UP:
      action = Up;
      break;
    case KEY_DOWN:
      action = Down;
      break;
    case RETURN_KEY:
      action = Action;
      break;
    default:
      action = NOSIG;
      break;
  }
  return action;
}