/**
 * @file qnlinking.h
 * @author Qiniu.com
 * @copyright 2019(c) Shanghai Qiniu Information Technologies Co., Ltd.
 * @brief linking-device-sdk API header file
 */

#ifndef __QNLINKING_API__
#define __QNLINKING_API__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#define LINK_TRUE 1
#define LINK_FALSE 0

/** @brief 返回值 */

typedef void* LinkInstance;

typedef enum {
        LINK_MEDIA_AUDIO = 1,
        LINK_MEDIA_VIDEO = 2,
}LinkMediaType;

typedef enum {
        LINK_VIDEO_NONE = 0,
        LINK_VIDEO_H264 = 1,
        LINK_VIDEO_H265 = 2,
}LinkVideoFormat;

typedef enum {
        LINK_AUDIO_NONE = 0,
        LINK_AUDIO_PCMU = 1,
        LINK_AUDIO_PCMA = 2,
        LINK_AUDIO_AAC =  3,
}LinkAudioFormat;

typedef enum {
        LINK_UPLOAD_TS = 1,
        LINK_UPLOAD_PIC = 2,
} LinkUploadKind;

typedef enum {
        LINK_UPLOAD_RESULT_OK = 1,
        LINK_UPLOAD_RESULT_FAIL = 2
} LinkUploadResult;

typedef enum {
    LINK_AUDIO_SAMPLERATE_SPECIAL,
    LINK_AUDIO_SAMPLERATE_11025HZ,
    LINK_AUDIO_SAMPLERATE_16000HZ,
    LINK_AUDIO_SAMPLERATE_22050HZ,
    LINK_AUDIO_SAMPLERATE_44100HZ,
} LinkAudioSampleRate;

typedef enum {
    LINK_AUDIO_BITDEPTH_8,
    LINK_AUDIO_BITDEPTH_16,
} LinkAudioBitDepth;

/** @brief 初始化参数 */
typedef struct {
    const char *pConfigRequestUrl;          /**< 获取业务配置的请求地址 */
    const char *pDeviceAk;                  /**< 设备 DAK */
    const char *pDeviceSk;                  /**< 设备 DSK */
} Link_InitOptions;

/** @brief 音视频参数 */
typedef struct {
        LinkAudioFormat nAudioFormat;           /**< 音频格式 */
        int nAudioChannels;                     /**< 音频通道数 */
        int nAudioSampleRate;                   /**< 音频采样率 */
        int nAudioBitDepth;                     /**< 音频位宽 */
        LinkVideoFormat nVideoFormat;           /**< 视频格式 */
} Link_MediaOptions;

/** @brief SSL相关参数 */
typedef struct {
    const char * cafile;        /**< CA证书文件路径 */
    const char * capath;        /**< CA证书目录，用于多CA证书文件 */
    const char * crtfile;       /**< 本地证书文件路径 */
    const char * keyfile;       /**< 本地证书对应密钥 */
    const char * keypassword;   /**< 本地证书对应密钥密码 */
    int sslVersion;             /**< SSL 版本 0:default; 1:TLS 1.0; 2:TLS 1.1; 3:TLS 1.2 */
    int enableServerCertAuth;   /**< 是否开启服务端证书校验 */
    int verifyConmonName;       /**< 是否验证服务端证书域名信息 */
} Link_SSLOptions;

#define Link_SSLOptions_initializer {NULL, NULL, NULL, NULL, NULL, 0, 0, 0}

/** @brief 视频片段信息 */
typedef struct {
        const char **keys;
        int *keylens;
        const char **values;
        int *valuelens;
        int len;
        int isOneShot;
}LinkSegmentMeta;

#define Link_SegmentMeta_initializer {NULL, NULL, 0, 0}

/** @brief TS文件信息 */
typedef struct {
        int64_t startTime;
        int64_t endTime;
        Link_MediaOptions media[2];
        LinkMediaType mediaType[2];
        int nCount;
        char sessionId[21];
        const LinkSegmentMeta * pMeta;
} Link_TSInfo;

/**
 * 图片上传回调函数类型
 *
 * @param[in] handle 实例句柄
 * @param[in] context 回调参数
 * @param[in] pFilename 上传图片的文件名
 */
typedef void Link_GetPicture(LinkInstance handle, void *context, const char *pFilename);

/**
 * 上传结果回调函数类型
 *
 * @param[in] handle 实例句柄
 * @param[in] context 回调参数
 * @param[in] LinkUploadKind 上传类型
 * @param[in] LinkUploadKind 上传结果
 */
typedef void Link_UploadStat(LinkInstance handle, void *context, LinkUploadKind uploadKind, LinkUploadResult uploadResult);

