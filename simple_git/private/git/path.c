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

const char* get_version_info_file()
{
    if (version_info_file[0] == '\0')
    {
        const char* p = get_git_cache();
        strcpy(version_info_file, p);
        strcat(version_info_file, "version.info");
    }

    return version_info_file;
}

char* get_project_cache()
{
    if (git_txt_file[0] == '\0')
    {
        // ��ȡ��ǰ�ļ�����·��
        char buf_project_path[MAX_PATH];
        _getcwd(buf_project_path, MAX_PATH);

        // �ֽ�·��
        FArray path_split;
        split_string(buf_project_path, "\\", &path_split);

        // �ƺ��ǵ���Ѱ�� git.txt �ļ� ?��*��
        char buf_path[MAX_PATH] = {0};
        for (int i = 0; i < path_split.size; i++)
        {
            char* p = get_array_element(&path_split, i);
            strcat(buf_path, p);
            strcat(buf_path, "\\");

            // �жϵ�ǰ�ļ������Ƿ���� .git/git.txt �ļ�
            char buf_tmp[MAX_PATH] = {0};
            strcpy(buf_tmp, buf_path);
            strcat(buf_tmp, ".git\\git.txt");
            if (_access(buf_tmp, 0) == 0)
            {
                strcpy(git_txt_file, buf_path);
                break; // ��������������
            }
        }

        destroy_array(&path_split);

        //if (git_project_path[0] == '\0')
        //{
        //	strcpy(git_project_path, buf_project_path);
        //	strcat(git_project_path, "\\");
        //	//printf("���Ƚ��� git init ���� ��ʼ��һ�����زֿ�");
        //}
    }

    return git_txt_file;
}

char* get_git_server_cache()
{
    if (git_server_cache[0] == '\0')
    {
        strcpy(git_server_cache, remote_origin);
        strcat(git_server_cache, "\\git_server");
    }

    return git_server_cache;
}

const char* get_git_cache()
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

const char* get_version_list_file()
{
    if (version_list_file[0] == '\0')
    {
        const char* p = get_git_cache();
        strcpy(version_list_file, p);
        strcat(version_list_file, "version.list");
    }

    return version_list_file;
}

char* get_user_ini_file()
{
    if (user_ini_file[0] == '\0')
    {
        if (get_git_cache()[0] != '\0')
        {
            strcpy(user_ini_file, get_git_cache());
            strcat(user_ini_file, "user.ini"); // �����ļ�
            if (_access(user_ini_file, 0) == -1)
            {
                create_file(user_ini_file);
            }
        }
    }

    return user_ini_file;
}



