#include "../../public/git/path.h"
#include "../../public/util/string_util.h"
#include "../../public/util/file_util.h"
#include "../../public/git/global.h"

char* get_git_client_cache()
{
    if (git_client_cache[0] == '\0')
    {
        const char* p = get_project_cache();
        if (p[0] != '\0')
        {
            strcat(git_client_cache, p);
            strcat(git_client_cache, "git_client");
        }
    }

    return git_client_cache;
}

const char* get_client_version_info_file()
{
    if (version_info_file[0] == '\0')
    {
        const char* p = get_client_git_cache();
        strcpy(version_info_file, p);
        strcat(version_info_file, "version.info");
    }

    return version_info_file;
}

char* get_project_cache()
{
    if (project_path[0] == '\0')
    {
        // 获取当前文件绝对路径
        char buf_project_path[MAX_PATH];
        _getcwd(buf_project_path, MAX_PATH);

        // 分解路径
        FArray path_split;
        split_string(buf_project_path, "\\", &path_split);

        // 似乎是迭代寻找 git.txt 文件
        char path_walker[MAX_PATH] = {0};
        for (int i = 0; i < path_split.size; i++)
        {
            char* p = get_array_element(&path_split, i);
            strcat(path_walker, p);
            strcat(path_walker, "\\");

            // 判断当前文件夹中是否存在 .git/git.txt 文件
            char git_txt_path[MAX_PATH] = {0};
            strcpy(git_txt_path, path_walker);
            strcat(git_txt_path, ".git\\git.txt");
            if (_access(git_txt_path, 0) == 0)
            {
                // 存在则表示找到项目路径了
                strcpy(project_path, path_walker);
                break;
            }
        }
        destroy_array(&path_split);
    }

    return project_path;
}

char* get_git_server_cache()
{
    if (git_server_cache[0] == '\0')
    {
        strcpy(git_server_cache, server_url);
        strcat(git_server_cache, "\\git_server");
    }

    return git_server_cache;
}

const char* get_client_git_cache()
{
    if (git_cache[0] == '\0')
    {
        const char* p = get_project_cache();
        if (p[0] != '\0')
        {
            strcpy(git_cache, p);
            strcat(git_cache, ".git\\");
        }
    }

    return git_cache;
}

const char* get_client_version_list_file()
{
    if (version_list_file[0] == '\0')
    {
        const char* p = get_client_git_cache();
        strcpy(version_list_file, p);
        strcat(version_list_file, "version.list");
    }

    return version_list_file;
}

char* get_client_user_ini_file()
{
    if (user_ini_file[0] == '\0')
    {
        if (get_client_git_cache()[0] != '\0')
        {
            strcpy(user_ini_file, get_client_git_cache());
            strcat(user_ini_file, "user.ini"); // 配置文件
            if (_access(user_ini_file, 0) == -1)
            {
                create_file(user_ini_file);
            }
        }
    }

    return user_ini_file;
}



