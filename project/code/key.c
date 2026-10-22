#include "key.h"
#include <string.h> // 用于 strlen 和 sprintf

// 【注意】请确保包含了所有硬件驱动函数的头文件, 尤其是 uart_write_buffer
// #include "zf_common_headfile.h"

// ===================================================================
// 1. 串口发送函数 (已修改为协议版)
// ===================================================================

/**
 * @brief 发送一个任务触发命令 (4字节协议)
 * @param cmd 要发送的单字符命令, e.g., '2', '3', 'S'
 */
void send_task_command(char cmd)
{
    uint8_t packet[4];
    packet[0] = 0xAA;         // 包头1
    packet[1] = 0x55;         // 包头2
    packet[2] = (uint8_t)cmd; // 命令
    packet[3] = (uint8_t)cmd; // 校验和 (就是命令本身)
    uart_write_buffer(OPENMV_UART, packet, 4);
}

/**
 * @brief 发送一个阈值调节命令 (7字节协议)
 * @param is_white 1表示白色阈值, 0表示黑色
 * @param index 参数索引 (0-5)
 * @param value 参数的新值
 */
void send_threshold_command(uint8_t is_white, uint8_t index, int8_t value)
{
    uint8_t packet[7];
    packet[0] = 0xAA;           // 包头1
    packet[1] = 0x55;           // 包头2
    packet[2] = 0xA1;           // 阈值调节的特殊命令码
    packet[3] = is_white;       // 颜色 1=white, 0=black
    packet[4] = index;          // L/A/B的min/max索引 (0-5)
    packet[5] = (uint8_t)value; // 新的阈值
    // 计算校验和
    packet[6] = (uint8_t)(packet[2] + packet[3] + packet[4] + packet[5]);
    uart_write_buffer(OPENMV_UART, packet, 7);
}

// ===================================================================
// 2. UI状态和变量定义 (保持不变)
// ===================================================================
typedef enum
{
    PAGE_MAIN,
    PAGE_ADJUST
} ui_page_t;

static ui_page_t current_page = PAGE_MAIN;
static uint8_t menu_active = 0;
int start_car_flag = 0;
int laps_count = 1;
static uint8_t main_page_selection = 0;
static uint8_t adjust_page_selection = 0;
static uint8_t adjust_is_white = 1;

static struct
{
    int8_t L_max_w, L_min_w, A_max_w, A_min_w, B_max_w, B_min_w;
    int8_t L_max_b, L_min_b, A_max_b, A_min_b, B_max_b, B_min_b;
} image_params = {71, 52, -4, -19, 10, -125, 22, 4, -6, -127, 27, -128};

// 函数前向声明
void display_main_page(void);
void display_adjust_page(void);

// ===================================================================
// 3. 显示函数 (保持不变)
// ===================================================================

/** @brief 绘制主页面 */
void display_main_page(void)
{
    char buf[22];
    int y = 5;
    tft180_clear();

    tft180_set_color(main_page_selection == 0 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "%c Car: %s", main_page_selection == 0 ? '>' : ' ', start_car_flag ? "R" : "S");
    tft180_show_string(5, y, buf);
    y += 10;

    tft180_set_color(main_page_selection == 1 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "%c Laps: %d", main_page_selection == 1 ? '>' : ' ', laps_count);
    tft180_show_string(5, y, buf);
    y += 10;

    tft180_set_color(main_page_selection == 2 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "%c Task 2", main_page_selection == 2 ? '>' : ' ');
    tft180_show_string(5, y, buf);
    y += 10;

    tft180_set_color(main_page_selection == 3 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "%c Task 3", main_page_selection == 3 ? '>' : ' ');
    tft180_show_string(5, y, buf);
    y += 10;

    tft180_set_color(main_page_selection == 4 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "%c Task 4", main_page_selection == 4 ? '>' : ' ');
    tft180_show_string(5, y, buf);
    y += 10;

    tft180_set_color(main_page_selection == 5 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "%c Task 5", main_page_selection == 5 ? '>' : ' ');
    tft180_show_string(5, y, buf);
    y += 10;

    tft180_set_color(main_page_selection == 6 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "%c Adjust", main_page_selection == 6 ? '>' : ' ');
    tft180_show_string(5, y, buf);
    y += 10;
    tft180_set_color(RGB565_CYAN, RGB565_BLACK);
    tft180_show_string(5, 145, "K1/2:Sel K3/4:Action");
}

