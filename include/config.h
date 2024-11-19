#pragma once

#define DISPLAY_TYPE_SSD1306 1
#define DISPLAY_TYPE_SH

#if __has_include("config_custom.h")
#include "config_custom.h"
#endif

#ifndef U8G2_DISPLAY_CLASS
#define U8G2_DISPLAY_CLASS U8G2_SH1106_128X64_NONAME_F_HW_I2C
#endif