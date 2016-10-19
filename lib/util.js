/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const request = require('request')
const regions = require('./regions')
const malware = require('./malware')
const fs = require('fs')
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

/**
 * Creates an ABlock client from a list URL
 * @param lisURL - The URL to check for obtaining the list.
 * @param filter - A filtering function that can be optionally specified.
 * It will be called with the URL's body and it can filter and return a new string.
 */
const makeAdBlockClientFromListURL = (listURL, filter) => {
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
      if (filter) {
        body = filter(body)
      }
      resolve(makeAdBlockClientFromString(body))
    })
  })
}

const makeAdBlockClientFromListUUID = (uuid) => {
  let list = regions.find((l) => l.uuid === uuid)
  if (!list) {
    list = malware.find((l) => l.uuid === uuid)
  }
  if (!list) {
    Promise.reject()
    return
  }

  return makeAdBlockClientFromListURL(list.listURL, list.filter)
}

const makeAdBlockClientFromFilePath = (filePath) => {
  return new Promise((resolve, reject) => {
    let filterRuleData
    if (filePath.constructor === Array) {
      filterRuleData = filePath.map((filePath) => fs.readFileSync(filePath, 'utf8'))
    } else {
      filterRuleData = fs.readFileSync(filePath, 'utf8')
    }
    resolve(makeAdBlockClientFromString(filterRuleData))
  })
}

module.exports = {
  makeAdBlockClientFromString,
  makeAdBlockClientFromListURL,
  makeAdBlockClientFromFilePath,
  makeAdBlockClientFromListUUID
}
