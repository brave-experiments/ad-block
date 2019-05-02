/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

const fs = require('fs')
const s3 = require('s3-client')
const commander = require('commander')
const path = require('path')
const dataFileVersion = 4

const client = s3.createClient({
  maxAsyncS3: 20,
  s3RetryCount: 3,
  s3RetryDelay: 1000,
  multipartUploadThreshold: 20971520,
  multipartUploadSize: 15728640,
  // See: http://docs.aws.amazon.com/AWSJavaScriptSDK/latest/AWS/Config.html#constructor-property
  s3Options: {}
})

const uploadFile = (key, filePath, filename) => {
  return new Promise((resolve, reject) => {
    var params = {
      localFile: filePath,
      // See: http://docs.aws.amazon.com/AWSJavaScriptSDK/latest/AWS/S3.html#putObject-property
      s3Params: {
        Bucket: 'adblock-data',
        Key: `${key}/${filename}`,
        ACL: 'public-read'
      }
    }
    var uploader = client.uploadFile(params)
    process.stdout.write(`Started uploading to: ${params.s3Params.Key}... `)
    uploader.on('error', function (err) {
      reject(new Error(`Unable to upload, do you have ~/.aws/credentials filled out? ${err}`))
    })
    uploader.on('end', function (params) {
      console.log('completed')
      resolve()
    })
  })
}

commander
  .option('-d, --dat [dat]', 'file path of the adblock .dat file to upload')
  .option('-p, --prod', 'whether the upload is for prod, if not specified uploads to the test location')
  .parse(process.argv)

// Queue up all the uploads one at a time to easily spot errors
let p = Promise.resolve()
const date = new Date().toISOString().split('.')[0]

if (commander.dat) {
  if (commander.prod) {
    p = p.then(uploadFile.bind(null, dataFileVersion, commander.dat, path.basename(commander.dat)))
    p = p.then(uploadFile.bind(null, `backups/${date}`, commander.dat, path.basename(commander.dat)))
  } else {
    p = p.then(uploadFile.bind(null, 'test', commander.dat, path.basename(commander.dat)))
  }
} else {
  const dataFilenames = fs.readdirSync('out')
  dataFilenames.forEach((filename) => {
    if (commander.prod) {
      p = p.then(uploadFile.bind(null, dataFileVersion, `out/${filename}`, filename))
      p = p.then(uploadFile.bind(null, `backups/${date}`, `out/${filename}`, filename))
    } else {
      p = p.then(uploadFile.bind(null, `test/${dataFileVersion}`, `out/${filename}`, filename))
    }
  })
}
p = p.catch((e) => {
  console.error('A problem was encounterd', e)
  process.exit(1)
})

process.on('uncaughtException', (err) => {
  console.error('Caught exception:', err)
  process.exit(1)
})

process.on('unhandledRejection', (err) => {
  console.error('Unhandled rejection:', err)
  process.exit(1)
})