/** @brief 绘制调参页面 */
void display_adjust_page(void)
{
    char buf[22];
    int y = 5;
    tft180_clear();

    if (adjust_is_white)
    {
        tft180_set_color(RGB565_YELLOW, RGB565_BLACK);
        tft180_show_string(5, y, "Adj: WHITE");
        y += 15;

        tft180_set_color(adjust_page_selection == 0 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c L_min: %d", adjust_page_selection == 0 ? '>' : ' ', image_params.L_min_w);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 1 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c L_max: %d", adjust_page_selection == 1 ? '>' : ' ', image_params.L_max_w);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 2 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c A_min: %d", adjust_page_selection == 2 ? '>' : ' ', image_params.A_min_w);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 3 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c A_max: %d", adjust_page_selection == 3 ? '>' : ' ', image_params.A_max_w);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 4 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c B_min: %d", adjust_page_selection == 4 ? '>' : ' ', image_params.B_min_w);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 5 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c B_max: %d", adjust_page_selection == 5 ? '>' : ' ', image_params.B_max_w);
        tft180_show_string(5, y, buf);
        y += 10;
    }
    else
    {
        tft180_set_color(RGB565_YELLOW, RGB565_BLACK);
        tft180_show_string(5, y, "Adj: BLACK");
        y += 15;

        tft180_set_color(adjust_page_selection == 0 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c L_min: %d", adjust_page_selection == 0 ? '>' : ' ', image_params.L_min_b);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 1 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c L_max: %d", adjust_page_selection == 1 ? '>' : ' ', image_params.L_max_b);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 2 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c A_min: %d", adjust_page_selection == 2 ? '>' : ' ', image_params.A_min_b);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 3 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c A_max: %d", adjust_page_selection == 3 ? '>' : ' ', image_params.A_max_b);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 4 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c B_min: %d", adjust_page_selection == 4 ? '>' : ' ', image_params.B_min_b);
        tft180_show_string(5, y, buf);
        y += 10;
        tft180_set_color(adjust_page_selection == 5 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
        sprintf(buf, "%c B_max: %d", adjust_page_selection == 5 ? '>' : ' ', image_params.B_max_b);
        tft180_show_string(5, y, buf);
    }

    tft180_set_color(adjust_page_selection == 6 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "%c Switch Color", adjust_page_selection == 6 ? '>' : ' ');
    tft180_show_string(5, 125, buf);

    tft180_set_color(adjust_page_selection == 7 ? RGB565_GREEN : RGB565_WHITE, RGB565_BLACK);
    sprintf(buf, "%c Back", adjust_page_selection == 7 ? '>' : ' ');
    tft180_show_string(5, 140, buf);
}

// ===================================================================
// 4. 按键处理逻辑 (已修改为调用新函数)
// ===================================================================

void handle_keys_main(uint8_t p[4])
{
    if (p[0])
        main_page_selection = (main_page_selection == 0) ? 6 : (main_page_selection - 1);
    if (p[1])
        main_page_selection = (main_page_selection + 1) % 7;

    if (p[2])
    { // K3 按下
        if (main_page_selection == 0)
            start_car_flag = !start_car_flag;
        else if (main_page_selection == 1)
            laps_count++;
        // 【已修改】调用新的协议发送函数
        else if (main_page_selection == 2)
            send_task_command('2');
        else if (main_page_selection == 3)
            send_task_command('3');
        else if (main_page_selection == 4)
            send_task_command('4');
        else if (main_page_selection == 5)
            send_task_command('5');
        else if (main_page_selection == 6)
        {
            current_page = PAGE_ADJUST;
            adjust_page_selection = 0;
            display_adjust_page();
            return;
        }
    }
    if (p[3])
    { // K4 按下
        if (main_page_selection == 1 && laps_count > 1)
            laps_count--;
    }
    display_main_page();
}

