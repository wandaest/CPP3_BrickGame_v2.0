#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "../brick_game/AbstractGameModel/AbstractGameModel.h"

namespace s21 {

class Controller {
public:
  explicit Controller(AbstractGameModel *abstract_game_model);
  ~Controller() = default;

  void userInput(UserAction_t action, bool hold);
  GameInfo_t updateCurrentState();
  GameState getGameState();

private:
  AbstractGameModel *abstract_game_model_;
};

} // namespace s21

#endif // CONTROLLER_H_