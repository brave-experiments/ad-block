/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Example invocations:
// node scripts/discoverBlockingRules.js  --uuid 03F91310-9244-40FA-BCF6-DA31B832F34D --host slashdot.org --location https://s.yimg.jp/images/ds/ult/toppage/rapidjp-1.0.0.js
// node scripts/discoverBlockingRules.js  --host www.cnet.com --location https://s0.2mdn.net/instream/html5/ima3.js

const commander = require('commander')
const {makeAdBlockClientFromListUUID, makeAdBlockClientFromFilePath} = require('../lib/util')
const {FilterOptions} = require('..')

commander
  .option('-u, --uuid [uuid]', 'UUID of the list to use')
  .option('-h, --host [host]', 'host of the page that is being loaded')
  .option('-l, --location [location]', 'URL to use for the check')
  .parse(process.argv)

let p = Promise.reject('Usage: node discoveryBlockingRules.js --location <location> --host <host> [--uuid <uuid>]')

if (commander.host && commander.location) {
  if (commander.uuid) {
    p = makeAdBlockClientFromListUUID(commander.uuid)
  } else {
    const defaultAdblockLists = [
      './test/data/easylist.txt',
      './test/data/ublock-unbreak.txt',
      './test/data/brave-unbreak.txt'
    ]
    p = makeAdBlockClientFromFilePath(defaultAdblockLists)
  }
}

p.then((adBlockClient) => {
  console.log('Parsing stats:', adBlockClient.getParsingStats())
  console.log('Matches: ', adBlockClient.matches(commander.location, FilterOptions.noFilterOption, commander.host))
}).catch((e) => {
  console.log('Error:', e)
})
