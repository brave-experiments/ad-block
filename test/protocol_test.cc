/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <algorithm>
#include <iostream>
#include <set>
#include "./protocol.h"
#include "./CppUnitLite/TestHarness.h"
#include "./CppUnitLite/Test.h"
#include "./util.h"

// Testing isBlockableProtocol
TEST(isBlockableProtocol, basic) {
  // Each of the below should be valid, since they're urls of protocols
  // we block against
  CHECK(isBlockableProtocol("http://example.com", 19));
  CHECK(isBlockableProtocol("https://example.com", 20));
  CHECK(isBlockableProtocol("ws://example.com/path", 22));
  CHECK(isBlockableProtocol("wss://example.com/path", 23));

  // Each of the following should also be valid, since we ignore "blob"
  // pre-prefixes on URLs.
  CHECK(isBlockableProtocol("blob:http://example.com", 24));
  CHECK(isBlockableProtocol("blob:https://example.com", 25));
  CHECK(isBlockableProtocol("blob:ws://example.com/path", 27));
  CHECK(isBlockableProtocol("blob:wss://example.com/path", 28));

  // We don't care about case either, so each of the following should also
  // pass.
  CHECK(isBlockableProtocol("hTtp://example.com", 19));
  CHECK(isBlockableProtocol("htTPs://example.com", 20));
  CHECK(isBlockableProtocol("WS://example.com/path", 22));
  CHECK(isBlockableProtocol("WSS://example.com/path", 23));

  // Each of the following should fail, since they are each non supported /
  // blockable protocols.

  // To short example
  CHECK(isBlockableProtocol("htt", 3) == false);

  // Bad protocol example.
  CHECK(isBlockableProtocol("htttp://example.com", 25) == false);

  // No protocol example
  CHECK(isBlockableProtocol("example.com", 12) == false);

  // PNG data url example (not a valid image)[
  const char *pngPixel = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEA==";
  int pngPixelLen = strlen(pngPixel);
  CHECK(isBlockableProtocol(pngPixel, pngPixelLen) == false);

  // SVG data url example
  const char *svgUrl = "data:image/svg+xml;utf8,<svg "
                       "xmlns='http://www.w3.org/2000/svg'></svg>";
  int svgUrlLen = strlen(svgUrl);
  CHECK(isBlockableProtocol(svgUrl, svgUrlLen) == false);
}
