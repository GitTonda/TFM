#ifndef TFM_UI_H
#define TFM_UI_H

#define CLEAR "\033[J"
#define RESET "\033[0m"
#define HOME "\033[H"
#define INVERT "\033[7m"

#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"

#define COLOR_HEADER "\033[1;36m"       // Bold Cyan
#define COLOR_FOOTER "\033[1;32m"       // Bold Green
#define COLOR_WARN   "\033[41;1;37m"    // Red Background, White Text
#define COLOR_RESET  "\033[0m"

#define H_DOUBLE "═"
#define V_DOUBLE "║"
#define H_SINGLE "─"
#define V_SINGLE "│"

#define MAX_DEPTH 2
#define BOX_WIDTH 70
#define HEADER_SIZE 4
#define FOOTER_SIZE 3

extern int scroll_offset;
extern int list_height;
extern int selected;
extern int current_command;

void update_Screen();
bool get_confirmation(const char *filename);

#endif