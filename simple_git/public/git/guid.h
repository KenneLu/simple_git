#include "../minimal.h"

typedef struct
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
}FGitGuid; // ģ��΢��� GUID �ṹ

// util
int hex_digit(char c); // ���ַ�ת�� 16 ��������
unsigned int hex_number(const char* hex_str); // ���ַ���ת�� 16 ��������

// GUID
bool is_guid_valid(FGitGuid* c_guid);
bool is_guid_valid_str(const char* c_guid);
void init_guid(FGitGuid* c_guid); // ��һ��(��ʼ��) FGitGuid
void create_guid(FGitGuid* c_guid); // ���� FGitGuid
void create_guid_str(char* c_guid);
void guid_to_string(char *str, const FGitGuid * c_guid);
void string_to_guid(const char* str, FGitGuid* c_guid);
bool guid_equal(const FGitGuid * c_guid_a, const FGitGuid * c_guid_b);
bool guid_equal_str(const char *guid_string, const FGitGuid * c_guid);
void guid_parse(const char* buf, FGitGuid* c_guid);