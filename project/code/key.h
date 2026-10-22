#ifndef CODE_KEY_H_
#define CODE_KEY_H_

#include "zf_common_headfile.h"

// 按键引脚定义
#define KEY1 (B21)
#define KEY2 (A30)
#define KEY3 (A23)
#define KEY4 (B24)

// 供其他文件使用的全局变量
extern   int start_car_flag;       // 发车标志
extern  int laps_count; // 圈数

// 函数声明
void KEY_Init(void);
void KEY_Scan(void);
void display_adjust_page(void);
void handle_keys_main(uint8_t p[4]);
void handle_keys_adjust(uint8_t p[4]);
void display_main_page(void);

#endif /* CODE_KEY_H_ */
