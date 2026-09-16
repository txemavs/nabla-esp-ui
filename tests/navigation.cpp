#include "../external_components/nabla_navigation/navigation.h"
#include <cassert>
namespace nabla {
const Node nodes[] = {{"Home",-1,"","",0},{"Settings",0,"","",0},{"Wi-Fi",1,"","",0}};
const int count = 3;
}
int main() {
  assert(nabla::children(0) == 1);
  assert(nabla::child(1, 0) == 2);
  assert(nabla::path(2) == "Home > Settings > Wi-Fi");
  assert(nabla::path(-1).empty());
}
