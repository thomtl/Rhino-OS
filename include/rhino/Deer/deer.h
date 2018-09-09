#pragma once
#include <rhino/common.h>
#include <rhino/Deer/deervga.h>
#include <rhino/Deer/deertext.h>
#include <rhino/Deer/deergui.h>

#define deer_clear_screen() (deer_clear_screen_col(0))

void deer_clear_screen_col(uint8_t col);
void deer_start();
