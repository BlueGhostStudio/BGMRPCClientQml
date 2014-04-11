function transFinal (transSocket,localFile) {
    localFile.close ()
    localFile.__destroy ()
    transSocket.close ()
    transSocket.__destroy ()
}

/*
 * state: 0 connecting, 1 connected, 2 fail, 3 download/update beginning, 4 downloading/updating, 5 finished
 */

function uploadFile (local, remote, progressCB, finished)
{
    if (progressCB === undefined) {
        progressCB = function (status, arg1, arg2) {
            switch (status) {
            case 0:
                console.log ("Transfer file from local:" + arg1 + " to remote:" + arg2)
                console.log ("Connecting...")
                break
            case 1:
                console.log ("Connected.")
                break
            case 2:
                console.log ("Fail: " + arg1)
                break
            case 3:
                console.log ("Upload has beginning. File size is " + arg1 + ".")
                break
            case 4:
                console.log ("Updating..." + arg1 + "/" + arg2)
                break
            case 5:
                console.log ("Updating finish")
                break
            }
        }
    }

    var localFile = LOCALFILES.file()
    if (localFile.open (local)) {
        var fileUploadSocket = RPC.newInstance ()
        fileUploadSocket.ONOpened = function () {
            progressCB (1)
            
            var fileSize = localFile.fileSize ()
            var maxLen = fileSize
            if (maxLen >= 1024) {
                maxLen = Math.round(fileSize / 100)
                if (maxLen < 1024)
                    maxLen = 1024
            }
            
            var updating = false
            var uploadedSize = 0
            
            fileUploadSocket.ONDone = function (rets) {
                if (rets [0]) {
                    progressCB (3, fileSize)
                    fileUploadSocket.sendRawData (fileSize)
                }
            }
            fileUploadSocket.ONDataWriten = function (size) {
                console.log (size)
                if (updating) {
                    uploadedSize += size
                    progressCB (4, uploadedSize, fileSize)
                } else
                    updating = true

                if (!localFile.atEnd)
                    fileUploadSocket.sendRawData (localFile.read (maxLen))
                else
                    progressCB (5)
            }
//            RPC.newRMethod ('file', 'upload', [remote], fileUploadSocket).call ()
            RPC.newRMethod ('test', "js", ["testUpload", remote], fileUploadSocket).call ()
        }

        fileUploadSocket.ONError = function () {
            progressCB (2, "Can't connect to host")
        }

        progressCB (0, local, remote)
        fileUploadSocket.connectToHost ()
    } else
        progressCB (2, "Can't open loacal file")
}

function downloadFile (remote, local, progressCB, finished) {
    if (progressCB === undefined) {
        progressCB = function (status, arg1, arg2) {
            switch (status) {
            case 0:
                console.log ("Transfer file from remote:" + arg1 + " to local:" + arg2)
                console.log ("Connecting...")
                break
            case 1:
                console.log ("Connected.")
                break
            case 2:
                console.log ("Fail: " + arg1)
                break
            case 3:
                console.log ("Download has beginning. File size is " + arg1 + ".")
                break
            case 4:
                console.log ("Downloading..." + arg1 + "/" + arg2)
                break
            case 5:
                console.log ("Download finish")
                break
            }
        }
    }

    var localFile = LOCALFILES.file ()

    if (localFile.open (local)) {
        var fileDownloadSocket = RPC.newInstance ()
        var fileSize = 0
        var downloadedSize = 0

        fileDownloadSocket.ONOpened = function () {
            progressCB (1)

            fileDownloadSocket.ONDone = function (rets) {
                fileSize = rets [1]
                if (rets [0])
                    progressCB (3, rets [1])
                else {
                    progressCB (2, "Remote can't open " + remote + ".")
                    transFinal (fileDownloadSocket, localFile)
                }
            }

            fileDownloadSocket.ONDataReceived = function (data) {
                downloadedSize += localFile.write (data)
                progressCB (4, downloadedSize, fileSize)

                if (downloadedSize >= fileSize) {
                    progressCB (5)
                    transFinal (fileDownloadSocket, localFile)
                    if (finished !== undefined)
                        finished ()
                }
            }

            RPC.newRMethod ("file", "download", [remote], fileDownloadSocket).call ()
        }

        fileDownloadSocket.ONError = function () {
            progressCB (2, "Can't connect to host.")
        }

        progressCB (0, local, remote)
        fileDownloadSocket.connectToHost ()
    } else {
        progressCB (2, "Local can't create " + local + ".")
    }
}
