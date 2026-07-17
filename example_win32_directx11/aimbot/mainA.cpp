#include "mainA.h"
#include "opencv2/opencv.hpp"
#include "main.h"
#include <Windows.h>
#include <cmath>
#include "mouse_interface.h"

static mouse_interface mouse;

void aimbot::aim_to(int x, int y, int box_w, int box_h)
{
    const int screen_width = GetSystemMetrics(SM_CXSCREEN);
    const int screen_height = GetSystemMetrics(SM_CYSCREEN);

    x = static_cast<int>(screen_width / 2 - ACTIVATION_RANGE / 2 + x + box_w / 2 + 5);
    y = static_cast<int>(screen_height / 2 - ACTIVATION_RANGE / 2 + y + (101 - static_cast<int>(var::aim_height)) + box_h / 4);

    const int x_offset = x - screen_width / 2;
    const int y_offset = y - screen_height / 2;

    const double cursor_speed = var::smooth / 10.0;
    const int move_x = static_cast<int>(x_offset / cursor_speed);
    const int move_y = static_cast<int>(y_offset / cursor_speed);

    static double x_smooth = 0.0;
    static double y_smooth = 0.0;

    x_smooth = var::smoothing_factor * x_smooth + (1.0 - var::smoothing_factor) * move_x;
    y_smooth = var::smoothing_factor * y_smooth + (1.0 - var::smoothing_factor) * move_y;

    const double acceleration_factor = 0.1;
    x_smooth += (move_x - x_smooth) * acceleration_factor;
    y_smooth += (move_y - y_smooth) * acceleration_factor;

    mouse.move(static_cast<int>(x_smooth), static_cast<int>(y_smooth));
}
