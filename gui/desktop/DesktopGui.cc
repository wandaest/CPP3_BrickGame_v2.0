#include "DesktopGui.h"

namespace s21 {

DesktopGui::DesktopGui(Controller *controller)
    : QMainWindow(nullptr), controller_(controller), user_action_(NOSIG),
      game_state_(START), label_(new QLabel(this)),
      hi_score_label_(new QLabel(this)), score_label_(new QLabel(this)),
      level_label_(new QLabel(this)), speed_label_(new QLabel(this)),
      timer_(new QTimer(this)) {
  setWindowTitle("Tetris");
  setFixedSize(530, 600);

  game_info_ = controller_->updateCurrentState();

  init_labels();
  connect(timer_, &QTimer::timeout, this, &DesktopGui::update_data);
  timer_->start(200);
}

DesktopGui::~DesktopGui() {
  delete timer_;
  delete label_;
  delete hi_score_label_;
  delete score_label_;
  delete level_label_;
  delete speed_label_;
}

void DesktopGui::init_labels() {
  QFont font;
  font.setPointSize(14);

  label_->setAlignment(Qt::AlignCenter);
  label_->setFont(font);
  label_->setGeometry(0, 270, 320, 60);

  QPalette palette;
  palette.setColor(QPalette::WindowText, Qt::white);

  hi_score_label_->setAlignment(Qt::AlignLeft);
  hi_score_label_->setFont(font);
  hi_score_label_->setGeometry(350, 70, 180, 30);
  hi_score_label_->setPalette(palette);

  score_label_->setAlignment(Qt::AlignLeft);
  score_label_->setFont(font);
  score_label_->setGeometry(350, 110, 180, 30);
  score_label_->setPalette(palette);

  level_label_->setAlignment(Qt::AlignLeft);
  level_label_->setFont(font);
  level_label_->setGeometry(350, 150, 100, 30);
  level_label_->setPalette(palette);

  speed_label_->setAlignment(Qt::AlignLeft);
  speed_label_->setFont(font);
  speed_label_->setGeometry(350, 190, 100, 30);
  speed_label_->setPalette(palette);
}

void DesktopGui::update_data() {
  controller_->userInput(user_action_, false);
  game_info_ = controller_->updateCurrentState();
  game_state_ = controller_->getGameState();

  repaint();

  if (game_state_ == GAMEOVER) {
    usleep(2000000);
    timer_->stop();
    print_phrase();
  } else if (game_state_ == EXIT_STATE) {
    usleep(2000000);
    timer_->stop();
    close();
  }

  user_action_ = NOSIG;
}

void DesktopGui::print_phrase() {
  if (game_state_ == START) {
    label_->setText("Press S to start!");
  } else if (game_state_ == PAUSE) {
    label_->setText("PAUSED");
  } else if (game_state_ == GAMEOVER) {
    label_->setText("GAME OVER!");
  } else if (game_state_ == MOVING) {
    label_->setText("");
  }
}

void DesktopGui::paintEvent(QPaintEvent *event) {
  QMainWindow::paintEvent(event);
  QPainter painter(this);

  // Window background.
  painter.setPen(QColor("#000000"));
  painter.setBrush(QBrush(QColor("#000000")));
  painter.drawRect(0, 0, 530, 600);

  // Game field background.
  painter.setBrush(QBrush(QColor("#2c2d2b")));
  painter.drawRect(1, 1, 309, 600);

  // Stats panel background.
  painter.setBrush(QBrush(QColor("#2c2d2b")));
  painter.drawRect(310, 1, 219, 600);

  // Draw snake/figures and apple.
  const int kVisibleRows = 20;
  int start_row = 0;
#ifdef TETRIS
  start_row = 3;
#endif
  for (int i = 0; i < kVisibleRows; i++) {
    int field_row = start_row + i;
    for (int j = 0; j < FIELD_COLS; j++) {
      if (game_info_.field[field_row][j] == 1) {
#ifdef TETRIS
        painter.setBrush(QBrush(QColor("#00BFFF")));
#else
        painter.setBrush(QBrush(QColor("#00FF00")));
#endif
        painter.drawRect(1 + 30 * j + 1 * j, 1 + 30 * i + 1 * i, 30, 30);
      } else if (game_info_.field[field_row][j] == 2) {
        painter.setBrush(QBrush(QColor("#FF0000")));
        painter.drawRect(1 + 30 * j + 1 * j, 1 + 30 * i + 1 * i, 30, 30);
      }
    }
  }

  // Draw next figure (Tetris only).
#ifdef TETRIS
  for (int i = 0; i < NEXT_FIELD_ROWS; i++) {
    for (int j = 0; j < NEXT_FIELD_COLS; j++) {
      if (game_info_.next[i][j] == 1) {
        painter.setBrush(QBrush(QColor("#00BFFF")));
        painter.drawRect(350 + 30 * j + 1 * j, 270 + 30 * i + 1 * i, 30, 30);
      }
    }
  }
#endif

  // Update stats.
  score_label_->setText("Score - " + QString::number(game_info_.score));
  hi_score_label_->setText("HiScore - " +
                           QString::number(game_info_.high_score));
  level_label_->setText("Level - " + QString::number(game_info_.level));
  speed_label_->setText("Speed - " + QString::number(game_info_.speed));
}

void DesktopGui::keyPressEvent(QKeyEvent *event) {
  int key = event->key();
  QString text = event->text().toLower();

  if (key == Qt::Key_Escape) {
    user_action_ = Terminate;
  } else if (text == "s") {
    user_action_ = Start;
  } else if (text == "p" || key == Qt::Key_Space) {
    user_action_ = Pause;
  } else if (key == Qt::Key_Up) {
    user_action_ = Up;
  } else if (key == Qt::Key_Down) {
    user_action_ = Down;
  } else if (key == Qt::Key_Left) {
    user_action_ = Left;
  } else if (key == Qt::Key_Right) {
    user_action_ = Right;
  } else if (key == Qt::Key_Return || key == Qt::Key_Enter) {
    user_action_ = Action;
  } else {
    user_action_ = NOSIG;
  }
}

} // namespace s21