#include "color.h"
#include "util.h"

color_t rgb(double red, double green, double blue) {
  color_t red_component = round(red) << 16;
  color_t green_component = round(green) << 8;
  color_t blue_component = round(blue);
  return red_component | green_component | blue_component;
}
