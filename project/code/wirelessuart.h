#ifndef __WIRELESSUART_H
#define __WIRELESSUART_H

#include "zf_common_headfile.h"

//引脚定义见

extern uint8 data_buffer[32];
extern uint8 data_len;
extern uint8 stop_flag;
void vofa_module_init(void);
void vofa_check_and_echo(void);
void wirelessuart_send_vofa(void);

#endif