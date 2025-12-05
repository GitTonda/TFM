#ifndef TMF_H
#define TMF_H

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "console.h"
#include "files.h"

void update_Screen();

void update_terminal_size();

#endif