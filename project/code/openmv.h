#ifndef _OPENMV_H_
#define _OPENMV_H_

#include "zf_common_typedef.h"
#include "zf_common_headfile.h"

// 定义OpenMV使用的串口配置
#define OPENMV_UART             (UART_1)        // OpenMV使用串口2
#define OPENMV_UART_BAUD        (115200)       // 波特率115200
#define OPENMV_TX            UART1_TX_A8     // 发送引脚
#define OPENMV_RX            UART1_RX_A9     // 接收引脚
#define OPENMV_UART_PRIORITY    (UART1_INT_IRQn)       // 中断优先级

// 数据缓冲区大小
#define OPENMV_BUFFER_SIZE      (64)                   // 缓冲区大小

// 函数声明
void    openmv_uart_init        (void);                                         // 初始化
void    openmv_uart_send        (uint8 *data, uint32 len);                     // 发送数据
void    openmv_uart_handler     (void);                                         // 数据处理

extern fifo_struct openmv_rx_fifo;              

#endif