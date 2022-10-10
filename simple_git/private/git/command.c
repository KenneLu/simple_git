#include "../../public/git/command.h"
#include "../../public/git/ignore.h"
#include "../../public/git/path.h"
#include "../../public/git/global.h"
#include "../../public/git/service.h"
#include "../../public/log/log.h"
#include "../../public/window/window_setting.h"
#include "../../public/util/string_util.h"

//////////////////////////////////////////////////////////////////////////////////////// util ↓
void init_path(FPath* p)
{
    p->size = 0;
    memset(p, 0, sizeof(p->data));
}



bool check_command(const char* buf_path)
{
    return buf_path[0] == -3
        || buf_path[0] == '\n'
        || buf_path[0] == '\0'
        || (buf_path[0] == ' '
            && buf_path[1] == ' ');
}

//比较不一样的 
//服务器没有的 客户端有的 要添加到服务器
void compare_list(const FGitVersions* server_version_list, const FFilePath* client_paths, FFilePath* out_paths)
{
    const char* client_url = get_project_cache();

    //添加 (不一样的)
    for (int i = 0; i < client_paths->index; i++)
    {
        char path_tmp[MAX_PATH] = {0}; //相对路径
        strcpy(path_tmp, client_paths->paths[i]);
        remove_string_start(path_tmp, client_url);

        int size = get_file_size(client_paths->paths[i]);

        bool b_exit = false;
        for (unsigned int j = 0; j < server_version_list->size; j++)
        {
            if (strcmp(server_version_list->paths[j].file_name, path_tmp) == 0) //比较相对路径
            {
                if (size != server_version_list->paths[j].file_size)
                {
                    strcpy(out_paths->paths[out_paths->index++], client_paths->paths[i]); //绝对路径
                }

                b_exit = true;
                break;
            }
        }

        if (!b_exit)
        {
            strcpy(out_paths->paths[out_paths->index++], client_paths->paths[i]); //绝对路径
        }
    }
}


char* get_current_time()
{
    return ctime(__TIME__);
}




