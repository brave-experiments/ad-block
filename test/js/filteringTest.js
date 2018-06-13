/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
/* global describe, it, before */

const assert = require('assert')
const {sanitizeABPInputPromise, filterRareRulesPromise, setRareRuleData} = require('../../lib/filtering')
const filteredOutRule = '*/test'
const predicate = (rule) => !rule.startsWith('*')

describe('filtering', function () {
  describe('filterPredicate', function () {
    it('Filters out rules that start with a *, for now', function () {
      assert(!predicate('*test/ad'))
    })
    it('Does not filter out rules with a *', function () {
      assert(predicate('test/*/ad'))
    })
  })

  describe('sanitizeABPInput', function () {
    before(() => {
      setRareRuleData('')
    })

    it('Rebuilds lists which do not have filtered out rules', function (done) {
      const I = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      sanitizeABPInputPromise(I, predicate)
        .then(result => {
          assert(result === I)
          done()
        })
    })

    it('Rebuilds lists which have filtered out rules at the start', function (done) {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      sanitizeABPInputPromise(`${filteredOutRule}\n${rules}`, predicate)
        .then(result => {
          assert(result === rules)
          done()
        })
    })

    it('Rebuilds lists which have filtered out rules at the end', function (done) {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      sanitizeABPInputPromise(`${rules}\n${filteredOutRule}`, predicate)
        .then(result => {
          assert(result === rules)
          done()
        })
    })

    it('Rebuilds lists which have filtered out rules in the middle', function (done) {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      sanitizeABPInputPromise(`&ad_channel=\n${filteredOutRule}\n&ad_classid=\n&ad_height=\n&ad_keyword=`, predicate)
        .then(result => {
          assert(result === rules)
          done()
        })
    })

    it('Rebuilds lists which have multiple filtered out rules', function (done) {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      sanitizeABPInputPromise(`${filteredOutRule}\n&ad_channel=\n${filteredOutRule}\n&ad_classid=\n&ad_height=\n&ad_keyword=`, predicate)
        .then(result => {
          assert(result === rules)
          done()
        })
    })
  })

  describe('filterRareRulesPromise', function () {
    const rareRuleText = [
      '&ad_channel=',
      '&ad_classid=',
      '&ad_height=',
      '&ad_keyword='
    ].join('\n')

    before(() => {
      setRareRuleData(rareRuleText)
    })

    it('rules set as rare should be filtered', function (done) {
      const testRules = [
        '&ad_channel=', // This one should not be returned.
        '.ad/'          // This one should.
      ]
      filterRareRulesPromise(testRules.join('\n'))
        .then(results => {
          const parsedResults = results.split('\n')
          console.log([parsedResults, rareRuleText, testRules])
          assert.equal(parsedResults.length, 1)
          assert.equal(parsedResults[0], '.ad/')
          done()
        })
    })

    it('should be no change when no overlap with rare rules', function (done) {
      const testRules = [
        '.ad/',  // This one should be returned.
        '.ad2/'  // This one should be returned too.
      ]
      filterRareRulesPromise(testRules.join('\n'))
        .then(results => {
          const parsedResults = results.split('\n')
          assert.equal(parsedResults.length, 2)
          assert(
            (parsedResults[0] === '.ad/' && parsedResults[1] === '.ad2/') ||
            (parsedResults[0] === '.ad2/' && parsedResults[1] === '.ad/')
          )
          done()
        })
    })
  })
})
