#include "s21_tetris.h"

// Изменение: Функции доступа к состоянию вместо глобальных переменных
GameInfo_t *game_info() {
  static GameInfo_t info = {0};
  static bool initialized = false;
  if (!initialized) {
    initialize_GameInfo_t(&info);
    initialized = true;
  }
  return &info;
}

static figure *next_fig() {
  static figure next = {0};
  static bool initialized = false;
  if (!initialized) {
    initialize_next_figure(&next);
    initialized = true;
  }
  return &next;
}

static figure *current_fig() {
  static figure current = {0};
  return &current;
}

GameState *game_state() {
  static GameState state = START;
  return &state;
}

static double *accumulated_time() {
  static double time = 0;
  return &time;
}

/**
 * @brief Таблица переходов состояния конечного автомата для обработки действий
 * пользователя.
 *
 * Таблица определяет, какое действие должно быть выполнено в зависимости от
 * текущего состояния игры и действия пользователя. Используется для реализации
 * конечного автомата.
 */
action fsm_table[8][9] = {
    {spawn, NULL, exitstate, NULL, NULL, NULL, NULL, NULL, NULL},
    {spawn, spawn, spawn, spawn, spawn, spawn, spawn, spawn, spawn},
    {NULL, paused, exitstate, move_figure_left, move_figure_right, NULL,
     move_figure_down, rotate, shifting},
    {shifting, shifting, shifting, shifting, shifting, shifting, shifting,
     shifting, shifting},
    {attaching, attaching, attaching, attaching, attaching, attaching,
     attaching, attaching, attaching},
    {paused, shifting, exitstate, paused, paused, paused, paused, paused,
     paused},
    {gameover, gameover, gameover, gameover, gameover, gameover, gameover,
     gameover, gameover},
    {exitstate, exitstate, exitstate, exitstate, exitstate, exitstate,
     exitstate, exitstate, exitstate}};

/**
 * @brief Обрабатывает пользовательский ввод и выполняет соответствующее
 * действие.
 *
 * Эта функция извлекает соответствующую функцию действия из таблицы конечного
 * автомата на основе текущего состояния игры и действия пользователя, и затем
 * выполняет это действие.
 *
 * @param sig Действие пользователя, полученное от функции get_user_action.
 * @param state Указатель на текущее состояние игры.
 * @param next_figure Указатель на следующую фигуру.
 * @param current_figure Указатель на текущую фигуру.
 * @param GameInfo Указатель на структуру с информацией о состоянии игры.
 */
void userInput(UserAction_t sig, bool hold) {
  GameParameters params = {game_state(), next_fig(), current_fig(), NULL};
  GameInfo_t *info = game_info();

  action act = fsm_table[*game_state()][sig];

  if (act) act(&params, info);  // Передаём адрес params

  if (hold && sig == Down && *game_state() == MOVING && !info->pause) {
    move_figure_down(&params, info);  // Передаём адрес params
  }
}