void version_iteration_update()
{
    //阻塞
    server_protocol_type_file_exit(server_url);
    server_protocol_content_file_exit(server_url);

    // 1.0 检测当前版本是否是最新 获取服务器最新版本号，然后比较 看看是不是一样的
    if (send_protocol_type(server_url, VERSION_PROTOCOL) &&
        send_protocol_content(server_url, get_project_cache()))
    {
        log_success("获取服务器 最新版本号");

        EGitProtocolType type = NONE;
        char buf_log[8196 * 1024] = {0};
        receive_content(&type, buf_log); //阻塞工作线程

        if (buf_log[0] == '\0')
        {
            log_error("buf_log = 不为空");

            return;
        }
        if (type == VERSION_PROTOCOL)
        {
            FGitCommit Server_latest_version;
            init_commit(&Server_latest_version);
            string_to_git_commit(&Server_latest_version, buf_log);

            FGitCommit client_latest_version;
            init_commit(&client_latest_version);
            get_git_commit(&client_latest_version);

            if (client_latest_version.name[0] == '\0' ||
                client_latest_version.commit[0] == '\0' ||
                client_latest_version.date[0] == '\0')
            {
                git_clone_func();

                return;
            }

            //比较版本是否一样
            if (!guid_equal(&client_latest_version.guid, &Server_latest_version.guid))
            {
                const char* client_url = get_project_cache(); //F:\\sadas\\asd\\ads\\

                //验证冲突
                char str_content[8196] = {0};
                guid_to_string(str_content, &client_latest_version.guid);
                strcat(str_content, "|");
                strcat(str_content, client_url);
                strcat(str_content, "|");

                char buf_local[MAX_PATH] = {0};
                _getcwd(buf_local, MAX_PATH);
                strcat(buf_local, "\\");
                remove_string_start(buf_local, get_project_cache());

                if (buf_local[0] == '\0')
                {
                    strcat(buf_local, "root");
                }

                strcat(str_content, buf_local);
                strcat(str_content, "|");

                server_protocol_type_file_exit(server_url);
                server_protocol_content_file_exit(server_url);

                char path_tmp[MAX_PATH] = {0};
                strcpy(path_tmp, client_url);
                remove_char_end(path_tmp, '\\');

                FFilePath local_paths;
                init_file_path(&local_paths);

                FFilePath paths;
                init_file_path(&paths);

                find_files(path_tmp, &local_paths, true);

                //移除 被忽略的
                for (int i = 0; i < local_paths.index; i++)
                {
                    if (!is_ignore(local_paths.paths[i]))
                    {
                        strcpy(paths.paths[paths.index++], local_paths.paths[i]);
                    }
                }

                //提取本地列表数据
                char buf_local_versions[8196 * 10] = {0};
                read_file(get_client_version_list_file(), buf_local_versions);

                FGitVersions local_versions;
                init_versions(&local_versions);
                string_to_versions(&local_versions, buf_local_versions);

                for (int i = 0; i < paths.index; i++)
                {
                    int size = get_file_size(paths.paths[i]);
                    remove_string_start(paths.paths[i], client_url);

                    char new_guid[MAX_PATH] = {0};
                    for (unsigned int j = 0; j < local_versions.size; j++)
                    {
                        if (strcmp(local_versions.paths[j].file_name, paths.paths[i]) == 0)
                        {
                            if (size != local_versions.paths[j].file_size)
                            {
                                create_guid(&local_versions.paths[j].crc);
                            }

                            guid_to_string(new_guid, &local_versions.paths[j].crc);
                            break;
                        }
                    }

                    if (!is_guid_valid_str(new_guid))
                    {
                        create_guid_str(new_guid);
                    }

                    char size_buf[MAX_PATH] = {0};
                    char* size_p = _itoa(size, size_buf, 10);

                    //模仿版本结构 其中只需要 路径 和 大小
                    placeholder_cat_s(str_content,
                                      "0\n%s\n%s\n%s\n",
                                      paths.paths[i], new_guid, size_p);
                }

                remove_char_end(str_content, '\n');

                if (send_protocol_type(server_url, VERSION_CLASH) &&
                    send_protocol_content(server_url, str_content))
                {
                    type = NONE;
                    memset(buf_log, 0, sizeof(buf_log));
                    receive_content(&type, buf_log); //阻塞工作线程
                    if (type == CLASH_CONTENT)
                    {
                        log_error("%s", buf_log);
                        log_error("请解决以上冲突 再进行更新数据...");
                    }
                    else if (type == NO_CLASH)
                    {
                        memset(str_content, 0, sizeof(str_content));
                        guid_to_string(str_content, &client_latest_version.guid);

                        strcat(str_content, "\n");
                        strcat(str_content, get_project_cache());

                        // 1.1 把我们当前的客户端版本发送到服务端 
                        server_protocol_type_file_exit(server_url);
                        server_protocol_content_file_exit(server_url);
                        if (send_protocol_type(server_url, VERSION_NEST) &&
                            send_protocol_content(server_url, str_content))
                        {
                            log_success("获取下一个需要更新的版本");

                            // 1.2 接受新版本号 存储在本地，替换
                            type = NONE;
                            memset(buf_log, 0, sizeof(buf_log));
                            receive_content(&type, buf_log); //阻塞工作线程

                            if (buf_log[0] == '\0')
                            {
                                log_error("buf_log = 不为空");

                                return;
                            }

                            //init_commit(&client_latest_version);
                            //string_to_git_commit(&client_latest_version, buf_log);

                            rewrite_file(get_client_version_info_file(), buf_log);

                            type = NONE;
                            memset(buf_log, 0, sizeof(buf_log));
                            receive_content(&type, buf_log); //阻塞工作线程

                            if (buf_log[0] == '\0')
                            {
                                log_error("buf_log = 不为空");

                                return;
                            }

                            if (type == VERSION_NEST_COMPLETE)
                            {
                                rewrite_file(get_client_version_list_file(), buf_log);

                                FGitVersions v_content;
                                init_versions(&v_content);
                                string_to_versions(&v_content, buf_log);

                                for (unsigned int i = 0; i < v_content.size; i++)
                                {
                                    if (v_content.paths[i].operation_type == ADD_FILE)
                                    {
                                        log_log("create server data : %s +++|%i byte|+++|",
                                                v_content.paths[i].file_name, v_content.paths[i].file_size);
                                    }
                                    else if (v_content.paths[i].operation_type == SUBTRACT_FILE)
                                    {
                                        log_success("remove server data : %s ----|%i byte|----|",
                                                    v_content.paths[i].file_name, v_content.paths[i].file_size);
                                    }
                                    else if (v_content.paths[i].operation_type == OVERLAP_FILE)
                                    {
                                        log_success("overlap server data : %s __+_+_+_|%i byte|_+_++|",
                                                    v_content.paths[i].file_name, v_content.paths[i].file_size);
                                    }
                                }

                                version_iteration_update();
                            }
                        }
                    }
                    else if (type == PROTOCOL_ERROR)
                    {
                        log_error("server call ：%s", buf_log);
                    }
                    else
                    {
                        log_error("服务器 奔溃了 `_`...");
                    }
                }
            }
            else
            {
                log_log("已经是最新了...");
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////// util ↑

//////////////////////////////////////////////////////////////////////////////////////// command ↓
void git_init_func()
{
    char tmp_path[256] = {0};

    // 创建 .git 文件夹
    strcat(tmp_path, ".\\.git\\");
    _mkdir(tmp_path);

    // git.txt 用于记录版本信息
    strcat(tmp_path, "git.txt");

    if (create_file(tmp_path)) // 创建文件
    {
        char* p = get_git_client_cache();
        _mkdir(p);

        // 需要忽略的文件
        char ignore_content_path[MAX_PATH] = {0};
        strcpy(ignore_content_path, get_client_git_cache());
        strcat(ignore_content_path, "ignore_path.ig");

        // 需要忽略的文件后缀
        char ignore_content_suffix[MAX_PATH] = {0};
        strcpy(ignore_content_suffix, get_client_git_cache());
        strcat(ignore_content_suffix, "ignore_suffix.ig");

        // 生成忽略文件夹内容
        char ignore_content[1024] = "log\\\n.git\\\n.vs\\\ngit_client\\\n";

        // 拷贝忽略文件夹内容至 project_name 中
        strcat(ignore_content, project_name);

        // 拼接需要忽略的文件
        rewrite_file(ignore_content_path, ignore_content);

        // 生成忽略文件
        memset(ignore_content, 0, sizeof(ignore_content));
        strcpy(ignore_content, ".ilk\n.pdb");
        rewrite_file(ignore_content_suffix, ignore_content);

        // 读取忽略的路径
        char buf_ignore[1024] = {0};
        read_file(ignore_content_path, buf_ignore);
        string_to_ignore_path(buf_ignore);

        // 读取忽略的后缀
        memset(buf_ignore, 0, sizeof(buf_ignore));
        read_file(ignore_content_suffix, buf_ignore);
        string_to_ignore_suffix(buf_ignore);

        // 配置 log 路径
        const char* project_path = get_project_cache();
        if (project_path[0] != '\0')
        {
            char p_tmp[MAX_PATH] = {0};
            strcat(p_tmp, project_path);
            strcat(p_tmp, "log");
            set_log_folder(p_tmp);
        }

        bool ret = log_success("本地初始化成功");
        if (!ret)
        {
            printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
        }
    }
    else // 文件创建失败
    {
        bool ret = log_error("创建项目路径 .git 失败");
        if (!ret)
        {
            printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
        }
    }
}

bool git_status_func()
{
    if (server_url[0] == '\0')
    {
        bool ret = log_error("未指定 服务器地址");
        if (!ret)
        {
            printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
        }
        return false;
    }

    //阻塞
    server_protocol_type_file_exit(server_url);
    server_protocol_content_file_exit(server_url);

    FGitCommit git_commit;
    get_git_commit(&git_commit);

    char buf_guid[1024] = {0};
    guid_to_string(buf_guid, &git_commit.guid);

    //sdad//asd//as
    const char* client_addr = get_project_cache();
    char buf_content[1024] = {0};
    strcat(buf_content, client_addr);
    remove_char_end(buf_content, '\\');
    strcat(buf_content, "\n");
    strcat(buf_content, buf_guid);

    if (send_protocol_type(server_url, VERSION_STATUS) &&
        send_protocol_content(server_url, buf_content))
    {
        log_success("地址 和 版本guid 发送成功");

        EGitProtocolType type = NONE;
        char buf_log[8196 * 1024] = {0};
        receive_content(&type, buf_log); //阻塞工作线程
        if (type == VERSION_STATUS)
        {
            if (buf_log[0] != '\0')
            {
                FGitVersions git_versions;
                init_versions(&git_versions);

                string_to_versions(&git_versions, buf_log);

                for (unsigned int i = 0; i < git_versions.size; i++)
                {
                    if (git_versions.paths[i].operation_type == ADD_FILE)
                    {
                        set_console_color(PALE_BLUE, 0);
                        printf("\n create server data :%s +++|%i byte|+++| \n", git_versions.paths[i].file_name,
                               git_versions.paths[i].file_size);
                        set_console_color(WHITE, 0);
                    }
                    else if (git_versions.paths[i].operation_type == SUBTRACT_FILE)
                    {
                        set_console_color(RED, 0);
                        printf("\n remove server data :%s ----|%i byte|----| \n", git_versions.paths[i].file_name,
                               git_versions.paths[i].file_size);
                        set_console_color(WHITE, 0);
                    }
                    else if (git_versions.paths[i].operation_type == OVERLAP_FILE)
                    {
                        set_console_color(GREY, 0);
                        printf("\n overlap server data :%s xxxxx|%i byte|xxxxx| \n", git_versions.paths[i].file_name,
                               git_versions.paths[i].file_size);
                        set_console_color(WHITE, 0);
                    }
                    else
                    {
                        set_console_color(WHITE, 0);
                        printf("\n server data :%s ---|%i byte| \n", git_versions.paths[i].file_name,
                               git_versions.paths[i].file_size);
                        set_console_color(WHITE, 0);
                    }
                }
            }
            else
            {
                log_error("服务器的版本 与提交的版本没有对上 或者 服务器数据版本未空~");
            }
        }
        else if (type == CLASH_CONTENT)
        {
            log_error("%s", buf_log);
        }
    }
    return true;
}

bool git_log_func()
{
    if (server_url[0] == '\0')
    {
        bool ret = log_error("未指定 服务器地址");
        if (!ret)
        {
            printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
        }
        return false;
    }

    //阻塞
    server_protocol_type_file_exit(server_url);
    server_protocol_content_file_exit(server_url);

    const char* client_addr = get_project_cache();
    if (send_protocol_type(server_url, VERSION_LOG) &&
        send_protocol_content(server_url, client_addr))
    {
        log_success("客户端地址 发送成功");

        EGitProtocolType type = NONE;
        char buf_log[8196] = {0};
        receive_content(&type, buf_log); //阻塞工作线程
        if (type == VERSION_LOG)
        {
            if (buf_log[0] != '\0')
            {
                FGitCommits git_commits;
                init_git_commits(&git_commits);

                string_to_git_commits(&git_commits, buf_log);
                for (unsigned int i = 0; i < git_commits.size; i++)
                {
                    char guid_buf[MAX_PATH] = {0};
                    guid_to_string(guid_buf, &git_commits.commits[i].guid);

                    set_console_color(YELLOW, 0);
                    printf("\n\n版   本  码：%s \r\n", guid_buf);
                    set_console_color(WHITE, 0);
                    printf("\n提 交 人 员：%s \r\n", git_commits.commits[i].name);
                    printf("\n提 交 信 息：%s \r\n", git_commits.commits[i].commit);
                    printf("\n日       期：%s \r\n", git_commits.commits[i].date);
                }
            }
            else
            {
                log_error("接受服务端 发送的bug没有任何有价值的信息");
            }
        }
        else
        {
            log_error("客户端接受失败");
        }
    }

    return true;
}

bool git_help()
{
    bool ret = log_log("获取renzhai git 帮助提示...");
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
        return false;
    }

    log_log("git --global user.name 账户的意思");
    log_log("git --global user.email email的意思");
    log_log("");
    log_log("start a working area (see also: git help tutorial)");
    log_log("git clone 可以输入一个URL 直接复制它到当前的磁盘");
    log_log("git init 初始化一个仓库");
    log_log("");
    log_log("work on the current change (see also: git help everyday)");
    log_log("git add 添加一个文件内容 可以上传到指定URL仓库");
    log_log("");
    log_log("examine the historyand state(see also : git help revisions)");
    log_log("git log 查看提交的日志信息");
    log_log("");
    log_log("collaborate (see also: git help workflows)");
    log_log("git pull 拉取URL对应的 数据，更新数据 只更新本地版本与URL仓库对应不同的内容");
    log_log("git push 把本地修改过的文件推送到URL对应的仓库中");
    log_log("有任何疑问可以 访问 主页 http://renzhai.net");

    return true;
}

void git_set_email_func(char* user_input)
{
    // git --global user.email xxxxxxx@163.com
    split_string_with_index(user_input, " ", user.email, 3);
    bool ret = log_success("设置email 成功 %s", user.email);
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
    }
    save_user_ini();
}

void git_set_password_func(char* user_input)
{
    split_string_with_index(user_input, " ", user.password, 3);
    bool ret = log_success("设置password 成功 %s", user.password);
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
    }
    save_user_ini();
}

void git_set_name_func(char* user_input)
{
    split_string_with_index(user_input, " ", user.account, 3);
    bool ret = log_success("设置name 成功 %s", user.account);
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
    }
    save_user_ini();
}

