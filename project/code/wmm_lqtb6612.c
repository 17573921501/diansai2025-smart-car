#include "wmm_lqtb6612.h"
#include "zf_common_headfile.h"

/************************************************************** */
/**********************编码器部分****************************** */
/************************************************************** */

// 在isr.c中定义了编码器中断处理函数，定时器计数

/************************************************************** */
/**********************编码器部分****************************** */
/************************************************************** */

/************************************************************** */
/**********************电机驱动部分****************************** */
/************************************************************** */

// float Velcity_Kp = 0.6, Velcity_Ki = 0.5, Velcity_Kd = 0.2; // 相关速度PID参数
// float Velcity_Kp = 0.5, Velcity_Ki = 0, Velcity_Kd = 0; // 相关速度PID参数
float Velcity_Kp = 20, Velcity_Ki = 1, Velcity_Kd = 1; // 相关速度PID参数lj

volatile int32 TargetSpeed_L, TargetSpeed_R;

/**
 * @brief 控制电机输出的函数
 *
 * 该函数根据输入的PWM值控制电机的转动方向和速度。
 * 如果PWM值为正，则电机正转；如果PWM值为负，则电机反转。
 * PWM值的绝对值决定了电机的转速。
 * 如果小车实际运行方向与代码逻辑相反，则交换一下方向控制引脚的高低逻辑即可。(AIN1和AIN2, BIN1和BIN2)
 *
 * 在zf_driver_pwm.h 中定义 #define PWM_DUTY_MAX        ( 10000 ) // PWM最大占空比
 *
 * @param PWMA 电机A的PWM值，正数表示正转，负数表示反转，绝对值表示转速
 * @param PWMB 电机B的PWM值，正数表示正转，负数表示反转，绝对值表示转速
 */
void Motor_Out(int PWM_L, int PWM_R)
{
    // 如果电机A的PWM值为正，则设置电机A正转
    if (PWM_R >= 0)
    {
        gpio_set_level(TB6612_AIN1_PIN, GPIO_LOW); // 设置AIN1引脚为高电平
        gpio_set_level(TB6612_AIN2_PIN,GPIO_HIGH);  // 设置AIN2引脚为低电平
        pwm_set_duty(TB6612_PWMA, PWM_R);           // 设置电机A的PWM占空比
    }
    // 如果电机A的PWM值为负，则设置电机A反转
    else
    {
        gpio_set_level(TB6612_AIN1_PIN, GPIO_HIGH);  // 设置AIN1引脚为低电平
        gpio_set_level(TB6612_AIN2_PIN, GPIO_LOW); // 设置AIN2引脚为高电平
        pwm_set_duty(TB6612_PWMA, ABS(PWM_R));          // 设置电机A的PWM占空比为PWM值的绝对值
    }

    // 如果电机B的PWM值为正，则设置电机B正转
    if (PWM_L >= 0)
    {
        gpio_set_level(TB6612_BIN1_PIN, GPIO_LOW); // 设置BIN1引脚为高电平
        gpio_set_level(TB6612_BIN2_PIN, GPIO_HIGH);  // 设置BIN2引脚为低电平
        pwm_set_duty(TB6612_PWMB, PWM_L);           // 设置电机B的PWM占空比
    }
    // 如果电机B的PWM值为负，则设置电机B反转
    else
    {

        gpio_set_level(TB6612_BIN1_PIN, GPIO_HIGH);  // 设置BIN1引脚为低电平
        gpio_set_level(TB6612_BIN2_PIN, GPIO_LOW); // 设置BIN2引脚为高电平
        pwm_set_duty(TB6612_PWMB, ABS(PWM_L));          // 设置电机B的PWM占空比为PWM值的绝对值
    }
}

// /************************以下代码来自轮趣************************************/
/************************************************************************
 * @brief 计算速度控制值，使用增量式PI控制器。
 *
 * 该函数根据目标速度和当前速度计算速度偏差，并使用增量式PI控制器来调整速度控制值。
 * 控制值通过积分和比例两部分组成，用于调节速度以达到目标速度。
 *
 * @param TargetVelocity 目标速度值。
 * @param CurrentVelocity 当前速度值。
 ***************************************************************************************/