/**
 * @brief Перемещает текущую фигуру вниз и обновляет состояние игры.
 *
 * Если игра находится на паузе, функция сбрасывает тайм-аут и продолжает игру.
 * Фигура перемещается на одну строку вниз. Если перемещение приводит к
 * прикреплению фигуры, состояние игры меняется на ATTACHING. В противном
 * случае, обновляется состояние игры и отображается текущее состояние. Фигура
 * удаляется с игрового поля.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void shifting(GameParameters *params, GameInfo_t *GameInfo) {
  if (GameInfo->pause == 1) {
    GameInfo->pause =
        0;  // Изменение: Убран timeout, так как он зависит от ncurses
  }
  remove_figure_from_field(&GameInfo->field, *params->current_figure);
  params->current_figure->center.row++;
  if (check_attaching(*GameInfo, *params)) {
    params->current_figure->center.row--;
    put_figure_on_field(&GameInfo->field, *params->current_figure);
    *params->state = ATTACHING;
  } else {
    put_figure_on_field(&GameInfo->field, *params->current_figure);
    *params->state = MOVING;
  }
}

/**
 * @brief Спавнит новую фигуру и проверяет окончание игры.
 *
 * Функция создает новую фигуру на основе следующей фигуры и проверяет
 * на наличие столкновений. Если фигура сталкивается с другими фигурами,
 * игра переходит в состояние GAMEOVER. В противном случае, обновляется скорость
 * игры, инициализируется следующая фигура, обновляется состояние игры и
 * отображается текущее состояние. Фигура удаляется с игрового поля.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void spawn(GameParameters *params, GameInfo_t *GameInfo) {
  spawn_new_figure(*params->next_figure, params->current_figure);
  if (check_collision(*GameInfo, *params->current_figure)) {
    *params->state = GAMEOVER;
  } else {
    update_speed(GameInfo);
    remove_figure_from_field(&GameInfo->next, *params->next_figure);
    initialize_next_figure(params->next_figure);
    put_figure_on_field(&GameInfo->next, *params->next_figure);
    put_figure_on_field(&GameInfo->field, *params->current_figure);
    *params->state = MOVING;
  }
}

/**
 * @brief Перемещает текущую фигуру вниз до тех пор, пока не обнаружит
 * столкновение.
 *
 * Функция перемещает текущую фигуру вниз по игровому полю, пока фигура не
 * начнет сталкиваться с другими фигурами или с нижней границей поля. После
 * этого фигура возвращается на одну строку вверх, и состояние игры меняется на
 * ATTACHING.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void move_figure_down(GameParameters *params, GameInfo_t *GameInfo) {
  remove_figure_from_field(&GameInfo->field, *params->current_figure);
  while (!check_attaching(*GameInfo, *params)) {
    params->current_figure->center.row++;
  }
  params->current_figure->center.row--;
  put_figure_on_field(&GameInfo->field, *params->current_figure);
  *params->state = ATTACHING;
}

/**
 * @brief Перемещает текущую фигуру вправо, если это возможно.
 *
 * Функция пытается переместить текущую фигуру на одну колонку вправо. Если
 * перемещение не приводит к столкновению с другими фигурами, фигура фактически
 * перемещается и обновляется состояние игры. Отображается текущее состояние
 * игры, и фигура удаляется с игрового поля.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void move_figure_right(GameParameters *params, GameInfo_t *GameInfo) {
  figure buffer_figure = *params->current_figure;
  buffer_figure.center.col++;
  remove_figure_from_field(&GameInfo->field, *params->current_figure);
  if (!check_collision(*GameInfo, buffer_figure)) {
    *params->current_figure = buffer_figure;
  }
  put_figure_on_field(&GameInfo->field, *params->current_figure);
  *params->state = MOVING;
}

/**
 * @brief Перемещает текущую фигуру влево, если это возможно.
 *
 * Функция пытается переместить текущую фигуру на одну колонку влево. Если
 * перемещение не приводит к столкновению с другими фигурами, фигура фактически
 * перемещается и обновляется состояние игры. Отображается текущее состояние
 * игры, и фигура удаляется с игрового поля.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void move_figure_left(GameParameters *params, GameInfo_t *GameInfo) {
  figure buffer_figure = *params->current_figure;
  buffer_figure.center.col--;
  remove_figure_from_field(&GameInfo->field, *params->current_figure);
  if (!check_collision(*GameInfo, buffer_figure)) {
    *params->current_figure = buffer_figure;
  }
  put_figure_on_field(&GameInfo->field, *params->current_figure);
  *params->state = MOVING;
}

/**
 * @brief Обрабатывает прикрепление текущей фигуры к игровому полю.
 *
 * После того, как фигура прикреплена к игровому полю, обновляется состояние
 * игры, и подсчитываются очки, которые зависят от количества очищенных линий.
 * Если текущий результат превышает рекордный, обновляется рекорд. Если верхняя
 * часть фигуры находится выше допустимого уровня, игра переходит в состояние
 * GAMEOVER. В противном случае игра переходит в состояние SPAWN для спауна
 * новой фигуры.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void attaching(GameParameters *params, GameInfo_t *GameInfo) {
  GameInfo->score += (pow(2, clear_full_lines(*params, GameInfo)) - 1) * 100;
  if (GameInfo->score > GameInfo->high_score) {
    GameInfo->high_score = GameInfo->score;
  }
  if (get_figure_top(*params->current_figure) < 3) {
    *params->state = GAMEOVER;
  } else {
    *params->state = SPAWN;
  }
}

/**
 * @brief Поворачивает текущую фигуру и обновляет состояние игры.
 *
 * Функция пытается повернуть текущую фигуру. Если поворот не приводит к
 * столкновению с другими фигурами и не нарушает правила игры, фигура фактически
 * поворачивается. Затем обновляется состояние игры, отображается текущее
 * состояние и фигура удаляется с игрового поля. Состояние игры устанавливается
 * на MOVING.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void rotate(GameParameters *params, GameInfo_t *GameInfo) {
  figure buffer = *params->current_figure;
  rotate_current_figure(&buffer);
  remove_figure_from_field(&GameInfo->field, *params->current_figure);
  if (!check_collision(*GameInfo, buffer) &&
      !check_collide_for_rotation(buffer, *GameInfo)) {
    *params->current_figure = buffer;
  }
  put_figure_on_field(&GameInfo->field, *params->current_figure);
  *params->state = MOVING;
}

/**
 * @brief Переключает игру в состояние паузы и отображает сообщение "PAUSED".
 *
 * Функция приостанавливает игру, устанавливая тайм-аут в неограниченное время и
 * устанавливая флаг паузы. На экране отображается сообщение "PAUSED",
 * выровненное по центру игрового поля. Состояние игры устанавливается на PAUSE.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void paused(GameParameters *params, GameInfo_t *GameInfo) {
  GameInfo->pause = 1;
  *params->state = PAUSE;
}

/**
 * @brief Обрабатывает состояние окончания игры и отображает сообщение "GAME
 * OVER!".
 *
 * Функция записывает рекордный результат в файл, отображает сообщение "GAME
 * OVER!" в центре игрового поля, ожидает ввода пользователя и делает паузу
 * перед выходом из состояния игры. Состояние игры устанавливается на GAMEOVER.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void gameover(GameParameters *params, GameInfo_t *GameInfo) {
  write_high_score(GameInfo);
  *params->state = GAMEOVER;
}

/**
 * @brief Завершает игру и переходит в состояние выхода.
 *
 * Функция записывает рекордный результат в файл и устанавливает состояние игры
 * на EXIT_STATE, что сигнализирует о завершении игры и выходе из неё.
 *
 * @param params Указатель на структуру GameParameters, содержащую параметры
 * игры.
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую информацию о
 * состоянии игры.
 */
