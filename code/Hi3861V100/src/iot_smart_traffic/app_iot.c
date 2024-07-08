/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include "iot_config.h"
#include "iot_main.h"
#include "iot_profile.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_connecter.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_watchdog.h"
#include "cjson_init.h"
#include "hi_stdlib.h"
#include "hi_pwm.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "iot_watchdog.h"
#include "iot_pwm.h"
#include "hi_io.h"
#include "app_iot.h"

#define IOT_PWM_PORT_PWM0 0
/* attribute initiative to report */
#define TAKE_THE_INITIATIVE_TO_REPORT
/* oc request id */
#define CN_COMMAND_INDEX                    "commands/request_id="
/* oc report HiSpark attribute */
#define EMERGENCYMODULE "EmergencyModule"
#define CONGESTIONMODULE "CongestionModule"

#define yellow_time 3
#define TASK_SLEEP_1000MS (1000)
#define Time (1000)

#define CLK IOT_IO_NAME_GPIO_0

#define speed1_1 IOT_IO_NAME_GPIO_1
#define speed1_2 IOT_IO_NAME_GPIO_2 
#define speed2_1 IOT_IO_NAME_GPIO_5
#define speed2_2 IOT_IO_NAME_GPIO_6
#define speed3_1 IOT_IO_NAME_GPIO_4
#define speed3_2 IOT_IO_NAME_GPIO_4

#define congestion IOT_IO_NAME_GPIO_13
#define emergency IOT_IO_NAME_GPIO_14

// #define RED_1 IOT_IO_NAME_GPIO_10
// #define GREEN_1 IOT_IO_NAME_GPIO_11
// #define YELLOW_1 IOT_IO_NAME_GPIO_12

#define RED_2 IOT_IO_NAME_GPIO_7
#define GREEN_2 IOT_IO_NAME_GPIO_8 
#define YELLOW_2 IOT_IO_NAME_GPIO_9

#define RED_3 IOT_IO_NAME_GPIO_10
#define GREEN_3 IOT_IO_NAME_GPIO_11
#define YELLOW_3 IOT_IO_NAME_GPIO_12

// #define RED_4 IOT_IO_NAME_GPIO_10
// #define GREEN_4 IOT_IO_NAME_GPIO_11
// #define YELLOW_4 IOT_IO_NAME_GPIO_12

Traffic traffic1 = {
    .red_time = 50,
    .green_time = 20,
    .current_time = 10,
    .current_state = "RED"
};

Traffic traffic2 = {
    .red_time = 50,
    .green_time = 20,
    .current_time = 30,
    .current_state="RED"
};

Traffic traffic3 = {
    .red_time = 60,
    .green_time = 10,
    .current_time = 10,
    .current_state="GREEN"
};
Traffic traffic4 = {
    .red_time = 50,
    .green_time = 20,
    .current_time = 3,
    .current_state = "YELLOW"
};

Road road1={
    .name="Road1",
    .car_speed=0,
    .is_report=0
};
Road road2={
    .name="Road2",
    .car_speed=0,
    .is_report=0
};
Road road3={
    .name="Road2",
    .car_speed=0,
    .is_report=0
};


int is_emergency=0;
int is_congestion=0;

static void oncongestion(void)
{
    IoTGpioSetOutputVal(congestion,IOT_GPIO_VALUE0);
}
static void offcongestion(void)
{
    IoTGpioSetOutputVal(congestion,IOT_GPIO_VALUE1);
}
static void onemergency(void)
{
    IoTGpioSetOutputVal(emergency,IOT_GPIO_VALUE0);
}
static void offemergency(void)
{
    IoTGpioSetOutputVal(emergency,IOT_GPIO_VALUE1);
}
static int capture1_1(void)
{
    IotGpioValue value=IOT_GPIO_VALUE1;
    IoTGpioGetInputVal(speed1_1,  &value);
    return value;
}
static int capture1_2(void)
{
    IotGpioValue value=IOT_GPIO_VALUE1;
    IoTGpioGetInputVal(speed1_2,  &value);
    return value;
}
static int capture2_1(void)
{
    IotGpioValue value=IOT_GPIO_VALUE1;
    IoTGpioGetInputVal(speed2_1,  &value);
    return value;
}
static int capture2_2(void)
{
    IotGpioValue value=IOT_GPIO_VALUE1;
    IoTGpioGetInputVal(speed2_2,  &value);
    return value;
}
static int capture3_1(void)
{
    IotGpioValue value=IOT_GPIO_VALUE1;
    IoTGpioGetInputVal(speed3_1,  &value);
    return value;
}
static int capture3_2(void)
{
    IotGpioValue value=IOT_GPIO_VALUE1;
    IoTGpioGetInputVal(speed3_2,  &value);
    return value;
}
static void EmergencyMsgRcvCallBack(char *payload)
{
    printf("PAYLOAD:%s\r\n", payload);
    if (strstr(payload, EMERGENCYMODULE) != NULL) {
        if (strstr(payload,"true") != NULL) { // RED LED
            onemergency();
            is_emergency=1;
        } else if (strstr(payload, "false") != NULL) {
            offemergency();
            is_emergency=0;
        }
    }
}
static void CongestionMsgRcvCallBack(char *payload)
{
    printf("PAYLOAD:%s\r\n", payload);
    if (strstr(payload, CONGESTIONMODULE) != NULL) {
       if (strstr(payload,"true") != NULL) { // RED LED
            oncongestion();
            is_congestion=1;
        } else if (strstr(payload, "false") != NULL) {
            offcongestion();
            is_congestion=0;
        }
    }
}
static void TimeMsgRcvCallBack(int qos, char *topic, char *payload)
{
    const char *requesID;
    char *tmp;
    IoTCmdResp resp;
    printf("RCVMSG:QOS:%d TOPIC:%s PAYLOAD:%s\r\n", qos, topic, payload);
    EmergencyMsgRcvCallBack(payload);
    CongestionMsgRcvCallBack(payload);
    tmp = strstr(topic, CN_COMMAND_INDEX);
    if (tmp != NULL) {
        requesID = tmp + strlen(CN_COMMAND_INDEX);
        resp.requestID = requesID;
        resp.respName = NULL;
        resp.retCode = 0;  
        resp.paras = NULL;
        (void)IoTProfileCmdResp(CONFIG_DEVICE_PWD, &resp);
    }
}

