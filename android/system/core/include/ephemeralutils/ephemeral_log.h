#ifndef __CUITILS_EPHEMERAL_CLIENT_H
#define __CUITILS_EPHEMERAL_CLIENT_H

#include "cutils/log.h"
#ifdef __cplusplus
extern "C" {
#endif
/* log information. used to monitor the flow of Strapp debug logs.*/
#define LOG_EPHEMERAL_ERROR_TAG            "Ephemeral_logE"
#define LOG_EPHEMERAL_INFO_TAG             "Ephemeral_logI" 

#define LOGPI(...)    EPHLOGI(LOG_EPHEMERAL_INFO_TAG, __VA_ARGS__)
#define LOGCLI(...)    EPHLOGI(LOG_EPHEMERAL_INFO_TAG, __VA_ARGS__)
#define LOGPE(...)    EPHLOGE(LOG_EPHEMERAL_ERROR_TAG, __VA_ARGS__)
typedef enum
{
  JCLASS = 0,
  OJCLASS = 1,
  STATIC_ASSET=2,
  NATIVE_LIB=3,
  DIRECTORY=4,
  DATABASE=5,
}ephemeral_log_type;

extern int ephemeral_log(const char* appname,ephemeral_log_type type,const char *logpath);
extern int ephemeral_truncate(const char* appname);
extern int ephemeral_clearlog(const char* appname);
#ifdef __cplusplus
}
#endif

#endif

