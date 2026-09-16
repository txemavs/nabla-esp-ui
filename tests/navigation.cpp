#include "../external_components/nabla_navigation/navigation.h"
#include <cassert>
namespace nabla {
const Node nodes[] = {{"Home",-1,"","",0},{"Settings",0,"","",0},{"Wi-Fi",1,"","",0}};
const int count = 3;
}
int main() {
  int calls = 0;
  bool online = false;
  nabla::commands[2].available = [&]() { return online; };
  nabla::commands[2].execute = [&]() { ++calls; };
  nabla::execute_command(-1);
  nabla::execute_command(128);
  nabla::execute_command(1);
  nabla::execute_command(2);
  assert(calls == 0);
  online = true;
  nabla::execute_command(2);
  assert(calls == 1);
  online = false;
  nabla::execute_command(2);
  assert(calls == 1);

  assert(nabla::children(0) == 1);
  assert(nabla::child(1, 0) == 2);
  assert(nabla::path(2) == "Home > Settings > Wi-Fi");
  assert(nabla::path(-1).empty());
}