void IotPublish(void)
{
   // 创建一个 IoTProfileServices 结构体实例
    IoTProfileServices myProfileServices;
    memset_s(&myProfileServices, sizeof(myProfileServices), 0, sizeof(myProfileServices));
    myProfileServices.serviceCount = 0; // 初始化服务计数为0

    // 创建第一个服务及属性
    IoTProfileService service1;
    IoTProfileKV property1;
    memset_s(&property1, sizeof(property1), 0, sizeof(property1));
    property1.type = EN_IOT_DATATYPE_INT;
    property1.key = "Red_time_host_forward";
    property1.iValue = traffic1.red_time;
    memset_s(&service1, sizeof(service1), 0, sizeof(service1));
    service1.serviceID = "TrafficLight";
    service1.serviceProperty = &property1;
    myProfileServices.services[myProfileServices.serviceCount] = service1;
    myProfileServices.serviceCount++;

    // 创建第二个服务及属性
    IoTProfileService service2;
    IoTProfileKV property2;
    memset_s(&property2, sizeof(property2), 0, sizeof(property2));
    property2.type = EN_IOT_DATATYPE_INT;
    property2.key = "Red_time_host_left";
    property2.iValue = traffic2.red_time;
    memset_s(&service2, sizeof(service2), 0, sizeof(service2));
    service2.serviceID = "TrafficLight";
    service2.serviceProperty = &property2;
    myProfileServices.services[myProfileServices.serviceCount] = service2;
    myProfileServices.serviceCount++;

    // 创建第三个服务及属性
    IoTProfileService service3;
    IoTProfileKV property3;
    memset_s(&property3, sizeof(property3), 0, sizeof(property3));
    property3.type = EN_IOT_DATATYPE_INT;
    property3.key = "Red_time_guest_forward";
    property3.iValue = traffic3.red_time;
    memset_s(&service3, sizeof(service3), 0, sizeof(service3));
    service3.serviceID = "TrafficLight";
    service3.serviceProperty = &property3;
    myProfileServices.services[myProfileServices.serviceCount] = service3;
    myProfileServices.serviceCount++;

    // 创建第四个服务及属性
    IoTProfileService service4;
    IoTProfileKV property4;
    memset_s(&property4, sizeof(property4), 0, sizeof(property4));
    property4.type = EN_IOT_DATATYPE_INT;
    property4.key = "Red_time_guest_left";
    property4.iValue = traffic4.red_time;
    memset_s(&service4, sizeof(service4), 0, sizeof(service4));
    service4.serviceID = "TrafficLight";
    service4.serviceProperty = &property4;
    myProfileServices.services[myProfileServices.serviceCount] = service4;
    myProfileServices.serviceCount++;

    // 创建第五个服务及属性
    IoTProfileService service5;
    IoTProfileKV property5;
    memset_s(&property5, sizeof(property5), 0, sizeof(property5));
    property5.type = EN_IOT_DATATYPE_INT;
    property5.key = "Green_time_host_forward";
    property5.iValue = traffic1.green_time;
    memset_s(&service5, sizeof(service5), 0, sizeof(service5));
    service5.serviceID = "TrafficLight";
    service5.serviceProperty = &property5;
    myProfileServices.services[myProfileServices.serviceCount] = service5;
    myProfileServices.serviceCount++;

    // 创建第六个服务及属性
    IoTProfileService service6;
    IoTProfileKV property6;
    memset_s(&property6, sizeof(property6), 0, sizeof(property6));
    property6.type = EN_IOT_DATATYPE_INT;
    property6.key = "Green_time_host_left";
    property6.iValue = traffic2.green_time;
    memset_s(&service6, sizeof(service6), 0, sizeof(service6));
    service6.serviceID = "TrafficLight";
    service6.serviceProperty = &property6;
    myProfileServices.services[myProfileServices.serviceCount] = service6;
    myProfileServices.serviceCount++;

    // 创建第七个服务及属性
    IoTProfileService service7;
    IoTProfileKV property7;
    memset_s(&property7, sizeof(property7), 0, sizeof(property7));
    property7.type = EN_IOT_DATATYPE_INT;
    property7.key = "Green_time_guest_forward";
    property7.iValue = traffic3.green_time;
    memset_s(&service7, sizeof(service7), 0, sizeof(service7));
    service7.serviceID="TrafficLight";
    service7.serviceProperty = &property7;
    myProfileServices.services[myProfileServices.serviceCount] = service7;
    myProfileServices.serviceCount++;

    // 创建第七个服务及属性
    IoTProfileService service8;
    IoTProfileKV property8;
    memset_s(&property8, sizeof(property8), 0, sizeof(property8));
    property8.type = EN_IOT_DATATYPE_INT;
    property8.key = "Green_time_guest_left";
    property8.iValue = traffic4.green_time;
    memset_s(&service8, sizeof(service8), 0, sizeof(service8));
    service8.serviceID="TrafficLight";
    service8.serviceProperty = &property8;
    myProfileServices.services[myProfileServices.serviceCount] = service8;
    myProfileServices.serviceCount++;
    
    // 创建第五个服务及属性
    IoTProfileService service9;
    IoTProfileKV property9;
    memset_s(&property9, sizeof(property9), 0, sizeof(property9));
    property9.type = EN_IOT_DATATYPE_INT;
    property9.key = "Current_time_host_forward";
    property9.iValue = traffic1.current_time;
    memset_s(&service9, sizeof(service9), 0, sizeof(service9));
    service9.serviceID = "TrafficLight";
    service9.serviceProperty = &property9;
    myProfileServices.services[myProfileServices.serviceCount] = service9;
    myProfileServices.serviceCount++;

    // 创建第六个服务及属性
    IoTProfileService service10;
    IoTProfileKV property10;
    memset_s(&property10, sizeof(property10), 0, sizeof(property10));
    property10.type = EN_IOT_DATATYPE_INT;
    property10.key = "Current_time_host_left";
    property10.iValue = traffic2.current_time;
    memset_s(&service10, sizeof(service10), 0, sizeof(service10));
    service10.serviceID = "TrafficLight";
    service10.serviceProperty = &property10;
    myProfileServices.services[myProfileServices.serviceCount] = service10;
    myProfileServices.serviceCount++;

    // 创建第七个服务及属性
    IoTProfileService service11;
    IoTProfileKV property11;
    memset_s(&property11, sizeof(property11), 0, sizeof(property11));
    property11.type = EN_IOT_DATATYPE_INT;
    property11.key = "Current_time_guest_forward";
    property11.iValue = traffic3.current_time;
    memset_s(&service11, sizeof(service11), 0, sizeof(service11));
    service11.serviceID="TrafficLight";
    service11.serviceProperty = &property11;
    myProfileServices.services[myProfileServices.serviceCount] = service11;
    myProfileServices.serviceCount++;


    IoTProfileService service12;
    IoTProfileKV property12;
    memset_s(&property12, sizeof(property12), 0, sizeof(property12));
    property12.type = EN_IOT_DATATYPE_INT;
    property12.key = "Current_time_guest_left";
    property12.iValue = traffic4.current_time;
    memset_s(&service12, sizeof(service12), 0, sizeof(service12));
    service12.serviceID="TrafficLight";
    service12.serviceProperty = &property12;
    myProfileServices.services[myProfileServices.serviceCount] = service12;
    myProfileServices.serviceCount++;

    IoTProfileService service13;
    IoTProfileKV property13;
    memset_s(&property13, sizeof(property13), 0, sizeof(property13));
    property13.type = EN_IOT_DATATYPE_STRING;
    property13.key = "Current_state_host_forward";
    if(is_emergency)
    {
        property13.value = "RED";
    }
    else{
    property13.value = traffic1.current_state;
    }
    memset_s(&service13, sizeof(service13), 0, sizeof(service13));
    service13.serviceID = "TrafficLight";
    service13.serviceProperty = &property13;
    myProfileServices.services[myProfileServices.serviceCount] = service13;
    myProfileServices.serviceCount++;


    IoTProfileService service14;
    IoTProfileKV property14;
    memset_s(&property14, sizeof(property14), 0, sizeof(property14));
    property14.type = EN_IOT_DATATYPE_STRING;
    property14.key = "Current_state_host_left";
    if(is_emergency)
    {
        property14.value = "RED";
    }
    else{
    property14.value = traffic2.current_state;
    }
    
    memset_s(&service14, sizeof(service14), 0,sizeof(service14));
    service14.serviceID = "TrafficLight";
    service14.serviceProperty = &property14;
    myProfileServices.services[myProfileServices.serviceCount] = service14;
    myProfileServices.serviceCount++;

    IoTProfileService service15;
    IoTProfileKV property15;
    memset_s(&property15, sizeof(property15), 0, sizeof(property15));
    property15.type = EN_IOT_DATATYPE_STRING;
    property15.key = "Current_state_guest_forward";
    if(is_emergency)
    {
        property15.value = "RED";
    }
    else{
    property15.value = traffic3.current_state;
    }
    memset_s(&service15, sizeof(service15), 0, sizeof(service15));
    service15.serviceID = "TrafficLight";
    service15.serviceProperty = &property15;
    myProfileServices.services[myProfileServices.serviceCount] = service15;
    myProfileServices.serviceCount++;


    IoTProfileService service16;
    IoTProfileKV property16;
    memset_s(&property16, sizeof(property16), 0, sizeof(property16));
    property16.type = EN_IOT_DATATYPE_STRING;
    property16.key = "Current_state_guest_left";
    if(is_emergency)
    {
        property16.value = "RED";
    }
    else{
    property16.value = traffic4.current_state;
    }
    memset_s(&service16, sizeof(service16), 0, sizeof(service16));
    service16.serviceID = "TrafficLight";
    service16.serviceProperty = &property16;
    myProfileServices.services[myProfileServices.serviceCount] = service16;
    myProfileServices.serviceCount++;

    
    IoTProfileService service_speed1;
    IoTProfileKV property_speed1;
    memset_s(&property_speed1, sizeof(property_speed1), 0, sizeof(property_speed1));
    property_speed1.type = EN_IOT_DATATYPE_STRING;
    property_speed1.key = "Road1";
    printf("car drives speed:road1 in %c\r\n",road1.car_speed);
    char str1[20];
    sprintf(str1, "road1-car-speed:%d",road1.car_speed);
    property_speed1.value = str1;
    memset_s(&service_speed1, sizeof(service_speed1), 0, sizeof(service_speed1));
    service_speed1.serviceID = "Speed";
    service_speed1.serviceProperty = &property_speed1;
    myProfileServices.services[myProfileServices.serviceCount] = service_speed1;
    myProfileServices.serviceCount++;

    
IoTProfileService service_speed2;
    IoTProfileKV property_speed2;
    memset_s(&property_speed2, sizeof(property_speed2), 0, sizeof(property_speed2));
    property_speed2.type = EN_IOT_DATATYPE_STRING;
    property_speed2.key = "Road2";
    printf("car drives speed:road2 in %c\r\n",road2.car_speed);
    char str2[20];
    sprintf(str2, "road2-car-speed:%d",road2.car_speed);
    property_speed2.value = str2;
    memset_s(&service_speed2, sizeof(service_speed2), 0, sizeof(service_speed2));
    service_speed2.serviceID = "Speed";
    service_speed2.serviceProperty = &property_speed2;
    myProfileServices.services[myProfileServices.serviceCount] = service_speed2;
    myProfileServices.serviceCount++;

    

    IoTProfileService service_speed3;
    IoTProfileKV property_speed3;
    memset_s(&property_speed3, sizeof(property_speed3), 0, sizeof(property_speed3));
    property_speed3.type = EN_IOT_DATATYPE_STRING;
    property_speed3.key = "Road3";
    printf("car drives speed:road3 in %c\r\n",road3.car_speed);
    char str3[20];
    sprintf(str3, "road3-car-speed:%d",road3.car_speed);
    property_speed3.value = str3;
    memset_s(&service_speed3, sizeof(service_speed3), 0, sizeof(service_speed3));
    service_speed3.serviceID = "Speed";
    service_speed3.serviceProperty = &property_speed3;
    myProfileServices.services[myProfileServices.serviceCount] = service_speed3;
    myProfileServices.serviceCount++;
    

    IoTProfileService service_emergency;
    IoTProfileKV property_emergency;
    memset_s(&property_emergency, sizeof(property_emergency), 0, sizeof(property_emergency));
    property_emergency.type = EN_IOT_DATATYPE_STRING;
    property_emergency.key = "Is_emergency";
    if(is_emergency)
    {
       property_emergency.value = "true";
    }
    else{
    property_emergency.value = "false";
    }
    memset_s(&service_emergency, sizeof(service_emergency), 0, sizeof(service_emergency));
    service_emergency.serviceID = "Alarm";
    service_emergency.serviceProperty = &property_emergency;
    myProfileServices.services[myProfileServices.serviceCount] = service_emergency;
    myProfileServices.serviceCount++;

    IoTProfilePropertyReport(CONFIG_DEVICE_ID, &myProfileServices);
}



