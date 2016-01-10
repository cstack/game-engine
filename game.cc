#include "game.h"

#include "platform/platform.h"
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
  game_state.player_location.x = TILE_WIDTH*2;
  game_state.player_location.y = TILE_HEIGHT*5;

  game_state.player_location.tile_map_x = 0;
  game_state.player_location.tile_map_y = 0;

  game_state.initialized = true;
}

void clear_screen(pixel_buffer_t* pixel_buffer) {
  draw_box(pixel_buffer, 0, 0, pixel_buffer->width, pixel_buffer->height, BLACK);
}

normalized_location_t normalize_location(location_t location) {
  normalized_location_t normalized_location;
  while (location.x < 0) {
    location.x += TILE_MAP_WIDTH * TILE_WIDTH;
    location.tile_map_x -= 1;
  }
  while (location.x > TILE_MAP_WIDTH * TILE_WIDTH) {
    location.x -= TILE_MAP_WIDTH * TILE_WIDTH;
    location.tile_map_x += 1;
  }
  normalized_location.x = location.x;
  normalized_location.tile_map_x = location.tile_map_x;

  while (location.y < 0) {
    location.y += TILE_MAP_HEIGHT * TILE_HEIGHT;
    location.tile_map_y -= 1;
  }
  while (location.y > TILE_MAP_HEIGHT * TILE_HEIGHT) {
    location.y -= TILE_MAP_HEIGHT * TILE_HEIGHT;
    location.tile_map_y += 1;
  }
  normalized_location.y = location.y;
  normalized_location.tile_map_y = location.tile_map_y;

  return normalized_location;
}

tile_map_t* get_tile_map(world_t* world, normalized_location_t location) {
  return world->tile_maps + (location.tile_map_y*WORLD_WIDTH + location.tile_map_x);
}

tile_t get_tile(tile_map_t* tile_map, normalized_location_t location) {
  uint row = location.y / TILE_HEIGHT;
  uint col = location.x / TILE_WIDTH;
  std::cout << "Getting tile in location:" << std::endl;
  std::cout << "x " << location.tile_map_x << "->" << location.x << "row (" << row << ")" << std::endl;
  std::cout << "y " << location.tile_map_y << "->" << location.y << "col (" << col << ")" << std::endl;
  return tile_map->tiles[row*TILE_MAP_WIDTH + col];
}

bool location_occupied(world_t* world, location_t location) {
  normalized_location_t normalized_location = normalize_location(location);
  tile_map_t* tile_map = get_tile_map(world, normalized_location);
  return get_tile(tile_map, normalized_location);
}

bool valid_player_location(world_t* world, location_t location) {
  location_t bottom_left = location;
  bottom_left.x -= PLAYER_WIDTH/2;
  location_t bottom_right = location;
  bottom_right.x += PLAYER_WIDTH/2;
  return !location_occupied(world, bottom_left) &&
    !location_occupied(world, bottom_right);
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
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
  };
  tile_t tiles_10[TILE_MAP_HEIGHT][TILE_MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  };
  tile_t tiles_11[TILE_MAP_HEIGHT][TILE_MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  };

  tile_map_t tile_maps[WORLD_HEIGHT][WORLD_WIDTH];
  tile_maps[0][0].tiles = (tile_t*) tiles_00;
  tile_maps[0][1].tiles = (tile_t*) tiles_01;
  tile_maps[1][0].tiles = (tile_t*) tiles_10;
  tile_maps[1][1].tiles = (tile_t*) tiles_11;

  world_t world;
  world.width = WORLD_WIDTH;
  world.height = WORLD_HEIGHT;
  world.tile_maps = (tile_map_t*) tile_maps;

  normalized_location_t player_location = normalize_location(game_state.player_location);
  tile_map_t* tile_map = get_tile_map(&world, player_location);

  clear_screen(pixel_buffer);

  // Move player
  location_t new_location = game_state.player_location;
  if (controller.right_pressed) {
    new_location.x += PLAYER_SPEED * dt;
  }
  if (controller.left_pressed) {
    new_location.x -= PLAYER_SPEED * dt;
  }
  if (valid_player_location(&world, new_location)) {
    game_state.player_location = new_location;
  }

  new_location = game_state.player_location;
  if (controller.down_pressed) {
    new_location.y += PLAYER_SPEED * dt;
  }
  if (controller.up_pressed) {
    new_location.y -= PLAYER_SPEED * dt;
  }
  if (valid_player_location(&world, new_location)) {
    game_state.player_location = new_location;
  }

  normalized_location_t normalized_player_location = normalize_location(game_state.player_location);
  game_state.player_location.tile_map_x = normalized_player_location.tile_map_x;
  game_state.player_location.tile_map_y = normalized_player_location.tile_map_y;
  game_state.player_location.x = normalized_player_location.x;
  game_state.player_location.y = normalized_player_location.y;

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
    game_state.player_location.x - PLAYER_WIDTH/2,
    game_state.player_location.y - PLAYER_HEIGHT,
    PLAYER_WIDTH,
    PLAYER_HEIGHT,
    PLAYER_COLOR
  );
}
