#pragma once

#include "../minimal.h"
#include "guid.h"

// �ύ�İ汾��Ϣ
typedef struct
{
    char name[MAX_PATH];
    char commit[MAX_PATH];
    char date[MAX_PATH];
    FGitGuid guid;
} FGitCommit;
FGitCommit commit;

// FGitCommit Last_commit;
typedef struct
{
    unsigned int size;
    FGitCommit commits[1024];
} FGitCommits;

// �ļ��汾��������
typedef enum
{
    NONE_File,
    ADD_FILE,
    SUBTRACT_FILE,
    OVERLAP_FILE,
} EVersionOperationType;

// �����ļ��İ汾
typedef struct
{
    EVersionOperationType operation_type;
    unsigned int file_size;
    char file_name[MAX_PATH];
    FGitGuid crc;
} FGitVersion;

// �����ļ��İ汾
typedef struct
{
    unsigned int size;
    FGitVersion paths[1024];
} FGitVersions;
FGitVersions versions;

// �����ļ��汾�б�
typedef struct
{
    unsigned int size;
    FGitVersions paths[1024];
} FGitVersionsList;

// git ��·��
typedef struct
{
    char path_src[MAX_PATH];
    char path_dis[MAX_PATH];
} FGitPath2D;

// git �����ύ������·��
typedef struct
{
    int size;
    FGitPath2D paths[MAX_PATH * 2];
} FGitPath2Ds;
FGitPath2Ds git_path_2ds;

void string_to_git_commit(FGitCommit* out_git_commit, const char* str); // �ַ�������
void string_to_git_commits(FGitCommits* out_git_commits, const char* str); // �ַ�������
void string_to_versions(FGitVersions* out_in_versions, const char* str); // �ַ�������
char* git_commit_to_string(char* out_str, const FGitCommit* git_commit); // �ַ�������
void versions_to_string(char* out_str, const FGitVersions* in_versions); // �ַ�������

// ��ʼ��
void init_commit(FGitCommit* git_commit); // ��ʼ�� git_commit Ϊ 0
void init_git_commits(FGitCommits* git_commits); // ��ʼ�� git_commits Ϊ 0
void init_versions(FGitVersions* versions); // ��ʼ�� versions Ϊ 0
void init_git_path_2ds(FGitPath2Ds* path_2ds); // ��ʼ�� path_2ds Ϊ 0

// core
char* get_git_commit(FGitCommit* out_git_commit); // ��ȡ�ͻ��� commit ����
void add_git_versions(const char* int_path, EVersionOperationType type, FGitVersions* out_versions);