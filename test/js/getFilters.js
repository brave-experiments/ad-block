/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
/* global describe, it, before, beforeEach */

const assert = require('assert')
const {AdBlockClient} = require('../..')

describe('getFilters', function () {
  before(function () {
    this.adBlockClient = new AdBlockClient()
  })
  beforeEach(function () {
    this.adBlockClient.clear()
  })
  describe('invalid input', function () {
    it('Returns an empty list when no filterType is specified', function () {
      assert.deepEqual(this.adBlockClient.getFilters(), [])
    })
    it('Returns an empty list when an invalid filterType is specified', function () {
      assert.deepEqual(this.adBlockClient.getFilters('fdsafsdfasdfs'), [])
    })
    it('Returns an empty list when an empty string filterType is specified', function () {
      assert.deepEqual(this.adBlockClient.getFilters(''), [])
    })
  })
  describe('returns filters of the correct types', function () {
    it('for basic filters', function () {
      this.adBlockClient.parse('basicfilter')
      assert.equal(this.adBlockClient.getFilters('filters').length, 1)
      assert.equal(this.adBlockClient.getFilters('cosmeticFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('htmlFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('exceptionFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprint').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprintExceptionFilters').length, 0)
    })
    it('for cosmetic filters', function () {
      this.adBlockClient.parse('##table[width="80%"]')
      assert.equal(this.adBlockClient.getFilters('filters').length, 0)
      assert.equal(this.adBlockClient.getFilters('cosmeticFilters').length, 1)
      assert.equal(this.adBlockClient.getFilters('htmlFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('exceptionFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprint').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprintExceptionFilters').length, 0)
    })
    it('for html filters', function () {
      this.adBlockClient.parse('example.org$$script[data-src="banner"]')
      assert.equal(this.adBlockClient.getFilters('filters').length, 0)
      assert.equal(this.adBlockClient.getFilters('cosmeticFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('htmlFilters').length, 1)
      assert.equal(this.adBlockClient.getFilters('exceptionFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprint').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprintExceptionFilters').length, 0)
    })
    it('for exception filters', function () {
      this.adBlockClient.parse('@@advice')
      assert.equal(this.adBlockClient.getFilters('filters').length, 0)
      assert.equal(this.adBlockClient.getFilters('cosmeticFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('htmlFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('exceptionFilters').length, 1)
      assert.equal(this.adBlockClient.getFilters('noFingerprint').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprintExceptionFilters').length, 0)
    })
    it('for noFingerprint', function () {
      this.adBlockClient.parse('adv')
      assert.equal(this.adBlockClient.getFilters('filters').length, 0)
      assert.equal(this.adBlockClient.getFilters('cosmeticFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('htmlFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('exceptionFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprint').length, 1)
      assert.equal(this.adBlockClient.getFilters('noFingerprintExceptionFilters').length, 0)
    })
    it('host anchored filters', function () {
      this.adBlockClient.parse('||test.com')
      assert.equal(this.adBlockClient.getFilters('filters').length, 0)
      assert.equal(this.adBlockClient.getFilters('cosmeticFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('htmlFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('exceptionFilters').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprint').length, 0)
      assert.equal(this.adBlockClient.getFilters('noFingerprintExceptionFilters').length, 0)
    })
  })
  describe('filter data is returned correctly', function () {
    it('parses filter data', function () {
      this.adBlockClient.parse('basicfilter')
      const filter = this.adBlockClient.getFilters('filters')[0]
      assert.equal(filter.data, 'basicfilter')
      assert.equal(Object.keys(filter).length, 1)
    })
    it('parses domainList', function () {
      this.adBlockClient.parse('somefilter$domain=test.com,test.net')
      const filter = this.adBlockClient.getFilters('filters')[0]
      assert.equal(filter.data, 'somefilter')
      // Currently limited to only return the first
      assert.equal(filter.domainList, 'test.com')
      assert.equal(Object.keys(filter).length, 2)
    })
  })
})
