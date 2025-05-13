#ifndef SNAKE_MODEL_H
#define SNAKE_MODEL_H

#include <vector>

#include "../AbstractGameModel/AbstractGameModel.h"
#include "../GameInfo/GameInfo.h"
#include "../tetris/s21_tetris.h"

namespace s21 {

class SnakeModel : public AbstractGameModel {
public:
  SnakeModel();
  ~SnakeModel() override;

  void userInput(UserAction_t action, bool hold) override;
  GameInfo_t updateCurrentState() override;
  GameState getState() override;
  void setAppleForTest(int row, int col);
  void setSnakeForTest(const std::vector<point> &snake);
  void moveSnake();

private:
  GameInfo_t _gameInfo;
  GameState _gameState;
  std::vector<point> _snake;
  point _apple;
  int _direction;
  int _speed;
  int _score;
  int _highScore;
  bool _accelerated;

  void initializeGame();
  void spawnSnake();
  void spawnApple();

  bool checkCollision();
  void eatApple();
  void updateField();
};

} // namespace s21

#endif // SNAKE_MODEL_H
