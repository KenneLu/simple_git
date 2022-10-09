#pragma once

#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h> // HKEY 关键字, _CRT_BEGIN_C_HEADER 关键字, _CRT_END_C_HEADER 关键字
#include <vcruntime.h> // _CRT_BEGIN_C_HEADER 和 _CRT_END_C_HEADER 这俩用来包含 Windows SDK Version
#include <assert.h>
#include <stdio.h>
#include <stdbool.h> // 加了这个 c 才能像 c++ 一样支持 bool 类型
#include <corecrt_wstdio.h>
#include <stdarg.h>
#include <combaseapi.h> // Windows GUID CoCreateGuid
#include <errno.h>
#include <stdlib.h>
#include <string.h> // strtok() 库函数
#include <io.h>
#include <direct.h>
#include <time.h>