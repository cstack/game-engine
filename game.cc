#include "platform/platform.h"
#include "engine/color.h"
#include "engine/util.h"

#include <string.h>
#include <iostream>

const uint TILE_MAP_WIDTH = 16;
const uint TILE_MAP_HEIGHT = 9;
const uint TILE_WIDTH = 1024 / TILE_MAP_WIDTH;
const uint TILE_HEIGHT = 576 / TILE_MAP_HEIGHT;
const color_t TILE_COLOR = rgb(100, 100, 100);
const bool TILE_MAP[TILE_MAP_HEIGHT][TILE_MAP_WIDTH] = {
  {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
  {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
};

const double PLAYER_WIDTH = TILE_WIDTH;
const double PLAYER_HEIGHT = PLAYER_WIDTH;
const color_t PLAYER_COLOR = rgb(150.3, 250.5, 20.6);
const double PLAYER_SPEED = 128; // in pixels per second

struct games_state_t {
  games_state_t() : initialized(false) {}
  bool initialized;
  double player_x;
  double player_y;
} game_state;

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

void initialize_game_state(games_state_t &game_state) {
  game_state.player_x = TILE_WIDTH*2;
  game_state.player_y = TILE_HEIGHT*5;

  game_state.initialized = true;
}

void clear_screen(pixel_buffer_t* pixel_buffer) {
  draw_box(pixel_buffer, 0, 0, pixel_buffer->width, pixel_buffer->height, BLACK);
}

void update(double dt, pixel_buffer_t* pixel_buffer, controller_t &controller) {
  if (!game_state.initialized) {
    initialize_game_state(game_state);
  }

  clear_screen(pixel_buffer);

  if (controller.right_pressed) {
    game_state.player_x += PLAYER_SPEED * dt;
  }
  if (controller.left_pressed) {
    game_state.player_x -= PLAYER_SPEED * dt;
  }
  if (controller.up_pressed) {
    game_state.player_y -= PLAYER_SPEED * dt;
  }
  if (controller.down_pressed) {
    game_state.player_y += PLAYER_SPEED * dt;
  }

  for (int row=0; row < TILE_MAP_HEIGHT; row++) {
    for (int col=0; col < TILE_MAP_WIDTH; col++) {
      if (TILE_MAP[row][col]) {
        draw_box(
          pixel_buffer,
          col*TILE_WIDTH,
          row*TILE_HEIGHT,
          TILE_WIDTH,
          TILE_HEIGHT,
          TILE_COLOR
        );
      }
    }
  }

  draw_box(
    pixel_buffer,
    game_state.player_x - PLAYER_WIDTH/2,
    game_state.player_y - PLAYER_HEIGHT,
    PLAYER_WIDTH,
    PLAYER_HEIGHT,
    PLAYER_COLOR
  );
}
