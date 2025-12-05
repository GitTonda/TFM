#include "../lib/tfm.h"

struct termios orig_termios;    // original terminal attributes
struct dirent **current_dir;    // array of pointers to directory entries
int selected = 2;               // what element in dir is being targeted
int n_files;                    // number of files in dir
int scroll_offset = 0;          // Index of the first file visible at the top
int list_height = 0;            // How many files fit on the screen

int main()
{
    enableRawMode();
    load_directory();
    update_terminal_size();
    update_Screen();

    for (int input = 0; input != -1;)
    {
        input = readKey();
        switch (input)
        {
            // Move upwards in dir
            case ARROW_UP:
            {
                selected = selected > 2 ? selected - 1 : n_files - 1;
                if (selected < scroll_offset) scroll_offset = selected;
                if (selected >= scroll_offset + list_height) scroll_offset = selected - list_height + 1;
                break;
            }

            case ARROW_DOWN:
            {
                selected = selected < n_files - 1 ? selected + 1 : 2;
                if (selected < scroll_offset) scroll_offset = selected;
                if (selected >= scroll_offset + list_height) scroll_offset = selected - list_height + 1;
                break;
            }

            // Go to parent directory
            case ARROW_LEFT:
            {
                char cwd[PATH_MAX];
                char previous_folder_name[256] = {0};

                if (getcwd(cwd, sizeof(cwd)) != NULL)
                {
                    char *last_slash = strrchr(cwd, '/');
                    if (last_slash != NULL)
                        strncpy(previous_folder_name, last_slash + 1, sizeof(previous_folder_name) - 1);
                }

                chdir("..");
                load_directory();
                selected = 2;
                scroll_offset = 0;

                if (strlen(previous_folder_name) > 0)
                    for (int i = 0; i < n_files; i++)
                        if (strcmp(current_dir[i]->d_name, previous_folder_name) == 0)
                        {
                            selected = i;
                            if (selected >= scroll_offset + list_height)
                                scroll_offset = selected - list_height + 2;
                            break;
                        }
                break;
            }

            // Enter in directory
            case ARROW_RIGHT:
            {
                struct dirent *entry = current_dir[selected];
                bool is_dir = (entry->d_type == DT_DIR);
                if (is_dir && strcmp(entry->d_name, ".") != 0 && chdir(entry->d_name) == 0)
                {
                    load_directory();
                    selected = 2;
                    scroll_offset = 0;
                }
                break;
            }

            // Quitting program
            case 'q':
            case 'Q':
            {
                free_directory();
                input = -1;
                break;
            }

            // No command associated to key
            default: break;
        }

        update_Screen();
    }

    return 0;
}


// Drawing part --------------------------------------------------------------------------------------------------------

#define CLEAR "\033[2J"
#define RESET "\033[0m"
#define HOME "\033[H"
#define INVERT "\033[7m"
#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"

#define BOX_WIDTH 40

const char* get_file_color(struct dirent *entry)
{
    if (entry->d_type == DT_DIR) return BLUE;

    if (entry->d_type == DT_REG)
    {
        struct stat sb;
        if (stat(entry->d_name, &sb) == 0 && sb.st_mode & S_IXUSR) return GREEN;
    }

    return RESET;
}

long get_directory_size()
{
    long total_bytes = 0;
    struct stat sb;
    for (int i = 0; i < n_files; i++)
        if (current_dir[i]->d_type == DT_REG && stat(current_dir[i]->d_name, &sb))
            total_bytes += sb.st_size;
    return total_bytes;
}

void update_terminal_size()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    list_height = w.ws_row - 5;
    if (list_height < 1) list_height = 1;
}

void update_Screen()
{
    printf(CLEAR HOME);

    char cwd[PATH_MAX];
    printf ("%s\r\n", (getcwd(cwd, sizeof(cwd)) != NULL) ? cwd : "Error getting path");

    long total_size = get_directory_size();
    if (total_size > 1024 * 1024) printf(" Size: %ld MB\r\n", total_size / (1024 * 1024));
    else if (total_size > 1024) printf(" Size: %ld KB\r\n", total_size / 1024);
    else printf(" Size: %ld bytes\r\n", total_size);

    printf("┌");
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf("─");
    printf("┐\r\n");

    for (int i = scroll_offset > 2 ? scroll_offset : 2; i < scroll_offset + list_height; i++)
    {
        if (i >= n_files) break;

        struct dirent *entry = current_dir[i];

        const char *select_code = (i == selected) ? INVERT : "";
        const char *color_code = get_file_color(entry);
        char decoration = (entry->d_type == DT_DIR) ? '/' : ' ';

        int name_len = (int) strlen(entry->d_name);
        if (name_len > BOX_WIDTH - 2) name_len = BOX_WIDTH - 2;
        int padding = BOX_WIDTH - name_len - 1;

        printf("│ ");
        printf("%s%s", color_code, select_code);
        printf("%.*s%c", name_len, entry->d_name, decoration);
        for (int p = 0; p < padding; p++) printf(" ");

        printf("%s", RESET);
        printf(" │\r\n");
    }

    printf("└");
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf("─");
    printf("┘\r\n");

    fflush(stdout);
}