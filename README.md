# 功能：

1. 相机拍摄、原图下载、原图删除、缩略图预览。

2. 参数读取与设置。 

3. 视频监控。视频监控数据流采用MJPEG编码，注意不是MPEG编码。MPEG 是对视频文件的压缩，存在帧间压缩，而 MJEPG 是视频流压缩，只存在帧内压缩。

# 外部依赖：

1. Qt 5.7

2. boost 1.59.0

# 内部使用的第三方库：

1. Src\sioclient， 来自于 https://github.com/socketio/socket.io-client-cpp ，Release 1.6.1，为与Qt共存，经过修改，将 emit 修改为 emitee。

2. Src\sioclient 依赖于 GeneralLibs\rapidjson，来自于 https://github.com/miloyip/rapidjson/tree/a36110e11874bcf35af854940e0ce910c19a8b49 ，@ a36110e，未作修改。

3. Src\sioclient 依赖于 GeneralLibs\websocketpp，来自于 https://github.com/zaphoyd/websocketpp/tree/ac5d7ea5af9734de965688b54a7860259887b537 ，@ ac5d7ea，经过修改，注释掉所有返回 `error::missing_required_header` 的语句。

