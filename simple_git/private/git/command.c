#include "../../public/git/command.h"
#include "../../public/git/ignore.h"
#include "../../public/git/path.h"
#include "../../public/git/global.h"
#include "../../public/git/service.h"
#include "../../public/log/log.h"
#include "../../public/window/window_setting.h"
#include "../../public/util/string_util.h"

//////////////////////////////////////////////////////////////////////////////////////// util ��
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

//�Ƚϲ�һ���� 
//������û�е� �ͻ����е� Ҫ��ӵ�������
void compare_list(const FGitVersions* server_version_list, const FFilePath* client_paths, FFilePath* out_paths)
{
    const char* client_url = get_project_cache();

    //��� (��һ����)
    for (int i = 0; i < client_paths->index; i++)
    {
        char path_tmp[MAX_PATH] = {0}; //���·��
        strcpy(path_tmp, client_paths->paths[i]);
        remove_string_start(path_tmp, client_url);

        int size = get_file_size(client_paths->paths[i]);

        bool b_exit = false;
        for (unsigned int j = 0; j < server_version_list->size; j++)
        {
            if (strcmp(server_version_list->paths[j].file_name, path_tmp) == 0) //�Ƚ����·��
            {
                if (size != server_version_list->paths[j].file_size)
                {
                    strcpy(out_paths->paths[out_paths->index++], client_paths->paths[i]); //����·��
                }

                b_exit = true;
                break;
            }
        }

        if (!b_exit)
        {
            strcpy(out_paths->paths[out_paths->index++], client_paths->paths[i]); //����·��
        }
    }
}


char* get_current_time()
{
    return ctime(__TIME__);
}




