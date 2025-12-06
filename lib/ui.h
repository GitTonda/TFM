#ifndef TFM_UI_H
#define TFM_UI_H

#define CLEAR "\033[J"
#define RESET "\033[0m"
#define HOME "\033[H"
#define INVERT "\033[7m"
#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"

#define MAX_DEPTH 2
#define BOX_WIDTH 70

extern int scroll_offset;
extern int list_height;
extern int selected;
extern int current_command;

void update_Screen();

#endif