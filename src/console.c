#include "../lib/TMF.h"
#include "../lib/console.h"

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    printf("\033[?1049h");
    printf("\033[?25l");
}

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

    printf("\033[?1049l");
    printf("\033[?25h");
}

int readKey()
{
    char c;
    int nread;

    // Read 1 byte
    nread = read(STDIN_FILENO, &c, 1);
    if (nread != 1) return '\0';

    // If we receive an ESC character, it might be an arrow key
    if (c == '\033') {
        char seq[3];

        // Try to read 2 more bytes
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\033';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\033';

        if (seq[0] == '[')
            switch (seq[1])
            {
                case 'A': return ARROW_UP;
                case 'B': return ARROW_DOWN;
                case 'C': return ARROW_RIGHT;
                case 'D': return ARROW_LEFT;
            }
        return '\033'; // Just a normal ESC key press
    }

    return c; // Just a normal character (a, b, c, 1, 2...)
}