void version_iteration_update()
{
    //����
    server_protocol_type_file_exit(server_url);
    server_protocol_content_file_exit(server_url);

    // 1.0 ��⵱ǰ�汾�Ƿ������� ��ȡ���������°汾�ţ�Ȼ��Ƚ� �����ǲ���һ����
    if (send_protocol_type(server_url, VERSION_PROTOCOL) &&
        send_protocol_content(server_url, get_project_cache()))
    {
        log_success("��ȡ������ ���°汾��");

        EGitProtocolType type = NONE;
        char buf_log[8196 * 1024] = {0};
        receive_content(&type, buf_log); //���������߳�

        if (buf_log[0] == '\0')
        {
            log_error("buf_log = ��Ϊ��");

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

            //�Ƚϰ汾�Ƿ�һ��
            if (!guid_equal(&client_latest_version.guid, &Server_latest_version.guid))
            {
                const char* client_url = get_project_cache(); //F:\\sadas\\asd\\ads\\

                //��֤��ͻ
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

                //�Ƴ� �����Ե�
                for (int i = 0; i < local_paths.index; i++)
                {
                    if (!is_ignore(local_paths.paths[i]))
                    {
                        strcpy(paths.paths[paths.index++], local_paths.paths[i]);
                    }
                }

                //��ȡ�����б�����
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

                    //ģ�°汾�ṹ ����ֻ��Ҫ ·�� �� ��С
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
                    receive_content(&type, buf_log); //���������߳�
                    if (type == CLASH_CONTENT)
                    {
                        log_error("%s", buf_log);
                        log_error("�������ϳ�ͻ �ٽ��и�������...");
                    }
                    else if (type == NO_CLASH)
                    {
                        memset(str_content, 0, sizeof(str_content));
                        guid_to_string(str_content, &client_latest_version.guid);

                        strcat(str_content, "\n");
                        strcat(str_content, get_project_cache());

                        // 1.1 �����ǵ�ǰ�Ŀͻ��˰汾���͵������ 
                        server_protocol_type_file_exit(server_url);
                        server_protocol_content_file_exit(server_url);
                        if (send_protocol_type(server_url, VERSION_NEST) &&
                            send_protocol_content(server_url, str_content))
                        {
                            log_success("��ȡ��һ����Ҫ���µİ汾");

                            // 1.2 �����°汾�� �洢�ڱ��أ��滻
                            type = NONE;
                            memset(buf_log, 0, sizeof(buf_log));
                            receive_content(&type, buf_log); //���������߳�

                            if (buf_log[0] == '\0')
                            {
                                log_error("buf_log = ��Ϊ��");

                                return;
                            }

                            //init_commit(&client_latest_version);
                            //string_to_git_commit(&client_latest_version, buf_log);

                            rewrite_file(get_client_version_info_file(), buf_log);

                            type = NONE;
                            memset(buf_log, 0, sizeof(buf_log));
                            receive_content(&type, buf_log); //���������߳�

                            if (buf_log[0] == '\0')
                            {
                                log_error("buf_log = ��Ϊ��");

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
                        log_error("server call ��%s", buf_log);
                    }
                    else
                    {
                        log_error("������ ������ `_`...");
                    }
                }
            }
            else
            {
                log_log("�Ѿ���������...");
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////// util ��

//////////////////////////////////////////////////////////////////////////////////////// command ��
void git_init_func()
{
    char tmp_path[256] = {0};

    // ���� .git �ļ���
    strcat(tmp_path, ".\\.git\\");
    _mkdir(tmp_path);

    // git.txt ���ڼ�¼�汾��Ϣ
    strcat(tmp_path, "git.txt");

    if (create_file(tmp_path)) // �����ļ�
    {
        char* p = get_git_client_cache();
        _mkdir(p);

        // ��Ҫ���Ե��ļ�
        char ignore_content_path[MAX_PATH] = {0};
        strcpy(ignore_content_path, get_client_git_cache());
        strcat(ignore_content_path, "ignore_path.ig");

        // ��Ҫ���Ե��ļ���׺
        char ignore_content_suffix[MAX_PATH] = {0};
        strcpy(ignore_content_suffix, get_client_git_cache());
        strcat(ignore_content_suffix, "ignore_suffix.ig");

        // ���ɺ����ļ�������
        char ignore_content[1024] = "log\\\n.git\\\n.vs\\\ngit_client\\\n";

        // ���������ļ��������� project_name ��
        strcat(ignore_content, project_name);

        // ƴ����Ҫ���Ե��ļ�
        rewrite_file(ignore_content_path, ignore_content);

        // ���ɺ����ļ�
        memset(ignore_content, 0, sizeof(ignore_content));
        strcpy(ignore_content, ".ilk\n.pdb");
        rewrite_file(ignore_content_suffix, ignore_content);

        // ��ȡ���Ե�·��
        char buf_ignore[1024] = {0};
        read_file(ignore_content_path, buf_ignore);
        string_to_ignore_path(buf_ignore);

        // ��ȡ���Եĺ�׺
        memset(buf_ignore, 0, sizeof(buf_ignore));
        read_file(ignore_content_suffix, buf_ignore);
        string_to_ignore_suffix(buf_ignore);

        // ���� log ·��
        const char* project_path = get_project_cache();
        if (project_path[0] != '\0')
        {
            char p_tmp[MAX_PATH] = {0};
            strcat(p_tmp, project_path);
            strcat(p_tmp, "log");
            set_log_folder(p_tmp);
        }

        bool ret = log_success("���س�ʼ���ɹ�");
        if (!ret)
        {
            printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
        }
    }
    else // �ļ�����ʧ��
    {
        bool ret = log_error("������Ŀ·�� .git ʧ��");
        if (!ret)
        {
            printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
        }
    }
}

bool git_status_func()
{
    if (server_url[0] == '\0')
    {
        bool ret = log_error("δָ�� ��������ַ");
        if (!ret)
        {
            printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
        }
        return false;
    }

    //����
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
        log_success("��ַ �� �汾guid ���ͳɹ�");

        EGitProtocolType type = NONE;
        char buf_log[8196 * 1024] = {0};
        receive_content(&type, buf_log); //���������߳�
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
                log_error("�������İ汾 ���ύ�İ汾û�ж��� ���� ���������ݰ汾δ��~");
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
        bool ret = log_error("δָ�� ��������ַ");
        if (!ret)
        {
            printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
        }
        return false;
    }

    //����
    server_protocol_type_file_exit(server_url);
    server_protocol_content_file_exit(server_url);

    const char* client_addr = get_project_cache();
    if (send_protocol_type(server_url, VERSION_LOG) &&
        send_protocol_content(server_url, client_addr))
    {
        log_success("�ͻ��˵�ַ ���ͳɹ�");

        EGitProtocolType type = NONE;
        char buf_log[8196] = {0};
        receive_content(&type, buf_log); //���������߳�
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
                    printf("\n\n��   ��  �룺%s \r\n", guid_buf);
                    set_console_color(WHITE, 0);
                    printf("\n�� �� �� Ա��%s \r\n", git_commits.commits[i].name);
                    printf("\n�� �� �� Ϣ��%s \r\n", git_commits.commits[i].commit);
                    printf("\n��       �ڣ�%s \r\n", git_commits.commits[i].date);
                }
            }
            else
            {
                log_error("���ܷ���� ���͵�bugû���κ��м�ֵ����Ϣ");
            }
        }
        else
        {
            log_error("�ͻ��˽���ʧ��");
        }
    }

    return true;
}

bool git_help()
{
    bool ret = log_log("��ȡrenzhai git ������ʾ...");
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
        return false;
    }

    log_log("git --global user.name �˻�����˼");
    log_log("git --global user.email email����˼");
    log_log("");
    log_log("start a working area (see also: git help tutorial)");
    log_log("git clone ��������һ��URL ֱ�Ӹ���������ǰ�Ĵ���");
    log_log("git init ��ʼ��һ���ֿ�");
    log_log("");
    log_log("work on the current change (see also: git help everyday)");
    log_log("git add ���һ���ļ����� �����ϴ���ָ��URL�ֿ�");
    log_log("");
    log_log("examine the historyand state(see also : git help revisions)");
    log_log("git log �鿴�ύ����־��Ϣ");
    log_log("");
    log_log("collaborate (see also: git help workflows)");
    log_log("git pull ��ȡURL��Ӧ�� ���ݣ��������� ֻ���±��ذ汾��URL�ֿ��Ӧ��ͬ������");
    log_log("git push �ѱ����޸Ĺ����ļ����͵�URL��Ӧ�Ĳֿ���");
    log_log("���κ����ʿ��� ���� ��ҳ http://renzhai.net");

    return true;
}

