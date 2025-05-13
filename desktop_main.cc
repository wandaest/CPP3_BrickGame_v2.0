#include <QApplication>
#include <QDir>

#include "./brick_game/snake/SnakeModel.h"
#include "./brick_game/tetris/s21_tetris_model.h"
#include "./controller/Controller.h"
#include "./gui/desktop/DesktopGui.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // Устанавливаем текущую директорию на директорию исполняемого файла
  QDir::setCurrent(QCoreApplication::applicationDirPath());

#ifdef TETRIS
  s21::TetrisModel model;
#else
  s21::SnakeModel model;
#endif
  s21::Controller controller(&model);
  s21::DesktopGui gui(&controller);
  gui.show();

  return app.exec();
}