// Last Update:2019-06-13 16:33:11
/**
 * @file log.h
 * @brief 
 * @author felix
 * @version 0.1.00
 * @date 2019-05-09
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

typedef struct {
    char *output;
    char *mac;
    int connected;
    void *mqtt_instance;
} log_info_t;

#define LOGI(args...) printf( "__%s %s %d __ : ", __FILE__, __FUNCTION__, __LINE__);printf(args)
#define LOGE(args...) LOGI(args)

int log_out( const char *file, const char *function, int line, const char *format, ...  );
void log_init( char *output );

#endif  /*LOG_H*/
