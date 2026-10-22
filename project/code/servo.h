#ifndef _servo_h_
#define _servo_h_

#include "zf_common_typedef.h"

#define SCREEN_POINTS 400       // 屏幕点数，可以加大屏幕点数，但是不能太大，否则会卡顿
#define X_SERVO_MIN 70.0f       // X舵机最小角度
#define X_SERVO_MAX 110.0f       // X舵机最大角度
#define Y_SERVO_MIN 70.0f       // Y舵机最小角度
#define Y_SERVO_MAX 110.0f       // Y舵机最大角度

// 舵机PWM相关定义
#define SERVO_PWM_FREQ         50                                               // PWM频率50Hz，周期20ms
#define SERVO_PWM_DUTY_MAX     1250                                             // 最大占空比 1.25ms (180度)
#define SERVO_PWM_DUTY_MIN     250                                              // 最小占空比 0.25ms (0度)
#define SERVO_PWM_DUTY_MID     750                                              // 中间占空比 0.75ms (90度)
#define SERVO_ANGLE_MAX        180                                              // 舵机最大角度180度
#define SERVO_ANGLE_MIN        0                                                // 舵机最小角度0度


// 使用PWM_TIM_A0的通道
#define SERVO_X_PWM                PWM_TIM_A1_CH0_B4                             // 舵机1使用A0引脚
#define SERVO_Y_PWM                PWM_TIM_A1_CH1_B5                             // 舵机2使用A1引脚

// 舵机ID定义
typedef enum
{
    SERVO1,                                                                     // 舵机1
    SERVO2,                                                                     // 舵机2
    SERVO_NUM,                                                                  // 舵机数量
} servo_index_enum;

//-------------------------------------------------------------------------------------------------------------------
// 函数声明
//-------------------------------------------------------------------------------------------------------------------
void    servo_init           (void);                                            // 舵机初始化
void    servo_set_angle      (servo_index_enum servo_n, uint16 angle);         // 设置舵机角度(0-180度)
void    servo_set_duty       (servo_index_enum servo_n, uint32 duty);          // 直接设置舵机PWM占空比
void    servo_set_middle     (servo_index_enum servo_n);                       // 设置舵机到中间位置(90度)
void    servo_set_min        (servo_index_enum servo_n);                       // 设置舵机到最小位置(0度)
void    servo_set_max        (servo_index_enum servo_n);                       // 设置舵机到最大位置(180度)
uint32  servo_angle_to_duty  (uint16 angle);                                   // 角度转换为占空比
uint16  servo_duty_to_angle  (uint32 duty);                                    // 占空比转换为角度

static inline float angle_to_duty_f(float angle);
void    draw_sine_wave_float(void);
void    draw_cosine_wave_float(void);
void    draw_triangle_wave_float(void);
void    draw_square_wave_float(void);

#endif 