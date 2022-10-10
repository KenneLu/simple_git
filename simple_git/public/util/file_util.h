#pragma once

#include "../minimal.h"

// ��ǰ�ļ��� 19 �µġ��ļ���������ֲ����

_CRT_BEGIN_C_HEADER
typedef struct
{
	int index;
	char paths[8196][1024];//MAX_PATH������Ŀռ�̫�����������
}FFilePath;

void init_file_path(FFilePath *file_path);
int copy_file(const char* Dest, const char* Src); // �����ļ� Src to Dest
void find_files(char const *folder_path, FFilePath *out_file_path, bool b_recursion); // ����Ѱ���ļ��������ҵ����ļ���ַ�������ж��ļ��Ƿ���ڣ���*��
bool create_file(char const *file_path); // �����ļ�
bool create_folder_path(char const *folder_path); // ����ȫ·���ļ���
bool read_file(const char *file_path,char *out_content); // ���ļ�
bool add_file(const char *file_path, char *content); // �������
bool rewrite_file(const char *file_path, char *content); // ��д�ļ�
unsigned int get_file_size(const char *file_path); // ��ȡ�ļ���С
unsigned int get_file_size_by_handle(FILE *file_handle); // ��ȡ�ļ���С
_CRT_END_C_HEADER