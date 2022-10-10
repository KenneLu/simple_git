#pragma once

#include "../minimal.h"

// 当前文件是 19 章的【文件检索】移植而来

_CRT_BEGIN_C_HEADER
typedef struct
{
	int index;
	char paths[8196][1024];//MAX_PATH【分配的空间太大会出现溢出】
}FFilePath;

void init_file_path(FFilePath *file_path);
int copy_file(const char* Dest, const char* Src); // 拷贝文件 Src to Dest
void find_files(char const *folder_path, FFilePath *out_file_path, bool b_recursion); // 这是寻找文件并返回找到的文件地址，还是判断文件是否存在？【*】
bool create_file(char const *file_path); // 创建文件
bool create_folder_path(char const *folder_path); // 创建全路径文件夹
bool read_file(const char *file_path,char *out_content); // 读文件
bool add_file(const char *file_path, char *content); // 添加内容
bool rewrite_file(const char *file_path, char *content); // 重写文件
unsigned int get_file_size(const char *file_path); // 获取文件大小
unsigned int get_file_size_by_handle(FILE *file_handle); // 获取文件大小
_CRT_END_C_HEADER