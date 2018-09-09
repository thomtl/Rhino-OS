#pragma once

#include <rhino/common.h>
#include <rhino/Deer/deer.h>

#define DEER_TERMINAL_WIDTH 40
#define DEER_TERMINAL_HEIGHT 25

void draw_pixel(size_t x, size_t y, uint8_t c);
void draw_circle(int32_t x0, int32_t y0, int32_t radius, uint8_t col);
void draw_rect(size_t x, size_t y, size_t w, size_t h, uint8_t c);