void git_set_email_func(char* user_input)
{
    // git --global user.email xxxxxxx@163.com
    split_string_with_index(user_input, " ", user.email, 3);
    bool ret = log_success("����email �ɹ� %s", user.email);
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
    }
    save_user_ini();
}

void git_set_password_func(char* user_input)
{
    split_string_with_index(user_input, " ", user.password, 3);
    bool ret = log_success("����password �ɹ� %s", user.password);
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
    }
    save_user_ini();
}

void git_set_name_func(char* user_input)
{
    split_string_with_index(user_input, " ", user.account, 3);
    bool ret = log_success("����name �ɹ� %s", user.account);
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
    }
    save_user_ini();
}

void git_clone_func()
{
    bool ret = log_success("���� git remote add origin �ɹ� %s", server_url);
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ���� log ��־ϵͳ�޷�ʹ��\n");
        return;
    }

    save_user_ini(); // ����ΪɶҪ save���ƺ����ðɣ���*��

    //��ȡ���°汾��
    if (send_protocol_type(server_url, VERSION_PROTOCOL) &&
        send_protocol_content(server_url, get_project_cache()))
    {
        log_success("��ȡ������ ���°汾��");

        EGitProtocolType type = NONE;
        char buf_log[8196 * 1024] = {0};
        receive_content(&type, buf_log); //���������߳�

        if (buf_log[0] == '\0')
        {
            log_error("��ȡ���°汾����");

            return;
        }

        // ����� type ����ô�� NONE ��� VERSION_PROTOCOL �ģ���*��
        if (type == VERSION_PROTOCOL)
        {
            // �汾�Ŵ���
            FGitCommit Server_latest_version;
            init_commit(&Server_latest_version);
            string_to_git_commit(&Server_latest_version, buf_log);
            rewrite_file(get_client_version_info_file(), buf_log);

            // �� EGitProtocolType �� git.txt ��Ϣ����Զ��
            if (send_protocol_type(server_url, GIT_CLONE) &&
                send_protocol_content(server_url, get_project_cache()))
            {
                log_success("��ȡ������ ���°汾��");

                EGitProtocolType type = NONE;
                char buf_log[8196 * 1024] = {0};
                receive_content(&type, buf_log); //���������߳�

                if (type == GIT_CLONE)
                {
                    // �� EGitProtocolType �� git.txt ��Ϣ����Զ��
                    if (send_protocol_type(server_url, VERSION_LIST) &&
                        send_protocol_content(server_url, get_project_cache()))
                    {
                        log_success("��ȡ������ ���°汾��");

                        EGitProtocolType type = NONE;
                        char buf_log[8196 * 1024] = {0};
                        receive_content(&type, buf_log); //���������߳�

                        if (type == VERSION_LIST)
                        {
                            // ������ version.list �ļ����ݣ��滻��Զ�˷������汾
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
    bool ret = log_success("����git remote add origin �ɹ� %s", server_url);
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");

        memset(server_url, 0, sizeof(server_url));
        return false;
    }

    return true;
}

bool git_pull_func()
{
    bool ret = log_log("׼����ʼ��ȥ��Ϣ...");
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
        return false;
    }

    if (server_url[0] == '\0')
    {
        log_error("����,��ͨ�� git remote add origin ָ����Ч�ķ�������ַ...");
    }

    //�������ǵİ汾��Ϣ
    version_iteration_update();
    return true;
}

void git_pull_client_data(const FGitVersions* server_version_list, const char* value, const char* heard_value)
{
    //��ʼ����
    init_path(&server_list);

    //pull
    FFilePath paths;
    init_file_path(&paths);
    find_files(value, &paths, true);

    //�Ƚϱ�����û����ӵ��ļ�
    //////////////////////////////////////////////////////////////////////////
    FFilePath out_paths;
    init_file_path(&out_paths);
    compare_list(server_version_list, &paths, &out_paths);

    //�Ƚϱ�����û��ɾ�����ļ�
    //////////////////////////////////////////////////////////////////////////
    //���Ͱ汾�Ƴ����·��
    server_protocol_type_file_exit(server_url);
    server_protocol_content_file_exit(server_url);

    //���������ȡ����ָ���б�
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
        receive_content(&type, buf_log); //���������߳�
        if (type == CHECK_REMOVE_FILENAME)
        {
            if (buf_log[0] != '\0')
            {
                //�������������б��ͱ������ݶԱ� ����ǲ����в����ڵģ�����в����ڵ��ȼ�¼
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

                //ȥ�����Ե�·��
                for (int i = 0; i < local_list_paths_org.index; i++)
                {
                    if (!is_ignore(local_list_paths_org.paths[i]))
                    {
                        strcpy(local_list_paths.paths[local_list_paths.index++], local_list_paths_org.paths[i]);
                    }
                }

                //��ʵ�����ݽ����Ų�
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
    //����Ƿ��б仯
    if (out_paths.index == 0 && server_list.size == 0)
    {
        log_error("���غͷ������汾һ�� ����û���κα仯������Ҫ�ϴ�");
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
            //�����ڷ�����
            {
                char buf_remote_path_tmp1[MAX_PATH] = {0};
                strcpy(buf_remote_path_tmp1, buf_remote_path_tmp);
                get_git_file_folder_inline(buf_remote_path_tmp1);

                if (!create_folder_path(buf_remote_path_tmp1))
                {
                    log_error("git������ ������һ����ͻ %s", buf_remote_path_tmp1);
                }
            }

            strcpy(git_path_2ds.paths[git_path_2ds.size].path_src, out_paths.paths[i]);
            strcpy(git_path_2ds.paths[git_path_2ds.size++].path_dis, buf_remote_path_tmp);
        }

        // cmd ���ڴ�ӡ���Ȱٷֱ�
        int b_value = (int)(((double)i / (double)out_paths.index) * 100 + 1);
        printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        printf("��ǰ����׼������Դ %3d%%", b_value);
        // \b�ĺ����ǣ������ӵ�ǰλ����ǰ�����ƶ�һ���ַ�������\n��\r��ֹͣ�ƶ��������Ӵ�λ�ÿ�ʼ���������ַ������ַ�\0�ͻ��з�\n���⣩��
    }

    // cmd ���ڴ�ӡ���Ȱٷֱ�
    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    printf("��ǰ����׼������Դ %3d%%", 100);
}

bool git_add_func(char* user_input)
{
    bool ret = log_log("׼����ʼ�������...");
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
        return false;
    }

    if (server_url[0] == '\0')
    {
        log_error("����,��ͨ�� git remote add origin ָ����Ч�ķ�������ַ...");
    }

    //��֤�汾�Ƿ�Ϊ���µ�
    server_protocol_type_file_exit(server_url);
    server_protocol_content_file_exit(server_url);
    if (send_protocol_type(server_url, VERSION_PROTOCOL) &&
        send_protocol_content(server_url, get_project_cache()))
    {
        EGitProtocolType type = NONE;
        char buf_log[8196 * 1024] = {0};
        receive_content(&type, buf_log); //���������߳�

        if (buf_log[0] == '\0')
        {
            log_error("buf_log = ��Ϊ��");

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
                log_error("���� ʹ��git clone ��¡һ����Ŀ�ӷ����");
                return -1;
            }

            //�Ƚϰ汾�Ƿ�һ��
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
                        log_error("git add ȱ�ٷ�������,���� git add .����·������������ ���� git add �����ļ��е����ƻ���·��");
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
                    receive_content(&type, buf_log); //���������߳�

                    if (type == FIND_VERSION_LIST)
                    {
                        FGitVersions server_version_list;
                        init_versions(&server_version_list);
                        //������ھͰѰ汾���з���
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
                                log_error("git add ȱ�ٷ�������,���� git add .����·������������ ���� git add �����ļ��е����ƻ���·��");
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
                            log_error("������Ҫͨ�� git remote add origin������һ��Զ��·��");
                        }
                    }
                    else
                    {
                        log_error("Э����� û�л�ȡ���б�Э��");
                    }
                }
            }
            else
            {
                log_error("���ص����ݲ������µ����� ��Ҫͨ�� git pull ��ȥ��������~~");
            }
        }
    }
    return true;
}