static void Speed_1(void)
{
    while(1)
    {
    if (!capture1_1())
    {
        int counter=0;
        while(capture1_2() && counter<2000)
        {
            counter++;
            TaskMsleep(1);
        }
        if(counter<2000)
        {
            printf("car drives in road1\r\n");
            road1.car_speed=(int)(100000/counter);
            printf("speed:%d\r\n",road1.car_speed);
            road1.is_report=1;
            TaskMsleep(200);
        }
        }
    
   if (!capture1_2())
    {
            int counter=0;
            while(capture1_1() && counter<2000)
            {
                counter++;
                TaskMsleep(1);
            }
            if(counter<2000)
            {
            printf("car drives in road1\r\n");
            road1.car_speed=(int)(1000/counter);
            printf("speed:%d\r\n",road1.car_speed);
            road1.is_report=1;
            TaskMsleep(200);
            }
        }
    }
}

static void Speed_2(void)
{
    while(1)
    {
    if (!capture2_1())
    {
        int counter=0;
        while(capture2_2() && counter<2000)
        {
            counter++;
            TaskMsleep(1);
        }
        if(counter<2000)
        {
            printf("car drives in road2\r\n");
            road2.car_speed=(int)(100000/counter);
            printf("speed:%d\r\n",road2.car_speed);
            road2.is_report=1;
            TaskMsleep(200);
        }
        }
    
   if (!capture2_2())
    {
            int counter=0;
            while(capture2_1() && counter<2000)
            {
                counter++;
                TaskMsleep(1);
            }
            if(counter<2000)
            {
            printf("car drives in road2\r\n");
            road2.car_speed=(int)(1000/counter);
            printf("speed:%d\r\n",road2.car_speed);
            road2.is_report=1;
            TaskMsleep(200);
            }
        }
    }
}
static void Speed_3(void)
{
    while(1)
    {
    if (!capture3_1())
    {
        int counter=0;
        while(capture3_2() && counter<2000)
        {
            counter++;
            TaskMsleep(1);
        }
        if(counter<2000)
        {
            printf("car drives in road3\r\n");
            road3.car_speed=(int)(100000/counter);
            printf("speed:%d\r\n",road3.car_speed);
            road3.is_report=1;
            TaskMsleep(200);
        }
        }
    
   if(!capture3_2())
    {
            int counter=0;
            while(capture3_1() && counter<2000)
            {
                counter++;
                TaskMsleep(1);
            }
            if(counter<2000)
            {
            printf("car drives in road3\r\n");
            road3.car_speed=(int)(1000/counter);
            printf("speed:%d\r\n",road3.car_speed);
            road3.is_report=1;
            TaskMsleep(200);
            }
        }
    }
}

