#include "game.h"

#include "platform/platform.h"
#include "engine/util.h"

#include <string.h>

game_state_t game_state;

const double PLAYER_WIDTH_METERS = 0.8;
const double PLAYER_WIDTH = PLAYER_WIDTH_METERS * METERS_TO_PIXELS;

const double PLAYER_HEIGHT_METERS = 1.0;
const double PLAYER_HEIGHT = PLAYER_HEIGHT_METERS * METERS_TO_PIXELS;

const color_t PLAYER_COLOR = rgb(150.3, 250.5, 20.6);

const double PLAYER_SPEED_METERS_PER_SECOND = 3.0;
const double PLAYER_SPEED = PLAYER_SPEED_METERS_PER_SECOND * METERS_TO_PIXELS;

// Define world layout
tile_t tiles[WORLD_HEIGHT][WORLD_WIDTH] = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1},
  {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

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
  game_state.player_location.tile_x = 2;
  game_state.player_location.tile_y = 5;
  game_state.player_location.x = 0;
  game_state.player_location.y = 0;

  game_state.world.width = WORLD_WIDTH;
  game_state.world.height = WORLD_HEIGHT;
  game_state.world.tiles = (tile_t*) tiles;

  game_state.initialized = true;
}

void clear_screen(pixel_buffer_t* pixel_buffer) {
  draw_box(pixel_buffer, 0, 0, pixel_buffer->width, pixel_buffer->height, BLACK);
}

#ifdef DEBUG
  void print_location(location_t location) {
    std::cout << "location(" << location.tile_x + location.x << ", " << location.tile_y + location.y << ")" << std::endl;
  }

  void print_screen_location(screen_location_t location) {
    std::cout << "screen_location(" << location.x << ", " << location.y << ")" << std::endl;
  }
#endif

screen_location_t get_screen_location(location_t location) {
  screen_location_t screen_location;
  screen_location.x = (location.tile_x % TILES_PER_SCREEN_X + location.x) * METERS_TO_PIXELS;
  screen_location.y = (location.tile_y % TILES_PER_SCREEN_Y + location.y) * METERS_TO_PIXELS;
  return screen_location;
}

tile_t get_tile(world_t* world, uint tile_x, uint tile_y) {
  return world->tiles[tile_y*world->width + tile_x];
}

tile_t get_tile(world_t* world, location_t location) {
  return get_tile(world, location.tile_x, location.tile_y);
}

bool location_occupied(world_t* world, uint tile_x, uint tile_y) {
  return get_tile(world, tile_x, tile_y);
}

bool location_occupied(world_t* world, location_t location) {
  return get_tile(world, location.tile_x, location.tile_y);
}

location_t update_location(location_t location, double dx, double dy) {
  // Should only change locations using this method so x and y stay within bounds of a tile map
  location.x += dx;
  location.y += dy;
  while (location.x < 0) {
    location.x += 1;
    location.tile_x -= 1;
  }
  while (location.x >= 1) {
    location.x -= 1;
    location.tile_x += 1;
  }
  while (location.y < 0) {
    location.y += 1;
    location.tile_y -= 1;
  }
  while (location.y >= 1) {
    location.y -= 1;
    location.tile_y += 1;
  }
  return location;
}

bool valid_player_location(world_t* world, location_t location) {
  location_t bottom_left = update_location(location, PLAYER_WIDTH_METERS / -2, 0);
  location_t bottom_right = update_location(location, PLAYER_WIDTH_METERS / 2, 0);
  return !location_occupied(world, bottom_left) &&
    !location_occupied(world, bottom_right);
}

void update(double dt, pixel_buffer_t* pixel_buffer, controller_t &controller) {
  if (!game_state.initialized) {
    initialize_game_state(game_state);
  }

  location_t player_location = game_state.player_location;

  clear_screen(pixel_buffer);

  // Move player
  meters dx = 0, dy = 0;
  if (controller.right_pressed) {
    dx += PLAYER_SPEED_METERS_PER_SECOND * dt;
  }
  if (controller.left_pressed) {
    dx -= PLAYER_SPEED_METERS_PER_SECOND * dt;
  }
  location_t new_location = update_location(player_location, dx, 0);
  if (valid_player_location(&(game_state.world), new_location)) {
    player_location = new_location;
  }
  if (controller.down_pressed) {
    dy += PLAYER_SPEED_METERS_PER_SECOND * dt;
  }
  if (controller.up_pressed) {
    dy -= PLAYER_SPEED_METERS_PER_SECOND * dt;
  }
  new_location = update_location(player_location, 0, dy);
  if (valid_player_location(&(game_state.world), new_location)) {
    player_location = new_location;
  }
  game_state.player_location = player_location;

  // Render tile map
  uint top_left_x = game_state.player_location.tile_x / TILES_PER_SCREEN_X * TILES_PER_SCREEN_X;
  uint top_left_y = game_state.player_location.tile_y / TILES_PER_SCREEN_Y * TILES_PER_SCREEN_Y;
  for (uint row=0; row < TILES_PER_SCREEN_Y; row++) {
    for (int col=0; col < TILES_PER_SCREEN_X; col++) {
      if (location_occupied(
          &(game_state.world),
          top_left_x + col,
          top_left_y + row
        )
      ) {
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
  screen_location_t screen_location = get_screen_location(player_location);
  draw_box(
    pixel_buffer,
    screen_location.x - PLAYER_WIDTH/2,
    screen_location.y - PLAYER_HEIGHT,
    PLAYER_WIDTH,
    PLAYER_HEIGHT,
    PLAYER_COLOR
  );
}