void exitstate(GameParameters *params, GameInfo_t *GameInfo) {
  write_high_score(GameInfo);
  *params->state = EXIT_STATE;
}

// Изменение: init_game использует reset_game
void init_game(GameInfo_t *GameInfo_out, figure *next_figure_out,
               figure *current_figure_out, GameState *state_out) {
  reset_game();
  *GameInfo_out = *game_info();
  *next_figure_out = *next_fig();
  *current_figure_out = *current_fig();
  *state_out = *game_state();
}

// Добавлено: Функция сброса состояния
void reset_game() {
  free_GameInfo_t_memory(game_info());
  initialize_GameInfo_t(game_info());
  initialize_next_figure(next_fig());
  spawn_new_figure(*next_fig(), current_fig());
  *game_state() = START;
  *accumulated_time() = 0;
}

GameInfo_t updateCurrentState() {
  GameParameters params = {game_state(), next_fig(), current_fig(), NULL};
  GameInfo_t *info = game_info();

  if (*game_state() == MOVING && !info->pause) {
    int drop_interval = 1500 - info->speed * 20;
    *accumulated_time() += 16.67;  // Предполагаем 60 FPS (16.67 мс)
    if (*accumulated_time() >= drop_interval) {
      shifting(&params, info);
      *accumulated_time() -= drop_interval;
    }
  }

  return *info;
}

