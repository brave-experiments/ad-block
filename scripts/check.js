/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// Example invocations:
// node scripts/check.js  --uuid 03F91310-9244-40FA-BCF6-DA31B832F34D --host slashdot.org --location https://s.yimg.jp/images/ds/ult/toppage/rapidjp-1.0.0.js
// node scripts/check.js  --host www.cnet.com --location https://s0.2mdn.net/instream/html5/ima3.js
// node scripts/check.js --dat ./out/SafeBrowsingData.dat --host excellentmovies.net --location https://excellentmovies.net

const commander = require('commander')
const {makeAdBlockClientFromListUUID, makeAdBlockClientFromDATFile, makeAdBlockClientFromFilePath, makeAdBlockClientFromString} = require('../lib/util')
const {FilterOptions} = require('..')

commander
  .option('-u, --uuid [uuid]', 'UUID of the list to use')
  .option('-d, --dat [dat]', 'file path of the adblock .dat file')
  .option('-f, --filter [filter]', 'Brave filter rules')
  .option('-h, --host [host]', 'host of the page that is being loaded')
  .option('-l, --location [location]', 'URL to use for the check')
  .option('-o, --output [output]', 'Optionally saves a DAT file')
  .parse(process.argv)

let p = Promise.reject('Usage: node check.js --location <location> --host <host> [--uuid <uuid>]')

if (commander.host && commander.location) {
  p.catch(() => {})
  if (commander.uuid) {
    p = makeAdBlockClientFromListUUID(commander.uuid)
  } else if (commander.dat) {
    p = makeAdBlockClientFromDATFile(commander.dat)
  } else if (commander.filter) {
    p = makeAdBlockClientFromString(commander.filter)
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
  if (commander.output) {
    require('fs').writeFileSync(commander.output, adBlockClient.serialize())
  }
}).catch((e) => {
  console.log('Error:', e)
})
