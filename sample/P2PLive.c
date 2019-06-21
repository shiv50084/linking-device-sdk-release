// Last Update:2019-06-21 17:15:50
/**
 * @file P2PLive.c
 * @brief 
 * @author felix
 * @version 0.1.00
 * @date 2019-04-24
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "ipc.h"
//#include "log.h"
#include "log_local.h"
#include "qnlinking.h"

enum {
    AUDIO_TYPE_AAC,
    AUDIO_TYPE_G711A,
    AUDIO_TYPE_G711U,
};

typedef struct {
    int running;
    int sts;
    LinkInstance handle;
} app_t;

enum {
    APP_STATUS_DISCONNECT,
    APP_STATUS_PUSH_STREAM,
};

static app_t app;

int VideoFrameCallBack ( uint8_t *frame, int len, int iskey, int64_t timestamp, int stream_no )
{
    int ret = 0;

    ret = Link_PushVideo( app.handle,
                         (void*)frame,
                         (size_t )len,
                         timestamp,
                         iskey,
                        0 
                        );
    if ( ret <  0 ) {
        LOGE("Link_PushVideo error\n");
        return -1;
    }
    return 0;
}

int AudioFrameCallBack ( uint8_t *frame, int len, int64_t timestamp, int stream_no )
{
    int ret = 0;

    ret = Link_PushAudio( app.handle,
                         (void*)frame,
                         (size_t)len,
                         timestamp,
                        0 
                        );
    if ( ret < 0 ) {
        LOGE("Link_PushAudio error\n");
        return -1;
    }
    return 0;
}


void log_callbck(int nLevel, char * pLog)
{
    //log_out2("%s", pLog );
    printf("%s", pLog );
}

static void reportUploadStatistic( void *pUserOpaque, LinkUploadKind uploadKind, LinkUploadResult uploadResult)
{
    static int total = 0, error = 0;

    if ( uploadKind == LINK_UPLOAD_TS ) {
        if ( uploadResult != LINK_UPLOAD_RESULT_OK ) {
            error ++;
        }
        total++;
        LOGI("total : %d error : %d percent : %%%f\n", total, error, error*1.0/total*100 );
    }

}


static int getFileSize( char *_pFileName)
{
    FILE *fp = fopen( _pFileName, "r"); 
    int size = 0;
    
    if( !fp ) {
       LOGE("fopen file %s error\n", _pFileName );
        return -1;
    }

    fseek(fp, 0L, SEEK_END ); 
    size = ftell(fp); 
    fclose(fp);

    return size;
}

static int removeFile( char *_pFileName )
{
    int ret = 0;
    char okfile[512] = { 0 };

    if ( access( _pFileName, R_OK ) == 0  ) {
        ret = remove( _pFileName );
        if ( ret != 0 ) {
            LOGE("remove file %s error, ret = %d\n", _pFileName, ret );
        }
    } else {
        LOGE("file %s not exist \n", _pFileName );
    }

    sprintf( okfile, "%s.ok", _pFileName );
    if ( access( okfile, R_OK ) == 0  ) {
        ret = remove( okfile );
        if ( ret != 0 ) {
            LOGE("remove file %s error, ret = %d\n", okfile, ret );
        }
    } else {
        /*LOGE("file %s not exist \n", okfile );*/
    }

    return 0;
}


int eventCallBack( int event, void *data )
{
    int ret = 0;

    switch( event ) {
    case EVENT_CAPTURE_PICTURE_SUCCESS:
        {
            char *filename = data;

            if ( !filename )
                return -1;

            int filesize = getFileSize( filename );
            FILE *fp = fopen( filename, "r" );

            if ( !fp ) {
                LOGE("open file %s error", filename );
                return -1;
            }
            char *pbuf = (char *)malloc( filesize );
            if ( !pbuf ) {
                fclose( fp );
                return -1;
            }

            fread( pbuf, filesize, 1, fp );
            ret = Link_PushPicture( app.handle, pbuf, filesize, filename );
            if ( ret < 0 ) {
                free( pbuf );
                fclose( fp );
                LOGE("Link_PushPicture error\n");
                return -1;
            }
            removeFile( filename );
            free( pbuf );
            fclose( fp );
        }
        break;
    case EVENT_MOTION_DETECTION:
        {
            LinkSegmentMeta metas;
            metas.len = 1;
            char *keys[1] = {"type"};
            metas.keys = (const char **)keys;
            int keylens[1] = {4};
            metas.keylens = keylens;
            char *values[1] = {"move"};
            metas.values = (const char **)values;
            int valuelens[1] = {4};
            metas.valuelens = valuelens;

            Link_SegmentStart( app.handle, &metas );
        }
        break;
    case EVENT_MOTION_DETECTION_DISAPEER:
        Link_SegmentEnd( app.handle );
        break;
    default:
        break;
    }

    return 0;
}

