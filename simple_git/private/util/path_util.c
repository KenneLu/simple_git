#include "../../public/util/path_util.h"
#include "../../public/util/string_util.h"

void string_to_path(const char* str, FPath* f_path)
{
    FArray array;
    split_string(str, "\n", &array);

    for (int i = 0; i < array.size; i++)
    {
        strcpy(f_path->data[f_path->size++], get_array_element(&array, i));
    }

    destroy_array(&array);
}

void path_to_string(char* str, const FPath* f_path)
{
    for (int i = 0; i < f_path->size; i ++)
    {
        strcat(str, f_path->data[i]);
        strcat(str, "\n");
    }

    remove_char_end(str, '\n');
}

void get_git_file_folder_inline(char* file_path)
{
    // 路径单位化
    normalization_path(file_path); // \\ /

    // 分离出文件名
    char filename[1024] = {0};
    get_path_filename(filename, file_path);

    // 将路径中的文件名移除【这里好像有bug，remove_string_start是移除第一次出现的sub_str，可移除文件名应该是要移除末尾第一次出现。如果文件名不带后缀，且与路径中的文件夹名字相同，会导致移除文件夹名，而不是文件名】【*】
    remove_string_start(file_path, filename);
}

void get_file_folder(char* out_folder, const char* file_path)
{
    strcpy(out_folder, file_path);
    get_git_file_folder_inline(out_folder);
}

void get_path_filename(char* filename, const char* file_path)
{
    FArray c_array;
    if (strstr(file_path, "\\"))
    {
        split_string(file_path, "\\", &c_array);
    }
    else if (strstr(file_path, "/"))
    {
        split_string(file_path, "/", &c_array);
    }

    char* value = get_array_element(&c_array, c_array.size - 1);
    strcpy(filename, value);
    destroy_array(&c_array);
}

void normalization_path(char* c_path)
{
    replace_all_char(c_path, '\\', '/');
}

