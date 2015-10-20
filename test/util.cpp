#include "CppUnitLite/TestHarness.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "util.h"

using namespace std;

string getFileContents(const char *filename)
{
  ifstream in(filename, ios::in);
  if (in) {
    ostringstream contents;
    contents << in.rdbuf();
    in.close();
    return(contents.str());
  }
  throw(errno);
}

bool compareNums(int actual, int expected) {
  if (actual != expected) {
    cout << "Actual: " << actual << endl << "Expected: " << expected << endl;
    return false;
  }
  return true;
}
