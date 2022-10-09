#pragma once

#include "../minimal.h"

_CRT_BEGIN_C_HEADER
/* 注册表的数据结构
 * 【https://baike.baidu.com/item/%E6%B3%A8%E5%86%8C%E8%A1%A8/101856?fr=aladdin】
 * 【https://baike.baidu.com/item/%E6%B3%A8%E5%86%8C%E8%A1%A8%E9%94%AE%E5%80%BC/1381584#2】
 * 注册表由 ①键（也叫“项”）、②子键（子项）和 ③值 构成
 * 可以把【键】类比成结构体，把【值】类比成结构体内部的变量
 * 【键】就是分支中的【文件夹】
 * 【子键】就是这个文件夹当中的【子文件夹】
 * 【键】可以有一个或多个【值】，每个值的名称各不相同，如果一个值的名称为空，则该值为该项的【默认值】
 * 【值】是键内部的元素，包含 名称、类型、数据
*/
typedef struct
{
	char name[MAX_PATH]; // 名称
	DWORD type; // 数据类型【字符串值, 二进制值, DWDORD, QWORD, 多字符串值, 可扩充字符串值】
	char data[1024]; // 数据
}FRegValue; // 注册表值

typedef struct
{
	HKEY hkey; // 六大主键 HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_USERS, HKEY_CURRENT_CONFIG
	char name[MAX_PATH]; // 键名
	FRegValue value[1024]; // 注册表值
	int value_count; // 注册表值计数，省去遍历 value 数组元素个数
}FRegKey; // 注册表键

// 初始化 reg_key
void init_reg_key(FRegKey *info);

// 在注册表里注册软件的键
bool register_reg_key(const FRegKey *r_key);

// 卸载键
bool delete_register_reg_key(HKEY hKey, const char *lpSubKey);

// 卸载一个空键【example】
bool delete_reg_key(HKEY hKey, const char *lpSubKey);
_CRT_END_C_HEADER


/*
//运行批处理 以管理员启动
//mshta vbscript:CreateObject("Shell.Application").ShellExecute("%~dp0git_install.exe","1 F:\c\c\20\20.exe F:\c\c\git_install\git_install\icon1.ico test_git1","","runas",1)(window.close)&&exit

//下面是案例
#define REG_SHELL_DIRECTORY					"*\\Shell\\"						// 在对象上鼠标右键栏添加 Git 按钮
#define REG_DIRECTORY						"Directory\\Background\\shell\\"	// 在空白的地区点击右键栏添加 Git 按钮
#define REG_DESKTOPBACKGROUND				"DesktopBackground\\shell\\"		// 在空白的地区点击右键栏添加 Git 按钮

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

	if (strcmp(commit_type ,"1") == 0) //安装
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
			printf("\n 1 需要管理员权限~~ \n");
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
			printf("\n 2 需要管理员权限~~ \n");
			system("pause");

			return 1;
		}
	}
	else if (strcmp(commit_type, "2") == 0) //卸载
	{
		if (register_reg_key(HKEY_CLASSES_ROOT, buf_reg_key))
		{
			delete_reg_key(HKEY_CLASSES_ROOT, REG_PATH_TEST, name);
		}
	}

	return 0;
}
*/