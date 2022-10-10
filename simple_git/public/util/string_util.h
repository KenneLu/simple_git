#pragma once

#include "../minimal.h"
#include "../../public/array/array.h"

_CRT_BEGIN_C_HEADER
int find_string(char* str, char const* sub_str); // �ҵ� sub_str ��һ�γ��ֵ�λ��
char* cut_string(const char* str, int cut_start, int cut_len); // �и��ַ���
void split_string(const char* str, const char* delim, FArray* out_array); // ����ַ�������������
char* split_string_with_index(const char* str, const char* delim, char* out_str, int index); // ����ַ���������ĳ��Ԫ��

void remove_char_start(char* str, char sub_char); // �Ƴ���һ�����ֵ� sub_char
void remove_char_end(char* str, char sub_char); // �Ƴ����һ�����ֵ� sub_char
void remove_all_char(char* str, char sub_char); // �Ƴ����� sub_char
void replace_all_char(char* str, const char old_char, const char new_char); // �滻���� old_char Ϊ new_char

void remove_string_start(char* str, char const* sub_str); // �Ƴ���һ�����ֵ� sub_str

int placeholder_cat(char* output, char* format, ...); // ƴ��ռλ��(example��)
int placeholder_cat_s(char* output, char* format, ...); // ƴ��ռλ��(רҵ��)

_CRT_END_C_HEADER
