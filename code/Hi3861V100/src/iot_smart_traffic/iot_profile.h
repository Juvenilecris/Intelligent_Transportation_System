/*
 * 代码大部分来源于HiSilicon (Shanghai) Technologies CO.的/verder/Hisilion/demo/oc_demo/iot_profile.h
 * 在海思代码的基础上，代码做出一定的修改：新定义了结构体IoTProfileServices和IoTMsg 
 */

#ifndef IOT_PROFILE_H_
#define IOT_PROFILE_H_
#include "iot_profile.h"

#define OC_BEEP_STATUS_ON       ((hi_u8) 0x01)
#define OC_BEEP_STATUS_OFF      ((hi_u8) 0x00)

////< enum all the data type for the oc profile
typedef enum {
    EN_IOT_DATATYPE_INT = 0,
    EN_IOT_DATATYPE_LONG,
    EN_IOT_DATATYPE_FLOAT,
    EN_IOT_DATATYPE_DOUBLE,
    EN_IOT_DATATYPE_STRING, ///< must be ended with '\0'
    EN_IOT_DATATYPE_LAST,
}IoTDataType;


typedef struct {
    void    *nxt;  ///< ponit to the next key
    const char   *key;
    const char   *value;
    int   iValue;
    float  LedValue;
    IoTDataType  type;
}IoTProfileKV;

typedef struct {
    void *nxt;
    char *serviceID; ///< the service id in the profile, which could not be NULL
    char *eventTime; ///< eventtime, which could be NULL means use the platform time
    IoTProfileKV *serviceProperty; ///< the property in the profile, which could not be NULL
}IoTProfileService;
#define MAX_SERVICES_COUNT 20
/*
新定义的结构体：IoTProfileServices;
成员是IoTProfileService（服务）数组和当前服务的个数
*/
typedef struct {
    IoTProfileService services[MAX_SERVICES_COUNT];
    int serviceCount;
} IoTProfileServices;

typedef struct {
    int  retCode; ///< response code, 0 success while others failed
    const char   *respName; ///< response name
    const char   *requestID;///< specified by the message command
    IoTProfileKV  *paras;  ///< the command paras
}IoTCmdResp;

typedef struct {
    const char   *content; ///消息内容
}IoTMsg;

/**
 * Use this function to make the command response here
 * and you must supplied the device id, and the payload defines as IoTCmdResp_t
 *
*/
int IoTProfileCmdResp(char *deviceID, IoTCmdResp *payload);
/**
 * use this function to report the property to the iot platform
 *
*/
int IoTProfilePropertyReport(char *deviceID, IoTProfileServices *payload);
int IoTProfileMsgUp(char *deviceID, IoTMsg *payload);
#endif