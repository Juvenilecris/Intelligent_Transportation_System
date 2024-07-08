## Hi3861V100部分的代码说明
```
该部分工程是再海思提供的/verder/Hisilion/demo/oc_demo例程基础上进行开发的
其中"cjson_init.c",
    "hal_iot_gpio_ex.c",
    "iot_main.c",
    "iot_profile.c",
    "wifi_connecter.c"等源码的大部分都来源于海思的例程，仅作少许的修改。
    其相关内容大多是网络连接配置、云端连接配置等

    而"app_iot.c"文件是团队开发的主要源码，里面涉及团队作品在Hi3861V100端的主要逻辑
    其中的函数也都是调用openharmony接口或者海思sdk的接口所开发的
```