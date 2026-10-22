#ifndef _WMM_LQTB6612_H
#define _WMM_LQTB6612_H

#include "zf_common_typedef.h"

//引脚已经适配绿色主控板
#define TB6612_PORT (GPIOA)
#define TB6612_E1A_PIN (A13)
#define TB6612_E1B_PIN (B23)

#define TB6612_E2A_PIN (B27)
#define TB6612_E2B_PIN (A29)

#define TB6612_AIN1_PIN (B13)
#define TB6612_AIN2_PIN (B3)
#define TB6612_BIN1_PIN (B7)
#define TB6612_BIN2_PIN (B6)

// 使用TIMA0输出功能-
#define TB6612_PWMA (PWM_TIM_A0_CH1_B12)
#define TB6612_PWMB (PWM_TIM_A0_CH3_A12)

#define ABS(a) (a > 0 ? a : (-a)) // 取绝对值

// extern volatile int16 Encoder_Count_A;
// extern volatile int16 Encoder_Count_B;

extern volatile int32 TargetSpeed_L, TargetSpeed_R;
// /************************以下代码来自嘉立创************************************/
// typedef struct
// {
//   float kp, ki, kd; 						      // 三个静态系数
//   float change_p, change_i, change_d;	          // 三个动态参数
//   float error, last_error; 						  // 误差、之前误差
//   float max_change_i; 							  // 积分限幅
//   float output, max_output; 				      // 输出、输出限幅
//   int target;                                     // 目标
// }PID;

// //串级PID的结构体，包含两个单级PID
// typedef struct
// {
//     PID inner; //内环
//     PID outer; //外环
//     float output; //串级输出
// }CascadePID;
// /************************以上代码来自嘉立创************************************/

void Motor_Out(int PWM_L, int PWM_R);
void TB6612_init(void);             // 初始化
void TB6612_stop(void);              // 停止电机输出

int Velocity_A(int TargetVelocity, int CurrentVelocity); // 速度环
int Velocity_B(int TargetVelocity, int CurrentVelocity); // 速度环



#endif // !_WMM_LQTB6612_H
