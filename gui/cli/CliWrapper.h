#ifndef CLIWRAPPER_H
#define CLIWRAPPER_H

#include "../../controller/Controller.h"

extern "C" {
#include "CliView.h"
}

namespace s21 {

class CliWrapper {
public:
  explicit CliWrapper(Controller *controller);
  void loop();

private:
  Controller *controller_;
};

} // namespace s21

#endif // S21_CLI_WRAPPER_H_