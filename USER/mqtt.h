#ifndef __MQTT_H
#define __MQTT_H	
#include "sys.h"
//@��ʼ������
void gpio_init(void);
//@tcp/ip����
uint8_t tcp_ip_connect(void);
//onene�������ݵ�ƽ̨
void onenet_send_data(void);
//@���ӵ�OneNet
//@��������
#endif 
