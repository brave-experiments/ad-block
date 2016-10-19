const {ABPFilterParser, FilterOptions} = require('..')
const path = require('path')
const fs = require('fs')
const regions = require('../lib/regions')
const malware = require('../lib/malware')
const request = require('request')
const braveUnbreakPath = './test/data/brave-unbreak.txt'
const {getListBufferFromURL} = require('../lib/util')

let totalExceptionFalsePositives = 0
let totalNumFalsePositives = 0
let totalTime = 0

const generateDataFileFromString = (filterRuleData, outputDATFilename) => {
  const parser = new ABPFilterParser()
  if (filterRuleData.constructor === Array) {
    filterRuleData.forEach(filterRuleDataItem => parser.parse(filterRuleDataItem))
  } else {
    parser.parse(filterRuleData)
  }

  console.log('Parsing stats:', parser.getParsingStats())
  parser.enableBadFingerprintDetection()
  checkSiteList(parser, top500URLList20k)
  parser.generateBadFingerprintsHeader('badFingerprints.h')
  const serializedData = parser.serialize()
  if (!fs.existsSync('out')) {
    fs.mkdirSync('./out')
  }
  fs.writeFileSync(path.join('out', outputDATFilename), serializedData)
}

const generateDataFileFromURL = (listURL, outputDATFilename) => {
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
      const braveUnbreakBody = fs.readFileSync(braveUnbreakPath, 'utf8')
      generateDataFileFromString([body, braveUnbreakBody], outputDATFilename)
      resolve()
    })
  })
}

const generateDataFileFromPath = (filePath, outputDATFilename) => {
  let filterRuleData
  if (filePath.constructor === Array) {
    filterRuleData = filePath.map((filePath) => fs.readFileSync(filePath))
  } else {
    filterRuleData = fs.readFileSync(filePath)
  }
  generateDataFileFromString(filterRuleData, outputDATFilename)
}

const generateDataFilesForAllRegions = () => {
  let p = Promise.resolve()
  regions.forEach((l) => {
    p = p.then(generateDataFileFromURL.bind(null, l.listURL, `${l.uuid}.dat`))
  })
  return p
}

const generateDataFilesForMalware = (outputDATFilename) => {
  const promises = malware.map((l) => getListBufferFromURL(l.listURL, l.filter))
  Promise.all(promises).then((values) => {
    generateDataFileFromString(values, outputDATFilename)
  })
}

const checkSiteList = (parser, siteList) => {
  const start = new Date().getTime()
  siteList.forEach(site => {
    // console.log('matches: ', parser.matches(site,  FilterOptions.image, 'slashdot.org'))
    parser.matches(site, FilterOptions.noFilterOption, 'slashdot.org')
  })
  const stats = parser.getMatchingStats()
  console.log('Matching stats:', stats)
  totalNumFalsePositives += stats.numFalsePositives
  totalExceptionFalsePositives += stats.numExceptionFalsePositives
  const end = new Date().getTime()
  const time = end - start
  totalTime += time
  console.log('done, time: ', time, 'ms')
  console.log(`Total time: ${totalTime / 1000}s ${totalTime % 1000}ms`)
  console.log(`Num false positives: ${totalNumFalsePositives}`)
  console.log(`Num exception false positives: ${totalExceptionFalsePositives}`)
}

// TODO: Convert these to get live lists
const defaultAdblockLists = [
  './test/data/easylist.txt',
  './test/data/ublock-unbreak.txt',
  braveUnbreakPath
]

const top500URLList20k = fs.readFileSync('./test/data/sitelist.txt', 'utf8').split('\n')
// const shortURLList = fs.readFileSync('./test/data/short-sitelist.txt', 'utf8').split('\n')

generateDataFileFromPath(defaultAdblockLists, 'ABPFilterParserData.dat')
generateDataFilesForAllRegions()
generateDataFilesForMalware('SafeBrowsingData.dat')
