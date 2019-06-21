// Last Update:2019-06-03 09:47:11
/**
 * @file ipc.c
 * @brief  将摄像头所具有的能力抽象出通用的一组接口
 * @author felix
 * @version 0.1.00
 * @date 2019-03-15
 */

#include <stdlib.h>
#include <stdio.h>
#include "ipc.h"

#define BASIC() printf("[ %s %s() +%d ] ", __FILE__, __FUNCTION__, __LINE__ )
#define LOGI(args...) BASIC();printf(args)
#define LOGE(args...) LOGI(args)

static ipc_dev_t *ipc;

int ipc_init( ipc_param_t *param )
{
    int ret = 0;

    if ( !param ) {
        LOGE("check param error\n");
        goto err;
    }

    if ( !ipc || !ipc->init ) {
        LOGE("check ipc dev fail\n");
        goto err;
    }

    ret = ipc->init( ipc, param );
    if ( ret != 0 ) {
        LOGE("ipc init error\n");
        goto err;
    }

    return 0;
err:
    return -1;
}

void ipc_run()
{
    if ( !ipc || !ipc->run ) {
        LOGE("no ipc dev found!!!\n");
        return;
    }

    ipc->run( ipc );
}

int ipc_capture_picture( char *path, char *file )
{
    if ( !ipc | !ipc->capture_picture ) {
        LOGE("param check error\n");
        return -1;
    }

    ipc->capture_picture( ipc, path, file );
    return 0;
}

int ipc_get_media_config( media_config_t *config )
{
    if ( !ipc | !ipc->get_media_config ) {
        LOGE("param check error\n");
        return -1;
    }

    return ipc->get_media_config( config );
}

int ipc_dev_register( ipc_dev_t *dev )
{
    if ( !dev ) {
        LOGE("check param error\n");
        goto err;
    }

    ipc = dev;
err:
    return -1;
}

