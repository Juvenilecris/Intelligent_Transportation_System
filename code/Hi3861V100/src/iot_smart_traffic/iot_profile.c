/*
 * 代码大部分来源自HiSilicon (Shanghai) Technologies CO.的/verder/Hisilion/demo/iot_profile.c
 * 代码在海思提供的代码基础上做出修改，添加了设备向平台发送json格式消息的接口
 * 以及对上报属性的接口 IoTProfilePropertyReport MakeProfilePropertyReport MakeServices进行修改
 * 并新定义了结构体IoTProfileServices 和IoTMsg 
 * 使在原来仅可以上报一次属性的基础上，使新的接口可以一次上报多个属性
 */

#include <string.h>
#include <hi_mem.h>
#include <cJSON.h>
#include "hi_stdlib.h"
#include "stdio.h"
#include "hi_mem.h"
#include "iot_main.h"
#include "iot_profile.h"

// format the report data to json string mode
static cJSON *FormatProfileValue(IoTProfileKV *kv)
{
    cJSON  *ret = NULL;
    switch (kv->type) {
        case EN_IOT_DATATYPE_INT:
            ret = cJSON_CreateNumber(kv->iValue);
            break;
        case EN_IOT_DATATYPE_LONG:
            ret = cJSON_CreateNumber((double)(*(long *)kv->value));
            break;
        case EN_IOT_DATATYPE_STRING:
            ret = cJSON_CreateString((const char *)kv->value);
            break;
        default:
            break;
    }
    return ret;
}

static cJSON *MakeKvs(IoTProfileKV *kvlst)
{
    cJSON *root;
    cJSON *kv;
    IoTProfileKV *kvInfo;

    // build a root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return root;
    }

    // add all the property to the properties
    kvInfo = kvlst;
    while (kvInfo != NULL) {
        kv = FormatProfileValue(kvInfo);
        if (kv == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
                root = NULL;
            }
            return root;
        }

        cJSON_AddItemToObject(root, kvInfo->key, kv);
        kvInfo = kvInfo->nxt;
    }
    // OK, now we return it
    return root;
}

#define CN_PROFILE_SERVICE_KEY_SERVICEID "service_id"
#define CN_PROFILE_SERVICE_KEY_PROPERTIIES "properties"
#define CN_PROFILE_SERVICE_KEY_EVENTTIME "event_time"
#define CN_PROFILE_KEY_SERVICES "services"
#define CN_PROFILE_CONTENT "content"

static cJSON *MakeService(IoTProfileService *serviceInfo)
{
    cJSON *root;
    cJSON *serviceID;
    cJSON *properties;
    cJSON *eventTime;

    // build a root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return root;
    }

    // add the serviceID node to the root node
    serviceID = cJSON_CreateString(serviceInfo->serviceID);
    if (serviceID == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
        return root;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_SERVICE_KEY_SERVICEID, serviceID);

    // add the properties node to the root
    properties = MakeKvs(serviceInfo->serviceProperty);
    if (properties == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
            cJSON_Delete(properties);
            root = NULL;
        }
        return root;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_SERVICE_KEY_PROPERTIIES, properties);
    // add the event time (optional) to the root
    if (serviceInfo->eventTime != NULL) {
        eventTime = cJSON_CreateString(serviceInfo->eventTime);
        if (eventTime == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
                root = NULL;
            }
            return root;
        }
        cJSON_AddItemToObjectCS(root, CN_PROFILE_SERVICE_KEY_EVENTTIME, eventTime);
    }
    // OK, now we return it
    return root;
}

static cJSON *MakeContent(IoTMsg *msg)
{
    cJSON *root;
    cJSON *content;

    // build a root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return root;
    }

    // add the serviceID node to the root node
    content = cJSON_CreateString(msg->content);
    if (content == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
        return root;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_CONTENT, content);
    return root;
}
static cJSON *MakeServices(IoTProfileServices *servicesPayload)
{
    cJSON *servicesNode = cJSON_CreateArray();
    if (servicesNode == NULL) {
        return NULL;
    }
    for(int i = 0; i < servicesPayload->serviceCount; i++)
     {
        cJSON *service;
        IoTProfileService*  serviceTmp;
        serviceTmp =&servicesPayload->services[i];
        while (serviceTmp!=NULL) {
            service = MakeService(serviceTmp);
            if (service == NULL) {
                return servicesNode;
            }
            cJSON_AddItemToArray(servicesNode, service);
            serviceTmp = serviceTmp->nxt;
    }
     }
    return servicesNode;
}

