#include "../../public/window/window_setting.h"

void set_console_color(EConsoleColor font_color, EConsoleColor background_color)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, (unsigned short)font_color + (unsigned short)background_color * 0x10);
}
