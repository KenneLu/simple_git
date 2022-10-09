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
    REMOVE_LOCAL_REQUEST,			//发送客户端移除 协议
    SAVE_VERSION_REQUEST,			//通知服务器 存储当前版本
    GIT_RESET_VERSION,				//还原版本
} EGitProtocolType;

//对应远端的名字
#define REMOTE_T_URL "\\git_server\\type.protocol"
#define REMOTE_C_URL "\\git_server\\content.protocol"

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
void init_operation(unsigned int type,void * value);
void init_network_data_buf_protocol();

bool git_connect(const char *url,const char *addr);
bool git_send(const char *url, const char *local_path);
void git_receive(EGitProtocolType *type,char *buf);//工作线程 去取我们的数据

bool git_send_protocol_type(const char *url, EGitProtocolType type); // 将本地 EGitProtocolType 存到远端 type.protocol
bool git_send_content(const char *url,const char *buf); // 将 buf 存到远端 content.protocol

void git_is_server_exit_type(const char *url);
void git_is_server_exit_content(const char *url);

void get_protocol_content(char *buf);
unsigned char get_protocol();

void git_connect_start();
void git_connect_end();

void save_user_ini(); // 存储用户信息到配置文件
void read_user_ini(); // 获取配置文件中的用户信息
_CRT_END_C_HEADER