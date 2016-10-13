/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const url = 'https://s.yimg.jp/images/ds/ult/toppage/rapidjp-1.0.0.js'
const uuid = '03F91310-9244-40FA-BCF6-DA31B832F34D'
const {makeAdBlockClientFromListUUID} = require('../lib/util')
const {FilterOptions} = require('..')

makeAdBlockClientFromListUUID(uuid).then((adBlockClient) => {
  console.log('Parsing stats:', adBlockClient.getParsingStats())
  console.log('Matches: ', adBlockClient.matches(url, FilterOptions.noFilterOption, 'slashdot.org'))
}).catch((e) => {
  console.log('Error:', e)
})
