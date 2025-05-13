#include "s21_tetris_model.h"

namespace s21 {

TetrisModel::TetrisModel() : _cyclesToShift(CYCLES) {
  initialize_GameInfo_t(::game_info());
}

TetrisModel::~TetrisModel() { free_GameInfo_t_memory(::game_info()); }

void TetrisModel::userInput(UserAction_t sig, bool hold) {
  ::userInput(sig, hold);
}

GameInfo_t TetrisModel::updateCurrentState() { return ::updateCurrentState(); }

GameState TetrisModel::getState() { return *game_state(); }

} // namespace s21