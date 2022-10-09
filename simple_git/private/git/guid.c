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

bool is_guid_valid_str(const char* c_guid)
{
    FGitGuid n_guid;

    init_guid(&n_guid);
    string_to_guid(c_guid, &n_guid);

    return is_guid_valid(&n_guid);
}

bool is_guid_valid(FGitGuid* c_guid)
{
    FGitGuid z_guid;
    init_guid(&z_guid);

    return !guid_equal(c_guid, &z_guid);
}

void init_guid(FGitGuid* c_guid)
{
    c_guid->a = 0;
    c_guid->b = 0;
    c_guid->c = 0;
    c_guid->d = 0;
}

void create_guid_str(char* c_guid)
{
    FGitGuid n_guid;
    init_guid(&n_guid);

    create_guid(&n_guid);

    guid_to_string(c_guid, &n_guid);
}

void create_guid(FGitGuid* c_guid)
{
    init_guid(c_guid); // 初始化
    assert(CoCreateGuid((GUID*)c_guid) == S_OK); // 新建 GUID
}

void guid_to_string(char* str, const FGitGuid* c_guid)
{
    // 注：%08X 表示 "输出 16 进制的占 8 位数"
    placeholder_cat_s(str, "%08X%08X%08X%08X", c_guid->a, c_guid->b, c_guid->c, c_guid->d);
}

void string_to_guid(const char* str, FGitGuid* c_guid)
{
    if (strlen(str) + 1 >= 32)
    {
        c_guid->a = hex_number(cut_string(str, 0, 8));
        c_guid->b = hex_number(cut_string(str, 8, 8));
        c_guid->c = hex_number(cut_string(str, 16, 8));
        c_guid->d = hex_number(cut_string(str, 24, 8));
    }
}

bool guid_equal(const FGitGuid* c_guid_a, const FGitGuid* c_guid_b)
{
    return c_guid_a->a == c_guid_b->a &&
        c_guid_a->b == c_guid_b->b &&
        c_guid_a->c == c_guid_b->c &&
        c_guid_a->d == c_guid_b->d;
}

bool guid_equal_str(const char* guid_string, const FGitGuid* c_guid)
{
    FGitGuid id;
    string_to_guid(guid_string, &id);

    return guid_equal(&id, c_guid);
}

void guid_parse(const char* buf, FGitGuid* c_guid)
{
    if (strlen(buf) == 32)
    {
        string_to_guid(buf, c_guid);
    }
}