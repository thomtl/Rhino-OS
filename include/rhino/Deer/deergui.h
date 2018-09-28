#pragma once

#include <rhino/common.h>
#include <rhino/Deer/deer.h>

#define DEER_TERMINAL_WIDTH 80
#define DEER_TERMINAL_HEIGHT 75

//#define DEER_CREATE_COLOUR(r, g, b) (((r << 11) | (g << 6) | (r << 1) | 0))

void draw_pixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b);
void draw_circle(int32_t x0, int32_t y0, int32_t radius, uint8_t r, uint8_t g, uint8_t b);
void draw_rect(size_t x, size_t y, size_t w, size_t h, uint8_t r, uint8_t g, uint8_t b);