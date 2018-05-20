/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
/* global describe, before, it */

const assert = require('assert')
const {AdBlockClient} = require('../..')
const {FilterOptions} = require('../..')

describe('matching', function () {
  describe('strange rules', function () {
    it('can start with a slash', function () {
      const client = new AdBlockClient()
      client.parse('/banner1.gif\n')
      assert(client.matches('http://www.brianbondy.com/public/banner1.gif', FilterOptions.image, 'slashdot.org'))
      assert(client.matches('http://www.brianbondy.com/public//banner1.gif', FilterOptions.image, 'slashdot.org'))
      assert(!client.matches('http://www.brianbondy.com/public/abanner1.gif', FilterOptions.image, 'slashdot.org'))
    })

    it('can start with a star', function () {
      const client = new AdBlockClient()
      client.parse('*/b/ss/*&aqe=$image,redirect=1x1-transparent.gif\n')
      assert(!client.matches('https://i.ytimg.com/vi/hxUAntt1z2c/hqdefault.jpg?custom=true&w=320&h=180&stc=true&jpg444=true&jpgq=90&sp=68&sigh=YNApXAOpSSoeEmCpXSLQwjnOdwY', FilterOptions.image, 'slashdot.org'))
      assert(client.matches('https://i.ytimg.com/b/ss/hxUAntt1z2c/hqdefault.jpg?a&aqe=$image,redirect=1x1-transparent.gif&dd', FilterOptions.image, 'slashdot.org'))
      assert(client.matches('https://i.ytimg.com/a/b/ss/hxUAntt1z2c/hqdefault.jpg?a&aqe=$image,redirect=1x1-transparent.gif&dd', FilterOptions.image, 'slashdot.org'))
      assert(!client.matches('https://i.ytimg.com/b/a/ss/hxUAntt1z2c/hqdefault.jpg?a&aqe=$image,redirect=1x1-transparent.gif&dd', FilterOptions.image, 'slashdot.org'))
    })
    it('can start with multiple stars', function () {
      const client = new AdBlockClient()
      client.parse('***/b/ss/*&aqe=$image,redirect=1x1-transparent.gif\n')
      assert(!client.matches('https://i.ytimg.com/vi/hxUAntt1z2c/hqdefault.jpg?custom=true&w=320&h=180&stc=true&jpg444=true&jpgq=90&sp=68&sigh=YNApXAOpSSoeEmCpXSLQwjnOdwY', FilterOptions.image, 'slashdot.org'))
      assert(client.matches('https://i.ytimg.com/b/ss/hxUAntt1z2c/hqdefault.jpg?a&aqe=$image,redirect=1x1-transparent.gif&dd', FilterOptions.image, 'slashdot.org'))
      assert(client.matches('https://i.ytimg.com/a/b/ss/hxUAntt1z2c/hqdefault.jpg?a&aqe=$image,redirect=1x1-transparent.gif&dd', FilterOptions.image, 'slashdot.org'))
      assert(!client.matches('https://i.ytimg.com/b/a/ss/hxUAntt1z2c/hqdefault.jpg?a&aqe=$image,redirect=1x1-transparent.gif&dd', FilterOptions.image, 'slashdot.org'))
    })
    it('can have multiple stars in the middle', function () {
      const client = new AdBlockClient()
      client.parse('a/********b\n')
      assert(client.matches('https://i.ytimg.com/a/d/e/f/b', FilterOptions.noFilterOption, 'slashdot.org'))
      assert(client.matches('https://i.ytimg.com/a/d/e/fb', FilterOptions.noFilterOption, 'slashdot.org'))
      assert(!client.matches('https://i.ytimg.com/a/d/e/fd', FilterOptions.noFilterOption, 'slashdot.org'))
    })
    it('can block everything with a single *', function () {
      const client = new AdBlockClient()
      client.parse('*')
      assert(client.matches('https://brianbondy.com/d', FilterOptions.noFilterOption, 'slashdot.org'))
      assert(client.matches('https://brianbondy.com', FilterOptions.noFilterOption, 'slashdot.org'))
    })
    it('can have no rule data', function () {
      const client = new AdBlockClient()
      client.parse('')
      assert(!client.matches('https://brianbondy.com/d', FilterOptions.noFilterOption, 'slashdot.org'))
      assert(!client.matches('https://brianbondy.com', FilterOptions.noFilterOption, 'slashdot.org'))
    })
    it('can have rule data with just a ^', function () {
      const client = new AdBlockClient()
      client.parse('^')
      assert(!client.matches('https://brianbondy.com', FilterOptions.noFilterOption, 'slashdot.org'))
      assert(!client.matches('https://brianbondy.com', FilterOptions.noFilterOption, 'slashdot.org'))
    })
  })
  describe('host anchored exception with matching first party exception', function () {
    before(function () {
      this.client = new AdBlockClient()
      this.client.parse('-google-analytics.\n@@||www.scrumpoker.online^$~third-party')
    })
    it('does not match', function () {
      assert(!this.client.matches('https://www.scrumpoker.online/js/angular-google-analytics.js', FilterOptions.script, 'www.scrumpoker.online'))
    })
    it('detects as a hash set save', function () {
      assert.equal(this.client.getMatchingStats().numExceptionHashSetSaves, 1)
    })
  })
  describe('host anchored exception with not matching first party exception', function () {
    before(function () {
      this.client = new AdBlockClient()
      this.client.parse('-google-analytics.\n@@||www.scrumpoker.online^$~third-party')
    })
    it('does match', function () {
      assert(this.client.matches('https://www.scrumpoker.online/js/angular-google-analytics.js', FilterOptions.script, 'www.brianbondy.com'))
    })
    it('detects as a hash set save', function () {
      assert.equal(this.client.getMatchingStats().numExceptionHashSetSaves, 1)
    })
  })
  describe("no-fingerprint rules", function () {
    it('can match against a no-fingerprint rule', function () {
      const client = new AdBlockClient()
      client.parse('adv')
      assert(client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'slashdot.org'))
      assert(!client.matches('https://brianbondy.com/omg', FilterOptions.noFilterOption, 'slashdot.org'))
    })
    it('can match against a no-fingerprint domain only rule', function () {
      const client = new AdBlockClient()
      client.parse('adv$domain=brianbondy.com')
      assert(client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'brianbondy.com'))
      assert(client.matches('https://digg.com/adv', FilterOptions.noFilterOption, 'brianbondy.com'))
      assert(!client.matches('https://digg.com/adv', FilterOptions.noFilterOption, 'slashdot.org'))
      assert(!client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'digg.com'))
    })
    it('can match against a no-fingerprint anti-domain only rule', function () {
      const client = new AdBlockClient()
      client.parse('adv$domain=~brianbondy.com')
      assert(!client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'brianbondy.com'))
      assert(!client.matches('https://digg.com/adv', FilterOptions.noFilterOption, 'brianbondy.com'))
      assert(client.matches('https://digg.com/adv', FilterOptions.noFilterOption, 'slashdot.org'))
      assert(client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'digg.com'))
    })
    it('can match against a no-fingerprint domain only exception rule', function () {
      const client = new AdBlockClient()
      client.parse('adv\n@@adv$domain=brianbondy.com')
      assert(!client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'brianbondy.com'))
      assert(!client.matches('https://digg.com/adv', FilterOptions.noFilterOption, 'brianbondy.com'))
      assert(client.matches('https://digg.com/adv', FilterOptions.noFilterOption, 'slashdot.org'))
      assert(client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'digg.com'))
    })
  })
  describe('findMatchingFilters return values', function () {
    before(function () {
      this.client = new AdBlockClient()
      this.client.parse('/pubads_\n@@||fastly.net/ad2/$image,script,xmlhttprequest\n.net/ad2/\n@@||fastly.net/ad2/$image,script,xmlhttprequest')
    })
    it('match', function () {
      const queryResult = this.client.findMatchingFilters('https://securepubads.g.doubleclick.net/gpt/pubads_impl_rendering_193.js?cb=194', FilterOptions.script, 'www.cnn.com')
      assert.equal(queryResult.matches, true)
      assert.equal(queryResult.matchingFilter, '/pubads_')
    })
    it('miss', function () {
      const queryResult = this.client.findMatchingFilters('https://cdn.cnn.com/cnn/.e1mo/img/4.0/logos/menu_entertainment.png', FilterOptions.image, 'www.cnn.com')
      assert.equal(queryResult.matches, false)
    })
    it('whitelisted', function () {
      const queryResult = this.client.findMatchingFilters('https://0914.global.ssl.fastly.net/ad2/script/x.js?cb=1523383475084', FilterOptions.script, 'www.cnn.com')
      assert.equal(queryResult.matches, false)
      assert.equal(queryResult.matchingFilter, '.net/ad2/')
      assert.equal(queryResult.matchingExceptionFilter, 'fastly.net/ad2/')
    })
  })
})
