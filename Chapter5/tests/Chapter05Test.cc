#include "../Include/graph_visualizer.h"
#include "../Include/node/return_node.h"
#include "../Include/node/stop_node.h"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

TEST(SimpleTest, testIfStmt) {
  std::string source = R"(
  int a = 1;
  if (arg == 1) {
     a = arg + 2;
  } else {
    a = arg - 3;
}
return a;
)";

  auto *parser = new Parser(source);
  StopNode *ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return Phi(Region17,(arg+2),(arg-3));",
            ret->print_1(builder).str());
}