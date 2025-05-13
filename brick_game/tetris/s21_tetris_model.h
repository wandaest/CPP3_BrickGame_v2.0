#ifndef TETRIS_MODEL_H
#define TETRIS_MODEL_H

#ifdef __cplusplus

#include "../CommonGameLogic/CommonGameLogic.h"

extern "C" {
#endif

#include "s21_tetris.h"

#ifdef __cplusplus
}
#endif

namespace s21 {

class TetrisModel : public CommonGameLogic {
public:
  TetrisModel();

  ~TetrisModel() override;

  void userInput(UserAction_t sig, bool hold);

  GameInfo_t updateCurrentState();

  GameState getState() override;

private:
  int _cyclesToShift;
};

} // namespace s21

#endif // TETRIS_MODEL_H