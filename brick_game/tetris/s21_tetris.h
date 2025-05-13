#ifndef TETRIS_H
#define TETRIS_H

#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../GameInfo/GameInfo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPAWN_ZONE_HEIGHT 3
#define FIELD_LEFT_COLUMN 0

// Определение размеров поля для отображения следующей фигуры.
#define NEXT_FIELD_ROWS 4
#define NEXT_FIELD_COLS 4

// Определение кодов клавиш для управления игрой.
#define ESCAPE_KEY 27
#define RETURN_KEY 10
#define SPACE_KEY 32
#define ENTER_KEY 10

// Определение размеров карты и области для отображения игровых данных.
#define MAP_ROWS 20
#define MAP_COLS 90

// Начальная позиция для отображения игрового поля.
#define BOARD_START_ROW 2

// Определение начальных координат для размещения фигуры на игровом поле и
// тайм-аут для начальной игры.
#define START_POS_X (BOARD_WIDTH / 2)
#define START_POS_Y (BOARD_HEIGHT)
#define INITIAL_GAME_TIMEOUT 150

// Определение размеров игрового поля, панели HUD и отступов.
#define BOARD_HEIGHT 21
#define BOARD_WIDTH 10
#define HUD_PANEL_WIDTH 8
#define BOARD_PADDING 3

// Перечисление возможных типов фигур.
typedef enum { O, I, S, Z, L, J, T } FigureType;

// Структура для хранения информации о фигуре, включая её тип, центр, точки и
// текущую ротацию.
typedef struct {
  FigureType type;
  point center;
  point points[4];
  int rotation;
} figure;

// Структура для передачи параметров игры между функциями.
typedef struct {
  GameState *state;       // Текущее состояние игры
  figure *next_figure;    // Следующая фигура
  figure *current_figure; // Текущая фигура
  bool *break_flag;       // Флаг для завершения игры
} GameParameters;

// Определение типа функции для обработки действий игрока.
typedef void (*action)(GameParameters *params, GameInfo_t *GameInfo);

// Прототипы функций для основной игровой логики и обработки.
// void run_game_loop(); // Запуск основного игрового цикла.

GameState *game_state();
GameInfo_t *game_info();

void userInput(UserAction_t sig, bool hold);
GameInfo_t updateCurrentState();
void reset_game();

void init_game(GameInfo_t *GameInfo, figure *next_figure,
               figure *current_figure, GameState *state);
void update_game(GameInfo_t *GameInfo, figure *next_figure,
                 figure *current_figure, GameState *state, UserAction_t sig,
                 double elapsed_time);

void allocate_matrix_memory(int ***matrix, int row,
                            int col); // Выделение памяти для матрицы.
void free_matrix_memory(int ***matrix,
                        int row); // Освобождение памяти матрицы.
void free_GameInfo_t_memory(
    GameInfo_t *GameInfo); // Освобождение памяти GameInfo_t.
void initialize_GameInfo_t(GameInfo_t *GameInfo); // Инициализация GameInfo_t.
void initialize_next_figure(
    figure *next_figure); // Инициализация следующей фигуры.
void initialize_O_figure(figure *next_figure); // Инициализация фигуры типа O.
void initialize_I_figure(figure *next_figure); // Инициализация фигуры типа I.
void initialize_S_figure(figure *next_figure); // Инициализация фигуры типа S.
void initialize_Z_figure(figure *next_figure); // Инициализация фигуры типа Z.
void initialize_L_figure(figure *next_figure); // Инициализация фигуры типа L.
void initialize_J_figure(figure *next_figure); // Инициализация фигуры типа J.
void initialize_T_figure(figure *next_figure); // Инициализация фигуры типа T.
void rotate_current_figure(figure *next_figure); // Поворот текущей фигуры.
bool check_collide_for_rotation(
    figure figure,
    GameInfo_t GameInfo); // Проверка на столкновение при повороте.
void remove_figure_from_field(
    int ***matrix, figure figure); // Удаление фигуры с игрового поля.
void put_figure_on_field(int ***matrix,
                         figure figure); // Размещение фигуры на игровом поле.
void spawn_new_figure(figure next_figure,
                      figure *current_figure); // Спавн новой фигуры.
bool check_collision(GameInfo_t GameInfo,
                     figure figure); // Проверка на столкновение.
bool check_attaching(GameInfo_t GameInfo,
                     GameParameters params); // Проверка на прикрепление фигуры.
int get_figure_bottom(figure figure); // Получение нижней координаты фигуры.
int get_figure_top(figure figure); // Получение верхней координаты фигуры.
int clear_full_lines(GameParameters params,
                     GameInfo_t *GameInfo); // Очистка полных линий.
void read_high_score(GameInfo_t *GameInfo); // Чтение высшего счёта.
void write_high_score(GameInfo_t *GameInfo); // Запись высшего счёта.
void update_speed(GameInfo_t *GameInfo); // Обновление скорости игры.
point get_absolute_position(
    figure figure, int i); // Получение абсолютной позиции точки фигуры.

// Прототипы функций для обработки действий игрока и состояния игры.
void spawn(GameParameters *params,
           GameInfo_t *GameInfo); // Спавн новой фигуры.
void move_figure_down(GameParameters *params,
                      GameInfo_t *GameInfo); // Перемещение фигуры вниз.
void move_figure_right(GameParameters *params,
                       GameInfo_t *GameInfo); // Перемещение фигуры вправо.
void move_figure_left(GameParameters *params,
                      GameInfo_t *GameInfo); // Перемещение фигуры влево.
void shifting(GameParameters *params,
              GameInfo_t *GameInfo); // Обработка изменения позиции фигуры.
void attaching(GameParameters *params,
               GameInfo_t *GameInfo); // Обработка прикрепления фигуры.
void paused(GameParameters *params,
            GameInfo_t *GameInfo); // Обработка состояния паузы.
void gameover(GameParameters *params,
              GameInfo_t *GameInfo); // Обработка завершения игры.
void exitstate(GameParameters *params,
               GameInfo_t *GameInfo); // Обработка выхода из игры.
void rotate(GameParameters *params,
            GameInfo_t *GameInfo); // Обработка поворота фигуры.

#ifdef __cplusplus
}
#endif

#endif // TETRIS_H
