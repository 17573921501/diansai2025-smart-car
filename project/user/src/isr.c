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
 * 文件名称          isr
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
 * 开发环境          MDK 5.37
 * 适用平台          MSPM0G3507
 * 店铺链接          https://seekfree.taobao.com/
 ********************************************************************************************************************/

#include "isr.h"
#include "zf_common_headfile.h"

volatile int16 Encoder_Count_A; // 左轮编码器计数
volatile int16 Encoder_Count_B; // 右轮编码器计数

volatile int16 Encoder_Speed_L; // 左轮编码器速度
volatile int16 Encoder_Speed_R; // 右轮编码器速度

volatile uint32 Encoder_jifen = 0; // 编码器计数总和

void TIMA0_IRQHandler(void) // 已经用于TB6612的PWM输出
{
  pit_callback_list[0](0, pit_callback_ptr_list[0]);
}

void TIMA1_IRQHandler(void) // 用于舵机pwm输出
{
  pit_callback_list[1](0, pit_callback_ptr_list[1]);
}

void TIMG0_IRQHandler(void) // 50ms 定时中断用于编码器计数
{
  pit_callback_list[2](0, pit_callback_ptr_list[2]);
  KEY_Scan(); // 扫描按键状态
  // 编码器计数定时器中断处理函数

  Encoder_Speed_R = Encoder_Count_A;
  Encoder_Count_A = 0;
  Encoder_Speed_L = Encoder_Count_B;
  Encoder_Count_B = 0;

  // Encoder_jifen += ABS(Encoder_Count_A) + ABS(Encoder_Count_B); // 累加编码器计数
}

void TIMG6_IRQHandler(void)
{
  pit_callback_list[3](0, pit_callback_ptr_list[3]);
}

void TIMG7_IRQHandler(void)
{
  pit_callback_list[4](0, pit_callback_ptr_list[4]);

  // Motor_Out(3000, 1000); // 输出电机速度

  // printf("TaTbEaEb:%d,%d,%d,%d\n", tempa, tempb, Encoder_Speed_L, Encoder_Speed_R);                 // 打印调试信息
  // printf("TaTbEaEb:%d,%d,%d,%d\n", TargetSpeed_L, TargetSpeed_R, Encoder_Speed_L, Encoder_Speed_R); // 打印调试信息

  //   int start_car_flag = 1; // 发车标志置位/////////////////////////////////////////////////

  if (start_car_flag == 1)
  {
    if (zhijiao_flag != 0)
    {
      system_delay_ms(50); // 延时50ms
      zhijiao_flag = 0;
      Track_LF04();
    }
    else
    {
      Track_LF04();
    }
    if (zhijaio_count >= laps_count * 4)
    // if (zhijaio_count >= 16)
    {
      TB6612_stop(); // 停止电机输出
                     // 满足指定圈数，蜂鸣器加停车，
    }
  }

  // Motor_Out(Velocity_A(TargetSpeed_L, Encoder_Speed_L), Velocity_B(TargetSpeed_R, Encoder_Speed_R)); // 输出电机速度
  // printf("TaTbEaEb:%d,%d,%d,%d\n", TargetSpeed_L, TargetSpeed_R, Encoder_Speed_L, Encoder_Speed_R);  // 打印调试信息
}

void TIMG8_IRQHandler(void)
{
  pit_callback_list[5](0, pit_callback_ptr_list[5]);
}

void TIMG12_IRQHandler(void)
{
  pit_callback_list[6](0, pit_callback_ptr_list[6]);
}

void UART0_IRQHandler(void)
{
  switch (DL_UART_getPendingInterrupt(UART0))
  {
  case DL_UART_IIDX_TX:
  {
    uart_callback_list[0](UART_INTERRUPT_STATE_TX, uart_callback_ptr_list[0]);
  }
  break;
  case DL_UART_IIDX_RX:
  {
    uart_callback_list[0](UART_INTERRUPT_STATE_RX, uart_callback_ptr_list[0]);
#if DEBUG_UART_USE_INTERRUPT
    debug_interrupr_handler();
#endif
  }
  break;

  default:
    break;
  }
  DL_UART_clearInterruptStatus(UART0, UART0->CPU_INT.RIS);
}

