/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const { AdBlockClient } = require('..')
const fs = require('fs')

const client = new AdBlockClient()
if (!fs.existsSync('out')) {
  fs.mkdirSync('./out')
}

client.generateDefaultManifestFile('out')
client.generateRegionalManifestFiles('out')

process.on('uncaughtException', (err) => {
  console.error('Caught exception:', err)
  process.exit(1)
})

process.on('unhandledRejection', (err) => {
  console.error('Unhandled rejection:', err)
  process.exit(1)
})
