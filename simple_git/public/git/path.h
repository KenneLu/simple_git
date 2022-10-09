#pragma once

char* get_project_cache(); // 本地项目路径

const char* get_git_cache(); // 本地 .git/

const char* get_version_list_file(); // 本地 .git/version.list
const char* get_version_info_file(); // 本地 .git/version.info
char* get_user_ini_file(); // 本地 .git/user.ini

char* get_git_client_cache(); // 本地 ./git_client/
char* get_git_server_cache(); // 远端 ./git_server/

