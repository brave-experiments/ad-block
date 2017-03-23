/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const request = require('request')
const regions = require('./regions')
const malware = require('./malware')
const {sanitizeABPInput} = require('./filtering')
const fs = require('fs')
const {AdBlockClient} = require('..')

const makeAdBlockClientFromString = (filterRuleData) => {
  return new Promise((resolve) => {
    const client = new AdBlockClient()
    if (filterRuleData.constructor === Array) {
      filterRuleData.forEach(filterRuleDataItem => client.parse(filterRuleDataItem))
    } else {
      client.parse(filterRuleData)
    }
    resolve(client)
  })
}

/**
 * Creates an ABlock client from a DAT file
 * @param datFilePath - The file path to the DAT file
 */
const makeAdBlockClientFromDATFile = (datFilePath) => {
  return new Promise((resolve, reject) => {
    fs.readFile(datFilePath, (err, data) => {
      if (err) {
        reject(err)
        return
      }
      const client = new AdBlockClient()
      client.deserialize(data)
      resolve(client)
    })
  })
}

/**
 * Creates an ABlock client from a list URL
 * @param listURL - The URL to check for obtaining the list.
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
      body = sanitizeABPInput(body)
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

const getListBufferFromURL = (listURL, filter) => {
  return new Promise((resolve, reject) => {
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
      body = sanitizeABPInput(body)
      resolve(body)
    })
  })
}

/**
 * Reads a list of sites in the format of one site per newline
 * from a file path and returns an array with the sites.
 */
const readSiteList = (path) =>
  fs.readFileSync(path, 'utf8').split('\n')

module.exports = {
  makeAdBlockClientFromString,
  makeAdBlockClientFromDATFile,
  makeAdBlockClientFromListURL,
  makeAdBlockClientFromFilePath,
  makeAdBlockClientFromListUUID,
  getListBufferFromURL,
  readSiteList
}
