// Parse simple from files
#include <iostream>
// Handling source file
#include <fstream>
#include "../Include/parser.h"
#include <iostream>

#define VERSION_STRING "0.0.1"

const std::string USAGE_TEXT = "TBD";

void print_usage() { std::cout << USAGE_TEXT << std::endl; }


std::size_t get_file_size(std::ifstream &file) {

  file.seekg(0, file.end);         // Go to the end of the file
  std::size_t size = file.tellg(); // Read the current position
  file.seekg(0, file.beg);         // Go to the beginning of the file
  return size;
}

std::string handle_file(const char*path_base) {
  std::ifstream in(path_base);
  if (!in) {
    std::cout << "FILE NOT FOUND AT THIS PATH: " << path_base << std::endl;
    exit(1);
  }

  std::size_t size = get_file_size(in);
  std::string result{};
  result.resize(size);
  std::string str(size, '\0');


  if (in.read(&str[0], static_cast<int>(size)))

    result = str;

  in.close();

  return result;
}

// Todo: use iterate here to reflect the test cases in
// Chapter09Test.cc

int main(int argc, char const **argv) {
  if (argc <= 1)
    print_usage();
  auto *parser = new Parser(handle_file(argv[1]));
  std::ostringstream builder;
  StopNode *ret = parser->parse(true)->iterate();
  ret->print(builder).str();
  mem.reset();
  return 0;
}