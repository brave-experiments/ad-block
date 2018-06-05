/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const request = require('request')

/**
 * URL that rare-filter rule data should be fetched from.
 *
 * @type string
 */
let rareRulesUrl = 'https://s3.amazonaws.com/com.brave.research.public/brave-unused-filters.txt'

/**
 * A set depicting ABP filter rules that are in tracked lists, but rarely
 *
 * used online.
 * @type undefined|Set
 */
let rareFilterRulesSet

/**
 * Array.prototype.filter style function for removing rarely used filter rules.
 *
 * This function depends on rareFilterRulesSet being set, and so should not
 * be called directly.  It should ony be called through filterRareRulesPromise.
 *
 * @param aRule {string} - a ABP fitler rule
 *
 * @return boolean - false if the rule is a known rarely used rule, and so
 *                   should be discarded from further consideration.  Otherwise,
 *                   true.
 */
const _filterRareRulesFn = aRule => rareFilterRulesSet.has(aRule) === false

/**
 * Sets the URL that rare rule data should be fetched from.
 *
 * By default this is a publically available, regularly updated URL
 * maintained by Brave.  This function can be used to change this value
 * for testing purposes.
 *
 * @param aUrl {string} - the URL that should be fetched from when
 *                        determining which filter rules are rarely applied.
 */
const setRareRuleDataUrl = aUrl => {
  rareRulesUrl = aUrl
}

/**
 * Sets the ABP rules that should be treated as "infreqently" observed.
 *
 * This only should be done in unit-test like situations.  Setting this
 * value as anything other than undefined will cause the provided
 * string to be treated as the list of rare ABP rules.
 *
 * @param rareRules {string} - A new-line seperated set of ABP filter rules
 *                             that should be treated as rare.
 */
const setRareRuleData = rareRules => {
  rareFilterRulesSet = new Set(rareRules.split('\n'))
}

/**
 * Filter-like function that removes rarely used ABP rules from a filter text
 *
 * @param rulesText {string} - a string, containing new-line seperated ABP
 *                             filter rules.  This will match something like
 *                             whats returned from easylist directly.
 *
 * @return Promise - A promise that either resolves to the filtered set of
 *                   fequently-used ABP rules, or rejects with a description
 *                   of the error encountered when fetching the unused
 *                   filters data.
 */
const filterRareRulesPromise = rulesText => {
  return new Promise((resolve, reject) => {
    if (rareFilterRulesSet !== undefined) {
      resolve(rulesText.split('\n').filter(_filterRareRulesFn).join('\n'))
      return
    }

    console.log(`Fetching unused rules data from ${rareRulesUrl}...`)
    request.get(rareRulesUrl, (error, response, body) => {
      if (error) {
        console.error(`Request error: ${error}`)
        reject()
        return
      }
      if (response.statusCode !== 200) {
        console.error(`Error status code ${response.statusCode} returned for URL: ${rareRulesUrl}`)
        reject()
        return
      }

      rareFilterRulesSet = new Set(body.trim().split('\n'))
      resolve(rulesText.split('\n').filter(_filterRareRulesFn).join('\n'))
    })
  })
}

const I = (x) => x

/**
 * Same as filterPredicate but will log if there is a LOG_OUTPUT env variable
 */
const filterPredicateWithPossibleLogging = (rule, filterPredicate = I) => {
  const result = filterPredicate(rule)
  if (process.env['LOG_OUTPUT'] && !result) {
    console.log('Filtering out rule: ', rule)
  }
  return result
}

/**
 * Mapping rule which reformats rules
 */
const mapRule = (rule) => rule

/**
 * Given a list of inputs returns a filtered list of rules that should be used.
 *
 * @param input {string} - ABP filter syntax to filter
 * @return A better filter list
 */
const sanitizeABPInput = (input, filterPredicate = I) =>
  input.split('\n')
    .filter((rule) =>
      filterPredicateWithPossibleLogging(rule, filterPredicate))
    .map(mapRule)
    .join('\n')

module.exports = {
  sanitizeABPInput,
  filterRareRulesPromise,
  setRareRuleData,
  setRareRuleDataUrl
}
