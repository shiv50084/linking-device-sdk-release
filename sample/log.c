// Last Update:2019-06-13 19:38:04
/**
 * @file log.c
 * @brief 
 * @author felix
 * @version 0.1.00
 * @date 2019-05-09
 */

#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <time.h>
#include "log_local.h"

#define BUFFER_SIZE 1024

static log_info_t log_info;

int GetCurrentTime( char *now_time )
{
    time_t now;
    struct tm *tm_now = NULL;

    time(&now);
    tm_now = localtime(&now);
    strftime( now_time, 200, "%Y-%m-%d %H:%M:%S", tm_now);

    return(0);
}

int log_out( const char *file, const char *function, int line, const char *format, ...  )
{
    char buffer[BUFFER_SIZE] = { 0 };
    va_list arg;
    int len = 0;
    char now[200] = { 0 };
 
    memset( now, 0, sizeof(now) );
    GetCurrentTime( now );
    len = sprintf( buffer, "[ %s ] ", now );
    len = sprintf( buffer+len, "[ %s %s +%d ] ", file, function, line );

    va_start( arg, format );
    vsprintf( buffer+strlen(buffer), format, arg );
    va_end( arg );

    printf("%s", buffer );
    return 0;

}

