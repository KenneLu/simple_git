#pragma once
#include "../minimal.h"

typedef enum
{
	MSG_SUCCESS = 0,
	MSG_LOG,
	MSG_WARNING,
	MSG_ERROR,
} EMessageType;

_CRT_BEGIN_C_HEADER

void set_log_folder(const char *log_folder_path);	// 配置 log 文件夹
const char *get_log_folder();			// 获取配置好的 log 文件夹【似乎没有被引用】【*】
const char *get_log_file();				// 获取 log 文件，没有则生成一个
bool logger(EMessageType type, char *format, ...); // 写入日志

// 以下写法是利用宏定义省去填写 message_type
#define log_success(...) logger(MSG_SUCCESS,__VA_ARGS__)
#define log_log(...) logger(MSG_LOG,__VA_ARGS__)
#define log_error(...) logger(MSG_ERROR,__VA_ARGS__)
#define log_warning(...) logger(MSG_WARNING,__VA_ARGS__)

_CRT_END_C_HEADER