void git_clone_func()
{
    bool ret = log_success("设置 git remote add origin 成功 %s", server_url);
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则 log 日志系统无法使用\n");
        return;
    }

    save_user_ini(); // 这里为啥要 save？似乎不用吧？【*】

    //获取最新版本号
    if (send_protocol_type(server_url, VERSION_PROTOCOL) &&
        send_protocol_content(server_url, get_project_cache()))
    {
        log_success("获取服务器 最新版本号");

        EGitProtocolType type = NONE;
        char buf_log[8196 * 1024] = {0};
        receive_content(&type, buf_log); //阻塞工作线程

        if (buf_log[0] == '\0')
        {
            log_error("获取最新版本出错");

            return;
        }

        // 这里的 type 是怎么从 NONE 变成 VERSION_PROTOCOL 的？【*】
        if (type == VERSION_PROTOCOL)
        {
            // 版本号处理
            FGitCommit Server_latest_version;
            init_commit(&Server_latest_version);
            string_to_git_commit(&Server_latest_version, buf_log);
            rewrite_file(get_client_version_info_file(), buf_log);

            // 将 EGitProtocolType 和 git.txt 信息存入远端
            if (send_protocol_type(server_url, GIT_CLONE) &&
                send_protocol_content(server_url, get_project_cache()))
            {
                log_success("获取服务器 最新版本号");

                EGitProtocolType type = NONE;
                char buf_log[8196 * 1024] = {0};
                receive_content(&type, buf_log); //阻塞工作线程

                if (type == GIT_CLONE)
                {
                    // 将 EGitProtocolType 和 git.txt 信息存入远端
                    if (send_protocol_type(server_url, VERSION_LIST) &&
                        send_protocol_content(server_url, get_project_cache()))
                    {
                        log_success("获取服务器 最新版本号");

                        EGitProtocolType type = NONE;
                        char buf_log[8196 * 1024] = {0};
                        receive_content(&type, buf_log); //阻塞工作线程

                        if (type == VERSION_LIST)
                        {
                            // 将本地 version.list 文件内容，替换成远端服务器版本
                            rewrite_file(get_client_version_list_file(), buf_log);
                            log_log("%s", buf_log);
                        }
                    }
                }
            }
        }
    }
}

