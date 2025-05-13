#include "./brick_game/snake/SnakeModel.h"
#include "./brick_game/tetris/s21_tetris_model.h"
#include "./controller/Controller.h"
#include "./gui/cli/CliWrapper.h"

#define _POSIX_C_SOURCE 200809L
#include <ncurses.h>
#include <unistd.h>

/**
 * @brief Основная функция игры.
 *
 * Функция инициализирует экран, запускает основной игровой цикл и завершает
 * работу.
 *
 * @return 0 В случае успешного завершения работы.
 */
int main() {
#ifdef TETRIS
  s21::TetrisModel model;
#else
  s21::SnakeModel model;
#endif

  s21::Controller controller(&model);
  s21::CliWrapper cliView(&controller);
  cliView.loop();

  return 0;
}