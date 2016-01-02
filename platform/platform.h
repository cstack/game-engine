#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

typedef uint32_t pixel_t;

struct pixel_buffer_t {
  int32_t height;
  uint32_t width;
  uint32_t stride;
  pixel_t* data;
};

struct controller_t {
  bool right_pressed;
  bool left_pressed;
  bool up_pressed;
  bool down_pressed;
  bool jump_pressed;
};

void update(double dt, pixel_buffer_t* pixel_buffer, controller_t &controller);

#endif
