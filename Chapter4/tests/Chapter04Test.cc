#include "../Include/graph_visualizer.h"
#include "../Include/node/return_node.h"

#include "parser.h"
#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

TEST(SimpleTest, Peephole) {
  auto *parser = new Parser("return 1+arg+2;");
  ReturnNode*ret = parser->parse();
  std::ostringstream builder;
  EXPECT_EQ("return (arg+3)", ret->print_1(builder).str());
 }