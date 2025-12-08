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

void draw_footer(const char *color_border, const char *color_content, const char *text)
{
    // 1. Top Border
    printf("%s╔", color_border);
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf("═");
    printf("╗%s\033[K\r\n", COLOR_RESET);

    // 2. Content Line
    // We inject 'color_content' before the text/padding spaces
    printf("%s%s %s%-*.*s%s %s%s\033[K\r\n",
           color_border, "║",
           color_content,
           BOX_WIDTH, BOX_WIDTH, text,
           color_border, "║", COLOR_RESET);

    // 3. Bottom Border
    printf("%s╚", color_border);
    for (int k = 0; k < BOX_WIDTH + 2; k++) printf("═");
    printf("╝%s\033[K\r\n", COLOR_RESET);
}

void update_Screen() {
    update_terminal_size();
    printf(HOME);

    draw_header();

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

char* get_input(const char *filename, const char *message, const char *color_border, const char *color_content)
{
    static char input_buf[256];
    memset(input_buf, 0, sizeof(input_buf));
    int index = 0;

    char display_str[512];
    char c;

    while (1)
    {
        if (filename) {
            snprintf(display_str, sizeof(display_str), "%s '%s': %s_", message, filename, input_buf);
        } else {
            snprintf(display_str, sizeof(display_str), "%s: %s_", message, input_buf);
        }

        // Move up 3 lines to overwrite footer
        printf("\033[3A");
        draw_footer(color_border, color_content, display_str);
        fflush(stdout);

        if (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (c == '\r' || c == '\n') {
                return input_buf; // Return input on Enter
            }
            else if (c == 127 || c == '\b') {
                if (index > 0) input_buf[--index] = '\0';
            }
            else if (c == 27) {
                return NULL; // Cancel on ESC
            }
            // If isprint fails for you, try commenting out '&& isprint(c)' to debug
            else if (index < sizeof(input_buf) - 1 && isprint(c)) {
                input_buf[index++] = c;
                input_buf[index] = '\0';
            }
        }
    }
}