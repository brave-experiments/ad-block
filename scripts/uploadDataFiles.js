const fs = require('fs')
const s3 = require('s3')
const commander = require('commander')
const path = require('path')

const client = s3.createClient({
  maxAsyncS3: 20,
  s3RetryCount: 3,
  s3RetryDelay: 1000,
  multipartUploadThreshold: 20971520,
  multipartUploadSize: 15728640,
  // See: http://docs.aws.amazon.com/AWSJavaScriptSDK/latest/AWS/Config.html#constructor-property
  s3Options: {}
})

const uploadFile = (filePath, filename) => {
  return new Promise((resolve, reject) => {
    var params = {
      localFile: filePath,
      // See: http://docs.aws.amazon.com/AWSJavaScriptSDK/latest/AWS/S3.html#putObject-property
      s3Params: {
        Bucket: 'adblock-data',
        Key: `2/${filename}`,
        ACL: 'public-read'
      }
    }
    var uploader = client.uploadFile(params)
    process.stdout.write(`Started uploading to: ${params.s3Params.Key}... `)
    uploader.on('error', function (err) {
      console.error('Unable to upload:', err.stack, 'Do you have ~/.aws/credentials filled out?')
      reject()
    })
    uploader.on('end', function (params) {
      console.log('completed')
      resolve()
    })
  })
}

commander
  .option('-d, --dat [dat]', 'file path of the adblock .dat file to upload')
  .parse(process.argv)

// Queue up all the uploads one at a time to easily spot errors
let p = Promise.resolve()

if (commander.dat) {
  p = p.then(uploadFile.bind(null, commander.dat, path.basename(commander.dat)))
} else {
  const dataFilenames = fs.readdirSync('out')
  dataFilenames.forEach((filename) => {
    p = p.then(uploadFile.bind(null, `out/${filename}`, filename))
  })
}
