#include "openmv.h"
#include "zf_common_headfile.h"



fifo_struct      openmv_rx_fifo;                                              // OpenMV接收FIFO结构体
static uint8     openmv_rx_buffer[OPENMV_BUFFER_SIZE];                        // OpenMV接收缓冲区
static uint8     openmv_rx_data;                                             // OpenMV接收数据
uint8 openmv_receive_flag;    

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     OpenMV串口接收的中断回调函数
// 参数说明     void
// 返回参数     void
// 使用示例     openmv_uart_rx_interrupt();
//-------------------------------------------------------------------------------------------------------------------
void openmv_uart_rx_interrupt(uint32 state, void *ptr)
{
    uart_query_byte(OPENMV_UART, &openmv_rx_data);                           // 读取串口数据
    fifo_write_buffer(&openmv_rx_fifo, &openmv_rx_data, 1);                 // 存入FIFO
		openmv_receive_flag = 1;    
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     OpenMV串口初始化
// 参数说明     void
// 返回参数     void
// 使用示例     openmv_uart_init();
//-------------------------------------------------------------------------------------------------------------------
void openmv_uart_init(void)
{
    fifo_init(&openmv_rx_fifo, FIFO_DATA_8BIT, openmv_rx_buffer, OPENMV_BUFFER_SIZE);   // 初始化FIFO
    
    uart_init(OPENMV_UART, OPENMV_UART_BAUD, OPENMV_TX, OPENMV_RX);          // 初始化串口
    
    uart_set_interrupt_config(OPENMV_UART, UART_INTERRUPT_CONFIG_RX_ENABLE);            // 使能串口接收中断
    interrupt_set_priority(OPENMV_UART_PRIORITY, 1);                                    // 设置中断优先级
    
    uart_set_callback(OPENMV_UART, openmv_uart_rx_interrupt, NULL);                     // 设置中断回调
    
    // 发送初始化成功信息
    uart_write_string(DEBUG_UART_INDEX, "OpenMV UART Init Success\r\n");
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     向OpenMV发送数据
// 参数说明     *data       数据指针
// 参数说明     len         数据长度
// 返回参数     void
// 使用示例     openmv_uart_send(data, len);
//-------------------------------------------------------------------------------------------------------------------
void openmv_uart_send(uint8 *data, uint32 len)
{
    uart_write_buffer(OPENMV_UART, data, len);                               // 发送数据
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     OpenMV数据处理
// 参数说明     void
// 返回参数     void
// 使用示例     openmv_uart_handler();
//-------------------------------------------------------------------------------------------------------------------
void openmv_uart_handler(void)
{
    uint8 rx_buf[OPENMV_BUFFER_SIZE];
    uint32 data_len;
    
   if(openmv_receive_flag)                                                    // 收到数据
    {
        data_len = fifo_used(&openmv_rx_fifo);                                // 查看FIFO数据长度
        if(data_len)                                                           // 读取到数据
        {
            // 先发送提示信息到调试串口
            uart_write_string(DEBUG_UART_INDEX, "\r\nOpenMV Data: ");
            
            // 读取OpenMV发送的数据，从fifo读取数据
					fifo_read_buffer(&openmv_rx_fifo, rx_buf, &data_len, FIFO_READ_AND_CLEAN);		//第一个参数为FIFO 对象指针，第二个参数为目标缓冲区指针，第三个参数为读取的数据长度，第四个参数读取并清空缓冲区
            
            // 发送到调试串口显示
            uart_write_buffer(DEBUG_UART_INDEX, rx_buf, data_len);
            uart_write_string(DEBUG_UART_INDEX, "\r\n");
            
            // 回复OpenMV
            uart_write_string(OPENMV_UART, "Data Received!\r\n");
        }
        openmv_receive_flag = 0;                                              // 清除接收标志
    }
}