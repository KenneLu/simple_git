#include "../../public/log/log.h"
#include "../../public/window/window_setting.h"
#include "../../public/util/string_util.h"

char log_folder[MAX_PATH] = {0}; // ��־�ļ���·��
char log_filename[MAX_PATH] = {0}; // ��־�ļ�·��

const char* get_log_file()
{
    // û����־�ļ�������һ��
    if (log_filename[0] == '\0')
    {
        // ������������־�ļ���·��
        if (log_folder[0] == '\0')
        {
            set_console_color(RED, BLACK);
            printf("\n You need to add the log path in set_log_folder(...) first.  \n");
            //assert(0);
            set_console_color(WHITE, BLACK);

            return NULL;
        }

        // ȷ����־�ļ���·���Ѿ�����
        const char* p = log_folder;
        char tmp_path[MAX_PATH] = {0};
        strcpy(tmp_path, p);
        strcat(tmp_path, "\\");
        _mkdir(tmp_path);

        // ��־�ļ�������ʱ��
        time_t now_time = time(NULL); // ���ƺ�û�б����á���*��
        char* p_time = ctime(__TIME__);
        remove_char_end(p_time, '\n');
        remove_char_end(p_time, '\:');
        remove_char_end(p_time, '\:');
        strcat(tmp_path, p_time);

        // ��־�ļ���׺Ϊ .txt
        const char file_suffix[] = ".txt";
        strcat(tmp_path, file_suffix);

        // ������־�ļ�
        FILE* hfile = NULL;
        if ((hfile = fopen(tmp_path, "a+")) != NULL)
        {
            strcpy(log_filename, tmp_path);
            fclose(hfile);
        }
    }

    // ������־�ļ�
    return log_filename;
}

const char* get_log_folder()
{
    if (log_folder[0] == '\0')
    {
        set_console_color(RED, BLACK);
        printf("\n You need to add the log path in set_log_folder(...) first.  \n");
        assert(0);
        set_console_color(WHITE, BLACK);
    }

    return log_folder;
}

void set_log_folder(const char* path)
{
    strcpy(log_folder, path);
}

bool logger(EMessageType type, char* format, ...)
{
    // ��־����
    char type_str[64] = {0};
    switch (type)
    {
    case MSG_SUCCESS:
        strcpy(type_str, "SUCCESS");
        break;
    case MSG_LOG:
        strcpy(type_str, "LOG");
        break;
    case MSG_WARNING:
        strcpy(type_str, "WARNING");
        break;
    case MSG_ERROR:
        strcpy(type_str, "ERROR");
        break;
    }

    // ��ȡ��־�ļ�
    const char* p = get_log_file();

    // д����־
    if (p != NULL)
    {
        FILE* hfile = NULL;
        if ((hfile = fopen(p, "a+")) != NULL)
        {
            // ��ʼ�� buf
            // ZeroMemory() �����Ͼ��� memset()
            char out_buf[8196 * 1024] = {0};
            ZeroMemory(out_buf, sizeof(char) * 8196 * 1024);


            va_list args;
            va_start(args, format);
            _vsnprintf_s(out_buf, 8196 * 1024 - 1, 8196 * 1024, format, args);
            va_end(args);
            out_buf[8196 * 1024 - 1] = 0;

            // ��ȡʱ��
            char* time = ctime(__TIME__); // \n
            remove_char_end(time, '\n');

            // ƴ��ռλ��
            char text_buf[8196 * 1024] = {0};
            placeholder_cat_s(text_buf, "[%s] [%s] %s \r\n", type_str, time, out_buf); // �����ƺ������滻����Ķ���

            // ������־��ɫ
            switch (type)
            {
            case MSG_SUCCESS:
                set_console_color(PALE_GREEN, BLACK);
                break;
            case MSG_LOG:
                set_console_color(WHITE, BLACK);
                break;
            case MSG_WARNING:
                set_console_color(YELLOW, BLACK);
                break;
            case MSG_ERROR:
                set_console_color(RED, BLACK);
                break;
            }
            printf(text_buf);
            fprintf(hfile, text_buf);
            set_console_color(WHITE, BLACK);
            fclose(hfile);
        }

        return true;
    }

    return false;
}