/**
 * @brief Выделяет память для двумерной матрицы целых чисел.
 *
 * Функция выделяет память для матрицы размером row x col и инициализирует её
 * нулевыми значениями.
 *
 * @param matrix Указатель на указатель на матрицу целых чисел.
 * @param row Количество строк в матрице.
 * @param col Количество столбцов в матрице.
 */
void allocate_matrix_memory(int ***matrix, int row, int col) {
  *matrix = (int **)calloc(row, sizeof(int *));
  for (int i = 0; i < row; i++) {
    (*matrix)[i] = (int *)calloc(col, sizeof(int));
  }
}

/**
 * @brief Освобождает память, занятую двумерной матрицей целых чисел.
 *
 * Функция освобождает память, выделенную для матрицы, и сама матрица.
 *
 * @param matrix Указатель на указатель на матрицу целых чисел.
 * @param row Количество строк в матрице.
 */
void free_matrix_memory(int ***matrix, int row) {
  for (int i = 0; i < row; i++) {
    free((*matrix)[i]);
  }
  free(*matrix);
}

/**
 * @brief Инициализирует структуру GameInfo_t.
 *
 * Функция выделяет память для игровых полей, инициализирует начальные значения
 * для очков, уровня, скорости и флага паузы, а также считывает рекордный
 * результат из файла.
 *
 * @param GameInfo Указатель на структуру GameInfo_t, которую нужно
 * инициализировать.
 */
void initialize_GameInfo_t(GameInfo_t *GameInfo) {
  allocate_matrix_memory(&GameInfo->field, FIELD_ROWS, FIELD_COLS);
  allocate_matrix_memory(&GameInfo->next, NEXT_FIELD_ROWS, NEXT_FIELD_COLS);
  GameInfo->score = 0;
  read_high_score(GameInfo);
  GameInfo->level = 1;
  GameInfo->speed = 1;
  GameInfo->pause = 0;
}

/**
 * @brief Читает рекордный результат из файла.
 *
 * Функция открывает файл, считывает рекордный результат и
 * сохраняет его в структуре GameInfo. Если файл не существует, рекордный
 * результат остаётся равным нулю.
 *
 * @param GameInfo Указатель на структуру GameInfo_t, в которую записывается
 * рекордный результат.
 */
void read_high_score(GameInfo_t *GameInfo) {
  GameInfo->high_score = 0;
  FILE *high_score = fopen(SCORE_FILE_PATH, "r");
  if (high_score) {
    fscanf(high_score, "%d", &GameInfo->high_score);
    fclose(high_score);
  }
}

/**
 * @brief Записывает рекордный результат в файл.
 *
 * Функция открывает файл для записи и сохраняет текущий
 * рекордный результат в этом файле.
 *
 * @param GameInfo Указатель на структуру GameInfo_t, содержащую рекордный
 * результат.
 */
void write_high_score(GameInfo_t *GameInfo) {
  FILE *high_score = fopen(SCORE_FILE_PATH, "w");
  if (high_score) {
    fprintf(high_score, "%d", GameInfo->high_score);
    fclose(high_score);
  }
}

/**
 * @brief Освобождает память, занятую структурой GameInfo_t.
 *
 * Функция освобождает память, выделенную для полей field и next в структуре
 * GameInfo_t.
 *
 * @param GameInfo Указатель на структуру GameInfo_t, память которой требуется
 * освободить.
 */
void free_GameInfo_t_memory(GameInfo_t *GameInfo) {
  free_matrix_memory(&GameInfo->field, FIELD_ROWS);
  free_matrix_memory(&GameInfo->next, NEXT_FIELD_ROWS);
}

/**
 * @brief Инициализирует фигуру типа O.
 *
 * Функция задаёт координаты точек фигуры O (квадрат), устанавливает тип фигуры.
 *
 * @param next_figure Указатель на структуру figure, представляющую фигуру типа
 * O.
 */
