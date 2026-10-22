#include "LF04.h"
#include "zf_common_headfile.h"

float error = 0;
float last_error = 0;
volatile int zhijiao_flag = 0;  // 直角弯标志
volatile int zhijaio_count = 0; // 直角弯计数

void LF04_Init(void)
{
    // 初始化4个引脚为下拉输入
    gpio_init(DH1_PIN, GPI, GPIO_HIGH, GPI_PULL_DOWN);
    gpio_init(DH2_PIN, GPI, GPIO_HIGH, GPI_PULL_DOWN);
    gpio_init(DH3_PIN, GPI, GPIO_HIGH, GPI_PULL_DOWN);
    gpio_init(DH4_PIN, GPI, GPIO_HIGH, GPI_PULL_DOWN);
}

// 历史 PID 差速方案 / TargetSpeed 开环差速方案的注释副本已删除，需要时翻 git 历史恢复

void Track_LF04(void)
{
    int DH_state = (DH1 << 3) | (DH2 << 2) | (DH3 << 1) | DH4; // 将传感器状态组合成一个整数
    switch (DH_state)
    {

    case 0: // 0000 十字路口 - 停止两秒且鸣笛后直线通过

        error = 0;
        break;

    case 1: // 0001 左直角弯

        error = -150;
        zhijiao_flag = 1; // 设置直角弯标志
        zhijaio_count++;
        break;

    case 8: // 1000 右直角弯

        error = 150;
        zhijiao_flag = 2; // 设置直角弯标志
        zhijaio_count++;
        break;

    case 7: // 0111 左大弯

        error = -100;
        break;

    case 14: // 1110 右大弯

        error = 100;
        break;

    case 11: // 1011 左微调

        error = -30;
        break;

    case 13: // 1101 右微调

        error = 30;
        break;

    case 9: // 1001 直行

        error = 0;
        break;
    case 15: // 1111 丢线情况
        if (zhijiao_flag == 1)
        {
            // 如果是左直角弯丢线，继续左转
            error = -150;
        }
        else if (zhijiao_flag == 2)
        {
            // 如果是右直角弯丢线，继续右转
            error = 150;
        }
        break;

    default:
        // 其他情况
        // error = 0;
        break;
    }
    // 位置式PID
    static float kp = 15, ki = 0, kd = 21;
    static float integral = 0;
    static float base_speed = 2500; // 可根据实际调整

    integral += error;
    float output = kp * error + ki * integral + kd * (error - last_error);

    last_error = error;

    // 计算左右电机PWM
    int left_pwm = base_speed + output;
    int right_pwm = base_speed - output;
    printf("pwmLRO:%d,%d,%f,%d\n", left_pwm, right_pwm, output, zhijaio_count); // 打印调试信息

    // 限幅
    if (left_pwm > PWM_DUTY_MAX - 2000)
        left_pwm = PWM_DUTY_MAX - 2000;
    if (left_pwm < -PWM_DUTY_MAX + 2000)
        left_pwm = -PWM_DUTY_MAX + 2000;
    if (right_pwm > PWM_DUTY_MAX - 2000)
        right_pwm = PWM_DUTY_MAX - 2000;
    if (right_pwm < -PWM_DUTY_MAX + 2000)
        right_pwm = -PWM_DUTY_MAX + 2000;
    // 确保PWM值在合理范围内

    // 输出到电机
    Motor_Out(left_pwm, right_pwm);
}
