#include "wirelessuart.h"

// 假设这些全局变量在别处定义
uint8 data_buffer[32];
uint8 data_len;
uint8 stop_flag = 0;
uint8 wzq_buffer[64];

void vofa_module_init(void)
{
    wireless_uart_init();
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string("SEEKFREE wireless uart demo.\r\n");
}


/**
 * @brief 检查并回显从无线串口接收到的数据。
 * 
 * 该函数从无线串口缓冲区读取数据，检查是否接收到数据，并根据接收到的数据内容进行相应的处理。
 * 如果接收到数据，则尝试解析数据内容，并根据解析结果执行相应的操作。如果解析失败，则返回未知命令提示。
 * 最后，清空缓冲区，为下次接收做准备。
 */
void vofa_check_and_echo(void)
{
    // 1. 从无线串口缓冲区读取数据
    data_len = (uint8)wireless_uart_read_buffer(data_buffer, 32);
    
    // 2. 检查是否收到了数据
    if(data_len != 0)
    {
        // 准备解析
        data_buffer[data_len] = '\0'; // 添加字符串结束符，保证sscanf安全
        float value = 0;
        char response_buffer[40];

        // 3. 尝试匹配各种命令格式
        // --- 速度环参数 ---
        if(sscanf((const char*)data_buffer, "SP%f", &value) == 1)
        {
            //Speed.KP = value;
            //sprintf(response_buffer, "Speed.KP set to: %.2f\r\n", Speed.KP);
            wireless_uart_send_string(response_buffer);
        }
        
        // --- 未知命令处理 ---
        else
        {
            wireless_uart_send_string("Unknown Command: ");
            wireless_uart_send_buffer(data_buffer, data_len);
            wireless_uart_send_string("\r\n");
        }
        
        // 4. 清空缓冲区，为下次接收做准备
        memset(data_buffer, 0, 32);
    }
}



void wirelessuart_send_vofa(void)        //波形函数
{
    //sprintf(wzq_buffer, "%f,%f\n",Deviation.PIDout,Gyro_Data.z);
    //wireless_uart_send_string(wzq_buffer);
    //sprintf(wzq_buffer, "%f,%f\n", Speed.SetValue, Speed.ActValue);
    //sprintf(wzq_buffer, "%f,%f,%f,%f,%f\n", Angle.SetValue,Angle.ActValue,a,b,a-b);
    //wireless_uart_send_string(wzq_buffer);
}