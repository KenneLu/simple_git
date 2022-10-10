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

void init_operation(unsigned int protocol_type, void* connect_time_init)
{
    if ((EGitProtocolType)protocol_type == CONNECT_TIME_OUT)
    {
        connect_time = *(float*)connect_time_init;
    }
    else if (0) //可以扩展设置
    {
        //......
    }
    else
    {
    }
}

void init_network_data_buf()
{
    memset(network_data_buf, 0, 512);
    recv_protocol = NONE;
}

bool connect_is_alive(const char* url, const char* addr)
{
    if (send_protocol_type(url, HELLO) &&
        send_protocol_content(url, addr))
    {
        EGitProtocolType type = NONE;

        char buf[512] = {0};
        receive_content(&type, buf); //阻塞

        if (type == HEI)
        {
            return true;
        }
    }

    return false;
}

bool send_file(const char* url, const char* client_file)
{
    if (copy_file(url, client_file) == 0)
    {
        return true;
    }

    return false;
}

void receive_content(EGitProtocolType* type, char* out_content)
{
    git_connect_start();

    SuspendThread(hwork);

    git_connect_end();

    // 拿到 network_data_buf
    strcpy(out_content, network_data_buf);
    *type = recv_protocol;

    // 清空 network_data_buf
    init_network_data_buf();
}

bool send_protocol_type(const char* url, EGitProtocolType protocol_type)
{
    // 当前路径下创建一个 type.protocol 文件
    char client_type_protocol_file[BUF_SIZE] = {0};
    _getcwd(client_type_protocol_file, BUF_SIZE);
    strcat(client_type_protocol_file, "\\");
    strcat(client_type_protocol_file, "type.protocol");
    
    if (create_file(client_type_protocol_file))
    {
        // 写入 EGitProtocolType 进 type.protocol 文件
        char buf[BUF_SIZE] = {0};
        char* content = _itoa((int)protocol_type, buf, 10);
        add_file(client_type_protocol_file, content);

        // 拼接远端服务器 type.protocol 文件路径
        char server_type_protocol_file[BUF_SIZE] = {0};
        strcpy(server_type_protocol_file, url);
        strcat(server_type_protocol_file, SERVER_TYPE_URL);

        // 将本地 type.protocol 文件发送远端
        if (send_file(server_type_protocol_file, client_type_protocol_file))
        {
            // 删除本地 type.protocol 文件
            remove(client_type_protocol_file);
            return true;
        }
    }

    return false;
}

bool send_protocol_content(const char* url, const char* content)
{
    // 当前路径下创建一个 type.protocol 文件
    char client_content_protocol_file[BUF_SIZE] = {0};
    _getcwd(client_content_protocol_file, BUF_SIZE);
    strcat(client_content_protocol_file, "\\");
    strcat(client_content_protocol_file, "content.protocol");

    if (create_file(client_content_protocol_file))
    {
        // 写入 buf
        add_file(client_content_protocol_file, content);

        // 拼接远端服务器 type.protocol 文件路径
        char server_content_protocol_file[BUF_SIZE] = {0};
        strcpy(server_content_protocol_file, url);
        strcat(server_content_protocol_file, SERVER_CONTENT_URL);

        // 将本地 type.protocol 发送远端
        if (send_file(server_content_protocol_file, client_content_protocol_file))
        {
            // 删除本地 type.protocol 文件
            remove(client_content_protocol_file);
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

void server_protocol_type_file_exit(const char* url)
{
    char server_type_protocol_file[BUF_SIZE] = {0};
    strcpy(server_type_protocol_file, url);

    strcat(server_type_protocol_file, SERVER_TYPE_URL);

    git_is_exit_type(server_type_protocol_file);
}

void server_protocol_content_file_exit(const char* url)
{
    char server_content_protocol_file[BUF_SIZE] = {0};
    strcpy(server_content_protocol_file, url);

    strcat(server_content_protocol_file, SERVER_CONTENT_URL);

    git_is_exit_type(server_content_protocol_file);
}

void read_client_protocol_content(char* out_content)
{
    char client_content_protocol_file[MAX_PATH] = {0};
    strcpy(client_content_protocol_file, get_git_client_cache());
    strcat(client_content_protocol_file, "\\content.protocol");
    read_file(client_content_protocol_file, out_content);

    remove(client_content_protocol_file);
}

unsigned char read_client_protocol_type()
{
    unsigned char type = 0;
    char client_type_protocol_file[MAX_PATH] = {0};
    strcpy(client_type_protocol_file, get_git_client_cache());
    strcat(client_type_protocol_file, "\\type.protocol");

    if (_access(client_type_protocol_file, 0) == 0)
    {
        char buf[MAX_PATH] = {0};
        read_file(client_type_protocol_file, buf);
        remove(client_type_protocol_file);
        type = atoi(buf);
    }

    return type; // 默认返回 0 NONE
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
    char* config_path_buf = get_client_user_ini_file();

    FILE* fp = NULL;
    if ((fp = fopen(config_path_buf, "w+")) != NULL)
    {
        fprintf(fp, "account=%s\n", user.account);
        fprintf(fp, "password=%s\n", user.password);
        fprintf(fp, "email=%s\n", user.email);
        fprintf(fp, "url=%s\n", server_url);

        fclose(fp);
    }
}

void read_user_ini()
{
    // 获取 ini 配置文件
    const char* config_file_path = get_client_user_ini_file();

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
                split_string_with_index(c_string, "=", server_url, 1);
            }
        }
        // 释放内存
        destroy_array(&config_content);
    }
}
