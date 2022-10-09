#pragma once

#include "../minimal.h"

// 当前文件是 19 章的【文件检索】移植而来

_CRT_BEGIN_C_HEADER
typedef struct
{
	int index;
	char paths[8196][1024];//MAX_PATH【分配的空间太大会出现溢出】
}FFilePath;

void init_def_c_paths(FFilePath *c_paths);
int copy_file(const char* Src, const char* Dest); // 拷贝文件 Src to Dest
void find_files(char const *in_path, FFilePath *str,bool b_recursion);
bool create_file(char const *filename); // 创建文件
bool create_folder_path(char const *folder_path); // 创建全路径文件夹
bool read_file(const char *file_path,char *buf);
bool add_file(const char *file_path, char *buf);
bool rewrite_file(const char *file_path, char *buf);
unsigned int get_file_size_by_filename(const char *filename);
unsigned int get_file_size(FILE *file_handle);
_CRT_END_C_HEADER