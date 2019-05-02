/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
/* global describe, it */

const assert = require('assert')
const { sanitizeABPInput, extraConcatRules } = require('../../lib/filtering')
const filteredOutRule = '*/test'
const predicate = (rule) => !rule.startsWith('*')

const sanitizeABPInputFiltered = (input, filterPredicate) =>
  sanitizeABPInput(input, filterPredicate).split('\n').slice(0, extraConcatRules.length * -1).join('\n')

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
      assert.strictEqual(sanitizeABPInputFiltered(I, predicate), I)
    })
    it('Rebuilds lists which have filtered out rules at the start', function () {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      assert.strictEqual(sanitizeABPInputFiltered(`${filteredOutRule}\n${rules}`, predicate), rules)
    })
    it('Rebuilds lists which have filtered out rules at the end', function () {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      assert.strictEqual(sanitizeABPInputFiltered(`${rules}\n${filteredOutRule}`, predicate), rules)
    })
    it('Rebuilds lists which have filtered out rules in the middle', function () {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      assert.strictEqual(sanitizeABPInputFiltered(`&ad_channel=\n${filteredOutRule}\n&ad_classid=\n&ad_height=\n&ad_keyword=`, predicate), rules)
    })
    it('Rebuilds lists which have multiple filtered out rules', function () {
      const rules = '&ad_channel=\n&ad_classid=\n&ad_height=\n&ad_keyword='
      assert.strictEqual(sanitizeABPInputFiltered(`${filteredOutRule}\n&ad_channel=\n${filteredOutRule}\n&ad_classid=\n&ad_height=\n&ad_keyword=`, predicate), rules)
    })
    it('Converts ima list to add forcecancel option', function () {
      const rule = '||imasdk.googleapis.com^$third-party'
      const mapToRule = '||imasdk.googleapis.com^$third-party,explicitcancel'
      assert.strictEqual(sanitizeABPInputFiltered(rule), mapToRule)
    })
  })
})
