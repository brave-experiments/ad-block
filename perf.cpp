#include "ABPFilterParser.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

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

int main(int argc, char**argv) {
  std::string &&easyListTxt = getFileContents("./test/data/easylist.txt");
  std::string &&siteList = getFileContents("./test/data/sitelist.txt");
  std::stringstream ss(siteList);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> sites(begin, end);

  // This is the site who's URLs are being checked, not the domain of the URL being checked.
  const char *currentPageDomain = "brianbondy.com";
  ABPFilterParser parser;
  parser.enableBadFingerprintDetection();

  //parser.parse("/public/ad/*");
  parser.parse(easyListTxt.c_str());

  int numBlocks = 0;
  int numSkips = 0;
  const clock_t begin_time = clock();
  std::for_each(sites.begin(), sites.end(), [&parser, currentPageDomain, &numBlocks, &numSkips](std::string const &urlToCheck) {
    if (parser.matches(urlToCheck.c_str(), FONoFilterOption, currentPageDomain)) {
      ++numBlocks;
    } else {
      ++numSkips;
    }
  });
  std::cout << "Time: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << "s" << endl;
  cout << "num blocks: " << numBlocks << ", num skips: " << numSkips << endl;
  cout << "False Positives: " << parser.numFalsePositives << ", exception false positives: " << parser.numExceptionFalsePositives << endl;
  cout << "Bloom filter saves: " << parser.numBloomFilterSaves << ", exception bloom filter saves: " << parser.numExceptionBloomFilterSaves << endl;
  parser.badFingerprintsHashSet->generateHeader("badFingerprints.h");
  return 0;
}
