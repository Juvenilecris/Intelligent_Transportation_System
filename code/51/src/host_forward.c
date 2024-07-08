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
void Delay600ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 26;
	j = 55;
	k = 171;
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
		for(p=0;p<8;p++){
		  Out = Data&0x80;     
		  Data=Data<<1;          
			InPut = 0;                                        
	    InPut = 1;
		}
		for(p=0;p<8;p++){
		  Out = Data1&0x80;     
		  Data1=Data1<<1;          
			InPut = 0;                                        
	    InPut = 1;
		}
		Send = 0;
	  Send = 1;
}
main(){
  u8 a; 
  u8 c;	
  //正常运行时间数组
	u8 k[69]={0xB0,0xA4,0xF9,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,
	                  0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,
		 0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9};
	//拥挤状态数组
	 u8 f[71]={0xB0,0xA4,0xF9,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,
	0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,
	 0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9,0xC0,0x90,0x80,0xF8,0x82,0x92,0x99,0xB0,0xA4,0xF9};
	while(1){
		Change = 1;
		Danger = 1;
		Give = 1;
		Send = 1;
		if(Change==1){
		for(a=0;a<69;a++)
		{
			while(Give==1){
				
			};	
			if(a<3)
				{
					if(Danger==1){
					hc595(k[a],0xC0);
					Yellow = 1;
					Red = 0;
					Green = 0;
					}
				  else if(Danger==0){hc595(0xC0,0xC0); Red=1;Yellow=0;Green = 0;}
				
				}
			else if(a>=3&&a<11){
				if(Danger==1)
					{
				  hc595(k[a],0x99);
					  Red = 1;
						Yellow = 0;
						Green = 0;
			 	  }
				else if(Danger==0)
					{
				 hc595(0xC0,0xC0);
						Red = 1;
						Yellow = 0;
						Green = 0;
				  }
			}
			else if(a>=11&&a<21){
				if(Danger==1)
					{
		    	hc595(k[a],0xB0);
					  Red = 1;
						Yellow = 0;
						Green = 0;
				  }
			   else if(Danger==0)
				 {
				 hc595(0xC0,0xC0);
					 	Red = 1;
						Yellow = 0;
						Green = 0;
				 }
			
			}
			else if(a>=21&&a<31){
		       if(Danger==1){			
						Red = 1;
						Yellow = 0;
						Green = 0;
		   	hc595(k[a],0xA4);}
			     else if(Danger==0){
					 hc595(0xC0,0xC0);
						Red = 1;
						Yellow = 0;
						Green = 0;
					 }
			}
			else if(a>=31&&a<41){
			   if(Danger==1){ 
					  Red = 1;
						Yellow = 0;
						Green = 0;
			   hc595(k[a],0xF9);
				 }
				 
				 else if(Danger==0){
					 	Red = 1;
						Yellow = 0;
						Green = 0;
				 hc595(0xC0,0xC0);
				 }
			}
			else if(a>=41&&a<50){
			    if(Danger==1){
			  hc595(k[a],0xC0);
						Red = 1;
						Yellow = 0;
						Green = 0;
					}
					else if(Danger==0){
					hc595(0xC0,0xC0);
						Red = 1;
						Yellow = 0;
						Green = 0;
					}
			
			}
			else if(a>=50&&a<60){
			    if(Danger==1){
			   hc595(k[a],0xF9);
					Red = 0;
						Yellow = 0;
						Green = 1;
					}
					else if(Danger==0){
						Red = 1;
						Yellow = 0;
						Green = 0;
					hc595(0xC0,0xC0);
					}
			
			}
			else if(a>=60&&a<70){
			   if(Danger==1)
				 {
			   hc595(k[a],0xC0);
					  Red = 0;
						Yellow = 0;
						Green = 1;
				 }
				 else if(Danger==0){
					 Red = 1;
						Yellow = 0;
						Green = 0;
				 hc595(0xC0,0xC0);
				 }
			}
				Delay1000ms();
		}
		a=0;
}
		//拥挤状态
		else if(Change==0){
					for(c=0;c<71;c++)
		{
			while(Give==1){};
				
				if(c==10)
				{
					if(Danger==1){
					hc595(f[0],f[c]);
										Red = 0;
						Yellow = 1;
						Green = 0;}
					else if(Danger==0){
					hc595(0xC0,0xC0);
					Red = 1;
					Yellow = 0;
					Green = 0;
					}
				}
			 if(c<3)
				{
				   if(Danger==1){
				   hc595(f[c],0xC0);
						 Red = 0;
					Yellow = 1;
					Green = 0;
					 }
					 else if(Danger==0){
						 Red = 1;
						Yellow = 0;
						Green = 0;
					 hc595(0xC0,0xC0);
					 }
				
				}
				else if(c>=3&&c<11){
				   if(Danger==1){
				   hc595(f[c],0xB0);
						 Red = 1;
					Yellow = 0;
					Green = 0;
					 }
					 else if(Danger==0){
					 hc595(0xC0,0xC0);
					 }
				}
				else if(c>=11&c<21){
				    if(Danger==1){
				   hc595(f[c],0xA4);
							Red = 1;
					Yellow = 0;
					Green = 0;
						}
						else if(Danger==0){
							Red = 1;
						Yellow = 0;
						Green = 0;
					 hc595(0xC0,0xC0);
					 }
				
				}
				else if(c>=21&&c<31){
				    if(Danger==1){
				     hc595(f[c],0xF9);
							Red = 1;
					Yellow = 0;
					Green = 0;
						}
						else if(Danger==0){
							Red = 1;
						Yellow = 0;
						Green = 0;
					 hc595(0xC0,0xC0);
					 }
				}
				else if(c>=31&c<41){
				   if(Danger==1){
				     hc595(f[c],0xC0);
						 Red = 1;
					Yellow = 0;
					Green = 0;
					 }
					 else if(Danger==0){
						 Red = 1;
						Yellow = 0;
						Green = 0;
					 hc595(0xC0,0xC0);
					 }			
				}
				else if(c>40&&c<51){
				  if(Danger==1){
				  hc595(f[c],0xA4);
						Red = 0;
					Yellow = 0;
					Green = 1;
					}
				 else if(Danger==0){
					 Red = 1;
						Yellow = 0;
						Green = 0;
					 hc595(0xC0,0xC0);
					 }
				}
				else if(c>=51&&c<61){
				   if(Danger==1){
				  hc595(f[c],0xF9);
						 Red = 0;
					Yellow = 0;
					Green = 1;
					 }
					 else if(Danger==0){
						 Red = 1;
						Yellow = 0;
						Green = 0;
					 hc595(0xC0,0xC0);
					 }
				}
				else if(c>=61&&c<=70){
				   if(Danger==1){
				  hc595(f[c],0xC0);
						 Red = 0;
					Yellow = 0;
					Green = 1;
					 }
					 else if(Danger==0){
						 Red = 1;
						Yellow = 0;
						Green = 0;
					 hc595(0xC0,0xC0);
					 }
				}
				Delay1000ms();	
		}
			c=0;		
		
 }	
	}
}
