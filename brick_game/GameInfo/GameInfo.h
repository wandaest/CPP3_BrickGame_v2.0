#ifndef GAMEINFO_H
#define GAMEINFO_H

// Определение размеров игрового поля. Основное поле включает зону для спауна
// фигур.
#define FIELD_ROWS 23 // 3 строки сверху используются для зоны спауна фигур
#define FIELD_COLS 10

// Определение размеров поля для отображения следующей фигуры.
#define NEXT_FIELD_ROWS 4
#define NEXT_FIELD_COLS 4

// Определение максимального уровня игры и количества очков для повышения
// уровня.
#define MAX_GAME_LEVEL 10
#define SCORE_PER_LEVEL 600
#define CYCLES 31

#ifdef TETRIS
#define SCORE_FILE_PATH "../src/game_score/t_score.txt"
#else
#define SCORE_FILE_PATH "../src/game_score/s_score.txt"
#endif

// Структура для хранения информации о состоянии игры.
typedef struct {
  int **field;    // Игровое поле
  int **next;     // Поле для следующей фигуры
  int score;      // Текущий счёт
  int high_score; // Высший счёт
  int level;      // Уровень сложности
  int speed;      // Скорость игры
  int pause;      // Флаг паузы
} GameInfo_t;

// Перечисление возможных состояний игры.
typedef enum {
  START = 0,
  SPAWN,
  MOVING,
  SHIFTING,
  ATTACHING,
  PAUSE,
  GAMEOVER,
  EXIT_STATE,
  WIN
} GameState;

// Перечисление возможных действий игрока.
typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action,
  NOSIG
} UserAction_t;

// Структура для хранения координат точки.
typedef struct {
  int row;
  int col;
} point;

// Оператор сравнения для point
#ifdef __cplusplus
inline bool operator==(const point &lhs, const point &rhs) {
  return lhs.row == rhs.row && lhs.col == rhs.col;
}
#endif

#endif