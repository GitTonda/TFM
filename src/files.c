#include "../lib/TMF.h"
#include "../lib/files.h"

void load_directory()
{
    if (current_dir)
    {
        for (int i = 0; i < n_files; i++) free(current_dir[i]);
        free(current_dir);
    }

    n_files = scandir(".", &current_dir, NULL, alphasort);

    if (n_files < 0)
    {
        perror("scandir");
        exit(EXIT_FAILURE);
    }
}