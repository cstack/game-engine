#include "game.h"

#include "platform/platform.h"
#include "engine/color.h"
#include "engine/util.h"

#include <string.h>

game_state_t game_state;

const double PLAYER_WIDTH = TILE_WIDTH;
const double PLAYER_HEIGHT = PLAYER_WIDTH;
const color_t PLAYER_COLOR = rgb(150.3, 250.5, 20.6);
const double PLAYER_SPEED = 128; // in pixels per second

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

void initialize_game_state(game_state_t &game_state) {
  game_state.player_x = TILE_WIDTH*2;
  game_state.player_y = TILE_HEIGHT*5;

  game_state.tile_map_x = 0;
  game_state.tile_map_y = 0;

  game_state.initialized = true;
}

void clear_screen(pixel_buffer_t* pixel_buffer) {
  draw_box(pixel_buffer, 0, 0, pixel_buffer->width, pixel_buffer->height, BLACK);
}

bool location_occupied(tile_map_t* tile_map, double x, double y) {
  if (x > TILE_MAP_WIDTH*TILE_WIDTH || x < 0) {
    return true;
  }
  if (y > TILE_MAP_HEIGHT*TILE_HEIGHT || y < 0) {
    return true;
  }
  uint row = y / TILE_HEIGHT;
  uint col = x / TILE_WIDTH;
  return tile_map->tiles[row*TILE_MAP_WIDTH + col];
}

bool valid_player_location(tile_map_t* tile_map, double x, double y) {
  return !location_occupied(tile_map, x - PLAYER_WIDTH/2, y) &&
    !location_occupied(tile_map, x + PLAYER_WIDTH/2, y);
}

void update(double dt, pixel_buffer_t* pixel_buffer, controller_t &controller) {
  if (!game_state.initialized) {
    std::cout << "Initializing game state" << std::endl;
    initialize_game_state(game_state);
  }

  // Define world layout
  tile_t tiles_00[TILE_MAP_HEIGHT][TILE_MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
  };
  tile_t tiles_01[TILE_MAP_HEIGHT][TILE_MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
  };
  tile_t tiles_10[TILE_MAP_HEIGHT][TILE_MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  };
  tile_t tiles_11[TILE_MAP_HEIGHT][TILE_MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  };

  #define WORLD_WIDTH 2
  #define WORLD_HEIGHT 2
  tile_map_t tile_maps[WORLD_HEIGHT][WORLD_WIDTH];
  tile_maps[0][0].tiles = (tile_t*) tiles_00;
  tile_maps[0][1].tiles = (tile_t*) tiles_01;
  tile_maps[1][0].tiles = (tile_t*) tiles_10;
  tile_maps[1][1].tiles = (tile_t*) tiles_11;

  world_t world;
  world.width = WORLD_WIDTH;
  world.height = WORLD_HEIGHT;
  world.tile_maps = (tile_map_t*) tile_maps;

  tile_map_t* tile_map = &world.tile_maps[game_state.tile_map_y*WORLD_WIDTH + game_state.tile_map_x];

  clear_screen(pixel_buffer);

  // Move player
  double new_player_x = game_state.player_x;
  double new_player_y = game_state.player_y;
  if (controller.right_pressed) {
    new_player_x += PLAYER_SPEED * dt;
  }
  if (controller.left_pressed) {
    new_player_x -= PLAYER_SPEED * dt;
  }
  if (controller.up_pressed) {
    new_player_y -= PLAYER_SPEED * dt;
  }
  if (controller.down_pressed) {
    new_player_y += PLAYER_SPEED * dt;
  }
  if (valid_player_location(tile_map, new_player_x, game_state.player_y)) {
    game_state.player_x = new_player_x;
  }
  if (valid_player_location(tile_map, game_state.player_x, new_player_y)) {
    game_state.player_y = new_player_y;
  }

  // Render tile map
  for (int row=0; row < TILE_MAP_HEIGHT; row++) {
    for (int col=0; col < TILE_MAP_WIDTH; col++) {
      if (tile_map->tiles[row*TILE_MAP_WIDTH + col]) {
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

  // Render Player
  draw_box(
    pixel_buffer,
    game_state.player_x - PLAYER_WIDTH/2,
    game_state.player_y - PLAYER_HEIGHT,
    PLAYER_WIDTH,
    PLAYER_HEIGHT,
    PLAYER_COLOR
  );
}
