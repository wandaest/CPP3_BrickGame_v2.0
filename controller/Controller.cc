#include "Controller.h"

namespace s21 {

Controller::Controller(AbstractGameModel *abstract_game_model)
    : abstract_game_model_(abstract_game_model) {}

void Controller::userInput(UserAction_t action, bool hold) {
  abstract_game_model_->userInput(action, hold);
}

GameInfo_t Controller::updateCurrentState() {
  return abstract_game_model_->updateCurrentState();
}

GameState Controller::getGameState() {
  return abstract_game_model_->getState();
}

} // namespace s21