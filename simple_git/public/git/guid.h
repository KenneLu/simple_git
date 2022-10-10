#include "../minimal.h"

typedef struct
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
}FGitGuid; // 模仿微软的 GUID 结构

// util
int hex_digit(char c); // 将字符转成 16 进制数字
unsigned int hex_number(const char* hex_str); // 将字符串转成 16 进制数字

// GUID
bool is_guid_valid(FGitGuid* guid);
bool is_guid_valid_str(const char* guid);
void init_guid(FGitGuid* guid); // 归一化(初始化) FGitGuid
void create_guid(FGitGuid* guid); // 创建 FGitGuid
void create_guid_str(char* guid_string); // 创建 FGitGuid 并转化成字符串
void guid_to_string(char *str, const FGitGuid * guid); // 字符串处理
void string_to_guid(FGitGuid* guid, const char* str); // 字符串处理
bool guid_equal(const FGitGuid * guid_a, const FGitGuid * guid_b); // 比较 guid
bool guid_equal_str(const char *guid_string, const FGitGuid * guid); // 比较 guid