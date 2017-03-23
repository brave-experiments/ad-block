/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
 * Predicate which returns true if a rule should be used
 */
const filterPredicate = (rule) =>
  // Due to a bug in Brave ad-block we currently choke up if a rule starts with a star
  // This just makes things safer until the bug is fixed.
  !rule.startsWith('*')

/**
 * Same as filterPredicate but will log if there is a LOG_OUTPUT env variable
 */
const filterPredicateWithPossibleLogging = (rule) => {
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
const sanitizeABPInput = (input) =>
  input.split('\n')
    .filter(filterPredicateWithPossibleLogging)
    .map(mapRule)
    .join('\n')

module.exports = {
  sanitizeABPInput,
  filterPredicate
}