/**
 * 输出TS文件回调函数类型
 *
 * @param[in] handle 实例句柄
 * @param[in] pBuffer TS内容
 * @param[in] size TS内容长度
 * @param[in] context 回调参数
 * @param[in] info TS信息
 */
typedef void Link_TsOutput(LinkInstance handle, const void *pBuffer, size_t size, void *context, Link_TSInfo info);

/** @brief TS封装和上传参数 */
typedef struct {
        Link_GetPicture *GetPicCb;              /**< 图片上传回调函数 */
        void *GetPicCb_Context;                 /**< 图片上传回调函数自定义参数 */
        Link_GetPicture *UpStatCb;              /**< 上传结果回调函数 */
        void *UpStatCb_Context;                 /**< 上传结果回调函数自定义参数*/
        Link_TsOutput *TsOutpubCb;              /**< 输出TS文件回调函数 */
        void *TsOutpubCb_Context;               /**< 输出TS文件回调函数自定义参数 */
        int nTsMaxSize;                         /**< TS文件的最大大小(单位字节)*/
        void * reserved1;                       /**< 预留1 */
        void * reserved2;                       /**< 预留2 */
}Link_UploadOptions;

/**
 * 初始化SDK
 *
 * 此函数不是线程安全函数。
 *
 * @return LINK_SUCCESS 成功; 其它值 失败
 */
int Link_Init(const Link_InitOptions * options);

/**
 * 注销SDK
 *
 * 此函数不是线程安全函数。
 *
 */
void Link_CleanUp();

/**
 * 创建实例
 *
 * @param[out] handle 实例句柄
 * @param[in] media_options 音视频参数
 * @param[in] upload_options 上传参数，不使用上传功能可以为 NULL
 * @param[in] ssl_options SSL相关参数，不使用SSL功能可以为 NULL
 *
 * @return LINK_SUCCESS 成功; 其它值 失败
 */
int Link_Create(LinkInstance * handle,
                const Link_MediaOptions * media_options,
                const Link_UploadOptions * upload_options,
                const Link_SSLOptions * ssl_options
                );

/**
 * 销毁实例
 *
 * @param[in] handle 实例句柄
 *
 */
void Link_Destroy(LinkInstance handle);

/**
 * 推送视频数据
 *
 * @param[in] handle 实例句柄
 * @param[in] pData 视频数据
 * @param[in] nDataLen 视频数据大小
 * @param[in] nTimestamp 视频时间戳, 如果存在音频，和音频时间戳一定要对应同一个基点
 * @param[in] nIsKeyFrame 是否是关键帧
 * @param[in] nFrameSysTime 帧对应的系统时间,单位为m毫秒。选填，可以为0
 * @return LINK_SUCCESS 成功; 其它值 失败
 */
int Link_PushVideo(LinkInstance handle,
                  const void * pData,
                  size_t nDataLen,
                  int64_t nTimestamp,
                  int nIsKeyFrame,
                  int64_t nFrameSysTime
                  );

/**
 * 推送音频流数据
 *
 * @param[in] handle 实例句柄
 * @param[in] pData 音频数据
 * @param[in] nDataLen 音频数据大小
 * @param[in] nTimestamp 音频时间戳，必须和视频时间戳对应同一个基点
 * @param[in] nFrameSysTime 帧对应的系统时间,单位为m毫秒。选填，可以为0
 * @return LINK_SUCCESS 成功; 其它值 失败
 */
int Link_PushAudio(LinkInstance handle,
                  const void * pData,
                  size_t nDataLen,
                  int64_t nTimestamp,
                  int64_t nFrameSysTime
                  );


/**
 * 推送图片上传
 *
 * @param[in] handle 实例句柄
 * @param[in] pBuf 图片内容
 * @param[in] nBuflen 图片内容长度
 * @param[in] pFilename 图片文件名
 *
 * @return LINK_SUCCESS 成功; 其它值 失败
 */
int Link_PushPicture(LinkInstance handle,
                    const void *pBuf,
                    size_t nBuflen,
                    const char *pFilename
                    );

/**
 * 开始视频片段
 *
 * @param[in] handle 实例句柄
 * @param[in] metas 自定义的元数据
 *
 * @return LINK_SUCCESS 成功; 其它值 失败
 */
int Link_SegmentStart(LinkInstance handle,
                      const LinkSegmentMeta * meta
                      );

/**
 * 结束视频片段
 *
 * @param[in] handle 实例句柄
 *
 * @return LINK_SUCCESS 成功; 其它值 失败
 */
int Link_SegmentEnd(LinkInstance handle);


#ifdef __cplusplus
}
#endif

#endif /* __QNLINKING_API__ */
