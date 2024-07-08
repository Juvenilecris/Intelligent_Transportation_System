# -*- encoding: utf-8 -*-
import random
import logging
import time
import json
import threading

import joblib

from IoT_device.client.IoT_client_config import IoTClientConfig
from IoT_device.client.IoT_client import IotClient
from IoT_device.request.services_properties import ServicesProperties
import numpy as np
from sklearn import svm
from sklearn.model_selection import train_test_split
"""
系统的Python端运作程序，Python端作为辅助设备端，将应用端发送的车辆行驶数据进行处理，并使用机器学习算法判断道路是否处于拥挤状态，再将状态上报给云端，
以及负责在模拟拥堵功能时随机生成车辆信息，并将车辆信息上报云端。
"""
# 日志设置
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)
#路类，系统共有主道一个（road1）、客道两个（road2，road3），每一个道路都设置一个速度检测点
class Road:
    def __init__(self,name):
        self.aver_speed = 0             #道路的平均速度
        self.aver_speed_last_hour = 0   #道路的最近一个小时的平均速度
        self.last_speed = 0             #道路上一辆车的平均速度
        self.flow_last_hour = 0         #道路最近一个小时的车流量
        self.flow = 0                   #道路的总车流量
        self.speed = []                 #道路经过所有车辆的车速
        self.time = []                  #道路经过所有车的时间
        self.name=name

    def report(self, service_property, index):
        #---------------------------------上报道路的交通数据------------------------------------
        service_property.add_service_property(service_id="Speed", property=f'Average_speed{index}', value=self.aver_speed)
        service_property.add_service_property(service_id="Speed", property=f'Last_Speed{index}', value=self.last_speed)
        service_property.add_service_property(service_id="Speed", property=f'Average_speed_last_hour{index}',value=self.aver_speed_last_hour)
        service_property.add_service_property(service_id="Speed", property=f'Flow{index}', value=self.flow)
        service_property.add_service_property(service_id="Speed", property=f'Flow_last_hour{index}', value=self.flow_last_hour)
#----------------------------Iot类------------------------
class Iot:
    def __init__(self):
        self.road1=Road('road1')
        self.road2=Road('road2')
        self.road3=Road('road3')
        self.congestion = False
        self.is_simulate_congestion=False
        self.pause_event = threading.Event()
        self.high_command=False
        self.congestion_model= joblib.load(r'E:\2024QRS\quickStart(python)\mqttdemo(python)\congestion_model.pkl')
        #-----------云端的mqtt连接信息----------
        client_cfg = IoTClientConfig(server_ip='a1c597e421.iot-mqtts.cn-north-4.myhuaweicloud.com',
                                     device_id='667e49152ca97925e0661c41_test',
                                     secret='sxsj10470', is_ssl=False)
#--------------------各个道路模拟拥堵情况的进程-----------
        self.thread_road2 = None
        self.thread_road1 = None

        self.thread_road3 = None
        self.thread_report = None
        #----属性上报服务----
        self.service_property = None

        # 创建设备
        self.iot_client = IotClient(client_cfg)
        self.iot_client.connect()  # 建立连接

        # 设备接受平台下发消息的响应
        def message_callback(device_message):
            logger.info(('device message, device id:  ', device_message.device_id))
            logger.info(('device message, id = ', device_message.id))
            logger.info(('device message, name: ', device_message.name))
            logger.info(('device message. content: ', device_message.content))
            #---------平台发送行驶车辆的车速信息-------------
            if device_message.content[:5]=='road1' and device_message.content[16:]!='0':#如果是道路1的车辆
                new_speed = int(device_message.content[16:])
                self.add_new_car(self.road1,new_speed)                                  #此时在交通数据库添加该车辆的信息
            if device_message.content[:5]=='road2' and device_message.content[16:]!='0':
                new_speed = int(device_message.content[16:])
                self.add_new_car(self.road2,new_speed)
            if device_message.content[:5]=='road3' and device_message.content[16:]!='0':
                new_speed = int(device_message.content[16:])
                self.add_new_car(self.road3,new_speed)

        # 响应平台下发命令
        def command_callback(request_id, command):
            logger.info(('Command, device id:  ', command.device_id))
            logger.info(('Command, service id = ', command.service_id))
            logger.info(('Command, command name: ', command.command_name))
            logger.info(('Command. paras: ', command.paras))
            # result_code:设置为零相应命令下发成功，为 1 下发命令失败
            self.iot_client.respond_command(request_id, result_code=0)
            #-------------平台下发模拟拥堵的命令------------------
            if command.service_id=='Simulate' and command.command_name=='SimulateCongestion':
                if command.paras['congestion']=='true':
                    print("---------------开始模拟---------------")
                    self.is_simulate_congestion = True  #各个道路模拟拥堵状况的进程被打开
                elif command.paras['congestion']=='false':
                    print("---------------模拟结束---------------")
                    self.is_simulate_congestion=False   #各个道路模拟拥堵状况的进程被关闭
            print('--------------------已响应命令---------------------')

        # 设置平台下发消息响应的回调
        self.iot_client.set_device_message_callback(message_callback)
        # 设置响应命令的回调
        self.iot_client.set_command_callback(command_callback)
        # 设置平台设置设备属性的回调

