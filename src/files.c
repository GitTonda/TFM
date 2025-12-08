#include "../lib/tfm.h"
#include "../lib/files.h"

void load_directory()
{
    free_directory();
    n_files = scandir(".", &current_dir, NULL, alphasort);

    if (n_files < 0)
    {
        perror("scandir");
        exit(EXIT_FAILURE);
    }
}

void free_directory()
{
    if (n_files > 0 && current_dir != NULL)
    {
        for (int i = 0; i < n_files; i++)free(current_dir[i]);
        free(current_dir);

        current_dir = NULL;
        n_files = 0;
    }
}

// TODO clean code here
unsigned long long calculate_folder_size(const char *path, int depth)
{
    if (depth > MAX_DEPTH) return 0;
    // Skip virtual filesystems to prevent inaccurate 128TB sizes
    if (strncmp(path, "/proc", 5) == 0 || strncmp(path, "/sys", 4) == 0 ||
        strncmp(path, "/dev", 4) == 0 || strncmp(path, "/run", 4) == 0) return 0;

    unsigned long long size = 0;
    DIR *d = opendir(path);
    if (!d) return 0;

    struct dirent *e;
    struct stat sb;
    char full[PATH_MAX];

    while ((e = readdir(d)) != NULL) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
        snprintf(full, sizeof(full), "%s/%s", path, e->d_name);
        if (lstat(full, &sb) == 0) {
            if (!S_ISDIR(sb.st_mode)) size += sb.st_size;
            if (S_ISDIR(sb.st_mode)) size += calculate_folder_size(full, depth + 1);
        }
    }
    closedir(d);
    return size;
}

unsigned long long get_directory_size()
{
    unsigned long long total = 0;
    struct stat sb;
    for (int i = 0; i < n_files; i++) {
        if (!strcmp(current_dir[i]->d_name, ".") || !strcmp(current_dir[i]->d_name, "..")) continue;
        if (lstat(current_dir[i]->d_name, &sb) == 0) {
            if (S_ISDIR(sb.st_mode)) {
                total += sb.st_size;
                total += calculate_folder_size(current_dir[i]->d_name, 1);
            } else total += sb.st_size;
        }
    }
    return total;
}

void delete()
{
    char* answer = get_input(current_dir[selected]->d_name, "DELETE? (y/n)", COLOR_WARN, COLOR_WARN);
    if (answer != NULL && (strcmp(answer, "y") == 0 || strcmp(answer, "Y") == 0))
    {
        char command[1024];
        snprintf(command, sizeof(command), "rm -rf \"%s\"", current_dir[selected]->d_name);
        system(command);
        selected = 2;
    }

    load_directory();
    update_Screen();
}

void create()
{
    char* new_file = get_input(NULL, "New File Name", COLOR_HEADER, COLOR_RESET);
    if (new_file != NULL)
    {
        char command[1024];
        snprintf(command, sizeof(command), "%s \"%s\"",new_file[strlen(new_file) - 1] == '/' ? "mkdir" : "touch", new_file);
        system(command);
    }

    load_directory();
    update_Screen();
}