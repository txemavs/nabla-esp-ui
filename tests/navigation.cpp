#include "../navigation/tree.h"
#include <cassert>
int main() {
  assert(nabla::nodes[0].parent == -1);
  for (int n = 1; n < nabla::count; n++) {
    int current = n, depth = 0;
    while (current != 0 && depth++ < nabla::count) {
      assert(nabla::valid(nabla::nodes[current].parent));
      current = nabla::nodes[current].parent;
    }
    assert(current == 0);
  }
  assert(nabla::child(1, 0) == 9);
  assert(nabla::child(1, 2) == 11);
  assert(nabla::child(1, 3) == 17);
  assert(nabla::child(1, 4) == -1);
  assert(nabla::path(9) == "Inicio > Ajustes > Wi-Fi");
  assert(nabla::path(14) == "Inicio > Sensores > Salón > Temperatura");
  assert(nabla::path(-1).empty());
}
