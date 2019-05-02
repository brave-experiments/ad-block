/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
/* global describe, it, before */

const assert = require('assert')
const { makeAdBlockClientFromString } = require('../../lib/util')
const { AdBlockClient, FilterOptions } = require('../..')

describe('serialization', function () {
  before(function (cb) {
    const ruleData = `
      [Adblock Plus 2.0]
      &video_ads_
      &videoadid=
      &view=ad&
      +advertorial.
      +adverts/
      -2/ads/
      -2011ad_
      -300x100ad2.
      -ad-001-
      -ad-180x150px.
      -ad-200x200-
      ! comment here
    `
    makeAdBlockClientFromString(ruleData).then((client) => {
      this.client = client
      this.data = this.client.serialize()
      this.client2 = new AdBlockClient()
      this.client2.deserialize(this.data)
      // Just to make sure things work properly with repeated deserializes
      this.client2.deserialize(this.data)
      cb()
    })
  })

  it('blocks things the same when created from serialized', function () {
    assert(this.client.matches('http://www.brianbondy.com?c=a&view=ad&b=2', FilterOptions.image, 'slashdot.org'))
    assert(!this.client.matches('http://www.brianbondy.com?c=a&view1=ad&b=2', FilterOptions.image, 'slashdot.org'))
    assert(this.client2.matches('http://www.brianbondy.com?c=a&view=ad&b=2', FilterOptions.image, 'slashdot.org'))
    assert(!this.client2.matches('http://www.brianbondy.com?c=a&view1=ad&b=2', FilterOptions.image, 'slashdot.org'))
  })
  it('deserialized client serializes the same', function () {
    this.client2.deserialize(this.data)
    // Just to make sure things work properly with repeated deserializes
    this.client2.deserialize(this.data)
    const data2 = this.client2.serialize()
    assert(this.data.equals(data2))
  })
  it('deserializes with the same number of filters', function () {
    const nonCommentFilterCount = 11
    assert.strictEqual(this.client.getParsingStats().numFilters, nonCommentFilterCount)
    assert.strictEqual(this.client2.getParsingStats().numFilters, nonCommentFilterCount)
  })
  it('serialized data does not include comment data', function () {
    assert(!this.data.toString().includes('comment'))
    assert(!this.data.toString().includes('Adblock Plus'))
  })

  describe('deserializing input', function () {
    it('does not throw on valid input', function () {
      const client = new AdBlockClient()
      client.deserialize(this.data)
      // Just to make sure things work properly with repeated deserializes
      client.deserialize(this.data)
    })

    it('throws on invalid input', function () {
      const badInput = 'not-good-data'
      let caughtError = false
      const newClient = new AdBlockClient()
      // Check to make sure the below doesn't throw
      try {
        newClient.deserialize(badInput)
        // Just to make sure things work properly with repeated deserializes
        newClient.deserialize(badInput)
      } catch (_) {
        caughtError = true
      }
      assert(caughtError)
    })
  })
  describe('tags', function () {
    it('preserves filter tags', function () {
      const client = new AdBlockClient()
      client.parse('testfilter$third-party,tag=blah')
      const filters1 = client.getFilters('filters')
      console.log('filters1', filters1)
      assert.strictEqual(filters1.length, 1)
      assert.strictEqual(filters1[0].tag, 'blah')

      const data = client.serialize()
      const client2 = new AdBlockClient()
      client2.deserialize(data)
      // Just to make sure things work properly with repeated deserializes
      client2.deserialize(data)
      const filters2 = client2.getFilters('filters')
      console.log('filters2', filters2)
      assert.strictEqual(filters2.length, 1)
      assert.strictEqual(filters2[0].tag, 'blah')
    })
  })
})
