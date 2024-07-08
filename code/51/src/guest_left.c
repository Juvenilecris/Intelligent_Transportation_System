#include <reg51.H>
#include<stdio.h>
#include <intrins.h>

sbit InPut = P2^0;//接收单个信号 上升沿触发 
sbit Out = P2^1;//8位输出数据
sbit Send = P2^2;//发送信号 上升沿触发0
sbit Change = P2^5; //hi3861v100 改变交通状态接口
sbit Danger = P2^6; //hi3861v100 应急状态
sbit Give = P2^7; //hi3861v100 接收到load信号
sbit Red = P0^0;//红灯
sbit Green = P0^1;//绿灯
sbit Yellow = P0^2;//黄灯
typedef unsigned char u16;
typedef unsigned char u8;
void Delay1000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 43;
	j = 6;
	k = 203;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}



//74HC595移位接受信号
void hc595(u16 Data,u16 Data1){
	int p ;
	Change = 1;
		for(p=0;p<8;p++){
		  Out = Data&0x80;     
		  Data=Data<<1;          
			InPut = 0;                                        
			
		/*  _nop_();
	    _nop_();*/
	    InPut = 1;
		}
		for(p=0;p<8;p++){
		  Out = Data1&0x80;     
		  Data1=Data1<<1;          
			InPut = 0;                                        
			
		  /*_nop_();
	    _nop_();*/
	    InPut = 1;
		}
		Send = 0;
   /* _nop_();
	  _nop_();*/
	  Send = 1;
}
main(){
	u8 a;
	u8 b;
	//正常运行数组
	u8 k[70]={0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xB0,0xA4,0xF9,
	          0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9};
	//拥挤运行数组
		
		while(1){
		Change = 1;
		Danger = 1;
		Give = 1;
		Send = 1;
		if(Change==1){
		 for(a=0;a<70;a++){
	    while(Give=1){
			
			};
			if(a==0){
					 if(Danger==1){
				 hc595(0xC0,0xF9);}
					 else if(Danger==0){hc595(0xC0,0xC0); Red=1;Yellow=0;Green = 0;}
				 }//10
				 if(a>0&&a<10){
					Red=1;Yellow=0;Green = 0;
				 hc595(k[a],0xC0);}//10-0
		     if(a==10){
					 if(Danger==1){
					 hc595(0xC0,0xA4);
						 Red=0;Yellow=0;Green = 1;
					 }
					 else if(Danger==0){hc595(0xC0,0xC0); Red=1;Yellow=0;Green = 0;}
				 }//20
				 if(a>10&&a<21){
				   if(Danger==1){
					 hc595(k[a],0xF9); Red=0;Yellow=0;Green = 1;
					 }
					 else if(Danger==0){hc595(0xC0,0xC0); Red=1;Yellow=0;Green = 0;}
				 }//19-10
		     if(a>=21&&a<33){
				   if(Danger==1){
				   hc595(k[a],0xC0); 
						 						 if(a>=21&&a<30){ Red=0;Yellow=0;Green = 1;}
						 else if(a>=30&&a<33){ Red=0;Yellow=1;Green = 0;}
					 }
					 else if(Danger==0){hc595(0xC0,0xC0); Red=1;Yellow=0;Green = 0;}
				 }//9-0-3
				 if(a>=33&a<=40){
				   if(Danger==1){
					 hc595(k[a],0x99);Red=1;Yellow=0;Green = 0;
					 }
					 else if(Danger==0){hc595(0xC0,0xC0); Red=1;Yellow=0;Green = 0;}
				 }//47/40
				 if(a>=40&&a<50){
				   if(Danger==1){
				    hc595(k[a],0xB0);Red=1;Yellow=0;Green = 0;
					 }
					 else if(Danger==0){hc595(0xC0,0xC0); Red=1;Yellow=0;Green = 0;}
				 }//39-30
				 if(a>=50&&a<60){
				   if(Danger==1)
					 {
				   hc595(k[a],0xA4);Red=1;Yellow=0;Green = 0;
					 }
					 else if(Danger==0){hc595(0xC0,0xC0); Red=1;Yellow=0;Green = 0;}
				 }//29-20
				 if(a>=60&&a<69){
				   if(Danger==1){
				   hc595(k[a],0xF9);Red=1;Yellow=0;Green = 0;
					 }
					 else if(Danger==0){hc595(0xC0,0xC0); Red=1;Yellow=0;Green = 0;}
				 
				   }//19-11
				 Delay1000ms();
		 }	
		 a=0;	
		}	
	}
}
	
