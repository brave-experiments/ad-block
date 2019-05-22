/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
/* global describe, before, it */

const assert = require('assert')
const { AdBlockClient } = require('../..')
const { FilterOptions } = require('../..')

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
      assert.strictEqual(this.client.getMatchingStats().numExceptionHashSetSaves, 1)
    })
  })
  describe('domain only host anchored filters', function () {
    before(function () {
      this.client = new AdBlockClient()
      this.client.parse('||imasdk.googleapis.com^$third-party\n@@||imasdk.googleapis.com/js/sdkloader/ima3.js$domain=cbc.ca|cbsnews.com|cbssports.com')
      const data = this.client.serialize()
      this.client2 = new AdBlockClient()
      this.client2.deserialize(data)
      // Just to make sure things work properly with repeated deserializes
      this.client2.deserialize(data)
    })
    it('should match exception filter from normal parsed list', function () {
      assert(!this.client2.matches('https://imasdk.googleapis.com/js/sdkloader/ima3.js?v=1.0fc2a9c5e010611944b364a71d43c8b5099f209f', FilterOptions.script, 'www.cbsnews.com'))
    })
    it('should match exception filter from deserialized list', function () {
      assert(!this.client.matches('https://imasdk.googleapis.com/js/sdkloader/ima3.js?v=1.0fc2a9c5e010611944b364a71d43c8b5099f209f', FilterOptions.script, 'www.cbsnews.com'))
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
      assert.strictEqual(this.client.getMatchingStats().numExceptionHashSetSaves, 1)
    })
  })
  describe('no-fingerprint rules', function () {
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
      assert.strictEqual(queryResult.matches, true)
      assert.strictEqual(queryResult.matchingFilter, '/pubads_')
    })
    it('miss', function () {
      const queryResult = this.client.findMatchingFilters('https://cdn.cnn.com/cnn/.e1mo/img/4.0/logos/menu_entertainment.png', FilterOptions.image, 'www.cnn.com')
      assert.strictEqual(queryResult.matches, false)
    })
    it('whitelisted', function () {
      const queryResult = this.client.findMatchingFilters('https://0914.global.ssl.fastly.net/ad2/script/x.js?cb=1523383475084', FilterOptions.script, 'www.cnn.com')
      assert.strictEqual(queryResult.matches, false)
      assert.strictEqual(queryResult.matchingFilter, '.net/ad2/')
      assert.strictEqual(queryResult.matchingExceptionFilter, 'fastly.net/ad2/')
    })
  })

  describe('original filter rules', function () {
    describe('returning original filter rule', function () {
      before(function () {
        this.client = new AdBlockClient()
        const rules = [
          '-google-analytics.',
          '@@||www.scrumpoker.online^$~third-party',
          '.net/ad',
          '@@||fastly.net/ad2/$image,script,xmlhttprequest'
        ]
        this.client.parse(rules.join('\n'), true)
      })
      it('match rule returned', function () {
        const queryResult = this.client.findMatchingFilters('https://www.scrumpoker.online/js/angular-google-analytics.js', FilterOptions.script, 'www.scrumpoker.online')
        assert.strictEqual(queryResult.matchingOrigRule, '-google-analytics.')
      })
      it('exception rule returned', function () {
        const queryResult = this.client.findMatchingFilters('https://www.scrumpoker.online/js/angular-google-analytics.js', FilterOptions.script, 'www.scrumpoker.online')
        assert.strictEqual(queryResult.matchingExceptionOrigRule, '@@||www.scrumpoker.online^$~third-party')
      })
      it('rules with multiple options', function () {
        const queryResult = this.client.findMatchingFilters('https://0914.global.ssl.fastly.net/ad2/img/x.gif?cb=1523404173915', FilterOptions.image, 'www.cnn.com')
        assert.strictEqual(queryResult.matches, false)
        assert.strictEqual(queryResult.matchingOrigRule, '.net/ad')
        assert.strictEqual(queryResult.matchingExceptionOrigRule, '@@||fastly.net/ad2/$image,script,xmlhttprequest')
      })
    })
    describe('do not return original filter text when not needed / requested', function () {
      before(function () {
        this.client = new AdBlockClient()
        const rules = [
          '-google-analytics.',
          '@@||www.scrumpoker.online^$~third-party',
          '.net/ad',
          '@@||fastly.net/ad2/$image,script,xmlhttprequest'
        ]
        this.client.parse(rules.join('\n')) // <- since not "true", then no rule data should be preserved
      })
      it('match rule returned', function () {
        const queryResult = this.client.findMatchingFilters('https://www.scrumpoker.online/js/angular-google-analytics.js', FilterOptions.script, 'www.scrumpoker.online')
        assert.strictEqual(queryResult.matchingOrigRule, undefined)
      })
      it('exception rule returned', function () {
        const queryResult = this.client.findMatchingFilters('https://www.scrumpoker.online/js/angular-google-analytics.js', FilterOptions.script, 'www.brianbondy.com')
        assert.strictEqual(queryResult.matchingExceptionOrigRule, undefined)
      })
      it('rules with multiple options', function () {
        const queryResult = this.client.findMatchingFilters('https://0914.global.ssl.fastly.net/ad2/img/x.gif?cb=1523404173915', FilterOptions.image, 'www.cnn.com')
        assert.strictEqual(queryResult.matches, false)
        assert.strictEqual(queryResult.matchingOrigRule, undefined)
        assert.strictEqual(queryResult.matchingExceptionOrigRule, undefined)
      })
    })
  })

  describe('Filters with unknown options are ignored', function () {
    it('known unsupported options are not blocked', function () {
      const client = new AdBlockClient()
      client.parse('adv$ping')
      assert(!client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'slashdot.org'))
    })
    it('CSPs are ignored', function () {
      const client = new AdBlockClient()
      client.parse('adv$csp=script-src \'self\' \'unsafe-inline\' \'unsafe-eval\' data: *.google.com *.gstatic.com *.google-analytics.com')
      assert(!client.matches('!https://brianbondy.com/adv', FilterOptions.noFilterOption, 'slashdot.org'))
    })
    it('unknown unsupported options are not blocked', function () {
      const client = new AdBlockClient()
      client.parse('adv$somethingnew=3')
      assert(!client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'slashdot.org'))
    })
    it('redirects are still blocked', function () {
      const client = new AdBlockClient()
      client.parse('adv$image,redirect=1x1-transparent.gif&dd')
      assert(client.matches('https://brianbondy.com/adv', FilterOptions.image, 'slashdot.org'))
    })
  })
  describe('Type option matching', function () {
    describe('font', function () {
      it('option matches for no resource type filters', function () {
        const client = new AdBlockClient()
        client.parse('adv')
        assert(client.matches('https://brianbondy.com/adv', FilterOptions.font, 'slashdot.org'))
      })
      it('option matches for same resource type', function () {
        const client = new AdBlockClient()
        client.parse('adv$font')
        assert(client.matches('https://brianbondy.com/adv', FilterOptions.font, 'slashdot.org'))
      })
      it('doesn\'t matche when resource type differs', function () {
        const client = new AdBlockClient()
        client.parse('adv$font')
        assert(!client.matches('https://brianbondy.com/adv', FilterOptions.image, 'slashdot.org'))
      })
      it('option matches for same resource type for rule with multiple types', function () {
        const client = new AdBlockClient()
        client.parse('adv$font,image,script')
        assert(client.matches('https://brianbondy.com/adv', FilterOptions.font, 'slashdot.org'))
      })
      it('option matches for rule without options', function () {
        const client = new AdBlockClient()
        client.parse('adv')
        assert(client.matches('https://brianbondy.com/adv', FilterOptions.font, 'slashdot.org'))
      })
    })
    describe('other', function () {
      it('option matches for rule without options', function () {
        const client = new AdBlockClient()
        client.parse('adv')
        assert(client.matches('https://brianbondy.com/adv', FilterOptions.other, 'slashdot.org'))
      })
    })
    describe('document', function () {
      it('should not match when filter rule has no type', function () {
        const client = new AdBlockClient()
        client.parse('adv')
        assert(!client.matches('https://brianbondy.com/adv', FilterOptions.document, 'slashdot.org'))
      })
      it('should not match when filter rule is an image', function () {
        const client = new AdBlockClient()
        client.parse('adv$image')
        assert(!client.matches('https://brianbondy.com/adv', FilterOptions.document, 'slashdot.org'))
      })
      it('should match when filter rule is a document', function () {
        const client = new AdBlockClient()
        client.parse('adv$document')
        assert(client.matches('https://brianbondy.com/adv', FilterOptions.document, 'slashdot.org'))
      })
      it('filter rule that is for documents should not match other types', function () {
        const client = new AdBlockClient()
        client.parse('adv$document')
        assert(!client.matches('https://brianbondy.com/adv', FilterOptions.other, 'slashdot.org'))
      })
    })
    describe('noFilterOption', function () {
      it('should not match when filter rule has no resource type', function () {
        const client = new AdBlockClient()
        client.parse('adv')
        assert(client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'slashdot.org'))
      })
      it('should not match when filter rule has a resource type', function () {
        const client = new AdBlockClient()
        client.parse('adv$image')
        assert(!client.matches('https://brianbondy.com/adv', FilterOptions.noFilterOption, 'slashdot.org'))
      })
    })
  })
  describe('left anchored exception filter', function () {
    describe('simple block match', function () {
      before(function () {
        this.client = new AdBlockClient()
        this.client.parse('|http://baddomain.example/')
      })
      it('matches exactly from the left', function () {
        assert(this.client.matches('http://baddomain.example/banner.gif', FilterOptions.image, 'http://baddomain.example/'))
      })
      it('does not match if scheme does not match', function () {
        assert(!this.client.matches('https://baddomain.example/banner.gif', FilterOptions.image, 'http://baddomain.example/'))
      })
      it('does not match not at start', function () {
        assert(!this.client.matches('http://gooddomain.example/analyze?http://baddomain.example', FilterOptions.image, 'http://baddomain.example/'))
      })
    })
  })
  describe('domain filtered rules', function () {
    describe('All exclude domain rules', function () {
      before(function () {
        this.client = new AdBlockClient()
        this.client.parse('wp-content/$domain=~aaaaa.com|~bbbbb.com|~ccccc.com')
      })

      it('filter has all anti domains but with different input domain should block', function () {
        assert(this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'www.brianbondy.com'))
      })
      it('filter has all anti domains and with a same input domain should not block', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'aaaaa.com'))
      })
    })
    describe('All include domain rules', function () {
      before(function () {
        this.client = new AdBlockClient()
        this.client.parse('wp-content/$domain=aaaaa.com|bbbbb.com|ccccc.com')
      })
      it('filter has all anti=false domains but with different input domain should not block', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'www.brianbondy.com'))
      })
      it('filter has all anti=false domains and with a same domain should not block', function () {
        assert(this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'aaaaa.com'))
      })
    })
    describe('Rule with blocking parent domain and subdomain which is allowed', function () {
      before(function () {
        this.client = new AdBlockClient()
        this.client.parse('wp-content/$domain=example.com|~foo.example.com|x.com|y.com|z.com')
      })
      it('for test URL of unrelated domain should not match', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'brianbondy.com'))
      })
      it('for test URL of parent should block', function () {
        assert(this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'example.com'))
      })
      it('for test URL of subdomain should allow', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'foo.example.com'))
      })
      it('for test URL of deeper subdomain should allow', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'boo.foo.example.com'))
      })
      it('for test URL of subdomain of top level rule should block', function () {
        assert(this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'boo.example.com'))
      })
    })
    describe('Rule with anti-block parent domain and subdomain which is blocked', function () {
      before(function () {
        this.client = new AdBlockClient()
        this.client.parse('wp-content/$domain=~example.com|foo.example.com|x.com|z.com|y.com')
      })
      it('for test URL of unrelated domain should not match', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'brianbondy.com'))
      })
      it('for test URL of parent should not block', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'example.com'))
      })
      it('for test URL of subdomain should block', function () {
        assert(this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'foo.example.com'))
      })
      it('for test URL of deeper subdomain should block', function () {
        assert(this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'boo.foo.example.com'))
      })
      it('for test URL of subdomain of top level rule should allow', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'boo.example.com'))
      })
    })

    describe('Three level deep rules', function () {
      before(function () {
        this.client = new AdBlockClient()
        this.client.parse('wp-content/$domain=example.com|~brave.example.com|foo.brave.example.com')
      })
      it('for test URL of unrelated domain should not match', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'brianbondy.com'))
      })
      it('for test URL of parent should block', function () {
        assert(this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'example.com'))
      })
      it('for test URL of exception subdomain should allow', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'brave.example.com'))
      })
      it('for test URL of deeper subdomain should block', function () {
        assert(this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'foo.brave.example.com'))
      })
    })
    describe('Three level deep rules reversed', function () {
      before(function () {
        this.client = new AdBlockClient()
        this.client.parse('wp-content/$domain=~example.com|brave.example.com|~foo.brave.example.com')
      })
      it('for test URL of unrelated domain should not match since there is a blocking domain', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'brianbondy.com'))
      })
      it('for test URL of parent should allow', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'example.com'))
      })
      it('for test URL of exception subdomain should block', function () {
        assert(this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'brave.example.com'))
      })
      it('for test URL of deeper subdomain should allow', function () {
        assert(!this.client.matches('https://www.sportsnet.ca/wp-content/plugins',
          FilterOptions.image, 'foo.brave.example.com'))
      })
    })

    describe('first-party host', function () {
      before(function () {
        this.client = new AdBlockClient()
        this.client.parse('analytics.brave.com^\n@@https://analytics.*/piwik.$~third-party')
      })
      it('for same host finds exception', function () {
        assert(!this.client.matches('https://analytics.brave.com/piwik.js',
          FilterOptions.image, 'analytics.brave.com'))
      })
      it('for same diff host does not find exception', function () {
        assert(this.client.matches('https://analytics.brave.com/piwik.js',
          FilterOptions.image, 'batcommunity.org'))
      })
      it('findMatchingFilters for same host finds exception', function () {
        const queryResult = this.client.findMatchingFilters('https://analytics.brave.com/piwik.js', FilterOptions.script, 'analytics.brave.com')
        assert.strictEqual(queryResult.matches, false)
        assert.strictEqual(queryResult.matchingFilter, 'analytics.brave.com^')
        assert.strictEqual(queryResult.matchingExceptionFilter, 'https://analytics.*/piwik.')
      })
      it('findMatchingFilters for same diff host does not find exception', function () {
        const queryResult = this.client.findMatchingFilters('https://analytics.brave.com/piwik.js', FilterOptions.script, 'batcommunity.org')
        assert.strictEqual(queryResult.matches, true)
        assert.strictEqual(queryResult.matchingFilter, 'analytics.brave.com^')
      })
    })

    describe('CSP rules', function () {
      before(function () {
        this.client = new AdBlockClient()
        this.client.parse('||merriam-webster.com^$csp=script-src "self" * "unsafe-inline"')
      })
      it('do not match since they are unsupported', function () {
        assert(!this.client.matches('https://www.merriam-webster.com/assets/test.min.css',
          FilterOptions.image, 'www.merriam-webster.com'))
      })
    })
  })
})
