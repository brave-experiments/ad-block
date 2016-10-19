/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const url = 'https://drivewaycleaninghatfield.co.uk'
const uuid = 'FBB430E8-3910-4761-9373-840FC3B43FF2'
const {makeAdBlockClientFromListUUID} = require('../lib/util')
const {FilterOptions} = require('..')

makeAdBlockClientFromListUUID(uuid).then((adBlockClient) => {
  console.log('Parsing stats:', adBlockClient.getParsingStats())
  console.log('Matches: ', adBlockClient.matches(url, FilterOptions.noFilterOption, 'brianbondy.com'))
}).catch((e) => {
  console.log('Error:', e)
})
