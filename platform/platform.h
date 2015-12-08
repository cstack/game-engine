#include <stdint.h>

typedef uint32_t pixel_t;
typedef pixel_t color_t;

struct pixel_buffer_t {
  int32_t height;
  uint32_t width;
  uint32_t stride;
  pixel_t* data;
};

color_t BLACK = 0x00000000;
color_t BLUE = 0x000000FF;
color_t GREEN = 0x0000FF00;
color_t CYAN = 0x0000FFFF;
color_t RED = 0x00FF0000;
color_t MAGENTA = 0x00FF00FF;
color_t YELLOW = 0x00FFFF00;
color_t WHITE = 0x00FFFFFF;

void update(double dt, pixel_buffer_t* pixel_buffer);