void handle_keys_adjust(uint8_t p[4])
{
    if (p[0])
        adjust_page_selection = (adjust_page_selection == 0) ? 7 : (adjust_page_selection - 1);
    if (p[1])
        adjust_page_selection = (adjust_page_selection + 1) % 8;

    int change = p[2] ? 5 : (p[3] ? -5 : 0);

    if (change != 0)
    {
        if (adjust_page_selection < 6)
        {
            // 【已修改】调用新的协议发送函数
            int8_t val_temp = 0;
            if (adjust_is_white)
            {
                if (adjust_page_selection == 0)
                {
                    image_params.L_min_w += change;
                    val_temp = image_params.L_min_w;
                }
                else if (adjust_page_selection == 1)
                {
                    image_params.L_max_w += change;
                    val_temp = image_params.L_max_w;
                }
                else if (adjust_page_selection == 2)
                {
                    image_params.A_min_w += change;
                    val_temp = image_params.A_min_w;
                }
                else if (adjust_page_selection == 3)
                {
                    image_params.A_max_w += change;
                    val_temp = image_params.A_max_w;
                }
                else if (adjust_page_selection == 4)
                {
                    image_params.B_min_w += change;
                    val_temp = image_params.B_min_w;
                }
                else if (adjust_page_selection == 5)
                {
                    image_params.B_max_w += change;
                    val_temp = image_params.B_max_w;
                }
                send_threshold_command(1, adjust_page_selection, val_temp);
            }
            else
            {
                if (adjust_page_selection == 0)
                {
                    image_params.L_min_b += change;
                    val_temp = image_params.L_min_b;
                }
                else if (adjust_page_selection == 1)
                {
                    image_params.L_max_b += change;
                    val_temp = image_params.L_max_b;
                }
                else if (adjust_page_selection == 2)
                {
                    image_params.A_min_b += change;
                    val_temp = image_params.A_min_b;
                }
                else if (adjust_page_selection == 3)
                {
                    image_params.A_max_b += change;
                    val_temp = image_params.A_max_b;
                }
                else if (adjust_page_selection == 4)
                {
                    image_params.B_min_b += change;
                    val_temp = image_params.B_min_b;
                }
                else if (adjust_page_selection == 5)
                {
                    image_params.B_max_b += change;
                    val_temp = image_params.B_max_b;
                }
                send_threshold_command(0, adjust_page_selection, val_temp);
            }
        }
        else if (adjust_page_selection == 6 && p[2])
        { // Switch Color
            adjust_is_white = !adjust_is_white;
        }
        else if (adjust_page_selection == 7 && p[2])
        { // Back
            current_page = PAGE_MAIN;
            display_main_page();
            return;
        }
    }
    display_adjust_page();
}

/** @brief 按键扫描总入口 */
void KEY_Scan(void)
{
    static uint8_t key_last[4] = {1, 1, 1, 1};
    uint8_t key_now[4] = {gpio_get_level(KEY1), gpio_get_level(KEY2), gpio_get_level(KEY3), gpio_get_level(KEY4)};
    uint8_t key_pressed[4] = {0};

    for (int i = 0; i < 4; i++)
    {
        if (!key_now[i] && key_last[i])
            key_pressed[i] = 1;
        key_last[i] = key_now[i];
    }

    if (!key_pressed[0] && !key_pressed[1] && !key_pressed[2] && !key_pressed[3])
        return;

    if (!menu_active)
    {
        menu_active = 1;
        display_main_page();
        return;
    }

    if (current_page == PAGE_MAIN)
    {
        handle_keys_main(key_pressed);
    }
    else
    {
        handle_keys_adjust(key_pressed);
    }
}

// ===================================================================
// 5. 初始化函数 (保持不变)
// ===================================================================
void KEY_Init(void)
{
    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);

    menu_active = 0;
    current_page = PAGE_MAIN;

    tft180_clear();
    tft180_set_color(RGB565_WHITE, RGB565_BLACK);
    tft180_show_string(15, 50, "System Ready");
    tft180_show_string(10, 70, "Press Key for Menu");
}