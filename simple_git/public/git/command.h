#pragma once
#include "../../public/git/version.h"
#include "../../public/util/file_util.h"
#include "../../public/util/path_util.h"

// util
void init_path(FPath* p);
bool check_command(const char* buf_path);
void compare_list(const FGitVersions* server_version_list, const FFilePath* client_paths, FFilePath* out_paths);
char* get_current_time(); // 获取当前时间
void version_iteration_update();

// command
void git_init_func(); // 初始化 .git, git.txt, ignore, 进行 log 注册
bool git_status_func();
bool git_log_func();
bool git_help();

void git_set_email_func(char* user_input);
void git_set_password_func(char* user_input);
void git_set_name_func(char* user_input);

void git_clone_func(); // 很奇怪，为何只做了版本的处理？【*】
bool git_remote_add_origin_func(char* new_remote_url); // 添加一个新远端【注意，这个语句实战中一般不需要，直接 git clone XXX 就会自动完成添加 XXX】
bool git_pull_func();
void git_pull_client_data(const FGitVersions* server_version_list, const char* value, const char* heard_value);

bool git_add_func(char* user_input);
bool git_commit_func(char* user_input);
bool git_push_func();