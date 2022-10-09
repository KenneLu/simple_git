#include "../../public/git/ignore.h"
#include "../../public/git/global.h"
#include "../../public/array/array.h"
#include "../../public/util/string_util.h"

void string_to_ignore_path(const char* buf)
{
    if (buf[0] != '\0')
    {
        /*char* p = get_git_project_path();*/

        FArray c_array;
        split_string(buf, "\n", &c_array);
        for (int i = 0; i < c_array.size; i++)
        {
            char* value = get_array_element(&c_array, i);

            /*strcpy(ignore_path.data[ignore_path.size], p);*/
            strcat(ignore_path.data[ignore_path.size++], value);
        }

        destroy_array(&c_array);
    }
}

void string_to_ignore_suffix(const char* buf)
{
    if (buf[0] != '\0')
    {
        FArray c_array;
        split_string(buf, "\n", &c_array);
        for (int i = 0; i < c_array.size; i++)
        {
            char* value = get_array_element(&c_array, i);

            strcpy(ignore_suffix.data[ignore_suffix.size++], value);
        }

        destroy_array(&c_array);
    }
}

bool is_exit_ignore(const char* path)
{
    //文件夹判定
    for (int i = 0; i < ignore_path.size; i++)
    {
        if (strstr(path, ignore_path.data[i]))
        {
            return true;
        }
    }

    //后缀类型判定
    for (int i = 0; i < ignore_suffix.size; i++)
    {
        if (strstr(path, ignore_suffix.data[i]))
        {
            return true;
        }
    }

    return false;
}