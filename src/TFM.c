#include "../lib/TMF.h"

struct termios orig_termios;    // original terminal attributes
struct dirent **current_dir;    // array of pointers to directory entries
int selected = 0;               // what element in dir is being targeted
int n_files;                    // number of files in dir

int main()
{
    enableRawMode();
    load_directory();
    update_Screen();

    int running = 1;
    for (int input; running;)
    {
        input = readKey();
        switch (input)
        {
            // Move upwards in dir
            case ARROW_UP:
            {
                if (selected > 0) selected--;
                break;
            }

            // Move downward in dir
            case ARROW_DOWN:
            {
                if (selected < n_files - 1) selected++;
                break;
            }

            // Quitting program
            case 'q':
            case 'Q':
            {
                running = 0;
                break;
            }

            // No command associated to key
            default: break;
        }

        update_Screen();
    }

    return 0;
}

void update_Screen()
{
    printf("\033[2J\033[H");

    for (int i = 0; i < n_files; i++)
    {
        if (i == selected)printf("\033[7m %s \033[0m\r\n", current_dir[i]->d_name);
        else printf(" %s\r\n", current_dir[i]->d_name);
    }

    fflush(stdout);
}