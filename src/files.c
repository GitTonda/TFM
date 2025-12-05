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