void initialize_O_figure(figure *next_figure) {
  next_figure->points[1].row = 1;
  next_figure->points[1].col = 0;
  next_figure->points[2].row = 1;
  next_figure->points[2].col = -1;
  next_figure->points[3].row = 0;
  next_figure->points[3].col = -1;
  next_figure->type = O;
}

/**
 * @brief Инициализирует фигуру типа I.
 *
 * Функция задаёт координаты точек фигуры I (палка), устанавливает тип фигуры.
 *
 * @param next_figure Указатель на структуру figure, представляющую фигуру типа
 * I.
 */
void initialize_I_figure(figure *next_figure) {
  next_figure->points[1].row = -1;
  next_figure->points[1].col = 0;
  next_figure->points[2].row = 1;
  next_figure->points[2].col = 0;
  next_figure->points[3].row = 2;
  next_figure->points[3].col = 0;
  next_figure->type = I;
}

/**
 * @brief Инициализирует фигуру типа S.
 *
 * Функция задаёт координаты точек фигуры S (зигзаг), устанавливает тип фигуры.
 *
 * @param next_figure Указатель на структуру figure, представляющую фигуру типа
 * S.
 */
void initialize_S_figure(figure *next_figure) {
  next_figure->points[1].row = 0;
  next_figure->points[1].col = 1;
  next_figure->points[2].row = 1;
  next_figure->points[2].col = 0;
  next_figure->points[3].row = 1;
  next_figure->points[3].col = -1;
  next_figure->type = S;
}

/**
 * @brief Инициализирует фигуру типа Z.
 *
 * Функция задаёт координаты точек фигуры Z (зигзаг наоборот), устанавливает тип
 * фигуры.
 *
 * @param next_figure Указатель на структуру figure, представляющую фигуру типа
 * Z.
 */
void initialize_Z_figure(figure *next_figure) {
  next_figure->points[1].row = 0;
  next_figure->points[1].col = -1;
  next_figure->points[2].row = 1;
  next_figure->points[2].col = 0;
  next_figure->points[3].row = 1;
  next_figure->points[3].col = 1;
  next_figure->type = Z;
}

/**
 * @brief Инициализирует фигуру типа L.
 *
 * Функция задаёт координаты точек фигуры L, устанавливает тип фигуры.
 *
 * @param next_figure Указатель на структуру figure, представляющую фигуру типа
 * L.
 */
void initialize_L_figure(figure *next_figure) {
  next_figure->points[1].row = 0;
  next_figure->points[1].col = -1;
  next_figure->points[2].row = 0;
  next_figure->points[2].col = 1;
  next_figure->points[3].row = 1;
  next_figure->points[3].col = -1;
  next_figure->type = L;
}

/**
 * @brief Инициализирует фигуру типа J.
 *
 * Функция задаёт координаты точек фигуры J, устанавливает тип фигуры.
 *
 * @param next_figure Указатель на структуру figure, представляющую фигуру типа
 * J.
 */
void initialize_J_figure(figure *next_figure) {
  next_figure->points[1].row = 0;
  next_figure->points[1].col = -1;
  next_figure->points[2].row = 0;
  next_figure->points[2].col = 1;
  next_figure->points[3].row = 1;
  next_figure->points[3].col = 1;
  next_figure->type = J;
}

/**
 * @brief Инициализирует фигуру типа T.
 *
 * Функция задаёт координаты точек фигуры T, устанавливает тип фигуры.
 *
 * @param next_figure Указатель на структуру figure, представляющую фигуру типа
 * T.
 */
void initialize_T_figure(figure *next_figure) {
  next_figure->points[1].row = 0;
  next_figure->points[1].col = -1;
  next_figure->points[2].row = 0;
  next_figure->points[2].col = 1;
  next_figure->points[3].row = 1;
  next_figure->points[3].col = 0;
  next_figure->type = T;
}

/**
 * @brief Инициализирует следующую фигуру.
 *
 * Функция случайным образом выбирает тип фигуры и инициализирует её координаты.
 * Также устанавливает начальное положение фигуры и её поворот.
 *
 * @param next_figure Указатель на структуру figure, представляющую следующую
 * фигуру.
 */
