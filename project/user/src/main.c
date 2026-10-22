/*********************************************************************************************************************
 * MSPM0G3507 Opensource Library 即（MSPM0G3507 开源库）是一个基于官方 SDK 接口的第三方开源库
 * Copyright (c) 2022 SEEKFREE 逐飞科技
 *
 * 本文件是 MSPM0G3507 开源库的一部分
 *
 * MSPM0G3507 开源库 是免费软件
 * 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
 * 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
 *
 * 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
 * 甚至没有隐含的适销性或适合特定用途的保证
 * 更多细节请参见 GPL
 *
 * 您应该在收到本开源库的同时收到一份 GPL 的副本
 * 如果没有，请参阅<https://www.gnu.org/licenses/>
 *
 * 额外注明：
 * 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
 * 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
 * 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
 * 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
 *
 * 文件名称          mian
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
 * 开发环境          MDK 5.37
 * 适用平台          MSPM0G3507
 * 店铺链接          https://seekfree.taobao.com/
 ********************************************************************************************************************/

#include "zf_common_headfile.h"

// **************************** 备注 ****************************
// 在startup_mspm0g350x_uvision.s的第43与54行将堆栈大小由0x00000400改为0x00001000
// 在zf_device_tft180.h中修改了引脚定义

// **************************** 代码区域 ****************************

#define LED (B22)
int temp = 200;

void user_init(void)
{
    // vofa_module_init(); // 无线串口模块初始化
    tft180_init(); // TFT 初始化
    TB6612_init(); // 电机驱动初始化

    pit_ms_init(PIT_TIM_G0, 50, NULL, NULL);  // 编码器计数定时器初始化,暂定50ms
    pit_ms_init(PIT_TIM_G7, 50, NULL, NULL); // 暂定50ms

     openmv_uart_init(); // 初始化OpenMV通信

    gpio_init(LED, GPO, GPIO_HIGH, GPO_PUSH_PULL); // 初始化LED引脚为输出

    LF04_Init(); // 循迹模块
    KEY_Init();  // 初始化按键
    // servo_init(); // 舵机初始化
    // interrupt_set_priority(TIMA0_INT_IRQn, 0); // 设置PIT0优先级为最高
    // interrupt_set_priority(TIMG0_INT_IRQn, 1); // 设置PIT0优先级为最高
    // interrupt_set_priority(UART0_INT_IRQn, 7); // 设置PIT0优先级为最高

    system_delay_ms(100);
    printf("Hello MSPM0G3507!!!\r\n");
}
void TEST(void)
{

    // tft180_show_char(0, 0, 'A');
    // tft180_show_string(0, 20, "Hello World");
    // tft180_show_int(0, 40, targetSpeedA, 4);

    // // // system_delay_ms(1000);
    // // printf("targetSpeedA:%d\r\n", targetSpeedA);
    // // // printf("Encoder_Count_A:%d\r\n", Encoder_Speed_L);
    // // // printf("Encoder_Count_B:%d\r\n", Encoder_Speed_R);

    // // // Motor_Out(targetSpeedA, targetSpeedA);

    // targetSpeedA -=50 ;
    // if (targetSpeedA < 200)
    // {

    //     targetSpeedA = 1300;
    // }

    // pwm_set_duty(SERVO_X_PWM, targetSpeedA); // 250-1250
    // pwm_set_duty(SERVO_Y_PWM, targetSpeedA);
    // system_delay_ms(300);
}

int main(void)
{
    clock_init(SYSTEM_CLOCK_80M); // 时钟配置及系统初始化<务必保留>
    debug_init();                 // 调试串口信息初始化
    user_init();                  // 用户代码初始化

    system_delay_ms(500);

    // targetSpeedA = 750;

    // pwm_set_duty(SERVO_X_PWM, targetSpeedA); // 250-1250
    // pwm_set_duty(SERVO_Y_PWM, targetSpeedA);
    // system_delay_ms(3000); 
    while (true)
    {

        // tft180_show_string(0, 0, "zhijaio_count");  // 在TFT屏幕上显示字符串
        // tft180_show_int(0, 20, zhijaio_count, 4);   // 显示直角弯计数
        // tft180_show_string(0, 40, "ES_A--B");       // 在TFT屏幕上显示字符串
        // tft180_show_int(0, 60, Encoder_Speed_L, 4); // 显示编码器A速度
        // tft180_show_int(0, 80, Encoder_Speed_R, 4); // 显示编码器B速度
        // printf("ESpeed_A, ESpeed_B: %d, %d\r\n", Encoder_Speed_L, Encoder_Speed_R); // 打印编码器速度到调试串口
        // printf("ESpeed_A, ESpeed_B: %d, %d\r\n", Encoder_Count_A, Encoder_Count_B); // 打印编码器速度到调试串口
        // TEST();

        system_delay_ms(500);
        gpio_toggle_level(LED); // 切换LED状态
    }
}
