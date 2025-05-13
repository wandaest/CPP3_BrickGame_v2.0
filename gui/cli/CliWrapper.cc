#include "CliWrapper.h"

#include <unistd.h>

namespace s21 {

CliWrapper::CliWrapper(Controller *controller) : controller_(controller) {}

void CliWrapper::loop() {
  init_gui();

  bool running = true;
  GameInfo_t game_info = controller_->updateCurrentState();
  GameState state = controller_->getGameState();

  while (running) {
    render_game(game_info, state);

    int input = getch();
    UserAction_t action = get_user_action(input);
    bool hold = (input != ERR);

    controller_->userInput(action, hold);
    game_info = controller_->updateCurrentState();
    state = controller_->getGameState();

    if (state == GAMEOVER) {
      timeout(-1);
      getch();
      sleep(2);
      running = false;
    } else if (state == EXIT_STATE) {
      running = false;
    }

    usleep(100000); // Delay for ~10 FPS.
  }

  endwin();
}

} // namespace s21