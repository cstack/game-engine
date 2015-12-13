#include "platform/platform.h"

#include <string.h>
#include <iostream>

int round(double value) {
  return (int) (value + 0.5);
}

color_t rgb(double red, double green, double blue) {
  uint red_component = round(red) << 16;
  uint green_component = round(green) << 8;
  uint blue_component = round(blue);
  return red_component | green_component | blue_component;
}

double clip(double value, double min, double max) {
  if (value < min)
    value = min;
  if (value > max)
    value = max;
  return value;
}

void put_pixel(pixel_buffer_t* pixel_buffer, uint x, uint y, color_t color) {
  uint offset = pixel_buffer->width * y + x;
  pixel_buffer->data[offset] = color;
}

void draw_box(pixel_buffer_t* pixel_buffer, double x, double y, double width, double height, color_t color) {
  int x1 = clip(x, 0, pixel_buffer->width);
  int x2 = clip(x+width, 0, pixel_buffer->width);
  int y1 = clip(y, 0, pixel_buffer->height);
  int y2 = clip(y+height, 0, pixel_buffer->height);

  for (int x_to_fill=x1; x_to_fill<x2; x_to_fill++) {
    for (int y_to_fill=y1; y_to_fill<y2; y_to_fill++) {
      put_pixel(pixel_buffer, x_to_fill, y_to_fill, color);
    }
  }
}

void update(double dt, pixel_buffer_t* pixel_buffer) {
  // Update and render
  color_t color = rgb(150.3, 250.5, 20.6);
  draw_box(pixel_buffer, -10, 90, 40, 10, color);
  draw_box(pixel_buffer, 30, 20, 100, 50, BLUE);
  draw_box(pixel_buffer, 40, 30, 20, 140, GREEN);
}
