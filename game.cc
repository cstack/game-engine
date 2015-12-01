#include "platform/platform.h"

#include <string.h>

void update(double dt, pixel_buffer_t* pixel_buffer) {
  // Update and render

  // Clear to WHITE
  memset(pixel_buffer->data, 0xFF, pixel_buffer->stride * pixel_buffer->height);
}