static void getPicCb(LinkInstance handle, void *context, const char *pFilename)
{
    ipc_capture_picture( "/tmp", (char *)pFilename );
}

static void logCb( int nLevel,  char * pLog)
{
    printf("%s", pLog );
}

int main( int argc, char *argv[] )
{
    char *output = NULL;
    int ret = 0;

    app.running = 1;

    int option_index = 0;
    int c = 0;
    char *dak = NULL;
    char *dsk = NULL;
    char *video = NULL;
    char *audio = NULL;
    struct option long_options[] = {
        {"dak",     required_argument, 0, 'a' },
        {"dsk",     required_argument, 0, 's' },
        {"log",     required_argument, 0, 'l' },
        {"video",     required_argument, 0, 'v' },
        {"audio",     required_argument, 0, 'u' },
        {0,         0,                 0,  0 }
    };

    while(1) {
        c = getopt_long(argc, argv, "a:s:l:v:u:",
                        long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
        case 0:
            printf("option %s", long_options[option_index].name);
            if (optarg)
                printf(" with arg %s", optarg);
            printf("\n");
            break;
        case 'a':
            dak = optarg;
            break;
        case 's':
            dsk = optarg;
            break;
        case 'l':
            output = optarg;
            break;
        case 'v':
            video = optarg;
            break;
        case 'u':
            audio = optarg;
            break;
        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if ( !dak || !dsk ) {
        LOGE("please pass dak and dsk\n");
        return 0;
    }

//    log_init( output );

    Link_InitOptions initOptions;
    LOGI("dak = %s, dsk = %s\n", dak, dsk );

    initOptions.pDeviceAk = dak;
    initOptions.pDeviceSk = dsk;
    initOptions.pConfigRequestUrl = "http://linking-device.qiniuapi.com/v1/device/config";
    ret = Link_Init( &initOptions );
    if ( ret < 0 ) {
        LOGE("Link_Init error\n");
        return 0;
    }

    ipc_param_t param =
    {
        .audio_type = AUDIO_AAC,
        .video_file = "./video.h264",
        .audio_file = "./audio.aac",
        .pic_file = "../media/pic.jpg",
        .video_cb = VideoFrameCallBack,
        .audio_cb = AudioFrameCallBack,
        .event_cb = eventCallBack,
        .log_output = NULL
    };

    param.video_file = video;
    LOGI("audio = %s\n", audio );
    param.audio_file = audio;
    ipc_init( &param );
    Link_MediaOptions mediaOptions = 
    {
        .nAudioFormat = LINK_AUDIO_AAC,
        .nAudioChannels = 1,
        .nAudioSampleRate = LINK_AUDIO_SAMPLERATE_16000HZ,
        .nAudioBitDepth = LINK_AUDIO_BITDEPTH_16,
        .nVideoFormat = LINK_VIDEO_H264,
    };
    media_config_t config;
    ret = ipc_get_media_config( &config );
    if ( ret == 0 && config.audio_type == AUDIO_G711A ) {
        LOGI("audio encode type is G711A\n");
        mediaOptions.nAudioFormat = LINK_AUDIO_PCMA;
    } else if ( config.audio_type == AUDIO_G711U ) {
        LOGI("audio encode type is G711U\n");
        mediaOptions.nAudioFormat = LINK_AUDIO_PCMU;
    } else {
        /* do nothing */
    }

    LOGI("config.sample_rate = %d\n", config.sample_rate );
    if ( config.sample_rate == 8000 )
        mediaOptions.nAudioSampleRate = LINK_AUDIO_SAMPLERATE_SPECIAL;

    Link_UploadOptions uploadOptions = 
    {
        .nTsMaxSize = 1572864,
        .reserved1 = reportUploadStatistic,
        .reserved2 = logCb,
        .GetPicCb = getPicCb,
    };
    ret = Link_Create( &app.handle,
                &mediaOptions,
                &uploadOptions,
                NULL 
                );
    if ( ret < 0 ) {
        LOGE("Link_Create error\n");
        return 0;
    }
    ipc_run();

    while( app.running ) {
        sleep( 10 );
    }

    return 0;
}

