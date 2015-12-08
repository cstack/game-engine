#include "platform/platform.h"

#include <string.h>
#include <iostream>

uint WHITE = 0xFFFFFFFF;
uint BLUE = 0x000000FF;
uint GREEN = 0x0000FF00;
uint colors[] = {
  0x00000000,
  0x000000FF,
  0x0000FF00,
  0x0000FFFF,
  0x00FF0000,
  0x00FF00FF,
  0x00FFFF00,
  0x00FFFFFF,
  0xFF000000,
  0xFF0000FF,
  0xFF00FF00,
  0xFF00FFFF,
  0xFFFF0000,
  0xFFFF00FF,
  0xFFFFFF00,
  0xFFFFFFFF,
};

double clip(double value, double min, double max) {
  if (value < min)
    value = min;
  if (value > max)
    value = max;
  return value;
}

void put_pixel(pixel_buffer_t* pixel_buffer, uint x, uint y, uint color) {
  uint offset = pixel_buffer->width * y + x;
  pixel_buffer->data[offset] = color;
}

void draw_box(pixel_buffer_t* pixel_buffer, double x, double y, double width, double height, uint color) {
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

void draw_bars(pixel_buffer_t* pixel_buffer) {
  uint num_sections = 16;
  uint section_height = pixel_buffer->height / num_sections;
  uint section_size = section_height * pixel_buffer->width;
  for (int i=0; i<num_sections; i++) {
    uint color = colors[i];
    for (int j=0; j< section_size; j++) {
      uint offset = section_size*i + j;
      pixel_buffer->data[offset] = color;
    }
  }
}



void update(double dt, pixel_buffer_t* pixel_buffer) {
  // Update and render
  draw_box(pixel_buffer, -10, 90, 40, 10, WHITE);
  draw_box(pixel_buffer, 30, 20, 100, 50, BLUE);
  draw_box(pixel_buffer, 40, 30, 20, 140, GREEN);
}
