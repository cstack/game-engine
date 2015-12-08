#include <stdint.h>

struct pixel_buffer_t {
  int32_t height;
  uint32_t width;
  uint32_t stride;
  uint32_t* data;
};

void update(double dt, pixel_buffer_t* pixel_buffer);
