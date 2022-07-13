#include <iostream>
#include "test_server/proto_test/msg.pb.h"

int main() {
  test::Content c;
  c.set_abc(12345);
  c.set_def("MMKMKMK");
  c.set_ghj(90000);
  test::AAA* aaa = c.mutable_jkl();
  aaa->set_aaa("XKXK");

  std::cout << c.jkl().aaa() << std::endl;
  std::cout << c.ppp() << std::endl;
  std::cout << c.has_jkl() << std::endl;
  std::cout << c.jkl().has_aaa() << std::endl;
  std::cout << ":" << c.jkl().aaa().empty() << std::endl;
  if (10) {
    std::cout << "PP:" << std::endl;
  }
  return 0;
}
