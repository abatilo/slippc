#include <algorithm>

#include "util.h"
#include "parser.h"
#include "analyzer.h"

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
  std::cout
    << "Usage: slippc -i <infile> [-j <jsonfile>] [-a <analysisfile>] [-f] [-d <debuglevel>] [-h]:" << std::endl
    << "  -i     Parse and analyze <infile> (not very useful on its own)" << std::endl
    << "  -j     Output <infile> in .json format to <jsonfile> (use \"-\" for stdout)" << std::endl
    << "  -a     Output an analysis of <infile> in .json format to <analysisfile> (use \"-\" for stdout)" << std::endl
    << "  -f     When used with -j <jsonfile>, write full frame info (instead of just frame deltas)" << std::endl
    << "  -d     Run at debug level <debuglevel> (show debug output)" << std::endl
    << "  -h     Show this help message" << std::endl
    ;
}

int main(int argc, char** argv) {
  if (cmdOptionExists(argv, argv+argc, "-h")) {
    printUsage();
    return 0;
  }

  int debug = 0;
  char* dlevel = getCmdOption(argv, argv + argc, "-d");
  if (dlevel) {
    if (dlevel[0] >= '0' && dlevel[0] <= '2') {
      debug = dlevel[0]-'0';
    } else {
      std::cerr << "Warning: invalid debug level" << std::endl;
    }
  }
  if (debug) {
    std::cout << "Running at debug level " << +debug << std::endl;
  }

  char * infile = getCmdOption(argv, argv + argc, "-i");
  if (not infile) {
    printUsage();
    return -1;
  }
  bool delta = not cmdOptionExists(argv, argv+argc, "-f");

  slip::Parser *p = new slip::Parser(debug);
  if (not p->load(infile)) {
    if (debug) {
      std::cout << "Could not load input; exiting" << std::endl;
    }
    delete p;
    return 2;
  }

  char * outfile = getCmdOption(argv, argv + argc, "-j");
  if (outfile) {
    if (outfile[0] == '-' && outfile[1] == '\0') {
      if (debug) {
        std::cout << "Writing Slippi JSON data to stdout" << std::endl;
      }
      std::cout << p->asJson(delta) << std::endl;
    } else {
      if (debug) {
        std::cout << "Saving Slippi JSON data to file" << std::endl;
      }
      p->save(outfile,delta);
    }
  }

  char * analysisfile = getCmdOption(argv, argv + argc, "-a");
  if (analysisfile) {
    slip::Analysis *a  = p->analyze();
    if (a->success) {
      if (analysisfile[0] == '-' && analysisfile[1] == '\0') {
        if (debug) {
          std::cout << "Writing analysis to stdout" << std::endl;
        }
        std::cout << a->asJson() << std::endl;
      } else {
        if (debug) {
          std::cout << "Saving analysis to file" << std::endl;
        }
        a->save(analysisfile);
      }
    }
    if (debug) {
      std::cout << "Deleting analysis" << std::endl;
    }

    delete a;
  }

  if (debug) {
    std::cout << "Cleaning up" << std::endl;
  }
  delete p;
  return 0;
}
