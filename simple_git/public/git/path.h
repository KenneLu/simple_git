#pragma once

char* get_project_cache(); // ������Ŀ·��

const char* get_git_cache(); // ���� .git/

const char* get_version_list_file(); // ���� .git/version.list
const char* get_version_info_file(); // ���� .git/version.info
char* get_user_ini_file(); // ���� .git/user.ini

char* get_git_client_cache(); // ���� ./git_client/
char* get_git_server_cache(); // Զ�� ./git_server/

