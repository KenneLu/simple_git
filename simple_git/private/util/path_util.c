#include "../../public/util/path_util.h"
#include "../../public/util/string_util.h"

void string_to_path(const char* str, FPath* path)
{
    FArray array;
    split_string(str, "\n", &array);

    for (int i = 0; i < array.size; i++)
    {
        strcpy(path->data[path->size++], get_array_element(&array, i));
    }

    destroy_array(&array);
}

void path_to_string(char* str, const FPath* path)
{
    for (int i = 0; i < path->size; i ++)
    {
        strcat(str, path->data[i]);
        strcat(str, "\n");
    }

    remove_char_end(str, '\n');
}

void get_get_file_folder_inline(char* path)
{
    // 路径单位化
    normalization_path(path); // \\ /

    // 分离出文件名
    char filename[1024] = {0};
    get_path_filename(filename, path);

    // 将路径中的文件名移除【这里好像有bug，remove_string_start是移除第一次出现的sub_str，可移除文件名应该是要移除末尾第一次出现。如果文件名不带后缀，且与路径中的文件夹名字相同，会导致移除文件夹名，而不是文件名】【*】
    remove_string_start(path, filename);
}

void get_file_folder(char* output, const char* path)
{
    strcpy(output, path);
    get_get_file_folder_inline(output);
}

void get_path_filename(char* filename, const char* path)
{
    FArray c_array;
    if (strstr(path, "\\"))
    {
        split_string(path, "\\", &c_array);
    }
    else if (strstr(path, "/"))
    {
        split_string(path, "/", &c_array);
    }

    char* value = get_array_element(&c_array, c_array.size - 1);
    strcpy(filename, value);
    destroy_array(&c_array);
}

void normalization_path(char* path)
{
    replace_all_char(path, '\\', '/');
}