void initialize_next_figure(figure *next_figure) {
  next_figure->center.row = 1;
  next_figure->center.col = 2;
  next_figure->rotation = -1;
  int random_figure = rand() % 7;
  next_figure->points[0].row = 0;
  next_figure->points[0].col = 0;
  switch ((FigureType)random_figure) {
    case O:
      initialize_O_figure(next_figure);
      break;
    case I:
      initialize_I_figure(next_figure);
      break;
    case S:
      initialize_S_figure(next_figure);
      break;
    case Z:
      initialize_Z_figure(next_figure);
      break;
    case L:
      initialize_L_figure(next_figure);
      break;
    case J:
      initialize_J_figure(next_figure);
      break;
    case T:
      initialize_T_figure(next_figure);
      break;
    default:
      break;
  }

  int i = rand() % 4;
  while (i > 0) {
    rotate_current_figure(next_figure);
    i--;
  }
}

/**
 * @brief Проверяет возможные коллизии фигуры при её вращении.
 *
 * Функция проверяет, возникает ли коллизия фигуры при её вращении на игровом
 * поле.
 *
 * @param figure Структура figure, представляющая фигуру, которую нужно
 * проверить.
 * @param GameInfo Структура GameInfo_t, содержащая текущее состояние игрового
 * поля.
 * @return true Если коллизия обнаружена, false в противном случае.
 */
bool check_collide_for_rotation(figure figure, GameInfo_t GameInfo) {
  bool rc = false;
  for (int i = 1; i < 4; i++) {
    if (abs(figure.points[i].row) != abs(figure.points[i].col)) {
      if (abs(figure.points[i].row) == 2 || abs(figure.points[i].col) == 2) {
        if ((GameInfo.field[figure.center.row + 2][figure.center.col - 2] ==
             1) ||
            (GameInfo.field[figure.center.row + 2][figure.center.col - 1] ==
             1) ||
            (GameInfo.field[figure.center.row + 1][figure.center.col - 2] ==
             1)) {
          rc = true;
          break;
        }
      } else {
        int absolute_column = get_absolute_position(figure, i).col;
        int absolute_row = get_absolute_position(figure, i).row;
        if (GameInfo
                .field[absolute_row + figure.rotation * figure.points[i].col]
                      [absolute_column -
                       figure.rotation * figure.points[i].row] == 1) {
          rc = true;
          break;
        }
      }
    }
  }
  return rc;
}

/**
 * @brief Поворачивает текущую фигуру на 90 градусов.
 *
 * Функция изменяет ориентацию фигуры в зависимости от её типа и текущего угла
 * поворота. Фигура типа I, Z и S поворачивается на 90 градусов в обе стороны.
 *
 * @param next_figure Указатель на структуру figure, представляющую фигуру,
 * которую нужно повернуть.
 */
void rotate_current_figure(figure *next_figure) {
  int buffer_row;
  if (next_figure->type != O) {
    for (int i = 0; i < 4; i++) {
      buffer_row = next_figure->points[i].row;
      next_figure->points[i].row =
          -next_figure->points[i].col * next_figure->rotation;
      next_figure->points[i].col = buffer_row * next_figure->rotation;
    }
    if (next_figure->type == I || next_figure->type == Z ||
        next_figure->type == S) {
      next_figure->rotation = -next_figure->rotation;
    }
  }
}

/**
 * @brief Удаляет фигуру с игрового поля.
 *
 * Функция устанавливает значение 0 в ячейках поля, занимаемых фигуре.
 *
 * @param matrix Указатель на матрицу, представляющую игровое поле.
 * @param figure Структура figure, представляющая фигуру, которую нужно удалить.
 */
void remove_figure_from_field(int ***matrix, figure figure) {
  for (int i = 0; i < 4; i++) {
    (*matrix)[get_absolute_position(figure, i).row]
             [get_absolute_position(figure, i).col] = 0;
  }
}

