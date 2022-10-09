#pragma once

#include "../minimal.h"

_CRT_BEGIN_C_HEADER
/* ע�������ݽṹ
 * ��https://baike.baidu.com/item/%E6%B3%A8%E5%86%8C%E8%A1%A8/101856?fr=aladdin��
 * ��https://baike.baidu.com/item/%E6%B3%A8%E5%86%8C%E8%A1%A8%E9%94%AE%E5%80%BC/1381584#2��
 * ע����� �ټ���Ҳ�С���������Ӽ�������� ��ֵ ����
 * ���԰ѡ�������ȳɽṹ�壬�ѡ�ֵ����ȳɽṹ���ڲ��ı���
 * ���������Ƿ�֧�еġ��ļ��С�
 * ���Ӽ�����������ļ��е��еġ����ļ��С�
 * ������������һ��������ֵ����ÿ��ֵ�����Ƹ�����ͬ�����һ��ֵ������Ϊ�գ����ֵΪ����ġ�Ĭ��ֵ��
 * ��ֵ���Ǽ��ڲ���Ԫ�أ����� ���ơ����͡�����
*/
typedef struct
{
	char name[MAX_PATH]; // ����
	DWORD type; // �������͡��ַ���ֵ, ������ֵ, DWDORD, QWORD, ���ַ���ֵ, �������ַ���ֵ��
	char data[1024]; // ����
}FRegValue; // ע���ֵ

typedef struct
{
	HKEY hkey; // �������� HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_USERS, HKEY_CURRENT_CONFIG
	char name[MAX_PATH]; // ����
	FRegValue value[1024]; // ע���ֵ
	int value_count; // ע���ֵ������ʡȥ���� value ����Ԫ�ظ���
}FRegKey; // ע����

// ��ʼ�� reg_key
void init_reg_key(FRegKey *info);

// ��ע�����ע������ļ�
bool register_reg_key(const FRegKey *r_key);

// ж�ؼ�
bool delete_register_reg_key(HKEY hKey, const char *lpSubKey);

// ж��һ���ռ���example��
bool delete_reg_key(HKEY hKey, const char *lpSubKey);
_CRT_END_C_HEADER


/*
//���������� �Թ���Ա����
//mshta vbscript:CreateObject("Shell.Application").ShellExecute("%~dp0git_install.exe","1 F:\c\c\20\20.exe F:\c\c\git_install\git_install\icon1.ico test_git1","","runas",1)(window.close)&&exit

//�����ǰ���
#define REG_SHELL_DIRECTORY					"*\\Shell\\"						// �ڶ���������Ҽ������ Git ��ť
#define REG_DIRECTORY						"Directory\\Background\\shell\\"	// �ڿհ׵ĵ�������Ҽ������ Git ��ť
#define REG_DESKTOPBACKGROUND				"DesktopBackground\\shell\\"		// �ڿհ׵ĵ�������Ҽ������ Git ��ť

#define REG_PATH_TEST REG_DIRECTORY
int main(int argc, char *argv[])
{
	const char *commit_type = argv[1];
	const char *path_exe = argv[2];
	const char *path_icon = argv[3];
	const char *name = argv[4];

	char buf_reg_key[MAX_PATH] = { 0 };
	char buf_reg_sub_key[MAX_PATH] = { 0 };
	strcpy(buf_reg_key, REG_PATH_TEST);
	strcat(buf_reg_key, name);

	strcpy(buf_reg_sub_key, REG_PATH_TEST);
	strcat(buf_reg_sub_key, name);
	strcat(buf_reg_sub_key, "\\command");

	if (strcmp(commit_type ,"1") == 0) //��װ
	{
		//r_key
		reg_key r_key;
		init_reg_key(&r_key);

		strcpy(r_key.filename, buf_reg_key);
		r_key.hkey = HKEY_CLASSES_ROOT;
		{
			r_key.value[r_key.size_value].type = REG_SZ;
			strcpy(r_key.value[r_key.size_value++].data, name);

			r_key.value[r_key.size_value].type = REG_SZ;
			strcpy(r_key.value[r_key.size_value].name, "icon");
			strcpy(r_key.value[r_key.size_value++].data, path_icon);
		}

		if (!register_info(&r_key))
		{
			printf("\n 1 ��Ҫ����ԱȨ��~~ \n");
			system("pause");

			return 1;
		}

		//sub r_key
		init_reg_key(&r_key);
		strcpy(r_key.filename, buf_reg_sub_key);
		r_key.hkey = HKEY_CLASSES_ROOT;
		{
			r_key.value[r_key.size_value].type = REG_SZ;
			strcpy(r_key.value[r_key.size_value++].data, path_exe);
		}

		if (!register_reg_key(&r_key))
		{
			printf("\n 2 ��Ҫ����ԱȨ��~~ \n");
			system("pause");

			return 1;
		}
	}
	else if (strcmp(commit_type, "2") == 0) //ж��
	{
		if (register_reg_key(HKEY_CLASSES_ROOT, buf_reg_key))
		{
			delete_reg_key(HKEY_CLASSES_ROOT, REG_PATH_TEST, name);
		}
	}

	return 0;
}
*/