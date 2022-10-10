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
bool is_guid_valid(FGitGuid* guid);
bool is_guid_valid_str(const char* guid);
void init_guid(FGitGuid* guid); // ��һ��(��ʼ��) FGitGuid
void create_guid(FGitGuid* guid); // ���� FGitGuid
void create_guid_str(char* guid_string); // ���� FGitGuid ��ת�����ַ���
void guid_to_string(char *str, const FGitGuid * guid); // �ַ�������
void string_to_guid(FGitGuid* guid, const char* str); // �ַ�������
bool guid_equal(const FGitGuid * guid_a, const FGitGuid * guid_b); // �Ƚ� guid
bool guid_equal_str(const char *guid_string, const FGitGuid * guid); // �Ƚ� guid