#pragma once
#include "../minimal.h"
#include "../util/path_util.h"

static char project_name[MAX_PATH] = {0};

static char version_info_file[MAX_PATH] = {0}; // version.info
static char version_list_file[MAX_PATH] = {0}; // version.list

static char git_cache[MAX_PATH] = {0}; // .git/
static char project_path[MAX_PATH] = {0}; // 项目路径

static char git_client_cache[MAX_PATH] = {0}; // 本地仓库 /git_client/
static char git_server_cache[MAX_PATH] = {0}; // 远端仓库 /git_server/

static char server_url[MAX_PATH] = {0}; // 远端路径 url

static char user_ini_file[MAX_PATH] = {0}; // 用户配置信息 user.ini

static int b_exit = false; // 退出程序标记位

static FPath server_list, ignore_path, ignore_suffix;
