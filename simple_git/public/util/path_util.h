#pragma once

#include "../minimal.h"

typedef struct
{
    int size;
    char data[8196][1024];
} FPath;

_CRT_BEGIN_C_HEADER
void string_to_path(const char* str, FPath* path); // �ַ�������
void path_to_string(char* str, const FPath* path); // �ַ�������
void get_get_file_folder_inline(char *path); // ��ȡ�ļ������ļ���(�ı� path ֵ)
void get_file_folder(char *output, const char *path); // ��ȡ�ļ������ļ���(���ı� path ֵ)
void get_path_filename(char *filename, const char *path); // ��ȡ·���ļ���
void normalization_path(char *path); // ��һ��(��λ��)���� "\\" ��Ϊ "/"��
_CRT_END_C_HEADER
