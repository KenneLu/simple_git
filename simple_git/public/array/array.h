#pragma once

#include "../minimal.h"

// 当前文件是 18 章的【simple_array 容器】移植而来

typedef struct
{
    char str_data[8196];
} FStringData;

typedef struct
{
    int size;
    FStringData* array_data;
} FArray;


_CRT_BEGIN_C_HEADER
void init_array(FArray* c_array); //初始化 分配内存
void destroy_array(FArray* c_array); // 释放内存
void add_array(FArray* c_array, char const* element); // 添加元素
void printf_array(FArray* c_array); // 打印数组
char* get_array_element(FArray* c_array, int index); // 按索引获取元素
bool array_element_exist(FArray* c_array, char const* element); // 判断元素是否存在
_CRT_END_C_HEADER
