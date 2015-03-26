#ifndef __CUITILS_EPHEMERAL_CLIENT_H
#define __CUITILS_EPHEMERAL_CLIENT_H

#include "cutils/log.h"
#ifdef __cplusplus
extern "C" {
#endif
/* log information. used to monitor the flow of Strapp debug logs.*/
#define LOG_EPHEMERAL_ERROR_TAG            "Ephemeral_E"
#define LOG_EPHEMERAL_INFO_TAG             "Ephemeral_I" 

#define LOGPI(...)    EPHLOGI(LOG_EPHEMERAL_INFO_TAG, __VA_ARGS__)
#define LOGCLI(...)    EPHLOGI(LOG_EPHEMERAL_INFO_TAG, __VA_ARGS__)
#define LOGPE(...)    EPHLOGE(LOG_EPHEMERAL_ERROR_TAG, __VA_ARGS__)
typedef enum
{
  JCLASS = 0,
  STATIC_ASSET=1,
  NATIVE_LIB=2
}ephemeral_req_type;

extern int ephemeral_get(int sockfd, ephemeral_req_type type,const char * name, void** buf);
extern int ephemeral_cleanup(int fd);
extern char* repl_str(const char *str, const char *old, const char *NEW);

#ifdef __cplusplus
}
#endif

#endif