bool git_remote_add_origin_func(char* new_remote_url)
{
    split_string_with_index(new_remote_url, " ", server_url, 4);
    bool ret = log_success("设置git remote add origin 成功 %s", server_url);
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");

        memset(server_url, 0, sizeof(server_url));
        return false;
    }

    return true;
}

bool git_pull_func()
{
    bool ret = log_log("准备开始拉去信息...");
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
        return false;
    }

    if (server_url[0] == '\0')
    {
        log_error("出错,请通过 git remote add origin 指定有效的服务器地址...");
    }

    //迭代我们的版本信息
    version_iteration_update();
    return true;
}

void git_pull_client_data(const FGitVersions* server_version_list, const char* value, const char* heard_value)
{
    //初始化表
    init_path(&server_list);

    //pull
    FFilePath paths;
    init_file_path(&paths);
    find_files(value, &paths, true);

    //比较本地有没有添加的文件
    //////////////////////////////////////////////////////////////////////////
    FFilePath out_paths;
    init_file_path(&out_paths);
    compare_list(server_version_list, &paths, &out_paths);

    //比较本地有没有删除的文件
    //////////////////////////////////////////////////////////////////////////
    //发送版本移除检测路径
    server_protocol_type_file_exit(server_url);
    server_protocol_content_file_exit(server_url);

    //向服务器获取本地指定列表
    const char* project_path = get_project_cache();
    char buf_content[1024];
    _getcwd(buf_content, MAX_PATH);
    strcat(buf_content, "\\");

    if (heard_value)
    {
        strcat(buf_content, heard_value);
    }

    remove_string_start(buf_content, project_path);
    if (buf_content[0] == '\0')
    {
        strcat(buf_content, "root");
    }
    else
    {
        remove_char_end(buf_content, '\\');
    }

    strcat(buf_content, "\n");
    strcat(buf_content, get_project_cache());

    if (send_protocol_type(server_url, CHECK_REMOVE_FILENAME) &&
        send_protocol_content(server_url, buf_content))
    {
        EGitProtocolType type = NONE;
        char buf_log[8196 * 10] = {0};
        receive_content(&type, buf_log); //阻塞工作线程
        if (type == CHECK_REMOVE_FILENAME)
        {
            if (buf_log[0] != '\0')
            {
                //解析出服务器列表后和本地数据对比 检测是不是有不存在的，如果有不存在的先记录
                FPath server_list_tmp;
                init_path(&server_list_tmp);
                string_to_path(buf_log, &server_list_tmp);

                memset(buf_content, 0, sizeof(buf_content));
                _getcwd(buf_content, MAX_PATH);

                if (heard_value)
                {
                    strcat(buf_content, "\\");
                    strcat(buf_content, heard_value);
                }

                FFilePath local_list_paths;
                init_file_path(&local_list_paths);

                FFilePath local_list_paths_org;
                init_file_path(&local_list_paths_org);
                find_files(buf_content, &local_list_paths_org, true);

                //去掉忽略的路径
                for (int i = 0; i < local_list_paths_org.index; i++)
                {
                    if (!is_ignore(local_list_paths_org.paths[i]))
                    {
                        strcpy(local_list_paths.paths[local_list_paths.index++], local_list_paths_org.paths[i]);
                    }
                }

                //对实体数据进行排查
                for (int i = 0; i < server_list_tmp.size; i++)
                {
                    bool b_exit_filename = false;
                    for (int j = 0; j < local_list_paths.index; j++)
                    {
                        remove_string_start(local_list_paths.paths[j], project_path);
                        if (strcmp(local_list_paths.paths[j], server_list_tmp.data[i]) == 0)
                        {
                            b_exit_filename = true;
                            break;
                        }
                    }

                    if (!b_exit_filename)
                    {
                        strcpy(server_list.data[server_list.size++], server_list_tmp.data[i]);
                    }
                }
            }
        }
    }
    //检测是否有变化
    if (out_paths.index == 0 && server_list.size == 0)
    {
        log_error("本地和服务器版本一致 本地没有任何变化，不需要上传");
        return;
    }

    for (int i = 0; i < out_paths.index; i++)
    {
        if (!is_ignore(out_paths.paths[i]))
        {
            char buf_tmp[MAX_PATH];
            strcpy(buf_tmp, out_paths.paths[i]);
            remove_string_start(buf_tmp, value);
            //Debug\2.exe

            char buf_remote_path_tmp[MAX_PATH] = {0};
            strcpy(buf_remote_path_tmp, server_url);

            char new_buf_content[MAX_PATH] = {0};
            _getcwd(new_buf_content, MAX_PATH);
            strcat(new_buf_content, "\\");
            const char* p = get_project_cache();
            remove_string_start(new_buf_content, p);

            strcat(buf_remote_path_tmp, "\\");
            strcat(buf_remote_path_tmp, new_buf_content);

            if (heard_value != NULL)
            {
                strcat(buf_remote_path_tmp, heard_value);
            }

            strcat(buf_remote_path_tmp, buf_tmp);

            //F:\c\c\2\x64\Debug\2.exe
            //发生在服务器
            {
                char buf_remote_path_tmp1[MAX_PATH] = {0};
                strcpy(buf_remote_path_tmp1, buf_remote_path_tmp);
                get_git_file_folder_inline(buf_remote_path_tmp1);

                if (!create_folder_path(buf_remote_path_tmp1))
                {
                    log_error("git服务器 产生了一个冲突 %s", buf_remote_path_tmp1);
                }
            }

            strcpy(git_path_2ds.paths[git_path_2ds.size].path_src, out_paths.paths[i]);
            strcpy(git_path_2ds.paths[git_path_2ds.size++].path_dis, buf_remote_path_tmp);
        }

        // cmd 窗口打印进度百分比
        int b_value = (int)(((double)i / (double)out_paths.index) * 100 + 1);
        printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        printf("当前正在准备的资源 %3d%%", b_value);
        // \b的含义是，将光标从当前位置向前（左）移动一个字符（遇到\n或\r则停止移动），并从此位置开始输出后面的字符（空字符\0和换行符\n除外）。
    }

    // cmd 窗口打印进度百分比
    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    printf("当前正在准备的资源 %3d%%", 100);
}

