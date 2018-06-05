/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
/* global describe, it, before, after */

const http = require('http')
const assert = require('assert')
const {sanitizeABPInput, filterRareRulesPromise, setRareRuleDataUrl} = require('../../lib/filtering')
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
    it('Rebuilds lists which do not have filtered out rules', function () {
      const I = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      assert(sanitizeABPInput(I, predicate) === I)
    })
    it('Rebuilds lists which have filtered out rules at the start', function () {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      assert(sanitizeABPInput(`${filteredOutRule}\n${rules}`, predicate) === rules)
    })
    it('Rebuilds lists which have filtered out rules at the end', function () {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      assert(sanitizeABPInput(`${rules}\n${filteredOutRule}`, predicate) === rules)
    })
    it('Rebuilds lists which have filtered out rules in the middle', function () {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      assert(sanitizeABPInput(`&ad_channel=\n${filteredOutRule}\n&ad_classid=\n&ad_height=\n&ad_keyword=`, predicate) === rules)
    })
    it('Rebuilds lists which have multiple filtered out rules', function () {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      assert(sanitizeABPInput(`${filteredOutRule}\n&ad_channel=\n${filteredOutRule}\n&ad_classid=\n&ad_height=\n&ad_keyword=`, predicate) === rules)
    })
  })
  describe('filter out rare rules', function () {
    const rareRuleText = [
      '&ad_channel=',
      '&ad_classid=',
      '&ad_height=',
      '&ad_keyword='
    ].join('\n')
    const testServer = http.createServer((req, res) => {
      res.writeHead(200, {'Content-Type': 'text/plain'})
      res.end(rareRuleText)
    })

    before(function (done) {
      testServer.listen(0, '0.0.0.0', () => {
        const testPort = testServer.address().port
        setRareRuleDataUrl(`http://localhost:${testPort}/rules.txt`)
        done()
      })
    })

    after(function () {
      testServer.close()
    })

    it('rules set as rare should be filtered', function (done) {
      const testRules = [
        '&ad_channel=', // This one should not be returned.
        '.ad/'          // This one should.
      ]
      filterRareRulesPromise(testRules.join('\n'))
        .then(results => {
          const parsedResults = results.split('\n')
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
