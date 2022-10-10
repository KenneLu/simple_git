#pragma once

#include "../minimal.h"

typedef struct
{
    int size;
    char data[8196][1024];
} FPath;

_CRT_BEGIN_C_HEADER
void string_to_path(const char* str, FPath* f_path); // 字符串处理
void path_to_string(char* str, const FPath* f_path); // 字符串处理
void get_git_file_folder_inline(char *file_path); // 获取文件所在文件夹(改变 file_path 值)
void get_file_folder(char *out_folder, const char *file_path); // 获取文件所在文件夹(不改变 file_path 值)
void get_path_filename(char *filename, const char *file_path); // 获取路径文件名
void normalization_path(char *c_path); // 归一化(或单位化)【把 "\\" 改为 "/"】
_CRT_END_C_HEADER