static void Traffic_light_1(void)
{  
    // IoTGpioSetOutputVal(RED_1,IOT_GPIO_VALUE1);
    // IoTGpioSetOutputVal(GREEN_1,IOT_GPIO_VALUE0);
    // IoTGpioSetOutputVal(YELLOW_1,IOT_GPIO_VALUE0); 
    while (1) {
        IoTGpioSetOutputVal(CLK,IOT_GPIO_VALUE1);
            while(is_emergency)
        {
            printf("emergency!!!!!!!\r\n");
            // IoTGpioSetOutputVal(RED_1,IOT_GPIO_VALUE1);
            // IoTGpioSetOutputVal(GREEN_1,IOT_GPIO_VALUE0);
            // IoTGpioSetOutputVal(YELLOW_1,IOT_GPIO_VALUE0);
        };
    if (strcmp(traffic1.current_state, "RED") == 0 && traffic1.current_time == 1) {
  
        traffic1.current_state= "GREEN";
        traffic1.current_time = traffic1.green_time;
        // IoTGpioSetOutputVal(RED_1,IOT_GPIO_VALUE0);
        // IoTGpioSetOutputVal(GREEN_1,IOT_GPIO_VALUE1);
        // IoTGpioSetOutputVal(YELLOW_1,IOT_GPIO_VALUE0);
        
    } else if (strcmp(traffic1.current_state, "GREEN") == 0 && traffic1.current_time == 1) {
        traffic1.current_state="YELLOW";
        traffic1.current_time = 3;
        // IoTGpioSetOutputVal(RED_1,IOT_GPIO_VALUE0);
        // IoTGpioSetOutputVal(GREEN_1,IOT_GPIO_VALUE0);
        // IoTGpioSetOutputVal(YELLOW_1,IOT_GPIO_VALUE1);
       

    } else if (strcmp(traffic1.current_state, "YELLOW") == 0 && traffic1.current_time == 1) {
        traffic1.current_state="RED";
        traffic1.current_time = traffic1.red_time - 3;
        // IoTGpioSetOutputVal(RED_1,IOT_GPIO_VALUE1);
        // IoTGpioSetOutputVal(GREEN_1,IOT_GPIO_VALUE0);
        // IoTGpioSetOutputVal(YELLOW_1,IOT_GPIO_VALUE0);
       
    } else {
        traffic1.current_time -= 1; 
    }
    TaskMsleep(500);
    IoTGpioSetOutputVal(CLK,IOT_GPIO_VALUE0);
    TaskMsleep(500);
    }
}

