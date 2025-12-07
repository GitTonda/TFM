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

void update_terminal_size()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    list_height = w.ws_row - (HEADER_SIZE + FOOTER_SIZE);
    if (list_height < 1) list_height = 1;
}

// TODO clean
void draw_header()
{
    char cwd[PATH_MAX];
    char *pstr = getcwd(cwd, sizeof(cwd)) ? cwd : "Error";

    long tsize = get_directory_size();
    char sstr[64];
    if (tsize > 1073741824) snprintf(sstr, 64, "Size: %.2f GB", (double)tsize / 1073741824);
    else if (tsize > 1048576) snprintf(sstr, 64, "Size: %.2f MB", (double)tsize / 1048576);
    else snprintf(sstr, 64, "Size: %ld bytes", tsize);

    printf("%s╔", COLOR_HEADER);
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf(H_DOUBLE);
    printf("╗%s\033[K\r\n", COLOR_RESET);

    // Path Logic
    int plen = strlen(pstr);
    if (plen <= BOX_WIDTH) printf("%s%s %-*s %s%s\033[K\r\n", COLOR_HEADER, V_DOUBLE, BOX_WIDTH, pstr, V_DOUBLE, COLOR_RESET);
    else printf("%s%s ...%-*s %s%s\033[K\r\n", COLOR_HEADER, V_DOUBLE, BOX_WIDTH - 3, pstr + (plen - (BOX_WIDTH - 3)), V_DOUBLE, COLOR_RESET);

    printf("%s%s %-*s %s%s\033[K\r\n", COLOR_HEADER, V_DOUBLE, BOX_WIDTH, sstr, V_DOUBLE, COLOR_RESET);

    printf("%s╚", COLOR_HEADER);
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf(H_DOUBLE);
    printf("╝%s\033[K\r\n", COLOR_RESET);
}

void draw_footer(const char *color_border, const char *color_content, const char *text) {
    // 1. Top Border
    printf("%s╔", color_border);
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf(H_DOUBLE);
    printf("╗%s\033[K\r\n", COLOR_RESET);

    // 2. Content Line
    // We print the color_content code, then the text, then enough spaces to fill the box
    // The background color will naturally paint those spaces.
    printf("%s%s %-*.*s %s%s%s%s\033[K\r\n",
           color_border, V_DOUBLE,     // Left Border
           BOX_WIDTH, BOX_WIDTH, text, // Text with padding logic handled by printf
           COLOR_RESET, color_border, V_DOUBLE, COLOR_RESET); // Right Border

    // 3. Bottom Border
    printf("%s╚", color_border);
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf(H_DOUBLE);
    printf("╝%s\033[K\r\n", COLOR_RESET);
}

void update_Screen() {
    update_terminal_size();
    printf(HOME);

    draw_header();

    // --- LIST ---
    printf("┌"); for (int k = 0; k < BOX_WIDTH + 2; k++) printf(H_SINGLE); printf("┐\033[K\r\n");

    for (int i = scroll_offset > 2 ? scroll_offset : 2; i < scroll_offset + list_height; i++) {
        if (i >= n_files) break;
        struct dirent *e = current_dir[i];

        int nlen = strlen(e->d_name);
        if (nlen > BOX_WIDTH - 2) nlen = BOX_WIDTH - 2;

        printf("│ %s%s%.*s%c%*s%s │\033[K\r\n",
               get_file_color(e),
               (i == selected) ? INVERT : "",
               nlen, e->d_name,
               (e->d_type == DT_DIR) ? '/' : ' ',
               BOX_WIDTH - nlen - 1, "", // Padding
               RESET);
    }
    printf("└"); for (int k = 0; k < BOX_WIDTH + 2; k++) printf(H_SINGLE); printf("┘\033[K\r\n");

    // --- 3. FOOTER (GREEN THEME) ---
    char footer_text[64];
    char cmd_char = (current_command == 0) ? ' ' : current_command;
    snprintf(footer_text, 64, "CMD: [%c]  Navigate: Arrows", cmd_char);

    // Pass 'COLOR_FOOTER' for borders, and empty string "" for content (standard black bg)
    draw_footer(COLOR_FOOTER, "", footer_text);

    // Clear everything below just in case list shrank
    printf("\033[J");
    fflush(stdout);
}

bool get_confirmation(const char *filename) {
    // 1. Prepare red footer text
    char msg[128];
    snprintf(msg, sizeof(msg), "DELETE '%s'? (y/n)", filename);

    // 2. Move cursor up 3 lines (overwrite existing footer)
    // We assume footer is 3 lines tall.
    printf("\033[3A");

    // 3. Draw Red Footer
    draw_footer(COLOR_WARN, COLOR_WARN, msg);
    fflush(stdout);

    // 4. Blocking Input
    char input;
    while (1) {
        if (read(STDIN_FILENO, &input, 1) == 1) {
            if (input == 'y' || input == 'Y') return true;
            if (input == 'n' || input == 'N' || input == 'q' || input == 27) return false;
        }
    }
}