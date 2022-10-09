#pragma once

#include "../minimal.h"

_CRT_BEGIN_C_HEADER
typedef enum 
{
	BLACK = 0,			// 黑色		
	BLUE,				// 蓝色		
	GREEN,				// 绿色		
	LIGHT_BLUE,			// 浅绿色	
	RED,				// 红色		
	VIOLET,				// 紫色		
	YELLOW,				// 黄色		
	WHITE,				// 白色		
	GREY,				// 灰色
	PALE_BLUE,			// 淡蓝色
	PALE_GREEN,			// 淡绿色
	LIGHT_GREEN,		// 淡浅绿色
	PALE_RED,			// 淡红色
	LAVENDER,			// 淡紫色
	CANARY_YELLOW,		// 淡黄色
	BRIGHT_WHITE,		// 亮白色
} EConsoleColor;

// set_console_color
void set_console_color(EConsoleColor font_color, EConsoleColor background_color);
_CRT_END_C_HEADER