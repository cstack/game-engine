#include <stdint.h>

typedef uint32_t pixel_t;
typedef pixel_t color_t;

struct pixel_buffer_t {
  int32_t height;
  uint32_t width;
  uint32_t stride;
  pixel_t* data;
};

const color_t BLACK = 0x00000000;
const color_t BLUE = 0x000000FF;
const color_t GREEN = 0x0000FF00;
const color_t CYAN = 0x0000FFFF;
const color_t RED = 0x00FF0000;
const color_t MAGENTA = 0x00FF00FF;
const color_t YELLOW = 0x00FFFF00;
const color_t WHITE = 0x00FFFFFF;

void update(double dt, pixel_buffer_t* pixel_buffer);
