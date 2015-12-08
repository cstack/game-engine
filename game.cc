#include "platform/platform.h"

#include <string.h>
#include <iostream>

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
  draw_bars(pixel_buffer);
}
