#ifndef MODEL_CONSTRUCTOR_H
#define MODEL_CONSTRUCTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../GameInfo/GameInfo.h"

#ifdef __cplusplus
}
#endif

namespace s21 {

class AbstractGameModel {
public:
  virtual ~AbstractGameModel() = default;

  virtual void userInput(UserAction_t action, bool hold) = 0;

  virtual GameInfo_t updateCurrentState() = 0;

  virtual GameState getState() = 0;
};
} // namespace s21

#endif // MODEL_CONSTRUCTOR_H