# 基于交通堵塞预测的智慧交通系统<a name="ZH-CN_TOPIC_0000001130176841"></a>
2024年嵌入式芯片与系统设计大赛应用赛道作品
## 系统结构和功能
```
1)	道路情况的实时监控: 用户可通过微信小程序实时获取道路状态信息，包括道路红绿灯状态、车辆速度、实时车流量以及系统预测的拥堵情况，随时了解路况以提前调整出行计划。
2)	交通拥堵预测: 系统综合总车流量、平均车速等交通数据，通过机器学习算法，精准预测路段的交通拥堵情况，为交通信号控制提供依据。
3)	动态交通信号: 根据实时交通状况对道路拥挤的预测结果，系统自动调整各路口的红绿灯时长，以优化交通流畅度，减少拥堵发生率。
4)	应急状况处理: 当出现救护车、消防车等应急车辆执行任务时，相关管理人员在微信小程序上可以启动道路应急状态，此时系统会自动触发全路口红灯设置，以确保应急车辆的交通通畅，并保障了行人安全，等待应急状态结束后道路自动恢复正常的交通信号。
5)	模拟拥堵功能: 为了模拟系统对道路拥挤的预测，系统设置一键模拟道路拥挤的功能：系统随机生成车辆行驶信息，并根据模拟数据对道路拥堵情况进行预测，帮助验证系统的可靠性。

![系统结构图]()
```



## 硬件端
-  硬件要求：Hi3861V100核心板，51单片机最小系统，YL-62避障传感器，低电平触发蜂鸣器，红绿灯模块，74HC595驱动的二位共阴极数码管模块；硬件搭建效果图及组成结构图如下图所示。
![软件系统总体框图]()
-   Hi3861V100核心板
![Hi3861V100为核心板的智能家居套件]()
-  YL-62避障传感器
![YL-62避障传感器]()

- 软件系统总体框图
![软件系统总体框图]()
- 红绿灯模块
![红绿灯模块]()
- 74HC595驱动的二位共阴极数码管模块
![74HC595驱动的二位共阴极数码管模块]()

### 道路模型
- 道路模型简绘图
![道路模型简绘图]()

## 软件端
- 软件系统介绍
```
系统使用华为云Iot基础版实例作为云端，微信小程序作为应用端，Python端作为辅助设备端。由硬件开发板将交通数据上报给云端。软件系统的总体框图如下所示。
```
![软件系统总体框图]()
- 设备端
```
设备端实现交通信号的变化与车辆速度、流量数据的采集，并上报给云端。同时会响应来自平台的控制命令。
```
- 微信小程序
```
微信小程序作为应用端，负责将云端接收到的实时交通数据进行显示，并将车辆行驶的数据发送给Python端，以及使平台向设备发送控制命令。
```
![微信小程序页面效果图]()
- Python端
```
python端作为辅助设备端，将应用端发送的车辆行驶数据进行处理，并使用机器学习算法判断道路是否处于拥挤状态，再将状态上报给云端，以及负责在模拟拥堵功能时随机生成车辆信息，并将车辆信息上报云端。
```

## 代码框架
-  HI3861V100端
```
Hi3861V100/src/iot_smart_traffic
├── app_iot.c                #系统在hi38861端的主程序
├── app_iot.h  
├── BUILD.gn                 # BUILD.gn文件由三部分内容,static_library中指定业务模块的编译结果,sources中指定静态库.a所依赖的.c文件 include_dirs中指定source所需要依赖的.h文件。
├── cjson_init.c             # json格式的数据配置
├── cjson_init.h          
├── hal_iot_gpio_ex.c        # 引脚的功能等配置文件
├── iot_config.h             # 连接华为云的配置头文件（需要修改wifi名称密码，和华为云id信息）
├── iot_gpio_ex.h            # 引脚配置文件 
├── iot_main.c               # 华为云连接
├── iot_main.h               # 
├── iot_profile.c            # 基于mqtt协议与华为云通信的接口 
├── iot_profile.h            # 
├── wifi_connecter.c         # 连接wifi
└── wifi_connecter.h         # 
*说明：该目录应放在海思开发代码src\applications\sample\wifi-iot\app目录下使用
```
-  51单片机端
```
51/src
├── host_forward.c           #主道直行的交通信号的逻辑程序              
└── guest_left.c             #客道左转的交通信号的逻辑程序
*说明：51单片机负责主道直行和客道左转的交通信号
```
- 微信小程序
```
WeChat_Mini_Program/src
├── index.js               #实现微信小程序交互逻辑和页面的业务逻辑 
├── index.wxml             #构建微信小程序的页面
├── index.wxss             #定义小程序的样式和布局结构 
```
- Python端
```
Python/src
├── iot_smart_traffic.py         #python端工作的主程序 
├── congestion_svm.py            #在交通数据集下训练svm分类器的程序
Python/traffic_data.txt          #合成的交通数据集
```

## 参考资料

```
[1] 例程：src\verder\hisilion\demo\oc_demo
[2] 例程：src\vendor\hihope\hispark_pegasus\demo\28_easy_wifi
[3] 手把手讲解华为云物联网云平台的使用以及应用侧的开发(2024最新版) https://www.bilibili.com/video/BV1mr421c75S/?buvid=YD4B0ED2DBD3009A4B40967518BE584E6CA4&from_spmid=united.player-video-detail.0.0&is_story_h5=false&mid=469yEk4XSWQhIO0lSwz0CA%3D%3D&p=1&plat_id=122&share_from=ugc&share_medium=iphone&share_plat=ios&share_session_id=AB43F94E-2D06-4D0C-BEBF-7DD496DCDAB0&share_source=WEIXIN&share_tag=s_i&spmid=united.player-video-detail.0.0&timestamp=1719330456&unique_k=JOavrkm&up_id=68130189
[4] 华为云设备接入Iot帮助文档 https://support.huaweicloud.com/usermanual-iothub/iot_01_0015.html
[5] STM32项目复刻制作流程讲解、常见问题解决【合集】【实时更新】https://blog.csdn.net/xiaolong1126626497/article/details/134781708
[6] 张飞舟,杨东凯.《物联网应用与解决方案》(第2版).电子工业出版社.
[7]	连志安.《物联网嵌入式开发实战》.清华大学出版社.
[8]	齐耀龙.《OpenHarmony轻量设备开发理论与实践》.江苏润开鸿数字科技优先公司.
[9]	郗华.基于51单片机开发板的项目式教学设计[J].电子技术,2024,53(03):150-151.2 期刊.
[10] 陈智.基于STM32的智能循迹避障系统设计[J].无线互联科技,2024,21(08):8-11.

```