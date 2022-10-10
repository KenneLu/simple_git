#pragma once

#include "../minimal.h"

typedef enum
{
    NONE = 0,						//ʲô��������
    HELLO,							//�ͻ��� ������� ���� ȷ�� ��������
    HEI,							//����������
    COMMIT,							//�ύ��Ϣ
    COMMIT_VERSION_LIST,			//�ύ�汾�б�
    VERSION_LOG,					//�������� �汾�ύ��־
    VERSION_STATUS,					//�汾�޸����� ���ΰ汾״̬
    PROTOCOL_ERROR,					//�ڷ����������Ĵ�����Ϣ
    VERSION_PROTOCOL,				//��ȡ���·������汾��Ϣ
    VERSION_LIST,					//��ȡ���·������汾�б�
    VERSION_NEST,					//��ȡ�ڱ��汾�ŵĻ����ϵ���һ���汾��
    VERSION_NEST_COMPLETE,			//�������汾�б��ͳɹ�
    VERSION_CLASH,					//��� ��ͻ
    CLASH_CONTENT,					//��ȡ ��ͻ ����
    NO_CLASH,						//�޳�ͻ
    CHECK_REMOVE_FILENAME,			//�����û�д��ڿͻ���ɾ���ĸ��� �о�֪ͨ�����ɾ��������
    REMOVE_CONTENT,					//�ύ�Ƴ�������
    GIT_CLONE,						//��¡���������
    FILE_EXIT,						//�ļ��Ƿ����
    FIND_VERSION_LIST,				//���ݰ汾·��Ѱ�����ݶ�Ӧ�汾����
    CONNECT_TIME_OUT,				//���ӳ�ʱ
    REMOVE_CLIENT_REQUEST,			//���Ϳͻ����Ƴ� Э��
    SAVE_VERSION_REQUEST,			//֪ͨ������ �洢��ǰ�汾
    GIT_RESET_VERSION,				//��ԭ�汾
} EGitProtocolType;

//��ӦԶ�˵�����
#define SERVER_TYPE_URL "\\git_server\\type.protocol"
#define SERVER_CONTENT_URL "\\git_server\\content.protocol"

//���߳̽��п��� 
extern HANDLE hwork;
extern HANDLE hrev;
extern EGitProtocolType recv_protocol;
extern char network_data_buf[8192 * 1024];
extern float connect_time;
extern bool check_connect;

// �û���Ϣ
typedef struct
{
    char account[MAX_PATH];
    char password[MAX_PATH];
    char email[MAX_PATH];
} FGitUser;
FGitUser user; // ��Ҫһ��ȫ�ֵ�ʵ��

_CRT_BEGIN_C_HEADER
void init_operation(unsigned int protocol_type,void * connect_time_init); // ���� protocol_type ���ͳ�ʼ����Ӧ��ȫ�ֱ���
void init_network_data_buf(); // ������紫��Ļ�������

bool connect_is_alive(const char *url,const char *addr); // ���������Ƿ���ڣ�ͨ���Ƿ�����
bool send_file(const char *url, const char *client_file); // �������ļ�����Զ��
void receive_content(EGitProtocolType *type,char *out_content);//�����߳� ȥȡ���ǵ�����

bool send_protocol_type(const char *url, EGitProtocolType protocol_type); // �� protocol_type ����Զ�� 
bool send_protocol_content(const char *url,const char *content); // �� content ����Զ��

void server_protocol_type_file_exit(const char *url); // �����������Ƿ���� type.protocol
void server_protocol_content_file_exit(const char *url); // �����������Ƿ���� content.protocol

void read_client_protocol_content(char *out_content); // ��ȡ���ص�  content.protocol ����
unsigned char read_client_protocol_type(); // ��ȡ���ص�  type.protocol ����

void git_connect_start(); // ������ӿ�ʼ
void git_connect_end(); // ������ӽ���

void save_user_ini(); // �洢�û���Ϣ�������ļ�
void read_user_ini(); // ��ȡ�����ļ��е��û���Ϣ
_CRT_END_C_HEADER