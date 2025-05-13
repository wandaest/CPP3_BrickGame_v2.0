#ifndef S21_DESKTOP_GUI_H
#define S21_DESKTOP_GUI_H

#include <unistd.h>

#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPainter>
#include <QTimer>

#include "../../controller/Controller.h"

namespace s21 {

class DesktopGui : public QMainWindow {
  Q_OBJECT

public:
  explicit DesktopGui(Controller *controller);
  ~DesktopGui();

protected:
  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

private:
  void init_labels();
  void update_data();
  void print_phrase();

  Controller *controller_;
  UserAction_t user_action_;
  GameState game_state_;
  GameInfo_t game_info_;
  QLabel *label_;
  QLabel *hi_score_label_;
  QLabel *score_label_;
  QLabel *level_label_;
  QLabel *speed_label_;
  QTimer *timer_;
};

} // namespace s21

#endif // S21_DESKTOP_GUI_H