#ifndef __LF04_H
#define __LF04_H

#include "zf_common_headfile.h"

//定义传感器引脚 从左往右数(轮趣LF04)
#define DH1_PIN     (A22)
#define DH2_PIN     (B20)
#define DH3_PIN     (A24)
#define DH4_PIN     (A27)
//预留备用引脚，A02  A21  A17  A14


// 读取传感器状态的宏定义
#define DH1         gpio_get_level(DH1_PIN)
#define DH2         gpio_get_level(DH2_PIN)
#define DH3         gpio_get_level(DH3_PIN)
#define DH4         gpio_get_level(DH4_PIN)

extern volatile int zhijiao_flag ;//直角弯标志
extern volatile int zhijaio_count; //直角计数


void LF04_Init(void);
void Track_LF04(void);
#endif