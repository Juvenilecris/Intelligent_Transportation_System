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


#ifndef APP_IOT_H_
#define APP_IOT_H_
#include "app_iot.h"

typedef struct {
    int red_time;
    int green_time;
    int current_time;
    char* current_state;
}Traffic;

typedef struct {
    char* name;
    int car_speed;
    int is_report;
}Road;

void Init(void);

#endif