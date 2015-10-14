#include "CppUnitLite/TestHarness.h"
#include "ABPFilterParser.h"
#include <string>
#include "test.h"

TEST(x, y)
{
  std::string s1("Hello"), s2("Hello"), s3("world");
  CHECK_EQUAL(s1, s2);
  CHECK_EQUAL(s2, s1);
  CHECK(s1 != s3);
  CHECK(sum(1,2) == sum(2,1));
}
