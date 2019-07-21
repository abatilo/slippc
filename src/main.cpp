#include "util.h"
#include "parser.h"
#include <algorithm>

// https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
char* getCmdOption(char ** begin, char ** end, const std::string & option) {
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
  return std::find(begin, end, option) != end;
}

void printUsage() {
  std::cout << "Usage: slippc -i <infile> [-o <outfile>] [-f]" << std::endl;
}

int main(int argc, char** argv) {
  if (cmdOptionExists(argv, argv+argc, "-h")) {
    printUsage();
    return 0;
  }
  char * infile = getCmdOption(argv, argv + argc, "-i");
  if (not infile) {
    printUsage();
    return -1;
  }
  char * outfile = getCmdOption(argv, argv + argc, "-o");
  bool delta = not cmdOptionExists(argv, argv+argc, "-f");

  slip::Parser p;
  if (not p.load(infile)) {
    return 2;
  }
  p.summary();
  if (outfile) {
    p.save(outfile,delta);
  }
  p.cleanup();
  return 0;
}
