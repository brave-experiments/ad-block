/* Copyright (c) 2015 Brian R. Bondy. Distributed under the MPL2 license.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LISTS_DEFAULT_H_
#define LISTS_DEFAULT_H_

#include <vector>
#include <string>
#include "../filter_list.h"

const std::string kAdBlockDefaultComponentId("cffkpbalmllkdoenhmdmpbkajipdjfam");  // NOLINT
const std::string kAdBlockDefaultBase64PublicKey =
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAs0qzJmHSgIiw7IGFCxij"
    "1NnB5hJ5ZQ1LKW9htL4EBOaMJvmqaDs/wfq0nw/goBHWsqqkMBynRTu2Hxxirvdb"
    "cugn1Goys5QKPgAvKwDHJp9jlnADWm5xQvPQ4GE1mK1/I3ka9cEOCzPW6GI+wGLi"
    "VPx9VZrxHHsSBIJRaEB5Tyi5bj0CZ+kcfMnRTsXIBw3C6xJgCVKISQUkd8mawVvG"
    "vqOhBOogCdb9qza5eJ1Cgx8RWKucFfaWWxKLOelCiBMT1Hm1znAoVBHG/blhJJOD"
    "5HcH/heRrB4MvrE1J76WF3fvZ03aHVcnlLtQeiNNOZ7VbBDXdie8Nomf/QswbBGa"
    "VwIDAQAB";

const std::vector<FilterList> default_lists = {
  {
    "67F880F5-7602-4042-8A3D-01481FD7437A",
    "https://easylist.to/easylist/easylist.txt",
    "EasyList",
    {},
    "https://easylist.to/",
    "",
    ""
  }, {
    "48010209-AD34-4DF5-A80C-3D2A7C3920C0",
    "https://easylist.to/easylist/easyprivacy.txt",
    "EasyPrivacy",
    {},
    "https://easylist.to/",
    "",
    ""
  }, {
    "200392E7-9A0F-40DF-86EB-6AF7E4071322",
    "https://raw.githubusercontent.com/uBlockOrigin/uAssets/master/filters/unbreak.txt", // NOLINT
    "uBlock Unbreak",
    {},
    "https://github.com/gorhill/uBlock",
    "",
    ""
  }, {
    "2FBEB0BC-E2E1-4170-BAA9-05E76AAB5BA5",
    "https://raw.githubusercontent.com/brave/adblock-lists/master/brave-unbreak.txt", // NOLINT
    "Brave Unblock",
    {},
    "https://github.com/brave/adblock-lists",
    "",
    ""
  }, {
    "BCDF774A-7845-4121-B7EB-77EB66CEDF84",
    "https://raw.githubusercontent.com/brave/adblock-lists/master/coin-miners.txt", // NOLINT
    "NoCoin Filter List",
    {},
    "https://github.com/brave/adblock-lists",
    "",
    ""
  }
};

#endif  // LISTS_DEFAULT_H_
