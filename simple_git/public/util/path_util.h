#pragma once

#include "../minimal.h"

typedef struct
{
    int size;
    char data[8196][1024];
} FPath;

_CRT_BEGIN_C_HEADER
void string_to_path(const char* str, FPath* path); // 字符串处理
void path_to_string(char* str, const FPath* path); // 字符串处理
void get_get_file_folder_inline(char *path); // 获取文件所在文件夹(改变 path 值)
void get_file_folder(char *output, const char *path); // 获取文件所在文件夹(不改变 path 值)
void get_path_filename(char *filename, const char *path); // 获取路径文件名
void normalization_path(char *path); // 归一化(或单位化)【把 "\\" 改为 "/"】
_CRT_END_C_HEADER
