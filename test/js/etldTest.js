/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
/* global describe, it, before */

const fs = require('fs')
const assert = require('assert')
const {AdBlockClient, FilterOptions} = require('../..')

describe('etld+1 matching', function () {
  describe('third-party checking', function () {
    before(function () {
      const etldRules = fs.readFileSync('./test/data/public_suffix_list.dat', 'utf8')
      this.client = new AdBlockClient()
      this.client.parse('||s3.amazonaws.com^$third-party\n||not-matching-anything.com\ntrees^')
      this.client.parsePublicSuffixRules(etldRules)
    })

    it('do not apply third-party rules when domains are etld+1', function () {
      const result = this.client.findMatchingFilters(
        "http://www.spark-public.s3.amazonaws.com/ads.js",
        FilterOptions.script,
        "other.s3.amazonaws.com"
      );
      assert(result.matches, false)
    })

    it('apply third-party rules when domains are similar but not etld+1', function () {
      const result = this.client.findMatchingFilters(
        "http://www.spark-public.s3.amazonaws.com/ads.js",
        FilterOptions.script,
        "example.amazonaws.com"
      );
      assert(result.matches, true)
    })

    it('apply third-party rules when domains are not similar at all', function () {
      const result = this.client.findMatchingFilters(
        "http://www.spark-public.s3.amazonaws.com/ads.js",
        FilterOptions.script,
        "example.org"
      );
      assert(result.matches, true)
    })
  })

  describe('after serialization', function () {
    before(function () {
      const etldRules = fs.readFileSync('./test/data/public_suffix_list.dat', 'utf8')
      this.client = new AdBlockClient()
      this.client.parse('||s3.amazonaws.com^$third-party\n||not-matching-anything.com\ntrees^')
      this.client.parsePublicSuffixRules(etldRules)

      this.client2 = new AdBlockClient()
      this.client2.deserialize(this.client.serialize())
    })

    it('do not apply third-party rules when domains are etld+1', function () {
      const result = this.client2.findMatchingFilters(
        "http://www.spark-public.s3.amazonaws.com/ads.js",
        FilterOptions.script,
        "other.s3.amazonaws.com"
      );
      assert(result.matches, false)
    })

    it('apply third-party rules when domains are similar but not etld+1', function () {
      const result = this.client2.findMatchingFilters(
        "http://www.spark-public.s3.amazonaws.com/ads.js",
        FilterOptions.script,
        "example.amazonaws.com"
      );
      assert(result.matches, true)
    })

    it('apply third-party rules when domains are not similar at all', function () {
      const result = this.client2.findMatchingFilters(
        "http://www.spark-public.s3.amazonaws.com/ads.js",
        FilterOptions.script,
        "example.org"
      );
      assert(result.matches, true)
    })
  })
})