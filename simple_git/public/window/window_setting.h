#pragma once

#include "../minimal.h"

_CRT_BEGIN_C_HEADER
typedef enum 
{
	BLACK = 0,			// ��ɫ		
	BLUE,				// ��ɫ		
	GREEN,				// ��ɫ		
	LIGHT_BLUE,			// ǳ��ɫ	
	RED,				// ��ɫ		
	VIOLET,				// ��ɫ		
	YELLOW,				// ��ɫ		
	WHITE,				// ��ɫ		
	GREY,				// ��ɫ
	PALE_BLUE,			// ����ɫ
	PALE_GREEN,			// ����ɫ
	LIGHT_GREEN,		// ��ǳ��ɫ
	PALE_RED,			// ����ɫ
	LAVENDER,			// ����ɫ
	CANARY_YELLOW,		// ����ɫ
	BRIGHT_WHITE,		// ����ɫ
} EConsoleColor;

void set_console_color(EConsoleColor font_color, EConsoleColor background_color); // ���ÿ���̨�����ɫ
_CRT_END_C_HEADER