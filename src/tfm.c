#include "../lib/tfm.h"

struct termios orig_termios;    // original terminal attributes
struct dirent **current_dir;    // array of pointers to directory entries
int selected = 2;               // what element in dir is being targeted
int n_files;                    // number of files in dir
int scroll_offset = 0;          // Index of the first file visible at the top
int list_height = 0;            // How many files fit on the screen
int current_command = 0;        // current command being executed

int main()
{
    char buf[65536];
    setvbuf(stdout, buf, _IOFBF, sizeof(buf));

    enableRawMode();
    load_directory();
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

                update_Screen();
                break;
            }

            case ARROW_DOWN:
            {
                selected = selected < n_files - 1 ? selected + 1 : 2;
                if (selected < scroll_offset) scroll_offset = selected;
                if (selected >= scroll_offset + list_height) scroll_offset = selected - list_height + 1;

                update_Screen();
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

                update_Screen();
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

                update_Screen();
                break;
            }

            case 'h':
            case 'H':
            {
                current_command = input;
                update_Screen();
                break;
            }

            case 'r':
            case 'R':
            {
                if (get_confirmation(current_dir[selected]->d_name))
                {
                    delete();
                    selected = 2;
                }
                update_Screen();
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
    }

    return 0;
}
