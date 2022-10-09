#include "../../public/util/file_util.h"
#include "../../public/util/string_util.h"

void init_def_c_paths(FFilePath* c_paths)
{
    c_paths->index = 0;
    memset(c_paths->paths, 0, sizeof(c_paths->paths) - 1);
}

int copy_file(const char* Src, const char* Dest)
{
    //当前的缓存 缓存1MB大小，如果超过就会出问题 这个会在std C课程里面继续扩展
    char Buf[1024 * 1024] = {0};
    int FileSize = 0;
    FILE* FpSrc = NULL;
    FILE* FpDest = NULL;

    if ((FpSrc = fopen(Src, "rb")) != NULL)
    {
        if ((FpDest = fopen(Dest, "wb")) != NULL)
        {
            // 注意: fread(file) > 0 表示读出了文件，否则读文件内容失败 
            while ((FileSize = fread(Buf, 1, 512, FpSrc)) > 0)
            {
                fwrite(Buf, FileSize, sizeof(char), FpDest);
                memset(Buf, 0, sizeof(Buf));
            }

            fclose(FpSrc);
            fclose(FpDest);

            return 0;
        }
    }

    return -1;
}

void find_files(char const* in_path, FFilePath* str, bool b_recursion)
{
    struct _finddata_t finddata;

    long hfile = 0;
    char tmp_path[8196] = {0};
    strcpy(tmp_path, in_path);
    strcat(tmp_path, "\\*");
    if ((hfile = _findfirst(tmp_path, &finddata)) != -1)
    {
        do
        {
            if (finddata.attrib & _A_SUBDIR)
            {
                if (b_recursion)
                {
                    if (strcmp(finddata.name, ".") == 0 ||
                        strcmp(finddata.name, "..") == 0)
                    {
                        continue;
                    }

                    char new_path[8196] = {0};
                    strcpy(new_path, in_path);
                    strcat(new_path, "\\");
                    strcat(new_path, finddata.name);

                    find_files(new_path, str, b_recursion);
                }
            }
            else
            {
                strcpy(str->paths[str->index], in_path);
                strcat(str->paths[str->index], "\\");
                strcat(str->paths[str->index++], finddata.name);
            }
        }
        while (_findnext(hfile, &finddata) == 0);
        _findclose(hfile);
    }
}

bool create_file(char const* filename)
{
    FILE* fp = NULL;
    if ((fp = fopen(filename, "w+")) != NULL)
    {
        fclose(fp);

        return true;
    }

    return false;
}

bool create_folder_path(char const* folder_path)
{
    // 切分路径字符串
    FArray c_array;
    if (strstr(folder_path, "\\"))
    {
        split_string(folder_path, "\\", &c_array);
    }
    else if (strstr(folder_path, "/"))
    {
        split_string(folder_path, "/", &c_array);
    }

    // 文件路径不存在则生成相应文件夹
    char tmp_path[260] = {0};
    for (int i = 0; i < c_array.size; i++)
    {
        // 从根节点开始，逐一拼接路径
        char* folder = get_array_element(&c_array, i);
        strcat(folder, "\\");
        strcat(tmp_path, folder);
        // 无法访问
        if (_access(tmp_path, 0) == -1)
        {
            _mkdir(tmp_path); // 生成文件夹
        }
    }

    // 释放内存
    destroy_array(&c_array);

    // 最后确保能正常访问
    return _access(tmp_path, 0) == 0;
}

bool read_file(const char* file_path, char* buf)
{
    FILE* fp = NULL;
    if ((fp = fopen(file_path, "r")) != NULL)
    {
        char buf_tmp[2048] = {0};
        int file_size = 0;
        while ((file_size = fread(buf_tmp, 1, 1024, fp)) > 0)
        {
            strcat(buf, buf_tmp);
            memset(buf_tmp, 0, sizeof(buf_tmp));
        }

        fclose(fp);

        return buf[0] != '\0';
    }

    return false;
}

bool add_file(const char* path, char* buf)
{
    FILE* fp = NULL;
    if ((fp = fopen(path, "a+")) != NULL)
    {
        fprintf(fp, "%s", buf);
        fclose(fp);

        return true;
    }

    return false;
}

bool rewrite_file(const char* file_path, char* buf)
{
    FILE* fp = NULL;
    if ((fp = fopen(file_path, "w+")) != NULL)
    {
        fprintf(fp, "%s", buf);
        fclose(fp);

        return true;
    }

    return false;
}

unsigned int get_file_size_by_filename(const char* filename)
{
    unsigned int file_size = 0;

    FILE* fp = NULL;
    if ((fp = fopen(filename, "r")) != NULL)
    {
        file_size = get_file_size(fp);

        fclose(fp);
    }

    return file_size;
}

//asdoiajoi ajs aoisjd oaisjd oiasjdoi asodao ijaosijd oaisdja index
unsigned int get_file_size(FILE* file_handle)
{
    unsigned int file_size = 0;

    unsigned int current_read_postion = ftell(file_handle);
    fseek(file_handle, 0, SEEK_END);
    file_size = ftell(file_handle);
    fseek(file_handle, current_read_postion, SEEK_SET);

    return file_size;
}
