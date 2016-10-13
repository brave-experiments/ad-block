/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const request = require('request');
const regions = require('./regions')
const {ABPFilterParser} = require('..')

const makeAdBlockClientFromString = (filterRuleData) => {
  const parser = new ABPFilterParser()
  if (filterRuleData.constructor === Array) {
    filterRuleData.forEach(filterRuleDataItem => parser.parse(filterRuleDataItem))
  } else {
    parser.parse(filterRuleData)
  }
  return parser
}

const makeAdBlockClientFromListURL = (listURL) => {
  return new Promise((resolve, reject) => {
    console.log(`${listURL}...`)
    request.get(listURL, function (error, response, body) {
      if (error) {
        console.error(`Request error: ${error}`)
        reject()
        return
      }
      if (response.statusCode !== 200) {
        console.error(`Error status code ${response.statusCode} returned for URL: ${listURL}`)
        reject()
        return
      }
      resolve(makeAdBlockClientFromString(body))
    })
  })
}

const makeAdBlockClientFromListUUID = (uuid) => {
  const region = regions.find((region) => region.uuid === uuid)
  if (!region) {
    Promise.reject()
    return
  }

  return makeAdBlockClientFromListURL(region.listURL)
}

module.exports = {
  makeAdBlockClientFromString,
  makeAdBlockClientFromListURL,
  makeAdBlockClientFromListUUID
}
