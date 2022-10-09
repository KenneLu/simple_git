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
    REMOVE_LOCAL_REQUEST,			//���Ϳͻ����Ƴ� Э��
    SAVE_VERSION_REQUEST,			//֪ͨ������ �洢��ǰ�汾
    GIT_RESET_VERSION,				//��ԭ�汾
} EGitProtocolType;

//��ӦԶ�˵�����
#define REMOTE_T_URL "\\git_server\\type.protocol"
#define REMOTE_C_URL "\\git_server\\content.protocol"

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
void init_operation(unsigned int type,void * value);
void init_network_data_buf_protocol();

bool git_connect(const char *url,const char *addr);
bool git_send(const char *url, const char *local_path);
void git_receive(EGitProtocolType *type,char *buf);//�����߳� ȥȡ���ǵ�����

bool git_send_protocol_type(const char *url, EGitProtocolType type); // ������ EGitProtocolType �浽Զ�� type.protocol
bool git_send_content(const char *url,const char *buf); // �� buf �浽Զ�� content.protocol

void git_is_server_exit_type(const char *url);
void git_is_server_exit_content(const char *url);

void get_protocol_content(char *buf);
unsigned char get_protocol();

void git_connect_start();
void git_connect_end();

void save_user_ini(); // �洢�û���Ϣ�������ļ�
void read_user_ini(); // ��ȡ�����ļ��е��û���Ϣ
_CRT_END_C_HEADER