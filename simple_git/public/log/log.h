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

void set_log_folder(const char *log_folder_path);	// ���� log �ļ���
const char *get_log_folder();			// ��ȡ���úõ� log �ļ��С��ƺ�û�б����á���*��
const char *get_log_file();				// ��ȡ log �ļ���û��������һ��
bool logger(EMessageType type, char *format, ...); // д����־

// ����д�������ú궨��ʡȥ��д message_type
#define log_success(...) logger(MSG_SUCCESS,__VA_ARGS__)
#define log_log(...) logger(MSG_LOG,__VA_ARGS__)
#define log_error(...) logger(MSG_ERROR,__VA_ARGS__)
#define log_warning(...) logger(MSG_WARNING,__VA_ARGS__)

_CRT_END_C_HEADER