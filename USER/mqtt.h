#ifndef __MQTT_H
#define __MQTT_H	
#include "sys.h"
//@初始化引脚
void gpio_init(void);
//@tcp/ip连接
uint8_t tcp_ip_connect(void);
//onene发送数据到平台
void onenet_send_data(void);
//无等待延时 switch方法进行连接
void no_wait_connect(void);
//@连接到OneNet
//@发送数据
#endif 
