#pragma once
#include "../minimal.h"
#include "../util/path_util.h"

static char project_name[MAX_PATH] = {0};

static char version_info_file[MAX_PATH] = {0}; // version.info
static char version_list_file[MAX_PATH] = {0}; // version.list

static char git_cache[MAX_PATH] = {0}; // .git/
static char git_txt_file[MAX_PATH] = {0}; // git.txt

static char git_client_cache[MAX_PATH] = {0}; // ���زֿ� /git_client/
static char git_server_cache[MAX_PATH] = {0}; // Զ�˲ֿ� /git_server/

static char remote_origin[MAX_PATH] = {0}; // Զ��·�� url

static char user_ini_file[MAX_PATH] = {0}; // �û�������Ϣ user.ini

static int b_exit = false; // �˳�������λ

static FPath server_list, ignore_path, ignore_suffix;