void UART1_IRQHandler(void)
{
  switch (DL_UART_getPendingInterrupt(UART1))
  {
  case DL_UART_IIDX_TX:
  {
    uart_callback_list[1](UART_INTERRUPT_STATE_TX, uart_callback_ptr_list[1]);
  }
  break;
  case DL_UART_IIDX_RX:
  {
    uart_callback_list[1](UART_INTERRUPT_STATE_RX, uart_callback_ptr_list[1]);

    // wifi_uart_callback();

    wireless_module_uart_handler(); // ??????????
  }
  break;

  default:
    break;
  }
  DL_UART_clearInterruptStatus(UART1, UART1->CPU_INT.RIS);
}

void UART2_IRQHandler(void)
{
  switch (DL_UART_getPendingInterrupt(UART2))
  {
  case DL_UART_IIDX_TX:
  {
    uart_callback_list[2](UART_INTERRUPT_STATE_TX, uart_callback_ptr_list[2]);
  }
  break;
  case DL_UART_IIDX_RX:
  {
    uart_callback_list[2](UART_INTERRUPT_STATE_RX, uart_callback_ptr_list[2]);
  }
  break;

  default:
    break;
  }
  DL_UART_clearInterruptStatus(UART2, UART2->CPU_INT.RIS);
}

void UART3_IRQHandler(void)
{
  switch (DL_UART_getPendingInterrupt(UART3))
  {
  case DL_UART_IIDX_TX:
  {
    uart_callback_list[3](UART_INTERRUPT_STATE_TX, uart_callback_ptr_list[3]);
  }
  break;
  case DL_UART_IIDX_RX:
  {
    uart_callback_list[3](UART_INTERRUPT_STATE_RX, uart_callback_ptr_list[3]);
  }
  break;

  default:
    break;
  }
  DL_UART_clearInterruptStatus(UART3, UART3->CPU_INT.RIS);
}

