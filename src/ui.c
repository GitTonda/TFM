#include "../lib/tfm.h"
#include "../lib/ui.h"

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

unsigned long long calculate_folder_size(const char *path, int current_depth)
{
    if (current_depth > MAX_DEPTH) return 0;

    unsigned long long size = 0;
    DIR *d = opendir(path);

    if (!d) return 0;

    struct dirent *entry;
    struct stat sb;
    char full_path[PATH_MAX];

    while ((entry = readdir(d)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (lstat(full_path, &sb) == 0)
        {
            size += sb.st_size;
            if (S_ISDIR(sb.st_mode)) size += calculate_folder_size(full_path, current_depth + 1);
        }
    }

    closedir(d);
    return size;
}

unsigned long long get_directory_size()
{
    unsigned long long total_bytes = 0;
    struct stat sb;

    for (int i = 0; i < n_files; i++)
    {
        struct dirent *entry = current_dir[i];

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        if (lstat(entry->d_name, &sb) == 0)
        {
            if (S_ISDIR(sb.st_mode))
            {
                total_bytes += sb.st_size;
                total_bytes += calculate_folder_size(entry->d_name, 1);
            }
            else total_bytes += sb.st_size;
        }
    }

    return total_bytes;
}

void update_terminal_size()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    list_height = w.ws_row - 8;
    if (list_height < 1) list_height = 1;
}

void update_Screen()
{
    update_terminal_size();
    printf(HOME);

    // -- HEADER --
    char cwd[PATH_MAX];
    char *path_str = getcwd(cwd, sizeof(cwd)) ? cwd : "Error getting path";
    long total_size = get_directory_size();
    char size_str[64];
    if (total_size > 1024 * 1024 * 1024) snprintf(size_str, 64, "Size: %.2f GB", (double)total_size / (1024 * 1024 * 1024));
    else if (total_size > 1024 * 1024)   snprintf(size_str, 64, "Size: %.2f MB", (double)total_size / (1024 * 1024));
    else if (total_size > 1024)          snprintf(size_str, 64, "Size: %.2f KB", (double)total_size / 1024);
    else                                 snprintf(size_str, 64, "Size: %ld bytes", total_size);
    printf("╔");
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf("═");
    printf("╗\r\n");
    int path_len = (int)strlen(path_str);
    printf("║ ");
    if (path_len <= BOX_WIDTH)
    {
        printf("%s", path_str);
        for (int p = 0; p < BOX_WIDTH - path_len; p++) printf(" ");
    }
    else
    {
        int visible_len = BOX_WIDTH - 3;
        const char *tail_ptr = path_str + (path_len - visible_len);
        printf("...%s", tail_ptr);
    }
    printf(" ║\r\n");
    int size_len = (int)strlen(size_str);
    if (size_len > BOX_WIDTH) size_len = BOX_WIDTH;
    int size_pad = BOX_WIDTH - size_len;
    printf("║ %s", size_str);
    for (int p = 0; p < size_pad; p++) printf(" ");
    printf(" ║\r\n");
    printf("╚");
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf("═");
    printf("╝\r\n");

    // -- DIRECTORY CONTENTS --
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

    // FOOTER
    // TODO footer

    printf (CLEAR);
    fflush(stdout);
}