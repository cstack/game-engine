#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef uint32_t color_t;

const color_t BLACK = 0x00000000;
const color_t BLUE = 0x000000FF;
const color_t GREEN = 0x0000FF00;
const color_t CYAN = 0x0000FFFF;
const color_t RED = 0x00FF0000;
const color_t MAGENTA = 0x00FF00FF;
const color_t YELLOW = 0x00FFFF00;
const color_t WHITE = 0x00FFFFFF;

color_t rgb(double red, double green, double blue);

#endif
