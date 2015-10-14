#include "CppUnitLite/TestHarness.h"
#include <string>
#include "util.h"

SimpleString StringFrom(const std::string& value)
{
    return SimpleString(value.c_str());
}

int main()
{
  TestResult tr;
  TestRegistry::runAllTests(tr);
  return 0;
}
