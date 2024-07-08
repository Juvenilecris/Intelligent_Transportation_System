// pages/ceshi/ceshi.js
const App = getApp();
Page({

  /**
   * 页面的初始数据
   */
  data: {
      TrafficLight: null,
      Speed: null,
      Alarm: null,
  },
  /* 获取华为云设备的token */
  gettoken:function()
  {
    console.log("开始获取token...");
    var that = this;
    wx.request({
      url:'https://iam.cn-north-4.myhuaweicloud.com/v3/auth/tokens',
      data:'{"auth": { "identity": {"methods": ["password"],"password": {"user": {"name": "juven","password": "sxsj104701","domain": {"name": "juvenlucky20210229"}}}},"scope": {"project": {"name": "cn-north-4"}}}}',
      method: 'POST',
      header: {'content-type': 'application/json' },  // 请求的 header 
      success: function(res){
          console.log("获取token成功");
          wx.showToast({title:'获取token成功',duration: 700});

          var token='';
          token=JSON.stringify(res.header['X-Subject-Token']);  //解析消息头token
          token=token.replaceAll("\"", "");
          wx.setStorageSync('token',token);   //把token写到缓存中,以便可以随时随地调用
      },
      fail: function () {
        console.log("获取token失败");
      }
    })
  },
  /* 获取设备影子（属性） */
  getshadow:function()
  {
  // console.log("开始获取影子");
      var that = this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token = wx.getStorageSync('token');   //读缓存中保存的token
      //console.log("我的toekn:\n"+token);    //打印完整消息
      wx.request({
          url: 'https://iotda.cn-north-4.myhuaweicloud.com/v5/iot/ccde213a96c64620992b48735c58cf1b/devices/667e49152ca97925e0661c41_test/shadow',
          data:'',
          method: 'GET',
          header: {'content-type': 'application/json', 'X-Auth-Token':token }, 
          success: function(res){
              // console.log(res);//打印完整消息
              that.setData({TrafficLight: res.data.shadow[2].reported.properties});
              that.setData({Speed: res.data.shadow[1].reported.properties});
              that.setData({Alarm: res.data.shadow[3].reported.properties});
          },
          fail:function(){
              console.log("获取影子失败");
          }
      });
  },
  /* 向设备发送紧急状况的命令 */
  setCommand_emergency:function(){
    var that = this; 
    var token = wx.getStorageSync('token'); 
    wx.request({
        url: 'https://iotda.cn-north-4.myhuaweicloud.com/v5/iot/ccde213a96c64620992b48735c58cf1b/devices/667e49152ca97925e0661c41_test/commands',
        data:'{"service_id": "Alarm","command_name": "EmergencyModule","paras": { "emergency": "true"}}',
        method: 'POST', 
        header: {'content-type': 'application/json','X-Auth-Token':token },
        success: function(res){
            console.log("道路切换为紧急状态");
        },
        fail:function(){
            // fail
            console.log("命令下发失败");
        },
    });
  },
  /* 向设备发送解除紧急的命令 */
  setCommand_not_emergency:function(){
    var that = this; 
    var token = wx.getStorageSync('token');
    wx.request({
        url: 'https://iotda.cn-north-4.myhuaweicloud.com/v5/iot/ccde213a96c64620992b48735c58cf1b/devices/667e49152ca97925e0661c41_test/commands',
        data:'{"service_id": "Alarm","command_name": "EmergencyModule","paras": { "emergency": "false"}}',
        method: 'POST',
        header: {'content-type': 'application/json','X-Auth-Token':token },
        success: function(res){
            console.log("道路应急状态解除");
        },
        fail:function(){
            console.log("命令下发失败");
        },
    });
  },
  /* 向设备发送模拟拥挤的命令 */
  setCommand_congestion:function(){
    var that = this;
    var token = wx.getStorageSync('token');
    wx.request({
        url: 'https://iotda.cn-north-4.myhuaweicloud.com/v5/iot/ccde213a96c64620992b48735c58cf1b/devices/667e49152ca97925e0661c41_test/commands',
        data:'{"service_id": "Simulate","command_name": "SimulateCongestion","paras": { "congestion": "true"}}',
        method: 'POST',
        header: {'content-type': 'application/json','X-Auth-Token':token },
        success: function(res){
            console.log("开始模拟拥挤");
        },
        fail:function(){
            console.log("命令下发失败");
        },
    });
  },
  /* 向设备发送模拟拥挤结束的命令 */
  setCommand_not_congestion:function(){
    var that = this;
    var token = wx.getStorageSync('token');
    wx.request({
        url: 'https://iotda.cn-north-4.myhuaweicloud.com/v5/iot/ccde213a96c64620992b48735c58cf1b/devices/667e49152ca97925e0661c41_test/commands',
        data:'{"service_id": "Simulate","command_name": "SimulateCongestion","paras": { "congestion": "false"}}',
        method: 'POST',
        header: {'content-type': 'application/json','X-Auth-Token':token },
        success: function(res){
            console.log("拥挤状态解除");
        },
        fail:function(){
            console.log("命令下发失败");
        },
    });
  },
  /* 向设备发送道路发生拥挤的命令 */
  setCommand_congestion_2:function(){
    var that = this; 
    var token = wx.getStorageSync('token');
    wx.request({
        url: 'https://iotda.cn-north-4.myhuaweicloud.com/v5/iot/ccde213a96c64620992b48735c58cf1b/devices/667e49152ca97925e0661c41_test/commands',
        data:'{"service_id": "Alarm","command_name": "CongestionModule","paras": { "congestion": "true"}}',
        method: 'POST', 
        header: {'content-type': 'application/json','X-Auth-Token':token }, 
        success: function(res){
            console.log("道路拥挤");
        },
        fail:function(){
            console.log("命令下发失败");
        },
    });
  },
  /* 向设备发送道路恢复正常（不拥堵）的命令 */
  setCommand_not_congestion_2:function(){
    var that = this;
    var token = wx.getStorageSync('token');
    wx.request({
        url: 'https://iotda.cn-north-4.myhuaweicloud.com/v5/iot/ccde213a96c64620992b48735c58cf1b/devices/667e49152ca97925e0661c41_test/commands',
        data:'{"service_id": "Alarm","command_name": "CongestionModule","paras": { "congestion": "false"}}',
        method: 'POST',
        header: {'content-type': 'application/json','X-Auth-Token':token }, 
        success: function(res){
            console.log("道路恢复正常");
        },
        fail:function(){
            console.log("命令下发失败");
        },
    });
  },
  /* 向设备发送相应消息 */
  setmessage:function(message){
    //console.log("开始下发消息");
    var that = this;
    var token = wx.getStorageSync('token');
    wx.request({
        url: 'https://iotda.cn-north-4.myhuaweicloud.com/v5/iot/ccde213a96c64620992b48735c58cf1b/devices/667e49152ca97925e0661c41_test/messages',
        data:JSON.stringify({"message":message}),
        method: 'POST',
        header: {'content-type': 'application/json','X-Auth-Token':token },
        success: function(res){
            console.log("消息发送成功");
        },
        fail:function(){
            console.log("消息发送失败");
        },
    });
  },
  /* 道路进入（解除）紧急状态按钮按下*/
  touchBtn_setCommand_emergency:function()
  {
    var that = this;
    if (that.data.Alarm.Is_emergency === "true") {
      wx.showToast({title:"应急状态解除", image:"/images/emergency.png", duration:2000});
      this.setCommand_not_emergency();}
    else {
      wx.showToast({title:"应急状态开始", image:"/images/emergency.png", duration:2000});
      this.setCommand_emergency();}

  },
  /* 模拟道路拥挤按钮按下*/
  touchBtn_setCommand_congestion:function()
  {
    wx.showToast({title:"模拟道路拥挤", image:"/images/car.png", duration:2000});
    this.setCommand_congestion();
      
  },
  /* 取消模拟道路拥挤按钮按下*/
  touchBtn_setCommand_not_congestion:function()
  {
    wx.showToast({title:"模拟拥挤结束", image:"/images/car.png", duration:2000});
    this.setCommand_not_congestion();
  },

  /* 生命周期函数--监听页面加载*/
  onLoad(options) {
    this.setData({navH: App.globalData.navHeight});  //获取导航栏高度

    this.gettoken();

    let prespeed1 = 0;
    let prespeed2 = 0;
    let prespeed3 = 0;

    let msg1 = null;
    let msg2 = null;
    let msg3 = null;

    let congestion_state = null;
    
    // 开始定时任务，每隔0.4秒调用一次getshadow函数
    this.timer = setInterval(() => {
      this.getshadow();
      let speed = this.data.Speed;
      let alarm = this.data.Alarm;

      // 每分钟显示道路信息
      if (this.counter % 150 == 0){   
        console.log("Road1平均车速: "+speed.Average_speed1);
        console.log("Road1总车流量: "+speed.Flow1);
        console.log('\n');
        console.log("Road2平均车速: "+speed.Average_speed2);
        console.log("Road2总车流量: "+speed.Flow2);
        console.log('\n');
        console.log("Road3平均车速: "+speed.Average_speed3);
        console.log("Road3总车流量: "+speed.Flow3);
        console.log('\n');
      }
      this.counter++;
      
      // 发送拥堵信号
      if (congestion_state!==alarm.Is_congestion && alarm.Is_congestion){   
        congestion_state = alarm.Is_congestion;
        this.setCommand_congestion_2();
      }
      if (congestion_state!==alarm.Is_congestion && !alarm.Is_congestion) {
        congestion_state = alarm.Is_congestion;
        this.setCommand_not_congestion_2();
      }
      
      // 车辆驶来提醒
      if (prespeed1 && prespeed1!=speed.Last_Speed1){  
        let msg = "Car on Road1 : "+speed.Last_Speed1+" km/h";
        wx.showToast({title:msg ,icon:'none', duration: 2000});
        console.log(msg);
        console.log('\n');
      }
      if (prespeed2 && prespeed2!=speed.Last_Speed2){ 
        let msg = "Car on Road2: "+speed.Last_Speed2+" km/h";
        wx.showToast({title:msg ,icon:'none', duration: 2000});
        console.log(msg);
        console.log('\n');
      }
      if (prespeed3 && prespeed3!=speed.Last_Speed3){  
        let msg = "Car on Road3: "+speed.Last_Speed3+" km/h";
        wx.showToast({title:msg ,icon:'none', duration: 2000});
        console.log(msg);
        console.log('\n');
      }
      prespeed1 = speed.Last_Speed1; 
      prespeed2 = speed.Last_Speed2;
      prespeed3 = speed.Last_Speed3;

      // 检测道路上车辆消息
      if (msg1!==speed.Road1){    
        msg1 = speed.Road1;
        this.setmessage(msg1);
      }
      if (msg2!==speed.Road2){
        msg2 = speed.Road2;
        this.setmessage(msg2);
      }
      if (msg3!==speed.Road3){
        msg3 = speed.Road3;
        this.setmessage(msg3);
      }

    }, 400);

    this.counter = 0;

  },

  onUnload() {
    // 清除定时器，防止页面销毁后定时任务继续执行
    if (this.timer) {
        clearInterval(this.timer);
    }
  }
})