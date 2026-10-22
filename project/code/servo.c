
#include "servo.h"
#include "zf_common_headfile.h"



// **************************** 代码区域 ****************************

// PWM通道数组
static pwm_channel_enum servo_pwm_map[SERVO_NUM] = 
{
    SERVO_X_PWM,                                                                // 舵机1
    SERVO_Y_PWM,                                                                // 舵机2
};

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     角度转换为占空比
// 参数说明     angle       角度 0-180
// 返回参数     uint32      占空比值 250-1250
// 使用示例     uint32 duty = servo_angle_to_duty(90);
// 备注信息     舵机角度范围0-180度，对应PWM脉宽0.25ms-1.25ms
//-------------------------------------------------------------------------------------------------------------------
uint32 servo_angle_to_duty(uint16 angle)
{
    uint32 duty;
    
    // 角度限幅
    if(angle > SERVO_ANGLE_MAX)
        angle = SERVO_ANGLE_MAX;
    
    // 线性映射：0度->250, 180度->1250
    duty = SERVO_PWM_DUTY_MIN + (uint32)((float)angle * (SERVO_PWM_DUTY_MAX - SERVO_PWM_DUTY_MIN) / SERVO_ANGLE_MAX);
    
    return duty;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     占空比转换为角度
// 参数说明     duty        占空比 250-1250
// 返回参数     uint16      角度值 0-180
// 使用示例     uint16 angle = servo_duty_to_angle(750);
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
uint16 servo_duty_to_angle(uint32 duty)
{
    uint16 angle;
    
    // 占空比限幅
    if(duty < SERVO_PWM_DUTY_MIN)
        duty = SERVO_PWM_DUTY_MIN;
    else if(duty > SERVO_PWM_DUTY_MAX)
        duty = SERVO_PWM_DUTY_MAX;
    
    // 线性映射：250->0度, 1250->180度
    angle = (uint16)((float)(duty - SERVO_PWM_DUTY_MIN) * SERVO_ANGLE_MAX / (SERVO_PWM_DUTY_MAX - SERVO_PWM_DUTY_MIN));
    
    return angle;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     舵机初始化
// 参数说明     void
// 返回参数     void
// 使用示例     servo_init();
// 备注信息     初始化所有舵机PWM通道，频率50Hz，初始位置90度
//-------------------------------------------------------------------------------------------------------------------
void servo_init(void)
{
    uint8 i;
    
    // 初始化所有舵机PWM通道
    for(i = 0; i < SERVO_NUM; i++)
    {
        // 初始化PWM，频率50Hz，初始占空比750（90度）
        // pwm_init函数内部会自动调用timer_clock_enable和timer_funciton_check
        pwm_init(servo_pwm_map[i], SERVO_PWM_FREQ, SERVO_PWM_DUTY_MID);
    }
    
    // 延时等待舵机稳定
    system_delay_ms(1000);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     设置舵机角度
// 参数说明     servo_n     舵机编号
// 参数说明     angle       角度 0-180
// 返回参数     void
// 使用示例     servo_set_angle(SERVO1, 90);
// 备注信息     舵机角度范围0-180度
//-------------------------------------------------------------------------------------------------------------------
void servo_set_angle(servo_index_enum servo_n, uint16 angle)
{
    uint32 duty;
    
    // 参数检查
    if(servo_n >= SERVO_NUM)
        return;
    
    // 角度转换为占空比
    duty = servo_angle_to_duty(angle);
    
    // 设置PWM占空比
    pwm_set_duty(servo_pwm_map[servo_n], duty);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     直接设置舵机PWM占空比
// 参数说明     servo_n     舵机编号
// 参数说明     duty        占空比 500-2500
// 返回参数     void
// 使用示例     servo_set_duty(SERVO1, 1500);
// 备注信息     
//-------------------------------------------------------------------------------------------------------------------
void servo_set_duty(servo_index_enum servo_n, uint32 duty)
{
    // 参数检查
    if(servo_n >= SERVO_NUM)
        return;
    
    // 占空比限幅
    if(duty < SERVO_PWM_DUTY_MIN)
        duty = SERVO_PWM_DUTY_MIN;
    else if(duty > SERVO_PWM_DUTY_MAX)
        duty = SERVO_PWM_DUTY_MAX;
    
    // 设置PWM
    pwm_set_duty(servo_pwm_map[servo_n], duty);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     设置舵机到中间位置
// 参数说明     servo_n     舵机编号
// 返回参数     void
// 使用示例     servo_set_middle(SERVO1);
// 备注信息     设置舵机到90度位置
//-------------------------------------------------------------------------------------------------------------------
void servo_set_middle(servo_index_enum servo_n)
{
    servo_set_angle(servo_n, 90);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     设置舵机到最小位置
// 参数说明     servo_n     舵机编号
// 返回参数     void
// 使用示例     servo_set_min(SERVO1);
// 备注信息     设置舵机到0度位置
//-------------------------------------------------------------------------------------------------------------------
void servo_set_min(servo_index_enum servo_n)
{
    servo_set_angle(servo_n, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     设置舵机到最大位置
// 参数说明     servo_n     舵机编号
// 返回参数     void
// 使用示例     servo_set_max(SERVO1);
// 备注信息     设置舵机到180度位置
//-------------------------------------------------------------------------------------------------------------------
static inline float angle_to_duty_f(float angle)
{
    if(angle < 0) angle = 0;
    if(angle > 180) angle = 180;
    return 250.0f + (angle / 180.0f) * (1250.0f - 250.0f);
}

void draw_sine_wave_float(void)
{
    for(int i=0; i<SCREEN_POINTS; i++)
    {
        float t = (float)i / (SCREEN_POINTS-1); // 0~1
        float x_angle = X_SERVO_MIN + (X_SERVO_MAX-X_SERVO_MIN) * t;
        float y_norm = 0.5f + 0.4f * sinf(2 * 3.1415926f * t);
        float y_angle = Y_SERVO_MIN + (Y_SERVO_MAX-Y_SERVO_MIN) * y_norm;

        float x_duty = angle_to_duty_f(x_angle);
        float y_duty = angle_to_duty_f(y_angle);

        servo_set_duty(SERVO1, (uint32)x_duty);
        servo_set_duty(SERVO2, (uint32)y_duty);

        system_delay_ms(10); // 速度可调
    }
}

// 余弦波
void draw_cosine_wave_float(void)
{
    for(int i=0; i<SCREEN_POINTS; i++)
    {
        float t = (float)i / (SCREEN_POINTS-1);
        float x_angle = X_SERVO_MIN + (X_SERVO_MAX-X_SERVO_MIN) * t;
        float y_norm = 0.5f + 0.4f * cosf(2 * 3.1415926f * t);
        float y_angle = Y_SERVO_MIN + (Y_SERVO_MAX-Y_SERVO_MIN) * y_norm;

        float x_duty = angle_to_duty_f(x_angle);
        float y_duty = angle_to_duty_f(y_angle);

        servo_set_duty(SERVO1, (uint32)x_duty);
        servo_set_duty(SERVO2, (uint32)y_duty);

        system_delay_ms(10);
    }
}

// 三角波
void draw_triangle_wave_float(void)
{
    for(int i=0; i<SCREEN_POINTS; i++)
    {
        float t = (float)i / (SCREEN_POINTS-1);
        float x_angle = X_SERVO_MIN + (X_SERVO_MAX-X_SERVO_MIN) * t;
        // 三角波公式：y = 2 * |2*(t-0.25) - floor(2*(t-0.25)+0.5)| - 1
        float tri = 2.0f * fabsf(2.0f * (t - 0.25f) - floorf(2.0f * (t - 0.25f) + 0.5f)) - 1.0f;
        float y_norm = 0.5f + 0.4f * tri;
        float y_angle = Y_SERVO_MIN + (Y_SERVO_MAX-Y_SERVO_MIN) * y_norm;

        float x_duty = angle_to_duty_f(x_angle);
        float y_duty = angle_to_duty_f(y_angle);

        servo_set_duty(SERVO1, (uint32)x_duty);
        servo_set_duty(SERVO2, (uint32)y_duty);

        system_delay_ms(10);
    }
}

// 方波
void draw_square_wave_float(void)
{
    for(int i=0; i<SCREEN_POINTS; i++)
    {
        float t = (float)i / (SCREEN_POINTS-1);
        float x_angle = X_SERVO_MIN + (X_SERVO_MAX-X_SERVO_MIN) * t;
        float y_norm = (fmodf(t, 0.2f) < 0.1f) ? 0.9f : 0.1f; // 0.9高，0.1低
        float y_angle = Y_SERVO_MIN + (Y_SERVO_MAX-Y_SERVO_MIN) * y_norm;

        float x_duty = angle_to_duty_f(x_angle);
        float y_duty = angle_to_duty_f(y_angle);

        servo_set_duty(SERVO1, (uint32)x_duty);
        servo_set_duty(SERVO2, (uint32)y_duty);

        system_delay_ms(10);
    }
}
