#include "../../public/util/string_util.h"

// "aaabcaabc"
// "abc"
// return 2
int find_string(char* str, char const* sub_str)
{
    int index = -1;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] = sub_str[0])
        {
            int sub_index = 1;
            while (sub_str[sub_index] == str[i + sub_index] && sub_str[sub_index] != '\0')
            {
                sub_index++;
            }
            if (sub_index == strlen(sub_str))
            {
                return i;
            }
        }
    }
    return index;
}

char* cut_string(const char* str, int cut_start, int cut_len)
{
    char* start = &str[cut_start];
    char* output = (char*)malloc(cut_len);
    memcpy_s(output, cut_len, start, cut_len);

    return output;
}

void split_string(const char* c_string, const char* delim, FArray* c_array)
{
    char string_copy[8196 * 10] = {0};
    strcpy(string_copy, c_string); // 深拷贝

    // 利用 strtok() 拆分字符串
    init_array(c_array);
    char* p = strtok(string_copy, delim);
    add_array(c_array, p); // 读取拆出的第一个子串
    while (p)
    {
        if ((p = strtok(NULL, delim)) != NULL)
        {
            // 继续拆分，NULL 是上一个的指针
            add_array(c_array, p); // 依次读取后面的字串
        }
    }
}

char* split_string_with_index(const char* str, const char* delim, char* out_str, int index)
{
    FArray c_array;
    split_string(str, delim, &c_array); // 拆分字符串
    char* p = get_array_element(&c_array, index); // 获取指定的部分
    remove_char_end(p, '\n'); // 剔除换行符
    strcpy(out_str, p); // 深拷贝
    destroy_array(&c_array); // 释放内存

    return out_str;
}

void remove_char_start(char* str, char sub_char)
{
    //��ͷ��ʼ��� Ȼ��ɾ������
    for (int i = 0; i < strlen(str) + 1; i++)
    {
        if (str[i] == sub_char)
        {
            do
            {
                str[i] = str[i + 1];
                i++;
            }
            while (str[i + 1] != '\0');
            str[i] = '\0';

            break;
        }
    }
}

void remove_char_end(char* str, char sub_char)
{
    for (int i = strlen(str); i >= 0; i--)
    {
        if (str[i] == sub_char)
        {
            strcpy(&str[i], &str[i + 1]);
            break;
        }
    }
}

void remove_all_char(char* str, char sub_char)
{
    for (int i = strlen(str); i >= 0; i--)
    {
        if (str[i] == sub_char)
        {
            strcpy(&str[i], &str[i + 1]);
        }
    }
}

void replace_all_char(char* str, const char old_char, const char new_char)
{
    int index = find_string(str, &old_char);
    if (index != -1)
    {
        str[index] = new_char;
        replace_all_char(str, old_char, new_char);
    }
}

void remove_string_start(char* str, char const* sub_str)
{
    int index = find_string(str, sub_str);
    if (index != -1)
    {
        strcpy(&str[index], &str[index + strlen(sub_str)]);
    }
}

int placeholder_cat(char* output, char* format, ...)
{
    va_list param_list;
    va_start(param_list, format);
    char param_char = *format++;

    for (int i = 0; param_char != '\0'; i++)
    {
        if (param_char != '\%' && *(format - 1) != '\%')
        {
            output[i] = param_char;
        }
        else
        {
            param_char = *format++;
            switch (param_char)
            {
            case 'c':
            case 'C':
                {
                    output[i] = va_arg(param_list, char);
                    break;
                }
            case 's':
            case 'S':
                {
                    char* p = va_arg(param_list, char*);
                    int len = strlen(p);
                    for (int j = 0; j < len;)
                    {
                        output[i++] = p[j++];
                    }
                    i--;

                    break;
                }
            case 'd':
            case 'D':
            case 'i':
            case 'I':
                {
                    int i_int = va_arg(param_list, int);
                    char s_int[8] = {0};
                    char* p = _itoa(i_int, s_int, 10);
                    int len = strlen(p);
                    for (int j = 0; j < len;)
                    {
                        output[i++] = p[j++];
                    }
                    i--;
                    break;
                }
            }
        }

        param_char = *format++;
    }

    // 返回拼接后的长度
    return strlen(output) + 1;
}

int placeholder_cat_s(char* output, char* format, ...)
{
    char buf[8196 * 1024] = {0};
    memset(buf, 0, sizeof(char) * 8196 * 1024);
    va_list args;
    va_start(args, format);
    _vsnprintf_s(buf, 8196 * 1024 - 1, 8196 * 1024, format, args);
    va_end(args);
    buf[8196 * 1024 - 1] = 0;

    strcat(output, buf);
    return strlen(output) + 1;
}