int Velocity_A(int TargetVelocity, int CurrentVelocity)
{
    int Bias; // 定义相关变量
    static int ControlVelocityA,
        Last_biasA; // 静态变量，函数调用结束后其值依然存在

    Bias = TargetVelocity - CurrentVelocity; // 求速度偏差

    ControlVelocityA +=
        Velcity_Ki * (Bias - Last_biasA) +
        Velcity_Kp * Bias; // 增量式PI控制器
                           // Velcity_Kp*(Bias-Last_bias) 作用为限制加速度
                           // Velcity_Ki*Bias 速度控制值由Bias不断积分得到
                           // 偏差越大加速度越大
    Last_biasA = Bias;
    if (ControlVelocityA > 6000)
        ControlVelocityA = 6000;
    else if (ControlVelocityA < -6000)
        ControlVelocityA = -6000;

    return ControlVelocityA; // 返回速度控制值
}

int Velocity_B(int TargetVelocity, int CurrentVelocity)
{
    int Bias; // 定义相关变量
    static int ControlVelocityB,
        Last_biasB; // 静态变量，函数调用结束后其值依然存在

    Bias = TargetVelocity - CurrentVelocity; // 求速度偏差

    ControlVelocityB +=
        Velcity_Ki * (Bias - Last_biasB) +
        Velcity_Kp * Bias; // 增量式PI控制器
                           // Velcity_Kp*(Bias-Last_bias) 作用为限制加速度
                           // Velcity_Ki*Bias 速度控制值由Bias不断积分得到
                           // 偏差越大加速度越大
    Last_biasB = Bias;
    if (ControlVelocityB > 6000)
        ControlVelocityB = 6000;
    else if (ControlVelocityB < -6000)
        ControlVelocityB = -6000;

    return ControlVelocityB; // 返回速度控制值
}

// /************************以下代码来自嘉立创************************************/

// /************************************************
// 功能：初始化PID各参数
// 参数：pid = 对应pid的结构体地址
//       p = pid的静态kp值
//       i = pid的静态ki值
//       d = pid的静态kd值
//       maxI = pid计算后的I最大值，即最大误差累加值
//       maxOut = PID最大输出值
//       target = 目标值
// ************************************************/
// void pid_init(PID* pid, float p, float i, float d, float maxI, float maxOut, int target)
// {
//     pid->kp = p;
//     pid->ki = i;
//     pid->kd = d;
//     pid->max_change_i = maxI;
//     pid->max_output = maxOut;
//     pid->target = target;
// }
// /******************************************************************
//  * 函 数 说 明：PID变化累计的参数清零
//  * 函 数 形 参：pid=对应的PID地址
// ******************************************************************/
// void pid_change_zero(PID* pid)
// {
//     pid->change_p = 0;
//     pid->change_i = 0;
//     pid->change_d = 0;
// }

// /****************************************************
// 功能：单级PID计算
// 参数：pid = pid的参数输入
//      target = 目标值
//      current = 当前值
// 返回：PID计算后的结果
// ****************************************************/
// float pid_calc(PID *pid, float target, float current)
// {
//     //用上一次的误差值更新 之前误差last_error
//     pid->last_error = pid->error;
//     //获取新的误差 = 目标值 - 当前值
//     pid->error = target - current;

//     //计算比例P = 目标值与实际值之间的误差e
//     float pout = pid->error;
//     //计算积分I = 误差e的累加
//     pid->change_i += pid->error;
//     //计算微分D = 当前误差e - 之前的误差last_e
//     float dout = pid->error - pid->last_error;

//     //积分I 限制不能超过正负最大值
//     if(pid->change_i > pid->max_change_i)
//     {
//       pid->change_i = pid->max_change_i;
//     }
//     else if(pid->change_i < -pid->max_change_i)
//     {
//       pid->change_i = -pid->max_change_i;
//     }

//     //计算输出PID_OUT = （Kp x P）+ （Ki x I）+（Kd x D）
//     pid->output = (pid->kp * pout) + (pid->ki * pid->change_i) + (pid->kd * dout);

