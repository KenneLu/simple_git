#include "../../public/git/guid.h"
#include "../../public/util/string_util.h"

int hex_digit(char c)
{
    int Result = 0;

    if (c >= '0' && c <= '9')
    {
        Result = c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        Result = c + 10 - 'a';
    }
    else if (c >= 'A' && c <= 'F')
    {
        Result = c + 10 - 'A';
    }
    else
    {
        Result = 0;
    }

    return Result;
}

unsigned int hex_number(const char* hex_str)
{
    unsigned int result = 0;
    while (*hex_str)
    {
        result *= 16;
        result += hex_digit(*hex_str++);
    }
    return result;
}

bool is_guid_valid_str(const char* guid)
{
    FGitGuid n_guid;

    init_guid(&n_guid);
    string_to_guid(&n_guid, guid);

    return is_guid_valid(&n_guid);
}

bool is_guid_valid(FGitGuid* guid)
{
    FGitGuid z_guid;
    init_guid(&z_guid);

    return !guid_equal(guid, &z_guid);
}

void init_guid(FGitGuid* guid)
{
    guid->a = 0;
    guid->b = 0;
    guid->c = 0;
    guid->d = 0;
}

void create_guid_str(char* guid_string)
{
    FGitGuid n_guid;
    init_guid(&n_guid);

    create_guid(&n_guid);

    guid_to_string(guid_string, &n_guid);
}

void create_guid(FGitGuid* guid)
{
    init_guid(guid); // 初始化
    assert(CoCreateGuid((GUID*)guid) == S_OK); // 新建 GUID
}

void guid_to_string(char* str, const FGitGuid* guid)
{
    // 注：%08X 表示 "输出 16 进制的占 8 位数"
    placeholder_cat_s(str, "%08X%08X%08X%08X", guid->a, guid->b, guid->c, guid->d);
}

void string_to_guid(FGitGuid* guid, const char* str)
{
    if (strlen(str) + 1 >= 32)
    {
        guid->a = hex_number(cut_string(str, 0, 8));
        guid->b = hex_number(cut_string(str, 8, 8));
        guid->c = hex_number(cut_string(str, 16, 8));
        guid->d = hex_number(cut_string(str, 24, 8));
    }
}

bool guid_equal(const FGitGuid* guid_a, const FGitGuid* guid_b)
{
    return guid_a->a == guid_b->a &&
        guid_a->b == guid_b->b &&
        guid_a->c == guid_b->c &&
        guid_a->d == guid_b->d;
}

bool guid_equal_str(const char* guid_string, const FGitGuid* guid)
{
    FGitGuid id;
    string_to_guid(&id, guid_string);

    return guid_equal(&id, guid);
}