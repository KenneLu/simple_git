#pragma once

#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h> // HKEY �ؼ���, _CRT_BEGIN_C_HEADER �ؼ���, _CRT_END_C_HEADER �ؼ���
#include <vcruntime.h> // _CRT_BEGIN_C_HEADER �� _CRT_END_C_HEADER ������������ Windows SDK Version
#include <assert.h>
#include <stdio.h>
#include <stdbool.h> // ������� c ������ c++ һ��֧�� bool ����
#include <corecrt_wstdio.h>
#include <stdarg.h>
#include <combaseapi.h> // Windows GUID CoCreateGuid
#include <errno.h>
#include <stdlib.h>
#include <string.h> // strtok() �⺯��
#include <io.h>
#include <direct.h>
#include <time.h>