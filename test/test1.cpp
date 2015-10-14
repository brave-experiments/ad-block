#include "CppUnitLite/TestHarness.h"
#include "ABPFilterParser.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include "test.h"
#include <iostream>

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

TEST(x, y)
{
  cout << getFileContents("./test/data/easylist.txt") << endl;

  string s1("Hello"), s2("Hello"), s3("world");
  CHECK_EQUAL(s1, s2);
  CHECK_EQUAL(s2, s1);
  CHECK(s1 != s3);
  CHECK(sum(1,2) == sum(2,1));
}