bool git_commit_func(char* user_input)
{
    bool ret = log_log("׼����ʼ�ύ������Ϣ...");
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
        return false;
    }

    if (server_url[0] == '\0')
    {
        log_error("����,��ͨ�� git remote add origin ָ����Ч�ķ�������ַ...");
    }

    if (git_path_2ds.size != 0 || server_list.size != 0) // ����ɶ����*��
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

        create_guid(&commit.guid); // ÿ���ύ����Ψһ�ģ�����һ���°汾 GUID

        log_log("�汾�ύ��ϢΪ �� %s", commit_content);
    }
    else
    {
        log_error("û�м����������κ��ļ��仯���п���û��ͨ��git add �������");
    }

    return true;
}

bool git_push_func()
{
    bool ret = log_log("׼����ʼ�ϲ� �������ύ��Զ�� �ֿ�...");
    if (!ret)
    {
        printf("\n���Ƚ��� git init ��ʼ��һ���ֿ� ����log��־ϵͳ�޷�ʹ��\n");
        return false;
    }

    if (server_url[0] == '\0')
    {
        log_error("����,��ͨ�� git remote add origin ָ����Ч�ķ�������ַ...");
    }

    if (git_path_2ds.size != 0 || server_list.size != 0)
    {
        if (commit.name != "")
        {
            const char* client_addr = get_project_cache();
            if (connect_is_alive(server_url, client_addr))
            {
                log_success("���ӷ������ɹ�", client_addr);

                //�����ǰ�汾 ׼��ӭ�����ύ�ĸ��汾
                init_versions(&versions);

                //���������ݷ��͵�������
                for (int i = 0; i < git_path_2ds.size; i++)
                {
                    EVersionOperationType operation_type = NONE_File;

                    char tmp_buf[1024] = {0};
                    strcat(tmp_buf, get_project_cache());
                    strcat(tmp_buf, "\n");
                    strcat(tmp_buf, git_path_2ds.paths[i].path_dis);

                    //���������ϵ��ļ��Ƿ����
                    //��������Ż� ������ǰ��git add��ȡ�� Ȼ������Ƿ���� ���ھͱ�Ǵ��ڵ� ��ʡ�������
                    if (send_protocol_type(server_url, FILE_EXIT) &&
                        send_protocol_content(server_url, tmp_buf))
                    {
                        EGitProtocolType type = NONE;
                        char buf_log[10] = {0};
                        receive_content(&type, buf_log); //���������߳�

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
                        log_log("��·���µ� %s ��ȥ �� %s �ɹ�", git_path_2ds.paths[i].path_src,
                                git_path_2ds.paths[i].path_dis);
                    }
                    else
                    {
                        log_error("��·���µ� %s ��ȥ �� %s ʧ��", git_path_2ds.paths[i].path_src,
                                  git_path_2ds.paths[i].path_dis);
                    }
                }

                //���·������汾
                //�汾����
                char buf_content[8196 * 1024] = {0};
                git_commit_to_string(buf_content, &commit);
                if (send_protocol_type(server_url, COMMIT)
                    && send_protocol_content(server_url, buf_content))
                {
                    //����
                    server_protocol_type_file_exit(server_url);
                    server_protocol_content_file_exit(server_url);

                    //�洢������
                    rewrite_file(get_client_version_info_file(), buf_content);

                    log_success("�汾��Ϣ���ͳɹ�");
                }

                //���ͱ����ѱ�ɾ����������Ϣ ֪ͨ������ɾ��������
                if (server_list.size != 0)
                {
                    memset(buf_content, 0, sizeof(buf_content));
                    path_to_string(buf_content, &server_list);

                    if (send_protocol_type(server_url, REMOVE_CONTENT) &&
                        send_protocol_content(server_url, buf_content))
                    {
                        //����
                        server_protocol_type_file_exit(server_url);
                        server_protocol_content_file_exit(server_url);

                        for (int i = 0; i < server_list.size; i++)
                        {
                            add_git_versions(server_list.data[i], SUBTRACT_FILE, &versions);
                            log_success("remove server :%s", server_list.data[i]);
                        }
                    }
                }

                //�б���
                memset(buf_content, 0, sizeof(buf_content));
                versions_to_string(buf_content, &versions);
                if (send_protocol_type(server_url, COMMIT_VERSION_LIST) &&
                    send_protocol_content(server_url, buf_content))
                {
                    //����
                    server_protocol_type_file_exit(server_url);
                    server_protocol_content_file_exit(server_url);

                    //�洢����
                    rewrite_file(get_client_version_list_file(), buf_content);

                    log_success("�汾���ݷ��ͳɹ� �汾�б�Ϊ��������");

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
                    log_success("��ǰ�İ汾��[%s] ֪ͨ���������д洢~", buf_content);
                }
            }
            else
            {
                log_error("����������ʧ��~~");
            }
        }
        else
        {
            log_error("�ύ��Ϣ����Ϊ�� ��ͨ�� git commit -m ��д����ύ����"); // ��֤�� commit ��Ϣ���� push
        }

        //�������
        init_versions(&versions);
        init_git_path_2ds(&git_path_2ds);
        init_commit(&commit);
    }
    else
    {
        log_error("û�м����������κ��ļ��仯���п���û��ͨ��git add �������");
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////// command ��