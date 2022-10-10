#pragma once

#include "../minimal.h"
#include "guid.h"

// 提交的版本信息
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

// 文件版本操作类型
typedef enum
{
    NONE_File,
    ADD_FILE,
    SUBTRACT_FILE,
    OVERLAP_FILE,
} EVersionOperationType;

// 单个文件的版本
typedef struct
{
    EVersionOperationType operation_type;
    unsigned int file_size;
    char file_name[MAX_PATH];
    FGitGuid crc;
} FGitVersion;

// 整体文件的版本
typedef struct
{
    unsigned int size;
    FGitVersion paths[1024];
} FGitVersions;
FGitVersions versions;

// 整体文件版本列表
typedef struct
{
    unsigned int size;
    FGitVersions paths[1024];
} FGitVersionsList;

// git 的路径
typedef struct
{
    char path_src[MAX_PATH];
    char path_dis[MAX_PATH];
} FGitPath2D;

// git 所有提交的所有路径
typedef struct
{
    int size;
    FGitPath2D paths[MAX_PATH * 2];
} FGitPath2Ds;
FGitPath2Ds git_path_2ds;

void string_to_git_commit(FGitCommit* out_git_commit, const char* str); // 字符串处理
void string_to_git_commits(FGitCommits* out_git_commits, const char* str); // 字符串处理
void string_to_versions(FGitVersions* out_in_versions, const char* str); // 字符串处理
char* git_commit_to_string(char* out_str, const FGitCommit* git_commit); // 字符串处理
void versions_to_string(char* out_str, const FGitVersions* in_versions); // 字符串处理

// 初始化
void init_commit(FGitCommit* git_commit); // 初始化 git_commit 为 0
void init_git_commits(FGitCommits* git_commits); // 初始化 git_commits 为 0
void init_versions(FGitVersions* versions); // 初始化 versions 为 0
void init_git_path_2ds(FGitPath2Ds* path_2ds); // 初始化 path_2ds 为 0

// core
char* get_git_commit(FGitCommit* out_git_commit); // 获取客户端 commit 内容
void add_git_versions(const char* int_path, EVersionOperationType type, FGitVersions* out_versions);