/**
 * @brief Размещает фигуру на игровом поле.
 *
 * Функция устанавливает значение 1 в ячейках поля, занимаемых фигуре.
 *
 * @param matrix Указатель на матрицу, представляющую игровое поле.
 * @param figure Структура figure, представляющая фигуру, которую нужно
 * разместить.
 */
void put_figure_on_field(int ***matrix, figure figure) {
  for (int i = 0; i < 4; i++) {
    (*matrix)[figure.center.row + figure.points[i].row]
             [figure.center.col + figure.points[i].col] = 1;
  }
}

/**
 * @brief Спавнит новую фигуру, устанавливая её в текущее положение.
 *
 * Функция копирует фигуру следующего типа в текущее состояние и устанавливает
 * её начальное положение.
 *
 * @param next_figure Фигура следующего типа, которую нужно спавнить.
 * @param current_figure Указатель на структуру figure, представляющую текущую
 * фигуру.
 */
void spawn_new_figure(figure next_figure, figure *current_figure) {
  *current_figure = next_figure;
  current_figure->center.col = 5;
  current_figure->center.row = 4 - get_figure_bottom(*current_figure);
}

/**
 * @brief Обновляет состояние игрового поля.
 *
 * Функция размещает фигуры на игровом поле и возвращает обновлённое состояние
 * GameInfo_t.
 *
 * @param GameInfo Указатель на структуру GameInfo_t, представляющую текущее
 * состояние игры.
 * @param next_figure Фигура следующего типа, которую нужно разместить на поле.
 * @param current_figure Текущая фигура, которую нужно разместить на поле.
 * @return Обновлённое состояние GameInfo_t.
 */
/*GameInfo_t updateCurrentState(GameInfo_t *GameInfo, figure next_figure,
                              figure current_figure) {
  put_figure_on_field(&GameInfo->next, next_figure);
  put_figure_on_field(&GameInfo->field, current_figure);
  return *GameInfo;
}*/

/**
 * @brief Проверяет, пересекается ли фигура с другими фигурами или границами
 * поля.
 *
 * Эта функция проверяет, есть ли столкновения фигуры с границами игрового поля
 * или с другими фигурами. Проверка проводится по каждому блоку фигуры.
 *
 * @param GameInfo Информация об игре, включая текущее состояние игрового поля.
 * @param figure Фигура, которую нужно проверить на столкновение.
 * @return Возвращает true, если фигура пересекается с другими фигурами или
 * границами, и false в противном случае.
 */
bool check_collision(GameInfo_t GameInfo, figure figure) {
  bool rc = false;
  for (int i = 0; i < 4; i++) {
    int absolute_column = get_absolute_position(figure, i).col;
    int absolute_row = get_absolute_position(figure, i).row;
    if ((absolute_column > FIELD_COLS - 1) || (absolute_column < 0)) {
      rc = true;
      break;
    }
    if (GameInfo.field[absolute_row][absolute_column] == 1) {
      rc = true;
    }
  }
  return rc;
}

/**
 * @brief Определяет абсолютное положение точки фигуры на игровом поле.
 *
 * Эта функция вычисляет абсолютные координаты точки фигуры в зависимости от
 * положения центра фигуры и координат точки относительно центра.
 *
 * @param figure Фигура, для которой нужно определить абсолютное положение.
 * @param i Индекс точки фигуры.
 * @return Абсолютные координаты точки фигуры.
 */
point get_absolute_position(figure figure, int i) {
  point absolute;
  absolute.row = figure.points[i].row + figure.center.row;
  absolute.col = figure.points[i].col + figure.center.col;
  return absolute;
}

/**
 * @brief Проверяет, прикрепляется ли фигура к игровому полю.
 *
 * Эта функция проверяет, прикреплена ли фигура к игровому полю, то есть
 * пересекается ли фигура с нижней границей поля или с другими фигурами.
 *
 * @param GameInfo Информация об игре, включая текущее состояние игрового поля.
 * @param params Параметры игры, включая текущую фигуру.
 * @return Возвращает true, если фигура прикрепляется, и false в противном
 * случае.
 */
