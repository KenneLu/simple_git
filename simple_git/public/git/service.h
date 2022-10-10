#pragma once

#include "../minimal.h"

typedef enum
{
    NONE = 0,						//什么都不发生
    HELLO,							//客户端 向服务器 发送 确认 用于链接
    HEI,							//服务器反馈
    COMMIT,							//提交信息
    COMMIT_VERSION_LIST,			//提交版本列表
    VERSION_LOG,					//请求服务端 版本提交日志
    VERSION_STATUS,					//版本修改内容 本次版本状态
    PROTOCOL_ERROR,					//在服务器发生的错误信息
    VERSION_PROTOCOL,				//获取最新服务器版本信息
    VERSION_LIST,					//获取最新服务器版本列表
    VERSION_NEST,					//获取在本版本号的基础上的下一个版本号
    VERSION_NEST_COMPLETE,			//服务器版本列表发送成功
    VERSION_CLASH,					//检测 冲突
    CLASH_CONTENT,					//获取 冲突 内容
    NO_CLASH,						//无冲突
    CHECK_REMOVE_FILENAME,			//检查有没有存在客户端删除的副本 有就通知服务端删除该数据
    REMOVE_CONTENT,					//提交移除的内容
    GIT_CLONE,						//克隆服务端数据
    FILE_EXIT,						//文件是否存在
    FIND_VERSION_LIST,				//根据版本路径寻找数据对应版本内容
    CONNECT_TIME_OUT,				//连接超时
    REMOVE_CLIENT_REQUEST,			//发送客户端移除 协议
    SAVE_VERSION_REQUEST,			//通知服务器 存储当前版本
    GIT_RESET_VERSION,				//还原版本
} EGitProtocolType;

//对应远端的名字
#define SERVER_TYPE_URL "\\git_server\\type.protocol"
#define SERVER_CONTENT_URL "\\git_server\\content.protocol"

//对线程进行控制 
extern HANDLE hwork;
extern HANDLE hrev;
extern EGitProtocolType recv_protocol;
extern char network_data_buf[8192 * 1024];
extern float connect_time;
extern bool check_connect;

// 用户信息
typedef struct
{
    char account[MAX_PATH];
    char password[MAX_PATH];
    char email[MAX_PATH];
} FGitUser;
FGitUser user; // 需要一个全局的实例

_CRT_BEGIN_C_HEADER
void init_operation(unsigned int protocol_type,void * connect_time_init); // 根据 protocol_type 类型初始化对应的全局变量
void init_network_data_buf(); // 清空网络传输的缓存数据

bool connect_is_alive(const char *url,const char *addr); // 检测服务器是否存在，通信是否正常
bool send_file(const char *url, const char *client_file); // 将本地文件发送远端
void receive_content(EGitProtocolType *type,char *out_content);//工作线程 去取我们的数据

bool send_protocol_type(const char *url, EGitProtocolType protocol_type); // 将 protocol_type 发送远端 
bool send_protocol_content(const char *url,const char *content); // 将 content 发送远端

void server_protocol_type_file_exit(const char *url); // 检测服务器端是否存在 type.protocol
void server_protocol_content_file_exit(const char *url); // 检测服务器端是否存在 content.protocol

void read_client_protocol_content(char *out_content); // 读取本地的  content.protocol 内容
unsigned char read_client_protocol_type(); // 读取本地的  type.protocol 内容

void git_connect_start(); // 标记链接开始
void git_connect_end(); // 标记链接结束

void save_user_ini(); // 存储用户信息到配置文件
void read_user_ini(); // 获取配置文件中的用户信息
_CRT_END_C_HEADER