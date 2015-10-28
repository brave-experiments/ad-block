#include "ABPFilterParser.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

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

void writeFile(const char *filename, const char *buffer, int length)
{
  ofstream outFile(filename, ios::out | ios::binary);
  if (outFile) {
    outFile.write(buffer, length);
    outFile.close();
    return;
  }
  throw(errno);
}


int main(int argc, char**argv) {
  std::string &&easyListTxt = getFileContents("./test/data/easylist.txt");
  const char *urlsToCheck[] = {
    "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html",
    "http://www.googletagservices.com/tag/js/gpt_mobile.js",
    "http://www.brianbondy.com"
  };

  // This is the site who's URLs are being checked, not the domain of the URL being checked.
  const char *currentPageDomain = "slashdot.org";

  // Parse easylist
  ABPFilterParser parser;
  parser.parse(easyListTxt.c_str());

  // Do the checks
  std::for_each(urlsToCheck, urlsToCheck + sizeof(urlsToCheck) / sizeof(urlsToCheck[0]), [&parser, currentPageDomain](std::string const &urlToCheck) {
    if (parser.matches(urlToCheck.c_str(), FONoFilterOption, currentPageDomain)) {
      cout << urlToCheck << ": You should block this URL!" << endl;
    } else {
      cout << urlToCheck << ": You should NOT block this URL!" << endl;
    }
  });

  int size;
  char *buffer = parser.serialize(size);
  writeFile("./ABPFilterParserData.dat", buffer, size);

  // Note that this buffer must be freed and will actually be used directly by the deserializer.
  ABPFilterParser parser2;
  parser2.deserialize(buffer);
  // Prints the same as parser.matches would
  std::for_each(urlsToCheck, urlsToCheck + sizeof(urlsToCheck) / sizeof(urlsToCheck[0]), [&parser2, currentPageDomain](std::string const &urlToCheck) {
    if (parser2.matches(urlToCheck.c_str(), FONoFilterOption, currentPageDomain)) {
      cout << urlToCheck << ": You should block this URL!" << endl;
    } else {
      cout << urlToCheck << ": You should NOT block this URL!" << endl;
    }
  });
  delete[] buffer;
  return 0;
}