bool check_attaching(GameInfo_t GameInfo, GameParameters params) {
  bool rc = false;
  for (int i = 0; i < 4; i++) {
    int absolute_column = get_absolute_position(*params.current_figure, i).col;
    int absolute_row = get_absolute_position(*params.current_figure, i).row;
    if ((absolute_row >= FIELD_ROWS - 1) ||  // Изменено с == FIELD_ROWS
        GameInfo.field[absolute_row][absolute_column] == 1) {
      rc = true;
    }
  }
  return rc;
}

/**
 * @brief Определяет самую нижнюю строку фигуры на игровом поле.
 *
 * Эта функция вычисляет наибольшую строку, занятую фигурой, чтобы определить,
 * насколько низко фигура опускается на поле.
 *
 * @param figure Фигура, для которой нужно определить самую нижнюю строку.
 * @return Индекс самой нижней строки, занятой фигурой.
 */
int get_figure_bottom(figure figure) {
  int max_row = figure.center.row;
  for (int i = 0; i < 4; i++) {
    int absolute_row = get_absolute_position(figure, i).row;
    if (absolute_row > max_row) {
      max_row = absolute_row;
    }
  }
  return max_row;
}

/**
 * @brief Определяет самую верхнюю строку фигуры на игровом поле.
 *
 * Эта функция вычисляет наименьшую строку, занятую фигурой, чтобы определить,
 * насколько высоко фигура поднимается на поле.
 *
 * @param figure Фигура, для которой нужно определить самую верхнюю строку.
 * @return Индекс самой верхней строки, занятой фигурой.
 */
int get_figure_top(figure figure) {
  int min_row = figure.center.row;
  for (int i = 0; i < 4; i++) {
    int absolute_row = get_absolute_position(figure, i).row;
    if (absolute_row < min_row) {
      min_row = absolute_row;
    }
  }
  return min_row;
}

/**
 * @brief Удаляет заполненные линии на игровом поле и возвращает количество
 * удаленных линий.
 *
 * Эта функция проверяет все строки, которые пересекаются с фигурой, и удаляет
 * заполненные строки. После удаления строки все строки выше опускаются вниз.
 *
 * @param params Параметры игры, включая текущую фигуру.
 * @param GameInfo Информация об игре, включая текущее состояние игрового поля.
 * @return Количество удаленных линий.
 */
int clear_full_lines(GameParameters params, GameInfo_t *GameInfo) {
  int rc = 0;
  int row_min = get_figure_top(*params.current_figure);
  int row_max = get_figure_bottom(*params.current_figure) + 1;
  for (int i = row_min; i < row_max; i++) {
    int gap = 0;
    for (int j = 0; j < FIELD_COLS; j++) {
      if (GameInfo->field[i][j] == 0) {
        gap++;
        break;
      }
    }
    if (gap == 0) {
      rc++;
      for (int k = i; k > 2; k--) {
        for (int m = 0; m < FIELD_COLS; m++) {
          GameInfo->field[k][m] = GameInfo->field[k - 1][m];
        }
      }
    }
  }
  return rc;
}

/**
 * @brief Обновляет скорость игры в зависимости от уровня.
 *
 * Эта функция изменяет уровень и скорость игры на основе текущего счета игрока.
 * Уровень и скорость увеличиваются по мере набора очков.
 *
 * @param GameInfo Информация об игре, включая текущий уровень и скорость.
 */
void update_speed(GameInfo_t *GameInfo) {
  if ((GameInfo->score > SCORE_PER_LEVEL * GameInfo->level) &&
      (GameInfo->level < MAX_GAME_LEVEL)) {
    GameInfo->level = GameInfo->score / SCORE_PER_LEVEL + 1;
    if (GameInfo->level > MAX_GAME_LEVEL) GameInfo->level = MAX_GAME_LEVEL;
    GameInfo->speed = GameInfo->level;
    // timeout(500 - GameInfo->speed * 40);
  }
}
