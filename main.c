#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

    printf("\033[?1049l");
    printf("\033[?25h");
}

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

int main()
{
    enableRawMode();
    printf("\033[HWelcome to TFM\r\n");

    char c;
    while (1)
    {
        if (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (c == 'q') break;
            printf("You pressed: %c (%d)\r\n\n", c, c);
        }
    }

    return 0;
}