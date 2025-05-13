#include "SnakeModel.h"

#include <cstdlib>
#include <ctime>

namespace s21 {

SnakeModel::SnakeModel() {
  _gameState = START;
  _direction = 1; // Вправо по умолчанию
  _speed = 1;
  _score = 0;
  _highScore = 0;
  _accelerated = false;

  // Инициализация поля
  _gameInfo.field = new int *[FIELD_ROWS];
  for (int i = 0; i < FIELD_ROWS; ++i) {
    _gameInfo.field[i] = new int[FIELD_COLS]();
  }
  _gameInfo.next = nullptr; // У змейки нет следующей фигуры
  _gameInfo.score = _score;
  _gameInfo.high_score = _highScore;
  _gameInfo.level = 1;
  _gameInfo.speed = _speed;
  _gameInfo.pause = 0;

  srand(time(nullptr));
  initializeGame();
}

SnakeModel::~SnakeModel() {
  for (int i = 0; i < FIELD_ROWS; ++i) {
    delete[] _gameInfo.field[i];
  }
  delete[] _gameInfo.field;
}

void SnakeModel::userInput(UserAction_t action, bool hold) {
  // Приоритетная обработка Terminate для всех состояний
  if (action == Terminate) {
    _gameState = EXIT_STATE;
    return;
  }

  switch (_gameState) {
  case START:
    if (action == Start)
      _gameState = SPAWN;
    break;
  case SPAWN:
    if (action == NOSIG)
      _gameState = MOVING;
    break;
  case MOVING:
    if (action == Left && _direction != 1)
      _direction = 3;
    else if (action == Right && _direction != 3)
      _direction = 1;
    else if (action == Up && _direction != 2)
      _direction = 0;
    else if (action == Down && _direction != 0)
      _direction = 2;
    else if (action == Pause)
      _gameState = PAUSE;
    _accelerated = (action == Action && hold);
    break;
  case PAUSE:
    if (action == Pause)
      _gameState = MOVING;
    break;
  default:
    break;
  }
}

GameInfo_t SnakeModel::updateCurrentState() {
  static int update_counter = 0;
  int interval = _accelerated ? 2 : 5;
  if (_gameState == MOVING) {
    update_counter++;
    if (update_counter >= interval) {
      moveSnake();
      if (_snake.size() >= 200) {
        _gameState = WIN;
      }
      updateField(); // Обновление поля после хода
      update_counter = 0;
    }
  }
  _gameInfo.score = _score;
  _gameInfo.high_score = _highScore;
  return _gameInfo;
}

GameState SnakeModel::getState() { return _gameState; }

void SnakeModel::initializeGame() {
  _snake.clear();
  spawnSnake();
  spawnApple();
  updateField();
}

void SnakeModel::spawnSnake() {
  _snake = {{5, 5}, {5, 4}, {5, 3}, {5, 2}}; // Начальная позиция змейки
}

void SnakeModel::spawnApple() {
  bool valid_position = false;
  while (!valid_position) {
    _apple.row = rand() % FIELD_ROWS;
    _apple.col = rand() % FIELD_COLS;
    valid_position = true;
    for (const auto &segment : _snake) {
      if (_apple == segment) {
        valid_position = false;
        break;
      }
    }
  }
}

void SnakeModel::setAppleForTest(int row, int col) {
  _apple.row = row;
  _apple.col = col;
  // Обновляем поле для теста
  updateField();
}

void SnakeModel::setSnakeForTest(const std::vector<point> &snake) {
  _snake = snake;
  updateField();
}

void SnakeModel::moveSnake() {
  point new_head = _snake.front();
  switch (_direction) {
  case 0:
    new_head.row--;
    break; // Вверх
  case 1:
    new_head.col++;
    break; // Вправо
  case 2:
    new_head.row++;
    break; // Вниз
  case 3:
    new_head.col--;
    break; // Влево
  }

  // Проверка столкновения со стенами
  if (new_head.col < 0 || new_head.col >= FIELD_COLS || new_head.row < 0 ||
      new_head.row >= FIELD_ROWS) {
    _gameState = GAMEOVER;
    return;
  }

  // Проверка столкновения с телом
  for (size_t i = 1; i < _snake.size(); ++i) {
    if (new_head == _snake[i]) {
      _gameState = GAMEOVER;
      return;
    }
  }

  // Движение змейки
  _snake.insert(_snake.begin(), new_head);
  if (new_head == _apple) {
    eatApple();
  } else {
    _snake.pop_back();
  }

  // Проверка на победу
  if (_snake.size() >= 200) {
    _gameState = WIN;
  }
}

void SnakeModel::eatApple() {
  _score += 10;
  if (_score > _highScore)
    _highScore = _score;
  spawnApple();
}

void SnakeModel::updateField() {
  // Очистка поля
  for (int i = 0; i < FIELD_ROWS; ++i) {
    for (int j = 0; j < FIELD_COLS; ++j) {
      _gameInfo.field[i][j] = 0;
    }
  }

  // Размещение змейки
  for (const auto &segment : _snake) {
    _gameInfo.field[segment.row][segment.col] = 1;
  }

  // Размещение яблока
  _gameInfo.field[_apple.row][_apple.col] = 2;
}

} // namespace s21