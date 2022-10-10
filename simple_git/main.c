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
    //ѭ��
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
        case HEI: //����˽��� ����ȷ��
            {
                recv_protocol = protocol;

                ResumeThread(hwork);
                break;
            }
        case REMOVE_CLIENT_REQUEST: //������֪ͨ�ͻ����Ƴ��ļ�
            {
                init_network_data_buf();
                read_client_protocol_content(network_data_buf);

                char path_tmp[MAX_PATH] = {0};
                strcpy(path_tmp, get_project_cache());
                strcat(path_tmp, network_data_buf);

                if (!is_ignore(path_tmp)) {
                    if (remove(path_tmp) == 0) {
                        log_success("�ӿͻ��� �ɹ��Ƴ� %s", path_tmp);
                    }
                    else {
                        log_error("�Ƴ�ʧ�� ����·�� ���� �ļ��Ƿ���� %s", path_tmp);
                    }
                }
                else {
                    log_log("���Ը�·�� %s", path_tmp);
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
                log_error("���� ����~");

                ResumeThread(hwork);
                break;
            }
        case CONNECT_TIME_OUT:
            {
                log_error("�ͻ��� ���ӳ�ʱ������������Ӧ,���������Ƿ��� ���ߵ�ַ�Ƿ���ڻ��߷������Ƿ�����~");

                ResumeThread(hwork);
            }
        }

        Sleep(500);
    }

    return 0;
}

void init_engine(int argc, char* argv[])
{
    //�������ӳ�ʱΪ 12
    float time_out = 12.f;
    init_operation(CONNECT_TIME_OUT, &time_out);

    const char* exe_path = argv[0];
    get_path_filename(project_name, exe_path);

    init_network_data_buf();

    // ��ȡ�û����ñ�
    read_user_ini();

    //�����߳�
    hwork = CreateThread(NULL, 1000000000, client_send, NULL, 0,NULL);
    hrev = CreateThread(NULL, 1000000000, client_receive, NULL, 0, NULL);

    //init_commit(&Last_commit;
    //��ʼ���ύ��Ϣ
    //char buf_info[8196] = { 0 };
    //read_file(get_git_current_version_filename(), buf_info);
    //string_to_git_commits(&Last_commit, buf_info);

    //��ʼ�����ǵ���Ŀ·��
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

    //��ȡ���Ե�·��
    char buf_ignore[1024] = {0};
    read_file(ignore_content_path, buf_ignore);
    string_to_ignore_path(buf_ignore);

    //��ȡ���Եĺ�׺
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

    //ѭ��
    while (!b_exit) {
        // ����ǰ����������
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

        // ����ָ��(ʵ���߳�����)
        fgets(user_input, 1024, stdin);

        // ִ��ָ��
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
            save_user_ini(); // ����ΪɶҪ save���ƺ����ðɣ���*��
        }
        else if (strstr(user_input, "git --global user.email")) {
            git_set_email_func(user_input);
        }
        else if (strstr(user_input, "git --global user.password")) {
            git_set_password_func(user_input);
        }
        else if (strstr(user_input, "git --global user.account")) {
            git_set_name_func(user_input); //openssl rsa //git �� �����ǣ�ssh-keygen -t rsa -C "asasdasd"
        }
        else if (strstr(user_input, "git add")) {
            if (!git_add_func(&user_input)) {
                continue;
            }
        }
        else if (strstr(user_input, "git reset")) {
            bool ret = log_error("���ڻ�ԭָ���汾...");
            if (!ret) {
                printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
            }
        }
        else if (strstr(user_input, "git clone")) {
            // ��ȡָ��
            split_string_with_index(user_input, " ", server_url, 2);
            // ִ�� clone ָ��
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
        // ��ҵ openSSL RSA ����
        else if (strstr(user_input, "ssh-keygen -t rsa -C")) {
            if (!log_error("ssh-keygen -t rsa -C  ��ָ�δ��� rsa������Ҫͨ�� OpenSSL Э��")) {
                printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
            }
        }
        // ��ҵ
        else if (strstr(user_input, "git diff")) {
            if (!log_error("���Լ�鱾���ļ� �� ��������ǰ�޸ĵ��ļ����ﲻͬ")) {
                printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
            }
        }
        // ��ҵ
        else if (strstr(user_input, "git checkout")) {
            bool ret = log_error("��ϲ��� ȡ���ļ�");
            if (!ret) {
                printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
            }
        }
        // ��ҵ
        else if (strstr(user_input, "git merge")) {
            bool ret = log_error("��Ҫ�ϲ��ĸ� ��");
            if (!ret) {
                printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
            }
        }
        // ��ҵ
        else if (strstr(user_input, "git tag")) {
            bool ret = log_error("ִ������Ӧ������ ����û�� git tag ����");
            if (!ret) {
                printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
            }
        }
        // ��ҵ
        else if (strstr(user_input, "git rebase")) {
            bool ret = log_error("ִ����git rebase ���Ǻ���û��Ч�� ���~");
            if (!ret) {
                printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
            }
        }
        else {
            bool ret = log_error("û���ҵ���ָ��...");
            if (!ret) {
                printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
                continue;
            }
            log_warning("��%s��", user_input);
            log_warning("��������ͨ�� git --help ���ַ�ʽ�鿴��Ӧ����~");
        }
    }
}

void exit_engine()
{
}

//git_guid guid;
//create_guid(&guid);
//
//printf("ԭ�� a=%d,b=%d,c=%d,d=%d \r\n", guid.a, guid.b, guid.c, guid.d);
//char buf[MAX_PATH] = { 0 };
//guid_to_string(buf, &guid);
//printf("�ַ��� %s \r\n", buf);
//normalization_guid(&guid);
//string_to_guid(&guid, buf);
//printf("��ԭ�� a=%d,b=%d,c=%d,d=%d \r\n", guid.a, guid.b, guid.c, guid.d);
//return;

int main(int argc, char* argv[]) //һ������ ����һ���߳�
{
    // �����ʼ��
    init_engine(argc, argv);

    // ѭ��
    while (!b_exit) {
        Sleep(1000);
    }

    //�˳�
    exit_engine();
    printf("���� �˳�;\r\n");


    system("pause");

    return 0;
}
