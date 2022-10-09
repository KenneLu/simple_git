#include "../../public/git/service.h"
#include "../../public/git/path.h"
#include "../../public/git/global.h"
#include "../../public/util/file_util.h"
#include "../../public/util/string_util.h"

#define BUF_SIZE 260

float connect_time = 10.f;
bool check_connect = false;
EGitProtocolType recv_protocol = NONE;
HANDLE hwork;
HANDLE hrev;
char network_data_buf[8192 * 1024] = {0};

void init_operation(unsigned int type, void* value)
{
    if ((EGitProtocolType)type == CONNECT_TIME_OUT)
    {
        connect_time = *(float*)value;
    }
    else if (0) //可以扩展设置
    {
        //......
    }
    else
    {
    }
}

void init_network_data_buf_protocol()
{
    memset(network_data_buf, 0, 512);
    recv_protocol = NONE;
}

bool git_connect(const char* url, const char* addr)
{
    if (git_send_protocol_type(url, HELLO) &&
        git_send_content(url, addr))
    {
        EGitProtocolType t = NONE;

        char buf[512] = {0};
        git_receive(&t, buf); //阻塞

        if (t == HEI)
        {
            return true;
        }
    }

    return false;
}

bool git_send(const char* url, const char* local_path)
{
    if (copy_file(local_path, url) == 0)
    {
        return true;
    }

    return false;
}

void git_receive(EGitProtocolType* type, char* buf)
{
    git_connect_start();

    SuspendThread(hwork);

    git_connect_end();

    // 拿到 network_data_buf
    strcpy(buf, network_data_buf);
    *type = recv_protocol;

    // 清空 network_data_buf
    init_network_data_buf_protocol();
}

bool git_send_protocol_type(const char* url, EGitProtocolType type)
{
    // 当前路径下创建一个 type.protocol 文件
    char local_buf_path[BUF_SIZE] = {0};
    _getcwd(local_buf_path, BUF_SIZE);
    strcat(local_buf_path, "\\");
    strcat(local_buf_path, "type.protocol");
    
    char buf[BUF_SIZE] = {0};
    if (create_file(local_buf_path))
    {
        // 写入 EGitProtocolType 进 type.protocol 文件
        char* content = _itoa((int)type, buf, 10);
        add_file(local_buf_path, content);

        // 拼接远端服务器 type.protocol 文件路径
        char url_param[BUF_SIZE] = {0};
        strcpy(url_param, url);
        strcat(url_param, REMOTE_T_URL);

        // 将本地 type.protocol 发送远端，并替换远端文件
        if (git_send(url_param, local_buf_path))
        {
            // 删除本地 type.protocol 文件
            remove(local_buf_path);
            return true;
        }
    }

    return false;
}

bool git_send_content(const char* url, const char* buf)
{
    // 当前路径下创建一个 type.protocol 文件
    char local_buf_path[BUF_SIZE] = {0};
    _getcwd(local_buf_path, BUF_SIZE);
    strcat(local_buf_path, "\\");
    strcat(local_buf_path, "content.protocol");

    if (create_file(local_buf_path))
    {
        // 写入 buf
        add_file(local_buf_path, buf);

        // 拼接远端服务器 type.protocol 文件路径
        char url_param[BUF_SIZE] = {0};
        strcpy(url_param, url);
        strcat(url_param, REMOTE_C_URL);

        // 将本地 type.protocol 发送远端，并替换远端文件
        if (git_send(url_param, local_buf_path))
        {
            // 删除本地 type.protocol 文件
            remove(local_buf_path);
            return true;
        }
    }

    return false;
}

void git_is_exit_type(const char* url_param)
{
    bool b_exit = true;
    while (b_exit)
    {
        if (_access(url_param, 0) == 0)
        {
            Sleep(100);
        }
        else
        {
            b_exit = false;
        }
    }
}

void git_is_server_exit_type(const char* url)
{
    char url_param[BUF_SIZE] = {0};
    strcpy(url_param, url);

    strcat(url_param, REMOTE_T_URL);

    git_is_exit_type(url_param);
}

void git_is_server_exit_content(const char* url)
{
    char url_param[BUF_SIZE] = {0};
    strcpy(url_param, url);

    strcat(url_param, REMOTE_C_URL);

    git_is_exit_type(url_param);
}

void get_protocol_content(char* buf)
{
    char path_buf[MAX_PATH] = {0};
    strcpy(path_buf, get_git_client_cache());
    strcat(path_buf, "\\content.protocol");
    read_file(path_buf, buf);

    remove(path_buf);
}

unsigned char get_protocol()
{
    unsigned char ret = 0;

    const char* p = get_git_client_cache();
    char path_tmp[MAX_PATH] = {0};
    strcpy(path_tmp, p);

    strcat(path_tmp, "\\type.protocol");

    if (_access(path_tmp, 0) == 0)
    {
        char buf[MAX_PATH] = {0};
        read_file(path_tmp, buf);

        remove(path_tmp);

        ret = atoi(buf);
    }

    return ret;
}

void git_connect_start()
{
    connect_time = 10;
    check_connect = true;
}

void git_connect_end()
{
    connect_time = 10;
    check_connect = false;
}

void save_user_ini()
{
    char* config_path_buf = get_user_ini_file();

    FILE* fp = NULL;
    if ((fp = fopen(config_path_buf, "w+")) != NULL)
    {
        fprintf(fp, "account=%s\n", user.account);
        fprintf(fp, "password=%s\n", user.password);
        fprintf(fp, "email=%s\n", user.email);
        fprintf(fp, "url=%s\n", remote_origin);

        fclose(fp);
    }
}

void read_user_ini()
{
    // 获取 ini 配置文件
    const char* config_file_path = get_user_ini_file();

    // 读取 ini 配置文件
    char config_buf[2048] = {0};
    if (read_file(config_file_path, config_buf))
    {
        // 按行拆分读到的内容
        FArray config_content;
        split_string(config_buf, "\n", &config_content);
        // 按类别读取参数值
        for (int index = 0; index < config_content.size; index++)
        {
            char* c_string = get_array_element(&config_content, index);
            if (strstr(c_string, "account="))
            {
                split_string_with_index(c_string, "=", user.account, 1);
            }
            else if (strstr(c_string, "password="))
            {
                split_string_with_index(c_string, "=", user.password, 1);
            }
            else if (strstr(c_string, "email="))
            {
                split_string_with_index(c_string, "=", user.email, 1);
            }
            else if (strstr(c_string, "url="))
            {
                split_string_with_index(c_string, "=", remote_origin, 1);
            }
        }
        // 释放内存
        destroy_array(&config_content);
    }
}
