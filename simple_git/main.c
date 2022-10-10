#include "main.h"
#include "public/log/log.h"
#include "public/window/window_setting.h"
#include "public/util/string_util.h"
#include "public/git/command.h"
#include "public/git/service.h"
#include "public/git/path.h"
#include "public/git/ignore.h"
#include "public/git/global.h"

DWORD WINAPI client_send(LPVOID lpParam)
{
    //循环
    engine_loop();
    return 0;
}

DWORD WINAPI client_receive(LPVOID lpParam)
{
    bool b_quit = false;
    while (!b_quit) {
        unsigned char protocol = read_client_protocol_type();

        if (check_connect) {
            if ((connect_time -= 0.5f) <= 0) {
                protocol = CONNECT_TIME_OUT;
            }
        }

        switch ((EGitProtocolType)protocol) {
        case NONE:
            break;
        case HEI: //服务端进行 返回确认
            {
                recv_protocol = protocol;

                ResumeThread(hwork);
                break;
            }
        case REMOVE_CLIENT_REQUEST: //服务器通知客户端移除文件
            {
                init_network_data_buf();
                read_client_protocol_content(network_data_buf);

                char path_tmp[MAX_PATH] = {0};
                strcpy(path_tmp, get_project_cache());
                strcat(path_tmp, network_data_buf);

                if (!is_ignore(path_tmp)) {
                    if (remove(path_tmp) == 0) {
                        log_success("从客户端 成功移除 %s", path_tmp);
                    }
                    else {
                        log_error("移除失败 请检查路径 或者 文件是否存在 %s", path_tmp);
                    }
                }
                else {
                    log_log("忽略该路径 %s", path_tmp);
                }

                break;
            }
        case VERSION_LOG:
        case VERSION_STATUS:
        case VERSION_PROTOCOL:
        case VERSION_NEST:
        case VERSION_NEST_COMPLETE:
        case CLASH_CONTENT:
        case NO_CLASH:
        case VERSION_LIST:
        case CHECK_REMOVE_FILENAME:
        case GIT_CLONE:
        case FILE_EXIT:
        case FIND_VERSION_LIST:
        case GIT_RESET_VERSION:
            {
                Sleep(100);

                init_network_data_buf();
                read_client_protocol_content(network_data_buf);

                recv_protocol = (EGitProtocolType)protocol;
                ResumeThread(hwork);
                break;
            }
        case PROTOCOL_ERROR:
            {
                log_error("服务 错误~");

                ResumeThread(hwork);
                break;
            }
        case CONNECT_TIME_OUT:
            {
                log_error("客户端 连接超时，服务器无响应,检查服务器是否奔溃 或者地址是否存在或者服务器是否启动~");

                ResumeThread(hwork);
            }
        }

        Sleep(500);
    }

    return 0;
}

void init_engine(int argc, char* argv[])
{
    //设置连接超时为 12
    float time_out = 12.f;
    init_operation(CONNECT_TIME_OUT, &time_out);

    const char* exe_path = argv[0];
    get_path_filename(project_name, exe_path);

    init_network_data_buf();

    // 读取用户配置表
    read_user_ini();

    //创建线程
    hwork = CreateThread(NULL, 1000000000, client_send, NULL, 0,NULL);
    hrev = CreateThread(NULL, 1000000000, client_receive, NULL, 0, NULL);

    //init_commit(&Last_commit;
    //初始化提交信息
    //char buf_info[8196] = { 0 };
    //read_file(get_git_current_version_filename(), buf_info);
    //string_to_git_commits(&Last_commit, buf_info);

    //初始化我们的项目路径
    const char* project_path = get_project_cache();
    if (project_path[0] != '\0') {
        char p_tmp[MAX_PATH] = {0};
        strcat(p_tmp, project_path);
        strcat(p_tmp, "log");
        set_log_folder(p_tmp);
    }

    char ignore_content_path[MAX_PATH] = {0};
    strcpy(ignore_content_path, get_client_git_cache());
    strcat(ignore_content_path, "ignore_path.ig");

    char ignore_content_suffix[MAX_PATH] = {0};
    strcpy(ignore_content_suffix, get_client_git_cache());
    strcat(ignore_content_suffix, "ignore_suffix.ig");

    //读取忽略的路径
    char buf_ignore[1024] = {0};
    read_file(ignore_content_path, buf_ignore);
    string_to_ignore_path(buf_ignore);

    //读取忽略的后缀
    memset(buf_ignore, 0, sizeof(buf_ignore));
    read_file(ignore_content_suffix, buf_ignore);
    string_to_ignore_suffix(buf_ignore);
}

//CreateThread
//ExitThread
//ResumeThread
//SuspendThread
//GetThreadPriority
//SetThreadPriority
//CreateRemoteThread
//PostThreadMessage
//GetCurrentThread
//GetCurrentThreadId
//GetThreadId(h)
//WaitForSingleObject
//WaitForMultipleObjects

