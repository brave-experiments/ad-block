Filter Format Compatibility
===

This project mainly aims to provide a highly optimized method of applying
EasyList-style rules to network requests, to perform URL filtering. The project
also supports other filter rule formats that have been developed and promoted
by other blocking tools.

This document describes which filter rule formats that are supported by
this project, and to what degree. Support is broken down into three tiers:

*  **full support**: The rule parser understands the rule, and the
                     matching engine understands how to apply it to a network
                     request.
*  **parsed but ignored**: The parser understands the rule, but the rule is
                           ignored by the matching engine.
*  **not supported**: The parser does not understand the rule, and does not
                      maintain any internal understanding of the rule.

[AdBlock Plus](https://adblockplus.org/filters)
---

AdBlock Plus has filters for blocking network requests, hiding page elements,
and rules for overwriting / excepting either of the above.

### Full Support
We support most of the rule formats defined by AdBlock Plus. Unless
specifically mentioned below, the rules are fully supported (both in "main"
and "exception" form).

### Parsed but Ignored
* All element rules, indicating that HTML elements should be hidden or collapsed.
* The `ping` option, which indiciates a request from a [`<a ping>`](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/a#attr-ping)
  element, or a [`navigator.sendBeacon()`](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/sendBeacon) call.
* The `popup` option, which indicates that new pages should not be opened
  in new tabs or windows from a given url.
* The `csp` option, which indicates a given [content security policy](https://developer.mozilla.org/en-US/docs/Web/HTTP/CSP)
  should be applied to the page.
* The `elemhide` option, which, when used in exception rules, indiciates that
  element hiding rules should not be applied to matching domains.
* The `generichide` option, which indicates that generic (i.e. non-domain
  specific) element hiding rules should not be applied to matching URLs and
  filters.
* The `genericblock` option, which indicates that generic (i.e. non-domain
  specific) network rules should not be applied to matching URLs.

If a filter rule has one of the above options applied to it, but also
some other supported options (e.g. the rule `example.org$script,popup`),
this library will apply the fully supported option, `script`, but ignore
the non-supported one, `popup`.

### Not Supported
* Regular expression based rules of any kind
* The `websocket` rule


[AdGuard](https://kb.adguard.com/en/general/adguard-ad-filters)
---
AdGuard supports a super set of AdBlockPlus's rule formats. This section
only describes support for rule formats supported by AdGuard, but not
AdBlockPlus. We do not support AdGuard filter rule formats not mentioned
below.

### Parsed but Ignored
* The [`empty`](https://kb.adguard.com/en/general/how-to-create-your-own-ad-filters#empty)
  rule, which indicates that matching requests should be sent an empty `200` request,
  instead of no request.


[uBlock](https://github.com/gorhill/uBlock/wiki/Static-filter-syntax)
---
uBlock supports a super set of AdBlockPlus's rule formats. This section
only describes support for rule formats supported by uBlock, but not
AdBlockPlus. We do not support uBlock filter rule formats not mentioned
below.

### Not Supported
* The [`important`](https://github.com/gorhill/uBlock/wiki/Static-filter-syntax#important)
  rule, which provides a way to mark a rule as ignored by exception rule.
* The [`redirect`](https://github.com/gorhill/uBlock/wiki/Static-filter-syntax#redirect)
  rule, which indicates that matching requests should be replaced with new
  requests.