bool git_add_func(char* user_input)
{
    bool ret = log_log("准备开始添加数据...");
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
        return false;
    }

    if (server_url[0] == '\0')
    {
        log_error("出错,请通过 git remote add origin 指定有效的服务器地址...");
    }

    //验证版本是否为最新的
    server_protocol_type_file_exit(server_url);
    server_protocol_content_file_exit(server_url);
    if (send_protocol_type(server_url, VERSION_PROTOCOL) &&
        send_protocol_content(server_url, get_project_cache()))
    {
        EGitProtocolType type = NONE;
        char buf_log[8196 * 1024] = {0};
        receive_content(&type, buf_log); //阻塞工作线程

        if (buf_log[0] == '\0')
        {
            log_error("buf_log = 不为空");

            return -1;
        }
        if (type == VERSION_PROTOCOL)
        {
            FGitCommit Server_latest_version;
            init_commit(&Server_latest_version);
            string_to_git_commit(&Server_latest_version, buf_log);

            FGitCommit client_latest_version;
            init_commit(&client_latest_version);
            if (!get_git_commit(&client_latest_version))
            {
                log_error("请先 使用git clone 克隆一个项目从服务端");
                return -1;
            }

            //比较版本是否一样
            if (guid_equal(&client_latest_version.guid, &Server_latest_version.guid))
            {
                server_protocol_type_file_exit(server_url);
                server_protocol_content_file_exit(server_url);

                char buf_path[1024] = {0};
                if (strstr(user_input, "git add ."))
                {
                    const char* p_path = get_project_cache();
                    _getcwd(buf_path, MAX_PATH);
                    strcat(buf_path, "\\");

                    remove_string_start(buf_path, p_path);

                    if (buf_path[0] == '\0')
                    {
                        strcat(buf_path, "root");
                    }
                    else
                    {
                        remove_char_end(buf_path, '\\');
                    }
                }
                else
                {
                    FArray c_array;
                    split_string(user_input, " ", &c_array);
                    strcpy(buf_path, get_array_element(&c_array, 2));
                    if (check_command(buf_path))
                    {
                        log_error("git add 缺少发送内容,比如 git add .传递路径下所有内容 或者 git add 具体文件夹的名称或者路径");
                        return -1;
                    }
                    remove_char_end(buf_path, '\n');
                    destroy_array(&c_array);
                }

                strcat(buf_path, "\n");
                strcat(buf_path, get_project_cache());

                if (send_protocol_type(server_url, FIND_VERSION_LIST) &&
                    send_protocol_content(server_url, buf_path))
                {
                    EGitProtocolType type = NONE;
                    char buf_log[8196 * 1024] = {0};
                    receive_content(&type, buf_log); //阻塞工作线程

                    if (type == FIND_VERSION_LIST)
                    {
                        FGitVersions server_version_list;
                        init_versions(&server_version_list);
                        //如果存在就把版本进行翻译
                        if (buf_log[0] != '\0')
                        {
                            string_to_versions(&server_version_list, buf_log);
                        }

                        if (server_url[0] != '\0')
                        {
                            FArray c_array;
                            split_string(user_input, " ", &c_array);
                            char* heard_value = get_array_element(&c_array, 2);
                            if (check_command(heard_value))
                            {
                                log_error("git add 缺少发送内容,比如 git add .传递路径下所有内容 或者 git add 具体文件夹的名称或者路径");
                                return -1;
                            }

                            remove_char_end(heard_value, '\n');

                            init_git_path_2ds(&git_path_2ds);
                            if (strstr(user_input, "git add ."))
                            {
                                char value[MAX_PATH];
                                _getcwd(value, MAX_PATH);

                                git_pull_client_data(&server_version_list, value, NULL);
                            }
                            else
                            {
                                char value[MAX_PATH];
                                _getcwd(value, MAX_PATH);

                                strcat(value, "\\");
                                strcat(value, heard_value);

                                git_pull_client_data(&server_version_list, value, heard_value);
                            }

                            destroy_array(&c_array);
                        }
                        else
                        {
                            log_error("我们需要通过 git remote add origin来设置一个远端路径");
                        }
                    }
                    else
                    {
                        log_error("协议出错 没有获取到列表协议");
                    }
                }
            }
            else
            {
                log_error("本地的数据不是最新的数据 需要通过 git pull 拉去最新数据~~");
            }
        }
    }
    return true;
}

