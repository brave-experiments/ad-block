const {ABPFilterParser, FilterOptions} = require('..')
const path = require('path')
const fs = require('fs')
const regions = require('../lib/regions')
const request = require('request');

const generateDataFileFromString = (filterRuleData, outputDATFilename) => {
  const parser = new ABPFilterParser()
  if (filterRuleData.constructor === Array) {
    filterRuleData.forEach(filterRuleDataItem => parser.parse(filterRuleDataItem))
  } else {
    parser.parse(filterRuleData)
  }
  // checkSiteList(parser, top500URLList20k)
  const serializedData = parser.serialize()
  if (!fs.existsSync('out')) {
    fs.mkdirSync('./out')
  }
  fs.writeFileSync(path.join('out', outputDATFilename), serializedData)
}

const generateDataFileFromURL = (listURL, outputDATFilename) => {
  request.get(listURL, function (error, response, body) {
    if (response.statusCode !== 200) {
      console.error(`Error status code ${response.statusCode} returned for URL: ${listURL}`)
      return
    }
    generateDataFileFromString(body, outputDATFilename)
  })
}

const generateDataFileFromPath = (filePath, outputDATFilename) => {
  let filterRuleData
  if (filePath.constructor == Array) {
    filterRuleData = filePath.map((filePath) => fs.readFileSync(filePath))
  } else {
    filterRuleData = fs.readFileSync(filePath)
  }
  generateDataFileFromString(filterRuleData, outputDATFilename)
}

const generateDataFilesForAllRegions = () =>
  regions.forEach((region) =>
    generateDataFileFromURL(region.listURL, `${region.uuid}.dat`))

const checkSiteList = (parser, siteList) => {
  const start = new Date().getTime();
  siteList.forEach(site => {
    // console.log('matches: ', parser.matches(site,  FilterOptions.image, 'slashdot.org'))
    parser.matches(site,  FilterOptions.image, 'slashdot.org')
  })
  const end = new Date().getTime();
  const time = end - start;
  console.log('done, time: ', time, 'ms')
}


const defaultAdblockLists = [
  './test/data/easylist.txt',
  './test/data/ublock-unbreak.txt',
  './test/data/brave-unbreak.txt'
]

const defaultMalwareLists = [
  './test/data/spam404-main-blacklist.txt',
  './test/data/disconnect-simple-malware.txt'
]

const top500URLList20k = fs.readFileSync('./test/data/sitelist.txt', 'utf8').split('\n')
const shortURLList = fs.readFileSync('./test/data/short-sitelist.txt', 'utf8').split('\n')

generateDataFileFromPath(defaultAdblockLists, 'ABPFilterParserData.dat')
generateDataFileFromPath(defaultMalwareLists, 'SafeBrowsingData.dat')
generateDataFilesForAllRegions()
