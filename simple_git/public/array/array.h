#pragma once

#include "../minimal.h"

// ��ǰ�ļ��� 18 �µġ�simple_array ��������ֲ����

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
void init_array(FArray* c_array); //��ʼ�� �����ڴ�
void destroy_array(FArray* c_array); // �ͷ��ڴ�
void add_array(FArray* c_array, char const* element); // ���Ԫ��
void printf_array(FArray* c_array); // ��ӡ����
char* get_array_element(FArray* c_array, int index); // ��������ȡԪ��
bool array_element_exist(FArray* c_array, char const* element); // �ж�Ԫ���Ƿ����
_CRT_END_C_HEADER
