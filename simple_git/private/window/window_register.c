#include "../../public/window/window_register.h"

void init_reg_key(FRegKey* info)
{
    // 全部清零初始化
    info->value_count = 0;
    memset(info->name, 0, sizeof(info->name));
    memset(info->value, 0, sizeof(info->value));
}

// RegCreateKeyEx() 返回一个 long，如果函数调用成功，则返回 0（ERROR_SUCCESS）。否则，返回值为文件 WINERROR.h 中定义的一个非零的错误代码。
// 《读取和写入注册表 RegOpenKeyEx、RegCloseKeyEx、RegQueryValueEx、RegCreateKeyEx、RegSetValue》
// 【https://blog.csdn.net/weixin_43069562/article/details/104204872】
// 【https://www.freesion.com/article/5472509359/】
/* 《 REGSAM samDesired 权限表》
    KEY_CREATE_LINK: 准许生成符号键
    KEY_CREATE_SUB_KEY: 准许生成子键
    KEY_ENUMERATE_SUB_KEYS:	准许生成枚举子键
    KEY_EXECUTE: 准许进行读操作
    KEY_NOTIFY: 准许更换通告
    KEY_QUERY_VALUE: 准许查询子键
    KEY_ALL_ACCESS: 提供完全访问
    KEY_READ: KEY_QUERY_VALUE、KEY_ENUMERATE_SUB_KEYS、KEY_NOTIFY 的组合
    KEY_SET_VALUE: 准许设置子键
    KEY_WRITE: KEY_SET_VALUE、KEY_CREATE_SUB_KEY的组合
    KEY_WOW64_32KEY: 表示64位系统中的应用程序应该在32位注册表视图上运行，32位系统会忽略该标志
    KEY_WOW64_64KEY: 表示64位系统中的应用程序应该在64位注册表视图上运行，32位系统会忽略该标志
*/
/*
    LONG RegCreateKeyEx(
        HKEY hKey, // 需要打开的主键的名称
        LPCTSTR lpSubKey, //需要打开的子键的名称
        DWORD Reserved, // 系统兼容性保留，必须设为 0
        LPTSTR lpClass, // 子健类别，为 NULL 表示默认类型
        DWORD dwOptions, // 注册类型，一般采用永久注册【REG_OPTION_NON_VOLATILE 永久】【REG_OPTION_VOLATILE 临时】【REG_OPTION_BACKUP_RESTORE 恢复】
        REGSAM samDesired, // 访问权限，详见《权限表》
        LPSECURITY_ATTRIBUTES lpSecurityAttributes, // 安全结构，定义返回的句柄是否可以被子进程继承，为 NULL 时不能继承
        PHKEY phkResult, // 返回将要打开键的句柄
        LPDWORD lpdwDisposition // 返回键状态【REG_CREATED_NEW_KEY 创建成功】【REG_OPENED_EXISTING_KEY 键已存在】，为 NULL 则不接收此信息
    );
 */
bool register_reg_key(const FRegKey* r_key)
{
    HKEY hResult; // 键句柄
    long hOption = -1; // 操作句柄
    // RegCreateKeyEx() 创建键
    hOption = RegCreateKeyEx(r_key->hkey, r_key->name, 0, NULL, REG_OPTION_NON_VOLATILE,
                             KEY_CREATE_SUB_KEY | KEY_ALL_ACCESS, NULL, &hResult, NULL);
    if (hOption == ERROR_SUCCESS)
    {
        for (int i = 0; i < r_key->value_count; i++)
        {
            char* value_name = r_key->value[i].name[0] == '\0' ? NULL : r_key->value[i].name;
            // RegSetValueEx() 创建值
            if ((hOption = RegSetValueEx(hResult, value_name, 0, r_key->value[i].type, r_key->value[i].data,
                                         sizeof(r_key->value[i].data))) != ERROR_SUCCESS)
            {
                printf("\n error code RegSetValueEx = %i \n", hOption);
            }
        }
        // 通过返回的句柄实现关闭键
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
        HKEY hKey, // 需要打开的主键的名称
        LPCTSTR lpSubKey, //需要打开的子键的名称
        DWORD ulOptions, // 保留位，必须设为 0
        REGSAM samDesired, // 访问权限
        PHKEY phkResult // 返回将要打开键的句柄
    )
    long RegDeleteKey(
      HKEY hKey, // 需要打开的主键的名称
      LPCTSTR lpSubKey //需要打开的子键的名称

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
    // 打开键【第一层】
    if ((hOption = RegOpenKeyEx(hKey, lpSubKey, 0, KEY_READ, &hresult)) == ERROR_SUCCESS)
    {
        // 查询键详情【第二层】
        DWORD sub_key_count; // 此处功能并没有完善，只是一个安全检查，真正的删除需要写成递归
        if ((hOption = RegQueryInfoKey(hresult, NULL, NULL, NULL, &sub_key_count, NULL, NULL, NULL, NULL, NULL, NULL,
                                       NULL)) == ERROR_SUCCESS)
        {
            char sub_key_name[MAX_PATH] = {0};
            for (DWORD i = 0; i < sub_key_count; i++)
            {
                // 枚举子键【第三层】
                if ((hOption = RegEnumKey(hresult, i, sub_key_name, MAX_PATH)) == ERROR_SUCCESS)
                {
                    // 删除子键
                    if (hOption = RegDeleteKey(hresult, sub_key_name) != ERROR_SUCCESS)
                    {
                        error_count++;
                        printf("\n error 改键  %s 存在子键 无法删除 \n", sub_key_name);
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
        // 关闭键
        RegCloseKey(hresult);
    }
    else
    {
        error_count++;
        printf("\n error %s 是无效 or 需要管理员权限...\n", lpSubKey);
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
