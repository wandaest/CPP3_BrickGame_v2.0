#include <gtest/gtest.h>

#include "../brick_game/snake/SnakeModel.h"

namespace s21 {

class SnakeModelTest : public ::testing::Test {
protected:
  void SetUp() override { model_ = new SnakeModel(); }

  void TearDown() override { delete model_; }

  SnakeModel *model_;
};

TEST_F(SnakeModelTest, InitializeGame) {
  GameInfo_t game_info = model_->updateCurrentState();

  // Проверяем начальное состояние
  EXPECT_EQ(model_->getState(), START);

  // Проверяем начальные позиции змейки и яблока
  bool snake_present = false;
  bool apple_present = false;
  for (int i = 0; i < FIELD_ROWS; ++i) {
    for (int j = 0; j < FIELD_COLS; ++j) {
      if (game_info.field[i][j] == 1)
        snake_present = true;
      if (game_info.field[i][j] == 2)
        apple_present = true;
    }
  }
  EXPECT_TRUE(snake_present);
  EXPECT_TRUE(apple_present);
}

TEST_F(SnakeModelTest, StateTransitionStartToMoving) {
  EXPECT_EQ(model_->getState(), START);
  model_->userInput(Start, false);
  EXPECT_EQ(model_->getState(), SPAWN);
  model_->userInput(NOSIG, false);
  EXPECT_EQ(model_->getState(), MOVING);
}

TEST_F(SnakeModelTest, StateTransitionPause) {
  model_->userInput(Start, false);
  model_->userInput(NOSIG, false);
  EXPECT_EQ(model_->getState(), MOVING);
  model_->userInput(Pause, false);
  EXPECT_EQ(model_->getState(), PAUSE);
  model_->userInput(Pause, false);
  EXPECT_EQ(model_->getState(), MOVING);
}

/*
TEST_F(SnakeModelTest, StateTransitionGameOverCollisionWall) {
  model_->userInput(Start, false);
  model_->userInput(NOSIG, false);
  EXPECT_EQ(model_->getState(), MOVING);

  // Ускоряем змейк
  model_->userInput(Action, true);  // Включаем ускорение (interval = 2)

  // Двигаем змейку влево до столкновения со стеной
  for (int i = 0; i < 12; ++i) {  // 6 шагов * 2 (interval)
    model_->userInput(Left, false);
    model_->updateCurrentState();
  }
  EXPECT_EQ(model_->getState(), GAMEOVER);
}
*/

/*
TEST_F(SnakeModelTest, StateTransitionGameOverCollisionSelf) {
  model_->userInput(Start, false);
  model_->userInput(NOSIG, false);
  EXPECT_EQ(model_->getState(), MOVING);

  // Ускоряем змейку
  model_->userInput(Action, true);  // Включаем ускорение (interval = 2)

  // Поворачиваем змейку, чтобы она столкнулась с собой
  model_->userInput(Down, false);
  for (int i = 0; i < 2; ++i) model_->updateCurrentState();
  model_->userInput(Left, false);
  for (int i = 0; i < 2; ++i) model_->updateCurrentState();
  model_->userInput(Up, false);
  for (int i = 0; i < 2; ++i) model_->updateCurrentState();
  model_->userInput(Right, false);
  for (int i = 0; i < 2; ++i) model_->updateCurrentState();

  EXPECT_EQ(model_->getState(), GAMEOVER);
}
*/

/*
TEST_F(SnakeModelTest, StateTransitionWin) {
  model_->userInput(Start, false);
  model_->userInput(NOSIG, false);
  EXPECT_EQ(model_->getState(), MOVING);

  // Ускоряем змейку
  model_->userInput(Action, true);  // Включаем ускорение (interval = 2)

  // Устанавливаем яблоко для теста
  model_->setAppleForTest(5, 6);  // Яблоко в (5, 6)

  // Первый шаг: съедаем яблоко в (5, 6)
  model_->userInput(Right, false);
  for (int j = 0; j < 2; ++j) model_->updateCurrentState();

  // Двигаем вниз, избегая выхода за пределы
  for (int i = 0; i < 17; ++i) {  // Максимум 17 шагов вниз (с 5 до 22)
    model_->setAppleForTest(6 + i, 6);  // Яблоко следует за змейкой
    model_->userInput(Down, false);
    for (int j = 0; j < 2; ++j) model_->updateCurrentState();
  }

  // Чередуем движение влево и вправо в строке 22
  int remaining_steps = 178;  // 196 - 1 (первое) - 17 (вниз) = 178
  int col = 6;  // Текущий столбец после движения вниз
  bool moving_left = true;  // Направление движения

  for (int i = 0; i < remaining_steps; ++i) {
    if (moving_left) {
      model_->userInput(Left, false);
      col--;
      if (col <= 0) moving_left = false;  // Дошли до левой стены, поворачиваем
    } else {
      model_->userInput(Right, false);
      col++;
      if (col >= 9) moving_left = true;  // Дошли до правой стены, поворачиваем
    }
    model_->setAppleForTest(22, col);  // Яблоко следует за змейкой
    for (int j = 0; j < 2; ++j) model_->updateCurrentState();
  }

  EXPECT_EQ(model_->getState(), WIN);
}
*/

TEST_F(SnakeModelTest, StateTransitionExit) {
  model_->userInput(Start, false);
  model_->userInput(NOSIG, false);
  EXPECT_EQ(model_->getState(), MOVING);

  model_->userInput(Terminate, false);
  EXPECT_EQ(model_->getState(), EXIT_STATE);
}

/*
TEST_F(SnakeModelTest, EatApple) {
  model_->userInput(Start, false);
  model_->userInput(NOSIG, false);

  GameInfo_t game_info = model_->updateCurrentState();
  game_info.field[5][6] = 2;  // Размещаем яблоко рядом с головой
  model_->userInput(Right, false);  // Двигаем вправо
  model_->updateCurrentState();

  EXPECT_EQ(game_info.score, 10);
  EXPECT_EQ(game_info.level, 1);
  EXPECT_EQ(game_info.speed, 1);
}
*/

TEST_F(SnakeModelTest, UpdateField) {
  model_->userInput(Start, false);
  model_->userInput(NOSIG, false);

  GameInfo_t game_info = model_->updateCurrentState();

  // Проверяем, что змейка и яблоко присутствуют на поле
  bool snake_present = false;
  bool apple_present = false;
  for (int i = 0; i < FIELD_ROWS; ++i) {
    for (int j = 0; j < FIELD_COLS; ++j) {
      if (game_info.field[i][j] == 1)
        snake_present = true;
      if (game_info.field[i][j] == 2)
        apple_present = true;
    }
  }
  EXPECT_TRUE(snake_present);
  EXPECT_TRUE(apple_present);
}

} // namespace s21

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}