// volatile uint8 TB6612_E1A_FLAG = 0; // 左轮编码器A相中断标志
// volatile uint8 TB6612_E1B_FLAG = 0; // 左轮编码器B相中断标志
// volatile uint8 TB6612_E2A_FLAG = 0; // 右轮编码器A相中断标志
// volatile uint8 TB6612_E2B_FLAG = 0; // 右轮编码器B相中断标志
void GROUP1_IRQHandler(void)
{
  uint8 exti_index = 0;
  uint8 exti_event = 0;

  uint32 register_temp = gpio_group[0]->CPU_INT.IIDX;
  if (register_temp)
  {
    exti_index = register_temp - 1;

    if (15 >= exti_index)
    {
      exti_event = (gpio_group[0]->POLARITY15_0 >> ((exti_index % 16) * 2)) & 0x03;
    }
    else
    {
      exti_event = (gpio_group[0]->POLARITY31_16 >> ((exti_index % 16) * 2)) & 0x03;
    }
    exti_callback_list[exti_index](exti_event, exti_callback_ptr_list[exti_index]);
  }
  else
  {
    register_temp = gpio_group[1]->CPU_INT.IIDX;
    if (register_temp)
    {
      exti_index = register_temp - 1;

      if (15 >= exti_index)
      {
        exti_event = (gpio_group[1]->POLARITY15_0 >> ((exti_index % 16) * 2)) & 0x03;
      }
      else
      {
        exti_event = (gpio_group[1]->POLARITY31_16 >> ((exti_index % 16) * 2)) & 0x03;
      }
      exti_callback_list[exti_index](exti_event, exti_callback_ptr_list[exti_index]);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 通过每个编码器的回调函数修改标志位
// 然后使用标志位判断编码器计数
// 双边沿中断,4倍频

void EXIT_TB6612_E1A(uint32_t pin, void *ptr)
{
  // TB6612_E1A_FLAG = 1; // 设置标志位

  if (gpio_get_level(TB6612_E1B_PIN) != gpio_get_level(TB6612_E1A_PIN)) // 如果AB相反，则为顺时针编码器计数自增
  {
    Encoder_Count_A++;
  }
  else
  {
    Encoder_Count_A--;
  }
  // 清除中断标志位
  // TB6612_E1A_FLAG = 0;
  // DL_GPIO_clearInterruptStatus(TB6612_PORT, TB6612_E1A_PIN); // 清除硬件中断标志
}
void EXIT_TB6612_E1B(uint32_t pin, void *ptr)
{
  // TB6612_E1B_FLAG = 1; // 设置标志位

  if (gpio_get_level(TB6612_E1A_PIN) == gpio_get_level(TB6612_E1B_PIN)) // 如果AB相同，则为顺时针编码器计数自增
  {
    Encoder_Count_A++;
  }
  else
  {
    Encoder_Count_A--;
  }
  // 清除中断标志位
  // TB6612_E1B_FLAG = 0;
  // DL_GPIO_clearInterruptStatus(TB6612_PORT, TB6612_E1B_PIN); // 清除硬件中断标志
}
void EXIT_TB6612_E2A(uint32_t pin, void *ptr)
{
  // TB6612_E2A_FLAG = 1; // 设置标志位
  if (gpio_get_level(TB6612_E2B_PIN) != gpio_get_level(TB6612_E2A_PIN)) // 如果AB相反，则为顺时针编码器计数自增
  {
    Encoder_Count_B--;
  }
  else
  {
    Encoder_Count_B++;
  }
  // 清除中断标志位
  // TB6612_E2A_FLAG = 0;
  // DL_GPIO_clearInterruptStatus(TB6612_PORT, TB6612_E2A_PIN);
}
void EXIT_TB6612_E2B(uint32_t pin, void *ptr)
{
  // TB6612_E2B_FLAG = 1; // 设置标志位
  if (gpio_get_level(TB6612_E2A_PIN) == gpio_get_level(TB6612_E2B_PIN)) // 如果AB相同，则为顺时针编码器计数自增
  {
    Encoder_Count_B--;
  }
  else
  {
    Encoder_Count_B++;
  }
  // 清除中断标志位
  // TB6612_E2B_FLAG = 0;
  // DL_GPIO_clearInterruptStatus(TB6612_PORT, TB6612_E2B_PIN);
}

/*void GROUP1_IRQHandler(void) {
  //尝试直接使用官方库

printf("GROUP1_IRQHandler: \n");
gpio_interrup = DL_GPIO_getEnabledInterruptStatus(GPIOA,ENCODERA_E1A_PIN|ENCODERA_E1B_PIN|ENCODERB_E2A_PIN|ENCODERB_E2B_PIN);
  //encoderA
  if((gpio_interrup & ENCODERA_E1A_PIN)==ENCODERA_E1A_PIN)
  {
    if(!DL_GPIO_readPins(GPIOA,ENCODERA_E1B_PIN))
    {
      Encoder_Count_A--;
    }
    else
    {
      Encoder_Count_A++;
    }
  }
  else if((gpio_interrup & ENCODERA_E1B_PIN)==ENCODERA_E1B_PIN)
  {
    if(!DL_GPIO_readPins(GPIOA,ENCODERA_E1A_PIN))
    {
      Encoder_Count_A++;
    }
    else
    {
      Encoder_Count_A--;
    }
  }
  //encoderB
  if((gpio_interrup & ENCODERB_E2A_PIN)==ENCODERB_E2A_PIN)
  {
    if(!DL_GPIO_readPins(GPIOA,ENCODERB_E2B_PIN))
    {
      Encoder_Count_B--;
    }
    else
    {
      Encoder_Count_B++;
    }
  }
  else if((gpio_interrup & ENCODERB_E2B_PIN)==ENCODERB_E2B_PIN)
  {
    if(!DL_GPIO_readPins(GPIOA,ENCODERB_E2A_PIN))
    {
      Encoder_Count_B++;
    }
    else
    {
      Encoder_Count_B--;
    }
  }
  DL_GPIO_clearInterruptStatus(GPIOA,ENCODERA_E1A_PIN|ENCODERA_E1B_PIN|ENCODERB_E2A_PIN|ENCODERB_E2B_PIN);

}*/

/*void GROUP1_IRQHandler(void) {
  // 定义局部变量，用于存储编码器状态和方向标志
  uint8_t Encodeg_flag, A_flag, B_flag, direction_flag;
  uint8_t Encodeg_flag_R, C_flag, D_flag, direction_flag_R;

  // printf("GROUP1_IRQHandler\n");

  // 根据中断组获取待处理的中断索引
  switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) {
    // 如果是TB6612_PORT的中断
  case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
    // 检查TB6612_E1A_PIN是否触发了中断
    if (DL_GPIO_getEnabledInterruptStatus(TB6612_PORT, TB6612_E1A_PIN)) {
      Encodeg_flag = 1; // A中断
      if (DL_GPIO_readPins(TB6612_PORT, TB6612_E1A_PIN))
        A_flag = 1;
      else
        A_flag = 0;
      if (DL_GPIO_readPins(TB6612_PORT, TB6612_E1B_PIN))
        B_flag = 1;
      else
        B_flag = 0;
      direction_flag = A_flag + B_flag +
                       Encodeg_flag; // 求和确定旋转方向，偶数向前，奇数向后
      if (direction_flag == 0 || direction_flag == 2)
        // 更改计数方向
        Encoder_Count_A--;
      else
        Encoder_Count_A++;

      DL_GPIO_clearInterruptStatus(TB6612_PORT, TB6612_E1A_PIN);
    }

    if (DL_GPIO_getEnabledInterruptStatus(TB6612_PORT, TB6612_E1B_PIN)) {
      Encodeg_flag = 0;
      if (DL_GPIO_readPins(TB6612_PORT, TB6612_E1A_PIN))
        A_flag = 1;
      else
        A_flag = 0;
      if (DL_GPIO_readPins(TB6612_PORT, TB6612_E1B_PIN))
        B_flag = 1;
      else
        B_flag = 0;
      direction_flag = A_flag + B_flag + Encodeg_flag;
      if (direction_flag == 0 || direction_flag == 2)
        // 更改计数方向
        Encoder_Count_A--;
      else
        Encoder_Count_A++;

      DL_GPIO_clearInterruptStatus(TB6612_PORT, TB6612_E1B_PIN);//清除中断标志位
    }

    if (DL_GPIO_getEnabledInterruptStatus(TB6612_PORT, TB6612_E2A_PIN)) {
      Encodeg_flag_R = 1;
      if (DL_GPIO_readPins(TB6612_PORT, TB6612_E2A_PIN))
        C_flag = 1;
      else
        C_flag = 0;
      if (DL_GPIO_readPins(TB6612_PORT, TB6612_E2B_PIN))
        D_flag = 1;
      else
        D_flag = 0;
      direction_flag_R = C_flag + D_flag + Encodeg_flag_R;
      if (direction_flag_R == 0 || direction_flag_R == 2)
        Encoder_Count_B++;
      else
        Encoder_Count_B--;

      DL_GPIO_clearInterruptStatus(TB6612_PORT, TB6612_E2A_PIN);
    }

    if (DL_GPIO_getEnabledInterruptStatus(TB6612_PORT, TB6612_E2B_PIN)) {
      Encodeg_flag_R = 0;
      if (DL_GPIO_readPins(TB6612_PORT, TB6612_E2A_PIN))
        C_flag = 1;
      else
        C_flag = 0;
      if (DL_GPIO_readPins(TB6612_PORT, TB6612_E2B_PIN))
        D_flag = 1;
      else
        D_flag = 0;
      direction_flag_R = C_flag + D_flag + Encodeg_flag_R;
      if (direction_flag_R == 0 || direction_flag_R == 2)
        Encoder_Count_B++;
      else
        Encoder_Count_B--;

      DL_GPIO_clearInterruptStatus(TB6612_PORT, TB6612_E2B_PIN);
    }

    break;
  }
}*/