#-------------添加平台下发车辆行驶信息到交通数据库-----------
    def add_new_car(self,road,new_speed):
        current_time = time.time()#记录当前车辆行驶的时间
        road.time.append(current_time)
        road.speed.append(new_speed)
        road.flow += 1
        road.aver_speed = int(sum(road.speed) / road.flow) if road.flow else 0
        # 更新最近一辆车的车速
        road.last_speed = new_speed
        self.iot_client.publish_message(f"car drives speed:{new_speed}km/h in {road.name}")
        # 计算最近一个小时内的平均车速和车流量
        one_hour_ago = current_time - 600                               #为了更好地演示模型地功能，此时并不是真正统计最近一个小时，而是统计最近十分钟内的交通数据
        recent_speeds = [speed for speed, time_stamp in zip(road.speed, road.time) if
                         time_stamp >= one_hour_ago]
        road.aver_speed_last_hour = int(sum(recent_speeds) / len(recent_speeds)) if recent_speeds else 0
        road.flow_last_hour = sum(1 for time_stamp in road.time if time_stamp >= one_hour_ago)
        if self.is_congestion() == 1:   #判断当前道路是否处于拥堵
            self.congestion = True      #拥堵标志置为真
        else:
            self.congestion=False
#-----------------------------------属性上报进程--------------------
    def report_properties(self):
        while True:
            print('--------------report---------------')
            # 按照产品模型设置属性
            self.service_property = ServicesProperties()#建立属性上报的服务
            self.road1.report(self.service_property,1)#上报主道的交通数据
            self.road2.report(self.service_property,2)#上报客道的交通数据
            self.road3.report(self.service_property,3)

            self.service_property.add_service_property(service_id="Alarm", property='Is_congestion',value=self.congestion)#上报当前是否处于拥堵
            self.iot_client.report_properties(service_properties=self.service_property.service_property, qos=1)
            time.sleep(1)#每1s上报1次

#---------------------------------模拟拥堵情况--------------------------
    def update_road(self,road):
        while True:
            while self.is_simulate_congestion:#此时开启了模拟拥堵状态的功能
                time_interval = random.randint(3, 8)  #每3-8s行驶一辆车
                time.sleep(time_interval)
                new_speed = random.randint(10, 30) #车的速度范围在10km/h-30km/h
                #----------------------将模拟生成的车辆信息添加到交通数据库里-----------------
                current_time = time.time()
                road.time.append(current_time)
                road.speed.append(new_speed)
                road.flow += 1
                road.aver_speed = int(sum(road.speed)/road.flow) if road.flow else 0
                # 更新最近一辆车的车速
                road.last_speed = new_speed
                print(f"{road}一辆车经过,速度: {new_speed} km/h")
                self.iot_client.publish_message(f"car drives speed:{new_speed}km/h in {road.name}")
                one_hour_ago = current_time-600
                recent_speeds = [speed for speed, time_stamp in zip(road.speed, road.time) if time_stamp >= one_hour_ago]
                road.aver_speed_last_hour = int(sum(recent_speeds) / len(recent_speeds)) if recent_speeds else 0
                road.flow_last_hour = sum(1 for time_stamp in road.time if time_stamp >= one_hour_ago)
                if self.is_congestion()==1:
                    self.congestion=True
                else:
                    self.congestion=False
#----------------根据SVM分类器预测当前道路是否处于道路拥挤状态-----------------
    def is_congestion(self):
        #---------加载当前的交通数据----------
        traffic_data = np.array([[self.road1.aver_speed, self.road1.aver_speed_last_hour,self.road1.last_speed,self.road1.flow, self.road1.flow_last_hour,self.road2.aver_speed, self.road2.aver_speed_last_hour,self.road2.last_speed,self.road2.flow, self.road2.flow_last_hour,self.road3.aver_speed, self.road3.aver_speed_last_hour,self.road3.last_speed,self.road3.flow, self.road3.flow_last_hour]])  # 输入你的15个特征值
        #------预测道路是否处于拥堵状态-----------
        predictions = self.congestion_model.predict(traffic_data)
        return int(predictions.item())
#----------------------------python端运行程序-------------------------
    def run(self):
        self.iot_client.start()                         # mqtt连接华为云线程启动
        self.thread_report = threading.Thread(target=self.report_properties)
        self.thread_report.start()                      #属性上报线程启动
        self.thread_road1 = threading.Thread(target=lambda: self.update_road(self.road1))
        self.thread_road2 = threading.Thread(target=lambda: self.update_road(self.road2))
        self.thread_road3 = threading.Thread(target=lambda: self.update_road(self.road3))

        self.thread_road1.start()                       #道路1模拟拥堵状态线程启动（但实际运作需等待模拟拥堵的命令）
        self.thread_road2.start()                       #道路2模拟拥堵状态线程启动（但实际运作需等待模拟拥堵的命令）
        self.thread_road3.start()                       #道路3模拟拥堵状态线程启动（但实际运作需等待模拟拥堵的命令）

        self.thread_road1.join()
        self.thread_road2.join()
        self.thread_road3.join()

        self.thread_report.join()
        
if __name__ == '__main__':
    iot=Iot()
    iot.run()