static void Traffic_light_2(void)
{  
    IoTGpioSetOutputVal(RED_2,IOT_GPIO_VALUE1);
    IoTGpioSetOutputVal(GREEN_2,IOT_GPIO_VALUE0);
    IoTGpioSetOutputVal(YELLOW_2,IOT_GPIO_VALUE0); 
    int i=70;
    while (i) {
            while(is_emergency)
    {
        IoTGpioSetOutputVal(RED_2,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(GREEN_2,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_2,IOT_GPIO_VALUE0);
    };
    if (strcmp(traffic2.current_state, "RED") == 0 && traffic2.current_time == 1) {
        traffic2.current_state="GREEN";
        traffic2.current_time = traffic2.green_time;
        IoTGpioSetOutputVal(RED_2,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(GREEN_2,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(YELLOW_2,IOT_GPIO_VALUE0);
    } else if (strcmp(traffic2.current_state, "GREEN") == 0 && traffic2.current_time == 1) {
        traffic2.current_state="YELLOW";
        traffic2.current_time = 3;
        IoTGpioSetOutputVal(RED_2,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(GREEN_2,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_2,IOT_GPIO_VALUE1);
    } else if (strcmp(traffic2.current_state, "YELLOW") == 0 && traffic2.current_time ==1) {
        traffic2.current_state="RED";
        traffic2.current_time = traffic2.red_time - 3;
        IoTGpioSetOutputVal(RED_2,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(GREEN_2,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_2,IOT_GPIO_VALUE0);
    } else {
        traffic2.current_time -= 1;
             if (strcmp(traffic2.current_state, "RED") == 0)
        {
        IoTGpioSetOutputVal(RED_2,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(GREEN_2,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_2,IOT_GPIO_VALUE0);

        }
        if (strcmp(traffic2.current_state, "GREEN") == 0)
        {
        IoTGpioSetOutputVal(RED_2,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(GREEN_2,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(YELLOW_2,IOT_GPIO_VALUE0);
        }
        if (strcmp(traffic2.current_state, "YELLOW") == 0)
        {
        IoTGpioSetOutputVal(RED_2,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(GREEN_2,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_2,IOT_GPIO_VALUE1);
        }
    }
    TaskMsleep(Time);
    i--;
    if(i==0)
    {
        i=70;
        if (is_congestion)
        {
            traffic2.red_time=60;
            traffic2.green_time=10;
        }
        else{
            traffic2.red_time=50;
            traffic2.green_time=20;
        }
    }
    }
}


static void Traffic_light_3(void)
{  
    IoTGpioSetOutputVal(RED_3,IOT_GPIO_VALUE0);
    IoTGpioSetOutputVal(GREEN_3,IOT_GPIO_VALUE1);
    IoTGpioSetOutputVal(YELLOW_3,IOT_GPIO_VALUE0); 
    while (1) {
            while(is_emergency)
    {
        IoTGpioSetOutputVal(RED_3,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(GREEN_3,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_3,IOT_GPIO_VALUE0);
    };
    
    if (strcmp(traffic3.current_state, "RED") == 0 && traffic3.current_time == 1) {
        traffic3.current_state="GREEN";
        traffic3.current_time = traffic3.green_time;
        IoTGpioSetOutputVal(RED_3,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(GREEN_3,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(YELLOW_3,IOT_GPIO_VALUE0);

    } else if (strcmp(traffic3.current_state, "GREEN") == 0 && traffic3.current_time == 1) {
        traffic3.current_state="YELLOW";
        traffic3.current_time = 3;
        IoTGpioSetOutputVal(RED_3,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(GREEN_3,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_3,IOT_GPIO_VALUE1);

    } else if (strcmp(traffic3.current_state, "YELLOW") == 0 && traffic3.current_time == 1) {
        traffic3.current_state="RED";
        traffic3.current_time = traffic3.red_time - 3;
        IoTGpioSetOutputVal(RED_3,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(GREEN_3,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_3,IOT_GPIO_VALUE0);
    } else {
        traffic3.current_time -= 1;
                if (strcmp(traffic3.current_state, "RED") == 0)
        {
 IoTGpioSetOutputVal(RED_3,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(GREEN_3,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_3,IOT_GPIO_VALUE0);

        }
        if (strcmp(traffic3.current_state, "GREEN") == 0)
        {
IoTGpioSetOutputVal(RED_3,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(GREEN_3,IOT_GPIO_VALUE1);
        IoTGpioSetOutputVal(YELLOW_3,IOT_GPIO_VALUE0);
        }
        if (strcmp(traffic3.current_state, "YELLOW") == 0)
        {

        IoTGpioSetOutputVal(RED_3,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(GREEN_3,IOT_GPIO_VALUE0);
        IoTGpioSetOutputVal(YELLOW_3,IOT_GPIO_VALUE1);
        }
    }
    TaskMsleep(Time);
    }
}

static void Traffic_light_4(void)
{
    // IoTGpioSetOutputVal(RED_4,IOT_GPIO_VALUE0);
    // IoTGpioSetOutputVal(GREEN_4,IOT_GPIO_VALUE0);
    // IoTGpioSetOutputVal(YELLOW_4,IOT_GPIO_VALUE1); 
    int i=70;
    while (i) {    
        while(is_emergency)
    {
        printf("emergency!!!!!!\r\n");
        // IoTGpioSetOutputVal(RED_4,IOT_GPIO_VALUE1);
        // IoTGpioSetOutputVal(GREEN_4,IOT_GPIO_VALUE0);
        // IoTGpioSetOutputVal(YELLOW_4,IOT_GPIO_VALUE0);
    };
    if (strcmp(traffic4.current_state, "RED") == 0 && traffic4.current_time == 1) {
        traffic4.current_state="GREEN";
        traffic4.current_time = traffic4.green_time;
        // IoTGpioSetOutputVal(RED_4,IOT_GPIO_VALUE0);
        // IoTGpioSetOutputVal(GREEN_4,IOT_GPIO_VALUE1);
        // IoTGpioSetOutputVal(YELLOW_4,IOT_GPIO_VALUE0);

    } else if (strcmp(traffic4.current_state, "GREEN") == 0 && traffic4.current_time == 1) {
        traffic4.current_state="YELLOW";
        traffic4.current_time = yellow_time;
        // IoTGpioSetOutputVal(RED_4,IOT_GPIO_VALUE0);
        // IoTGpioSetOutputVal(GREEN_4,IOT_GPIO_VALUE0);
        // IoTGpioSetOutputVal(YELLOW_4,IOT_GPIO_VALUE1);

    } else if (strcmp(traffic4.current_state, "YELLOW") == 0 && traffic4.current_time == 1) {
        traffic4.current_state="RED";
        traffic4.current_time = traffic4.red_time-3;
        // IoTGpioSetOutputVal(RED_4,IOT_GPIO_VALUE1);
        // IoTGpioSetOutputVal(GREEN_4,IOT_GPIO_VALUE0);
        // IoTGpioSetOutputVal(YELLOW_4,IOT_GPIO_VALUE0);
    } else {
        traffic4.current_time -= 1;
    }
    i--;
    if(i==0)
    {
        i=70;
        if (is_congestion)
        {
            traffic4.red_time=40;
            traffic4.green_time=30;
        }
        else{
            traffic4.red_time=50;
            traffic4.green_time=20;
        }
    }
    TaskMsleep(Time);
    }
}

static void Report(void)
{
    while(1)
    {
        IotPublish();
        TaskMsleep(800);
    }
}

void Init(void)
{
    IoTGpioInit(speed1_1);
    IoSetFunc(speed1_1,IOT_IO_FUNC_GPIO_1_GPIO);
    IoTGpioSetDir(speed1_1, IOT_GPIO_DIR_IN);
    hi_io_set_pull(speed1_1, IOT_IO_PULL_UP);

    IoTGpioInit(speed1_2);
    IoSetFunc(speed1_2, IOT_IO_FUNC_GPIO_2_GPIO);
    IoTGpioSetDir(speed1_2, IOT_GPIO_DIR_IN);
    hi_io_set_pull(speed1_2, IOT_IO_PULL_UP);

    IoTGpioInit(speed2_1);
    IoSetFunc(speed2_1, IOT_IO_FUNC_GPIO_6_GPIO);
    IoTGpioSetDir(speed2_1, IOT_GPIO_DIR_IN);
    hi_io_set_pull(speed2_1, IOT_IO_PULL_UP);

    IoTGpioInit(speed2_2);
    IoSetFunc(speed2_2, IOT_IO_FUNC_GPIO_7_GPIO);
    IoTGpioSetDir(speed2_2, IOT_GPIO_DIR_IN);
    hi_io_set_pull(speed2_2, IOT_IO_PULL_UP);

    IoTGpioInit(speed3_1);
    IoSetFunc(speed3_1, IOT_IO_FUNC_GPIO_8_GPIO);
    IoTGpioSetDir(speed3_1, IOT_GPIO_DIR_IN);
    hi_io_set_pull(speed3_1, IOT_IO_PULL_UP);

    IoTGpioInit(speed3_2);
    IoSetFunc(speed3_2, IOT_IO_FUNC_GPIO_9_GPIO);
    IoTGpioSetDir(speed3_2, IOT_GPIO_DIR_IN);
    hi_io_set_pull(speed3_2, IOT_IO_PULL_UP);

    
    IoTGpioInit(CLK);
    IoSetFunc(CLK,IOT_IO_FUNC_GPIO_0_GPIO);
    IoTGpioSetDir(CLK, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(CLK,IOT_GPIO_VALUE1);

    IoTGpioInit(congestion);
    IoSetFunc(congestion,IOT_IO_FUNC_GPIO_13_GPIO);
    IoTGpioSetDir(congestion, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(congestion,IOT_GPIO_VALUE1);

    IoTGpioInit(emergency);
    // 设置GPIO9的管脚复用关系为GPIO
    IoSetFunc  (emergency,IOT_IO_FUNC_GPIO_14_GPIO);
    // GPIO方向设置为输出
    IoTGpioSetDir(emergency, IOT_GPIO_DIR_OUT);

    IoTGpioSetOutputVal(emergency,IOT_GPIO_VALUE1);    

    IoTGpioInit(RED_3);
    // 设置GPIO9的管脚复用关系为GPIO
    IoSetFunc  (RED_3, IOT_IO_FUNC_GPIO_10_GPIO);
    // GPIO方向设置为输出

    IoTGpioSetDir(RED_3, IOT_GPIO_DIR_OUT);
    IoTGpioInit(GREEN_3);
    // 设置GPIO9的管脚复用关系为GPIO
    IoSetFunc  (GREEN_3, IOT_IO_FUNC_GPIO_11_GPIO);
    // GPIO方向设置为输出
    IoTGpioSetDir(GREEN_3, IOT_GPIO_DIR_OUT);

    IoTGpioInit(YELLOW_3);
    // 设置GPIO9的管脚复用关系为GPIO
    IoSetFunc  (YELLOW_3, IOT_IO_FUNC_GPIO_12_GPIO);
    // GPIO方向设置为输出
    IoTGpioSetDir(YELLOW_3, IOT_GPIO_DIR_OUT);

    IoTGpioInit(RED_2);
    // 设置GPIO9的管脚复用关系为GPIO
    IoSetFunc  (RED_2, IOT_IO_FUNC_GPIO_3_GPIO);
    // GPIO方向设置为输出
    IoTGpioSetDir(RED_2, IOT_GPIO_DIR_OUT);

    IoTGpioInit(GREEN_2);
    // 设置GPIO9的管脚复用关系为GPIO
    IoSetFunc  (GREEN_2, IOT_IO_FUNC_GPIO_4_GPIO);
    // GPIO方向设置为输出
    IoTGpioSetDir(GREEN_2, IOT_GPIO_DIR_OUT);

    IoTGpioInit(YELLOW_2);
    // 设置GPIO9的管脚复用关系为GPIO
    IoSetFunc  (YELLOW_2, IOT_IO_FUNC_GPIO_5_GPIO);
    // GPIO方向设置为输出
    IoTGpioSetDir(YELLOW_2, IOT_GPIO_DIR_OUT);
}
#define CN_IOT_TASK_STACKSIZE  0x2000
#define CN_IOT_TASK_PRIOR 24
#define CN_IOT_TASK_NAME "IOT_SMART_TRAFFIC"
static void Iot(void)
{
    ConnectToHotspot(); 
    CJsonInit();
    IoTMain();
    IoTSetMsgCallback(TimeMsgRcvCallBack);
    TaskMsleep(37000); 
    Init();
    osThreadAttr_t attr;
    IoTWatchDogDisable();

    attr.name = CN_IOT_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;

    if (osThreadNew((osThreadFunc_t)Report, NULL, &attr) == NULL) {
        printf("IOT_SMART_TRAFFIC Failed to create Report thread!\n");
    }
    IoTWatchDogDisable();
    if (osThreadNew((osThreadFunc_t)Speed_1, NULL, &attr) == NULL) {
        printf("IOT_SMART_TRAFFIC Failed to create Speed_1 thread!\n");
    }
    IoTWatchDogDisable();
    if (osThreadNew((osThreadFunc_t)Speed_2, NULL, &attr) == NULL) {
        printf("IOT_SMART_TRAFFIC Failed to create Speed_2 thread!\n");
    }
    IoTWatchDogDisable();
    if (osThreadNew((osThreadFunc_t)Speed_3, NULL, &attr) == NULL) {
        printf("IOT_SMART_TRAFFIC Failed to create Speed_3 thread!\n");
    }
    IoTWatchDogDisable();
    if (osThreadNew((osThreadFunc_t)Traffic_light_1,NULL, &attr) == NULL) {
        printf("IOT_SMART_TRAFFIC Failed to create Traffic_light_1 thread!\n");
    }
    IoTWatchDogDisable();
    attr.priority = CN_IOT_TASK_PRIOR;
    if (osThreadNew((osThreadFunc_t)Traffic_light_2,NULL, &attr) == NULL) {
        printf("IOT_SMART_TRAFFIC Failed to create Traffic_light_2 thread!\n");
    }
    IoTWatchDogDisable();
    attr.priority = CN_IOT_TASK_PRIOR;
    if (osThreadNew((osThreadFunc_t)Traffic_light_3,NULL, &attr) == NULL) {
        printf("IOT_SMART_TRAFFIC Failed to create Traffic_light_3 thread!\n");
    }
    IoTWatchDogDisable();
    if (osThreadNew((osThreadFunc_t)Traffic_light_4,NULL, &attr) == NULL) {
        printf("IOT_SMART_TRAFFIC Failed to create Traffic_light_4 thread!\n");
    }
}
SYS_RUN(Iot);

