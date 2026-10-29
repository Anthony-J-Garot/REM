#include "display_formatter.h"
#include <stdio.h>
#include <string.h>
#include "display_epaper.h"
#include "font.h"

void display_formatter_draw(float temp_f, float pressure_inhg, float humidity, float dew_pt_h)
{
    // Draw a black outer border spanning most of the screen
    epaper_draw_box(10, 10, 276, 108, EPAPER_COLOR_BLACK, false);

    // Draw a solid black header banner box
    epaper_draw_box(15, 15, 266, 25, EPAPER_COLOR_BLACK, true);

    // Draw a red accent box at the bottom
    epaper_draw_box(15, 85, 266, 25, EPAPER_COLOR_RED, true);
}