static char *MakeTopic(const char *fmt, const char *deviceId, const char *requestID)
{
    int len;
    char *ret = NULL;

    len = strlen(fmt) + strlen(deviceId);
    if (requestID != NULL) {
        len += strlen(requestID);
    }

    ret = hi_malloc(0, len);
    if (ret != NULL) {
        if (requestID != NULL) {
            if (snprintf_s(ret, len + 1, len, fmt, deviceId, requestID) < 0) {
                printf("string is null\r\n");
            }
        } else {
            if (snprintf_s(ret, len + 1, len, fmt, deviceId) < 0) {
                printf("string is null\r\n");
            }
        }
    }
    return ret;
}

#define CN_PROFILE_CMDRESP_KEY_RETCODE    "result_code"
#define CN_PROFILE_CMDRESP_KEY_RESPNAME    "response_name"
#define CN_PROFILE_CMDRESP_KEY_PARAS    "paras"
static char *MakeProfileCmdResp(IoTCmdResp *payload)
{
    char *ret = NULL;
    cJSON *root;
    cJSON *retCode;
    cJSON *respName;
    cJSON *paras;

    // create the root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return ret;
    }

    // create retcode and retdesc and add it to the root
    retCode = cJSON_CreateNumber(payload->retCode);
    if (retCode == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
        }
        return ret;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_CMDRESP_KEY_RETCODE, retCode);

    if (payload->respName != NULL) {
        respName = cJSON_CreateString(payload->respName);
        if (respName == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
            }
            return ret;
        }
        cJSON_AddItemToObjectCS(root, CN_PROFILE_CMDRESP_KEY_RESPNAME, respName);
    }

    if (payload->paras != NULL) {
        paras = MakeKvs(payload->paras);
        if (paras == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
            }
            return ret;
        }
        cJSON_AddItemToObjectCS(root, CN_PROFILE_CMDRESP_KEY_PARAS, paras);
    }

    // OK, now we make it to a buffer
    ret = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return ret;
}

#define CN_PROFILE_TOPICFMT_CMDRESP    "$oc/devices/%s/sys/commands/response/request_id=%s"
int IoTProfileCmdResp(char *deviceID, IoTCmdResp *payload)
{
    int ret = -1;
    char *topic;
    char *msg;

    if ((deviceID == NULL) || (payload == NULL) || (payload->requestID == NULL)) {
        return ret;
    }

    topic = MakeTopic(CN_PROFILE_TOPICFMT_CMDRESP, deviceID, payload->requestID);
    if (topic == NULL) {
        return ret;
    }
    msg = MakeProfileCmdResp(payload);
    if ((topic != NULL) && (msg != NULL)) {
        ret = IotSendMsg(0, topic, msg);
    }

    hi_free(0, topic);
    cJSON_free(msg);
    return ret;
}

static char *MakeProfilePropertyReport(IoTProfileServices *payload)
{
    char *ret = NULL;
    cJSON *root;
    cJSON *services;

    // create the root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return ret;
    }

    // create the services array node to the root
    services = MakeServices(payload);
    if (services == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
        }
        return ret;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_KEY_SERVICES, services);

    // OK, now we make it to a buffer
    ret = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return ret;
}
#define CN_PROFILE_TOPICFMT_PROPERTYREPORT    "$oc/devices/%s/sys/properties/report"
int IoTProfilePropertyReport(char *deviceID, IoTProfileServices *payload)
{
    int ret = -1;
    char *topic;
    char *msg;

    if ((deviceID == NULL) || (payload == NULL)) {
        return ret;
    }
    topic = MakeTopic(CN_PROFILE_TOPICFMT_PROPERTYREPORT, deviceID, NULL);
    if (topic == NULL) {
        return ret;
    }
    msg = MakeProfilePropertyReport(payload);
    if ((topic != NULL) && (msg != NULL)) {
        ret = IotSendMsg(0, topic, msg);
    }

    hi_free(0, topic);
    cJSON_free(msg);
    return ret;
}
#define CN_PROFILE_TOPICFMT_MsgUp    "$oc/devices/%s/sys/messages/up"


static char *MakeProfileMsgUp(IoTMsg *payload)
{
    char *ret = NULL;
    cJSON *root;
    cJSON *content;

    // create the root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return ret;
    }
    content=MakeContent(payload);
    if (content == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
        }
        return ret;
    }
    cJSON_AddItemToObjectCS(root, CN_PROFILE_CONTENT, content);
    // OK, now we make it to a buffer
    ret = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return ret;
}

int IoTProfileMsgUp(char *deviceID, IoTMsg *payload)
{
    int ret = -1;
    char *topic;
    char *msg;

    if ((deviceID == NULL) || (payload == NULL) || (payload->content == NULL) ) {
        return ret;
    }
    topic = MakeTopic(CN_PROFILE_TOPICFMT_MsgUp, deviceID, NULL);
    if (topic == NULL) {
        return ret;
    }
    msg = MakeProfileMsgUp(payload);
    if ((topic != NULL) && (msg != NULL)) {
        ret = IotSendMsg(0, topic, msg);
    }

    hi_free(0, topic);
    cJSON_free(msg);
    return ret;
}