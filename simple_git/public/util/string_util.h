#pragma once

#include "../minimal.h"
#include "../../public/array/array.h"

_CRT_BEGIN_C_HEADER
int find_string(char* str, char const* sub_str); // 找到 sub_str 第一次出现的位置
char* cut_string(const char* str, int cut_start, int cut_len); // 切割字符串
void split_string(const char* str, const char* delim, FArray* out_array); // 拆分字符串，返回数组
char* split_string_with_index(const char* str, const char* delim, char* out_str, int index); // 拆分字符串，返回某个元素

void remove_char_start(char* str, char sub_char); // 移除第一个出现的 sub_char
void remove_char_end(char* str, char sub_char); // 移除最后一个出现的 sub_char
void remove_all_char(char* str, char sub_char); // 移除所有 sub_char
void replace_all_char(char* str, const char old_char, const char new_char); // 替换所有 old_char 为 new_char

void remove_string_start(char* str, char const* sub_str); // 移除第一个出现的 sub_str

int placeholder_cat(char* output, char* format, ...); // 拼接占位符(example版)
int placeholder_cat_s(char* output, char* format, ...); // 拼接占位符(专业版)

_CRT_END_C_HEADER