bool git_commit_func(char* user_input)
{
    bool ret = log_log("准备开始提交数据信息...");
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
        return false;
    }

    if (server_url[0] == '\0')
    {
        log_error("出错,请通过 git remote add origin 指定有效的服务器地址...");
    }

    if (git_path_2ds.size != 0 || server_list.size != 0) // 这是啥？【*】
    {
        FArray c_array;
        split_string(user_input, "\"", &c_array);
        char* commit_content = get_array_element(&c_array, 1);

        init_commit(&commit);
        char* current_time = get_current_time();
        remove_char_end(current_time, '\n');

        strcpy(commit.date, current_time);
        strcpy(commit.name, user.account);
        strcpy(commit.commit, commit_content);

        create_guid(&commit.guid); // 每次提交都是唯一的，生成一个新版本 GUID

        log_log("版本提交信息为 ： %s", commit_content);
    }
    else
    {
        log_error("没有检测出本地有任何文件变化，有可能没有通过git add 进行添加");
    }

    return true;
}

bool git_push_func()
{
    bool ret = log_log("准备开始合并 将数据提交到远方 仓库...");
    if (!ret)
    {
        printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
        return false;
    }

    if (server_url[0] == '\0')
    {
        log_error("出错,请通过 git remote add origin 指定有效的服务器地址...");
    }

    if (git_path_2ds.size != 0 || server_list.size != 0)
    {
        if (commit.name != "")
        {
            const char* client_addr = get_project_cache();
            if (connect_is_alive(server_url, client_addr))
            {
                log_success("链接服务器成功", client_addr);

                //清除当前版本 准备迎接新提交的个版本
                init_versions(&versions);

                //将本地数据发送到服务器
                for (int i = 0; i < git_path_2ds.size; i++)
                {
                    EVersionOperationType operation_type = NONE_File;

                    char tmp_buf[1024] = {0};
                    strcat(tmp_buf, get_project_cache());
                    strcat(tmp_buf, "\n");
                    strcat(tmp_buf, git_path_2ds.paths[i].path_dis);

                    //检测服务器上的文件是否存在
                    //这里可以优化 比如在前面git add获取表 然后对照是否存在 存在就标记存在等 节省网络带宽
                    if (send_protocol_type(server_url, FILE_EXIT) &&
                        send_protocol_content(server_url, tmp_buf))
                    {
                        EGitProtocolType type = NONE;
                        char buf_log[10] = {0};
                        receive_content(&type, buf_log); //阻塞工作线程

                        if (type == FILE_EXIT)
                        {
                            if (strcmp(buf_log, "3") == 0)
                            {
                                operation_type = OVERLAP_FILE;
                            }
                            else if (strcmp(buf_log, "1") == 0)
                            {
                                operation_type = ADD_FILE;
                            }
                        }
                    }

                    if (send_file(git_path_2ds.paths[i].path_dis, git_path_2ds.paths[i].path_src))
                    {
                        add_git_versions(git_path_2ds.paths[i].path_src, operation_type, &versions);
                        log_log("将路径下的 %s 拉去 到 %s 成功", git_path_2ds.paths[i].path_src,
                                git_path_2ds.paths[i].path_dis);
                    }
                    else
                    {
                        log_error("将路径下的 %s 拉去 到 %s 失败", git_path_2ds.paths[i].path_src,
                                  git_path_2ds.paths[i].path_dis);
                    }
                }

                //更新服务器版本
                //版本发送
                char buf_content[8196 * 1024] = {0};
                git_commit_to_string(buf_content, &commit);
                if (send_protocol_type(server_url, COMMIT)
                    && send_protocol_content(server_url, buf_content))
                {
                    //阻塞
                    server_protocol_type_file_exit(server_url);
                    server_protocol_content_file_exit(server_url);

                    //存储到本地
                    rewrite_file(get_client_version_info_file(), buf_content);

                    log_success("版本信息发送成功");
                }

                //发送本地已被删除的数据信息 通知服务器删除该数据
                if (server_list.size != 0)
                {
                    memset(buf_content, 0, sizeof(buf_content));
                    path_to_string(buf_content, &server_list);

                    if (send_protocol_type(server_url, REMOVE_CONTENT) &&
                        send_protocol_content(server_url, buf_content))
                    {
                        //阻塞
                        server_protocol_type_file_exit(server_url);
                        server_protocol_content_file_exit(server_url);

                        for (int i = 0; i < server_list.size; i++)
                        {
                            add_git_versions(server_list.data[i], SUBTRACT_FILE, &versions);
                            log_success("remove server :%s", server_list.data[i]);
                        }
                    }
                }

                //列表发送
                memset(buf_content, 0, sizeof(buf_content));
                versions_to_string(buf_content, &versions);
                if (send_protocol_type(server_url, COMMIT_VERSION_LIST) &&
                    send_protocol_content(server_url, buf_content))
                {
                    //阻塞
                    server_protocol_type_file_exit(server_url);
                    server_protocol_content_file_exit(server_url);

                    //存储本地
                    rewrite_file(get_client_version_list_file(), buf_content);

                    log_success("版本数据发送成功 版本列表为下面内容");

                    for (unsigned int i = 0; i < versions.size; i++)
                    {
                        if (versions.paths[i].operation_type == ADD_FILE)
                        {
                            log_log("create server data : %s +++|%i byte|+++|", versions.paths[i].file_name,
                                    versions.paths[i].file_size);
                        }
                        else if (versions.paths[i].operation_type == SUBTRACT_FILE)
                        {
                            log_success("remove server data : %s ----|%i byte|----|",
                                        versions.paths[i].file_name, versions.paths[i].file_size);
                        }
                        else if (versions.paths[i].operation_type == OVERLAP_FILE)
                        {
                            log_success("overlap server data : %s __+_+_+_|%i byte|_+_++|",
                                        versions.paths[i].file_name, versions.paths[i].file_size);
                        }
                    }
                }

                memset(buf_content, 0, sizeof(buf_content));
                guid_to_string(buf_content, &commit.guid);
                if (send_protocol_type(server_url, SAVE_VERSION_REQUEST) &&
                    send_protocol_content(server_url, buf_content))
                {
                    log_success("当前的版本号[%s] 通知服务器进行存储~", buf_content);
                }
            }
            else
            {
                log_error("服务器链接失败~~");
            }
        }
        else
        {
            log_error("提交信息不能为空 请通过 git commit -m 填写相关提交内容"); // 保证有 commit 信息后再 push
        }

        //清除数据
        init_versions(&versions);
        init_git_path_2ds(&git_path_2ds);
        init_commit(&commit);
    }
    else
    {
        log_error("没有检测出本地有任何文件变化，有可能没有通过git add 进行添加");
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////// command ↑