void engine_loop()
{
    char user_input[1024] = {0};

    //循环
    while (!b_exit) {
        // 输入前的引导文字
        printf("\r\n");
        set_console_color(PALE_GREEN, 0);
        printf("%s ssh :", user.account);
        set_console_color(LAVENDER, 0);
        printf(" Windows64 ");
        set_console_color(CANARY_YELLOW, 0);
        char buf_color[MAX_PATH];
        _getcwd(buf_color, MAX_PATH);
        printf(" %s \r\n", buf_color);
        set_console_color(WHITE, 0);
        printf(" \r\n ");
        printf(" $ ");

        // 输入指令(实现线程阻塞)
        fgets(user_input, 1024, stdin);

        // 执行指令
        if (strstr(user_input, "exit")) {
            b_exit = 1;
        }
        else if (strstr(user_input, "git status")) {
            if (!git_status_func()) {
                continue;
            }
        }
        else if (strstr(user_input, "git log")) {
            if (!git_log_func()) {
                continue;
            }
        }
        else if (strstr(user_input, "git init")) {
            git_init_func();
        }
        else if (strstr(user_input, "git remote add origin")) {
            if (!git_remote_add_origin_func(user_input)) {
                continue;
            }
            save_user_ini(); // 这里为啥要 save？似乎不用吧？【*】
        }
        else if (strstr(user_input, "git --global user.email")) {
            git_set_email_func(user_input);
        }
        else if (strstr(user_input, "git --global user.password")) {
            git_set_password_func(user_input);
        }
        else if (strstr(user_input, "git --global user.account")) {
            git_set_name_func(user_input); //openssl rsa //git 的 命令是：ssh-keygen -t rsa -C "asasdasd"
        }
        else if (strstr(user_input, "git add")) {
            if (!git_add_func(&user_input)) {
                continue;
            }
        }
        else if (strstr(user_input, "git reset")) {
            bool ret = log_error("正在还原指定版本...");
            if (!ret) {
                printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
            }
        }
        else if (strstr(user_input, "git clone")) {
            // 获取指令
            split_string_with_index(user_input, " ", server_url, 2);
            // 执行 clone 指令
            git_clone_func();
        }
        else if (strstr(user_input, "git commit -m ")) {
            if (!git_commit_func(user_input)) {
                continue;
            }
        }
        else if (strstr(user_input, "git push -u origin master")
            || strstr(user_input, "git push")) {
            if (!git_push_func()) {
                continue;
            }
        }
        else if (strstr(user_input, "git pull")) //V
        {
            if (!git_pull_func()) {
                continue;
            }
        }
        else if (strstr(user_input, "git --help")) {
            if (!git_help()) {
                continue;
            }
        }
        // 作业 openSSL RSA 加密
        else if (strstr(user_input, "ssh-keygen -t rsa -C")) {
            if (!log_error("ssh-keygen -t rsa -C  该指令还未完成 rsa加密需要通过 OpenSSL 协助")) {
                printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
            }
        }
        // 作业
        else if (strstr(user_input, "git diff")) {
            if (!log_error("可以检查本地文件 和 服务器当前修改的文件哪里不同")) {
                printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
            }
        }
        // 作业
        else if (strstr(user_input, "git checkout")) {
            bool ret = log_error("诊断并且 取出文件");
            if (!ret) {
                printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
            }
        }
        // 作业
        else if (strstr(user_input, "git merge")) {
            bool ret = log_error("想要合并哪个 ？");
            if (!ret) {
                printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
            }
        }
        // 作业
        else if (strstr(user_input, "git tag")) {
            bool ret = log_error("执行了相应的内容 但是没有 git tag 代码");
            if (!ret) {
                printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
            }
        }
        // 作业
        else if (strstr(user_input, "git rebase")) {
            bool ret = log_error("执行了git rebase 但是好像没有效果 奇怪~");
            if (!ret) {
                printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
            }
        }
        else {
            bool ret = log_error("没有找到该指令...");
            if (!ret) {
                printf("\n请先进行 git init 初始化一个仓库 否则log日志系统无法使用\n");
                continue;
            }
            log_warning("【%s】", user_input);
            log_warning("你或许可以通过 git --help 这种方式查看相应命令~");
        }
    }
}

void exit_engine()
{
}

//git_guid guid;
//create_guid(&guid);
//
//printf("原来 a=%d,b=%d,c=%d,d=%d \r\n", guid.a, guid.b, guid.c, guid.d);
//char buf[MAX_PATH] = { 0 };
//guid_to_string(buf, &guid);
//printf("字符串 %s \r\n", buf);
//normalization_guid(&guid);
//string_to_guid(&guid, buf);
//printf("还原的 a=%d,b=%d,c=%d,d=%d \r\n", guid.a, guid.b, guid.c, guid.d);
//return;

int main(int argc, char* argv[]) //一个进程 至少一个线程
{
    // 引擎初始化
    init_engine(argc, argv);

    // 循环
    while (!b_exit) {
        Sleep(1000);
    }

    //退出
    exit_engine();
    printf("引擎 退出;\r\n");


    system("pause");

    return 0;
}
