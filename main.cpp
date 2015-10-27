#include "ABPFilterParser.h"
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
  const char *urlToCheck = "http://tpc.googlesyndication.com/safeframe/1-0-2/html/container.html";

  // This is the site who's URLs are being checked, not the domain of the URL being checked.
  const char *currentPageDomain = "slashdot.org";

  // Parse easylist
  ABPFilterParser parser;
  parser.parse(easyListTxt.c_str());

  // Do a check
  if (parser.matches(urlToCheck, FONoFilterOption, currentPageDomain)) {
    cout << "You should block this URL!" << endl;
  } else {
    cout << "You should NOT block this URL!" << endl;
  }

  writeFile("./bloomFilterData.dat", parser.bloomFilter->getBuffer(), parser.bloomFilter->getByteBufferSize());
  writeFile("./exceptionBloomFilterData.dat", parser.bloomFilter->getBuffer(), parser.bloomFilter->getByteBufferSize());

  // If you'd like to preload bloom filter data for faster checking
  ABPFilterParser parser2;
  parser2.initBloomFilter(parser.bloomFilter->getBuffer(), parser.bloomFilter->getByteBufferSize());
  parser2.initExceptionBloomFilter(parser.bloomFilter->getBuffer(), parser.bloomFilter->getByteBufferSize());
  if (parser2.matches("http://www.brianbondy.com", FONoFilterOption, currentPageDomain)) {
    cout << "You should block this URL!" << endl;
  } else {
    cout << "You should NOT block this URL!" << endl;
  }
  return 0;
}