//     //输出 限制不能超过正负最大值
//     if(pid->output > pid->max_output) pid->output = pid->max_output;
//     else if(pid->output < -pid->max_output) pid->output = -pid->max_output;

//     //返回PID计算的结果
//     return pid->output;
// }
// /******************************************************************
//  * 函 数 说 明：串级PID的计算函数
//  * 函 数 形 参：左到右依次是串级PID结构体,外环目标值,外环反馈值,内环反馈值
//  * 函 数 返 回：
//  * 作       者：
//  * 备       注：
// ******************************************************************/
// void pid_cascade_calc(CascadePID *pid, float outerRef, float outerFdb, int innerFdb)
// {
//     pid_calc(&pid->outer, outerRef, outerFdb); //计算外环
//     pid_calc(&pid->inner, pid->outer.output, innerFdb); //计算内环
//     //内环输出就是串级PID的输出
//     pid->output = pid->inner.output;
// }
// /////////////////////////////////////////////////////////////////////////////////////////////////
// //PID部分

// PID speed_pidA;
// PID speed_pidB;

// //定速PID初始化
// void speed_pid_init(void)
// {
//     pid_init(&speed_pidA, 35, 6, 10, 6000, 6000,100);
//     pid_init(&speed_pidB, 35, 6, 10, 6000, 6000,100);

// }
// //获取定速PID全局变量的地址
// PID* get_speed_pid(void)
// {
// 	return &speed_pid;
// }
// //获取定速PID的目标值
// int get_speed_pid_target(void)
// {
// 	return speed_pid.target;
// }

// /************************************************
// 功能：PID电机定速控制器
// 参数：target_speed = 目标值
// 返回：对应PID的地址
// ************************************************/
// PID motor_speed_control(int target_speed)
// {

// 	int PWM;

// 	PWM = pid_calc(&speed_pid, target_speed, get_encoder_count() );

// 	//控制刷新速度
// 	delay_cycles(80000*5);

// 	if( PWM > 0 )
// 	{
// 		set_motor(0, PWM);
// 	}
// 	else if( PWM < 0 )
// 	{
// 		PWM = -PWM;
// 		set_motor(PWM, 0);
// 	}
// 	return speed_pid;
// }
// // /************************以上代码来自嘉立创************************************/

/************************************************************** */
/**********************电机驱动部分****************************** */
/************************************************************** */
void TB6612_stop(void)
{
    // 停止电机输出，设置PWM占空比为0
    pwm_set_duty(TB6612_PWMA, 0);
    pwm_set_duty(TB6612_PWMB, 0);

    // 设置电机方向引脚为低电平，确保电机停止
    gpio_set_level(TB6612_AIN1_PIN, GPIO_LOW);
    gpio_set_level(TB6612_AIN2_PIN, GPIO_LOW);
    gpio_set_level(TB6612_BIN1_PIN, GPIO_LOW);
    gpio_set_level(TB6612_BIN2_PIN, GPIO_LOW);
}

void TB6612_init(void)
{
    // 电机方向引脚初始化
    gpio_init(TB6612_AIN1_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(TB6612_AIN2_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(TB6612_BIN1_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(TB6612_BIN2_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    // 电机PWM引脚初始化
    pwm_init(TB6612_PWMA, 17000, 0);
    pwm_init(TB6612_PWMB, 17000, 0);

    // 编码器外部中断初始化，带回调函数
    exti_init(TB6612_E1A_PIN, EXTI_TRIGGER_BOTH, EXIT_TB6612_E1A, NULL);
    exti_init(TB6612_E1B_PIN, EXTI_TRIGGER_BOTH, EXIT_TB6612_E1B, NULL);
    exti_init(TB6612_E2A_PIN, EXTI_TRIGGER_BOTH, EXIT_TB6612_E2A, NULL);
    exti_init(TB6612_E2B_PIN, EXTI_TRIGGER_BOTH, EXIT_TB6612_E2B, NULL);

    // 定时器测速,在isr.c中实现

    // interrupt_set_priority(GPIOA_INT_IRQn, 1); // 设置GPIOA中断优先级为1
    interrupt_global_enable(0); // 使能全局中断
}