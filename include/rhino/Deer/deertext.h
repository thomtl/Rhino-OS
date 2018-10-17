#pragma once

#include <rhino/common.h>
#include <rhino/Deer/deer.h>
#include <rhino/Deer/deergui.h>

#define DEER_TERMINAL_WIDTH (SCREEN_WIDTH / 8)
#define DEER_TERMINAL_HEIGHT (SCREEN_HEIGHT / 8)

void deer_printf(char* msg);
void deer_printchar(char c, char r, char g, char b, uint32_t x, uint32_t y);
void deer_putchar(char c, char r, char g, char b);
void deer_reset_terminal();