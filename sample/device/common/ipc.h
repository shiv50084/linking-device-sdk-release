// Last Update:2019-06-03 10:28:50
/**
 * @file ipc.h
 * @brief  ipc interface, support multi manufacturer ipc
 * @author felix
 * @version 0.1.00
 * @date 2019-03-15
 */

#ifndef IPC_H
#define IPC_H

#include <stdint.h>


enum {
    EVENT_CAPTURE_PICTURE_SUCCESS,
    EVENT_CAPTURE_PICTURE_FAIL,
    EVENT_MOTION_DETECTION,
    EVENT_MOTION_DETECTION_DISAPEER,
};

typedef enum {
    STREAM_MAIN,
    STREAM_SUB,
    STREAM_MAX,
} StreamChannel;

typedef enum {
    AUDIO_NONE,
    AUDIO_AAC,
    AUDIO_G711U,
    AUDIO_G711A,
} AudioType;

typedef enum {
    VIDEO_ENCODE_TYPE_H264,
    VIDEO_ENCODE_TYPE_H265,
} VideoEncodeType;

typedef struct {
    int sample_rate;
    int audio_type;
    int video_type;
    unsigned char is_audio_enable;
} media_config_t;

typedef struct {
    int audio_type;
    int stream_number;
    char *video_file;
    char *pic_file;
    char *audio_file;
    int (*video_cb) ( uint8_t *frame, int len, int iskey, int64_t timestamp, int stream_no );
    int (*audio_cb) ( uint8_t *frame, int len, int64_t timestamp, int stream_no );
    int (*event_cb)( int event, void *data );
    int (*log_output)( char *log );
} ipc_param_t;

typedef struct ipc_dev_t {
    int (*init)( struct ipc_dev_t *ipc, ipc_param_t *param );
    void (*run)( struct ipc_dev_t *ipc );
    int (*capture_picture)( struct ipc_dev_t *ipc, char *path, char *file );
    void (*deinit)( struct ipc_dev_t *ipc );
    int (*get_media_config)( media_config_t *config );
    void *priv;
} ipc_dev_t;

extern int ipc_init( ipc_param_t *param );
extern void ipc_run();
extern int ipc_dev_register( ipc_dev_t *dev );
extern int ipc_capture_picture( char *path, char *file );
extern int ipc_get_media_config( media_config_t *config );

#endif  /*IPC_H*/
