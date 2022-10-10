#pragma once

#include "../minimal.h"

typedef struct
{
    int size;
    char data[8196][1024];
} FPath;

_CRT_BEGIN_C_HEADER
void string_to_path(const char* str, FPath* f_path); // �ַ�������
void path_to_string(char* str, const FPath* f_path); // �ַ�������
void get_git_file_folder_inline(char *file_path); // ��ȡ�ļ������ļ���(�ı� file_path ֵ)
void get_file_folder(char *out_folder, const char *file_path); // ��ȡ�ļ������ļ���(���ı� file_path ֵ)
void get_path_filename(char *filename, const char *file_path); // ��ȡ·���ļ���
void normalization_path(char *c_path); // ��һ��(��λ��)���� "\\" ��Ϊ "/"��
_CRT_END_C_HEADER
