#include "../../public/window/window_register.h"

void init_reg_key(FRegKey* info)
{
    // ȫ�������ʼ��
    info->value_count = 0;
    memset(info->name, 0, sizeof(info->name));
    memset(info->value, 0, sizeof(info->value));
}

// RegCreateKeyEx() ����һ�� long������������óɹ����򷵻� 0��ERROR_SUCCESS�������򣬷���ֵΪ�ļ� WINERROR.h �ж����һ������Ĵ�����롣
// ����ȡ��д��ע��� RegOpenKeyEx��RegCloseKeyEx��RegQueryValueEx��RegCreateKeyEx��RegSetValue��
// ��https://blog.csdn.net/weixin_43069562/article/details/104204872��
// ��https://www.freesion.com/article/5472509359/��
/* �� REGSAM samDesired Ȩ�ޱ�
    KEY_CREATE_LINK: ׼�����ɷ��ż�
    KEY_CREATE_SUB_KEY: ׼�������Ӽ�
    KEY_ENUMERATE_SUB_KEYS:	׼������ö���Ӽ�
    KEY_EXECUTE: ׼����ж�����
    KEY_NOTIFY: ׼�����ͨ��
    KEY_QUERY_VALUE: ׼���ѯ�Ӽ�
    KEY_ALL_ACCESS: �ṩ��ȫ����
    KEY_READ: KEY_QUERY_VALUE��KEY_ENUMERATE_SUB_KEYS��KEY_NOTIFY �����
    KEY_SET_VALUE: ׼�������Ӽ�
    KEY_WRITE: KEY_SET_VALUE��KEY_CREATE_SUB_KEY�����
    KEY_WOW64_32KEY: ��ʾ64λϵͳ�е�Ӧ�ó���Ӧ����32λע�����ͼ�����У�32λϵͳ����Ըñ�־
    KEY_WOW64_64KEY: ��ʾ64λϵͳ�е�Ӧ�ó���Ӧ����64λע�����ͼ�����У�32λϵͳ����Ըñ�־
*/
/*
    LONG RegCreateKeyEx(
        HKEY hKey, // ��Ҫ�򿪵�����������
        LPCTSTR lpSubKey, //��Ҫ�򿪵��Ӽ�������
        DWORD Reserved, // ϵͳ�����Ա�����������Ϊ 0
        LPTSTR lpClass, // �ӽ����Ϊ NULL ��ʾĬ������
        DWORD dwOptions, // ע�����ͣ�һ���������ע�᡾REG_OPTION_NON_VOLATILE ���á���REG_OPTION_VOLATILE ��ʱ����REG_OPTION_BACKUP_RESTORE �ָ���
        REGSAM samDesired, // ����Ȩ�ޣ������Ȩ�ޱ�
        LPSECURITY_ATTRIBUTES lpSecurityAttributes, // ��ȫ�ṹ�����巵�صľ���Ƿ���Ա��ӽ��̼̳У�Ϊ NULL ʱ���ܼ̳�
        PHKEY phkResult, // ���ؽ�Ҫ�򿪼��ľ��
        LPDWORD lpdwDisposition // ���ؼ�״̬��REG_CREATED_NEW_KEY �����ɹ�����REG_OPENED_EXISTING_KEY ���Ѵ��ڡ���Ϊ NULL �򲻽��մ���Ϣ
    );
 */
bool register_reg_key(const FRegKey* r_key)
{
    HKEY hResult; // �����
    long hOption = -1; // �������
    // RegCreateKeyEx() ������
    hOption = RegCreateKeyEx(r_key->hkey, r_key->name, 0, NULL, REG_OPTION_NON_VOLATILE,
                             KEY_CREATE_SUB_KEY | KEY_ALL_ACCESS, NULL, &hResult, NULL);
    if (hOption == ERROR_SUCCESS)
    {
        for (int i = 0; i < r_key->value_count; i++)
        {
            char* value_name = r_key->value[i].name[0] == '\0' ? NULL : r_key->value[i].name;
            // RegSetValueEx() ����ֵ
            if ((hOption = RegSetValueEx(hResult, value_name, 0, r_key->value[i].type, r_key->value[i].data,
                                         sizeof(r_key->value[i].data))) != ERROR_SUCCESS)
            {
                printf("\n error code RegSetValueEx = %i \n", hOption);
            }
        }
        // ͨ�����صľ��ʵ�ֹرռ�
        RegCloseKey(&hResult);
        return true;
    }
    else
    {
        printf("\n error code RegCreateKeyEx = %i \n", hOption);
    }

    return false;
}

/*
    long RegOpenKeyEx(
        HKEY hKey, // ��Ҫ�򿪵�����������
        LPCTSTR lpSubKey, //��Ҫ�򿪵��Ӽ�������
        DWORD ulOptions, // ����λ��������Ϊ 0
        REGSAM samDesired, // ����Ȩ��
        PHKEY phkResult // ���ؽ�Ҫ�򿪼��ľ��
    )
    long RegDeleteKey(
      HKEY hKey, // ��Ҫ�򿪵�����������
      LPCTSTR lpSubKey //��Ҫ�򿪵��Ӽ�������

    );
*/
bool delete_reg_key(HKEY hKey, const char* lpSubKey)
{
    HKEY hResult;
    long hOption = -1;
    if ((hOption = RegOpenKeyEx(hKey, lpSubKey, 0, KEY_READ, &hResult)) == ERROR_SUCCESS)
    {
        if (hOption = RegDeleteKey(hResult, lpSubKey) != ERROR_SUCCESS)
        {
            return true;
        }
    }
    return false;
}

bool delete_register_reg_key(HKEY hKey, const char* lpSubKey)
{
    int error_count = 0;

    HKEY hresult;
    long hOption = -1;
    // �򿪼�����һ�㡿
    if ((hOption = RegOpenKeyEx(hKey, lpSubKey, 0, KEY_READ, &hresult)) == ERROR_SUCCESS)
    {
        // ��ѯ�����顾�ڶ��㡿
        DWORD sub_key_count; // �˴����ܲ�û�����ƣ�ֻ��һ����ȫ��飬������ɾ����Ҫд�ɵݹ�
        if ((hOption = RegQueryInfoKey(hresult, NULL, NULL, NULL, &sub_key_count, NULL, NULL, NULL, NULL, NULL, NULL,
                                       NULL)) == ERROR_SUCCESS)
        {
            char sub_key_name[MAX_PATH] = {0};
            for (DWORD i = 0; i < sub_key_count; i++)
            {
                // ö���Ӽ��������㡿
                if ((hOption = RegEnumKey(hresult, i, sub_key_name, MAX_PATH)) == ERROR_SUCCESS)
                {
                    // ɾ���Ӽ�
                    if (hOption = RegDeleteKey(hresult, sub_key_name) != ERROR_SUCCESS)
                    {
                        error_count++;
                        printf("\n error �ļ�  %s �����Ӽ� �޷�ɾ�� \n", sub_key_name);
                    }
                }
                else
                {
                    error_count++;
                    printf("\n error \n");
                }
            }
        }
        else
        {
            error_count++;
            printf("\n error \n");
        }
        // �رռ�
        RegCloseKey(hresult);
    }
    else
    {
        error_count++;
        printf("\n error %s ����Ч or ��Ҫ����ԱȨ��...\n", lpSubKey);
    }

    if (!error_count)
    {
        return true;
    }
    else
    {
        return false;
    }
}
