/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const request = require('request')
const { sanitizeABPInput } = require('./filtering')
const fs = require('fs')
const { AdBlockClient, adBlockLists } = require('..')

/**
 * Builds an adblock client, given one or more strings containing filter rules.
 *
 * @param filterRuleData -- either a string, describing adblock filter riles,
 *                          or an array of such strings.
 * @param options        -- an optional object, describing parse options.
 *                          currently the only used rule is "keepRuleText",
 *                          which is a boolean flag determine whether to keep
 *                          the original filter rule text.
 */
const makeAdBlockClientFromString = (filterRuleData, options) => {
  const keepRuleText = !!(options && options.keepRuleText)
  return new Promise((resolve) => {
    const client = new AdBlockClient()
    if (filterRuleData.constructor === Array) {
      filterRuleData.forEach(filterRuleDataItem => client.parse(filterRuleDataItem, keepRuleText))
    } else {
      client.parse(filterRuleData, keepRuleText)
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
 * @param options        -- an optional object, describing parse options.
 *                          currently the only used rule is "keepRuleText",
 *                          which is a boolean flag determine whether to keep
 *                          the original filter rule text.
 * It will be called with the URL's body and it can filter and return a new string.
 */
const getSingleListDataFromSingleURL = (listURL, filter, options) => {
  return new Promise((resolve, reject) => {
    request.get(listURL, function (error, response, body) {
      if (error) {
        reject(new Error(`Request error: ${error}`))
        return
      }
      if (response.statusCode !== 200) {
        reject(new Error(`Error status code ${response.statusCode} returned for URL: ${listURL}`))
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
 * Creates an AdBlock client from a list URL
 * @param listURL  -- The URL to check for obtaining the list.
 * @param filter   -- A filtering function that can be optionally specified.
 * @param options  -- an optional object, describing parse options.
 *                    currently the only used rule is "keepRuleText",
 *                    which is a boolean flag determine whether to keep
 *                    the original filter rule text.

 * It will be called with the URL's body and it can filter and return a new string.
 */
const makeAdBlockClientFromListURL = (listURL, filter, options) => {
  return new Promise((resolve, reject) => {
    if (listURL.constructor === Array) {
      const requestPromises = listURL.map((currentURL) => {
        console.log(`${currentURL}...`)
        return getSingleListDataFromSingleURL(currentURL, filter)
      })
      Promise.all(requestPromises).then((results) => {
        let body = results.join('\n')
        body = sanitizeABPInput(body)
        resolve(makeAdBlockClientFromString(body, options))
      }).catch((error) => {
        reject(new Error(`getSingleListDataFromSingleURL error: ${error}`))
      })
    } else {
      console.log(`${listURL}...`)
      getSingleListDataFromSingleURL(listURL, filter).then((listData) => {
        const body = sanitizeABPInput(listData)
        resolve(makeAdBlockClientFromString(body, options))
      }).catch((error) => {
        reject(new Error(`getSingleListDataFromSingleURL error: ${error}`))
      })
    }
  })
}

const getListFilterFunction = (uuid) => {
  if (uuid === 'FBB430E8-3910-4761-9373-840FC3B43FF2') {
    return (input) => input.split('\n').slice(4)
      .map((line) => `||${line}`).join('\n')
  }
  return undefined
}

/**
 * Creates an AdBlock client with the rules given in a list described by a UUID.
 *
 * See lists/* for the definitions of these UUIDs.
 *
 * @param uuid     -- a string, describing on of the UUIDs for a known filter
 *                    list.
 * @param options  -- an optional object, describing parse options.
 *                    currently the only used rule is "keepRuleText",
 *                    which is a boolean flag determine whether to keep
 *                    the original filter rule text.
 */
const makeAdBlockClientFromListUUID = (uuid, options) => {
  let list = adBlockLists.default.find((l) => l.uuid === uuid)
  if (!list) {
    list = adBlockLists.regions.find((l) => l.uuid === uuid)
  }
  if (!list) {
    list = adBlockLists.malware.find((l) => l.uuid === uuid)
  }
  if (!list) {
    return Promise.reject(new Error(`No list found for UUID ${uuid}`))
  }

  const filterFn = getListFilterFunction(uuid)
  return makeAdBlockClientFromListURL(list.listURL, filterFn, options)
}

/**
 * Builds an adblock client by reading one or more files filter rules off disk.
 *
 * @param filePath -- either a string, describing the path to a file of filter
 *                    rules on disk, or an array of the same.
 * @param options  -- an optional object, describing parse options.
 *                    currently the only used rule is "keepRuleText",
 *                    which is a boolean flag determine whether to keep
 *                    the original filter rule text.
 */
const makeAdBlockClientFromFilePath = (filePath, options) => {
  return new Promise((resolve, reject) => {
    let filterRuleData
    if (filePath.constructor === Array) {
      filterRuleData = filePath.map((filePath) => fs.readFileSync(filePath, 'utf8'))
    } else {
      filterRuleData = fs.readFileSync(filePath, 'utf8')
    }
    resolve(makeAdBlockClientFromString(filterRuleData, options))
  })
}

const getListBufferFromURL = (listURL, filter) => {
  return new Promise((resolve, reject) => {
    request.get(listURL, function (error, response, body) {
      if (error) {
        reject(new Error(`Request error: ${error}`))
        return
      }
      if (response.statusCode !== 200) {
        reject(new Error(`Error status code ${response.statusCode} returned for URL: ${listURL}`))
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
  readSiteList,
  getListFilterFunction
}
