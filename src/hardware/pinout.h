#ifndef PROJ_PINOUT_H
#define PROJ_PINOUT_H

#define CENTRALBTN 3
#define UPBTN 2
#define DOWNBTN 1
#define LEFTBTN 8
#define RIGHTBTN 21
#define UPLEFT 7
#define UPRIGHT 20
#define DOWNLEFT 6
#define DOWNRIGHT 0

#define I2C_SDA 10
#define I2C_SCL 9

#ifdef CONFIG_SIMULATOR
#undef RIGHTBTN
#undef UPRIGHT
#define RIGHTBTN 5
#define UPRIGHT 6

#endif
#if __has_include("pinout_custom.h")
#include "pinout_custom.h"
#endif

#endif
