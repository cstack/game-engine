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

const uint TILE_CHUNK_COUNT_X = 128;
const uint TILE_CHUNK_COUNT_Y = 128;

const uint KILOBYTE = 1024;
const uint MEGABYTE = 1024 * KILOBYTE;
const uint MEMORY_POOL_BYTES = 64 * MEGABYTE;

char memory_pool[MEMORY_POOL_BYTES] = {};
uint allocated_bytes = 0;

void assert(bool expression, const char* message) {
  if (!expression) {
    std::cout << message << std::endl;
    exit(1);
  }
}

void* allocate_memory(uint bytes) {
  assert(bytes <= MEMORY_POOL_BYTES - allocated_bytes, "Tried to allocate more than MEMORY_POOL_BYTES bytes");
  char* memory = memory_pool + allocated_bytes;
  allocated_bytes += bytes;
  return memory;
}

tile_chunk_t* make_tile_chunks() {
  uint num_tile_chunks = TILE_CHUNK_COUNT_X * TILE_CHUNK_COUNT_Y;
  tile_chunk_t* tile_chunks = (tile_chunk_t*) allocate_memory(num_tile_chunks * sizeof(tile_chunk_t));
  for (uint i = 0; i < num_tile_chunks; i++) {
    tile_chunks[i].tiles = (tile_t*) allocate_memory(TILE_CHUNK_SIZE * TILE_CHUNK_SIZE * sizeof(tile_t));
  }
  return tile_chunks;
}

tile_chunk_t* get_tile_chunk(world_t* world, uint tile_x, uint tile_y) {
  uint tile_chunk_x = tile_x >> TILE_CHUNK_LOWER_BITS;
  uint tile_chunk_y = tile_y >> TILE_CHUNK_LOWER_BITS;

  if (tile_chunk_x >= TILE_CHUNK_COUNT_X || tile_chunk_y >= TILE_CHUNK_COUNT_Y) {
    return nullptr;
  }
  
  return world->tile_chunks + tile_chunk_y*TILE_CHUNK_COUNT_X + tile_chunk_x;
}

void set_tile(world_t* world, uint tile_x, uint tile_y, tile_t value) {
  std::cout << "set_tile(" << tile_x << ", " << tile_y << ")" << std::endl;
  tile_chunk_t* tile_chunk = get_tile_chunk(world, tile_x, tile_y);

  // TODO: Lazily instantiate tiles
  assert(tile_chunk->tiles, "Tile chunk does not have any tiles allocated.");

  uint tile_x_relative_to_chunk = tile_x & TILE_CHUNK_LOWER_BITS_MASK;
  uint tile_y_relative_to_chunk = tile_y & TILE_CHUNK_LOWER_BITS_MASK;
  tile_chunk->tiles[tile_y_relative_to_chunk*TILE_CHUNK_SIZE + tile_x_relative_to_chunk] = value;
}

tile_t get_tile(world_t* world, uint tile_x, uint tile_y) {
  tile_chunk_t* tile_chunk = get_tile_chunk(world, tile_x, tile_y);

  if (!tile_chunk || !tile_chunk->tiles) {
    return 0; // We haven't defined anything at this location
  }

  uint tile_x_relative_to_chunk = tile_x & TILE_CHUNK_LOWER_BITS_MASK;
  uint tile_y_relative_to_chunk = tile_y & TILE_CHUNK_LOWER_BITS_MASK;
  tile_t tile = tile_chunk->tiles[tile_y_relative_to_chunk*TILE_CHUNK_SIZE + tile_x_relative_to_chunk];

  return tile;
}

void create_room(
  world_t* world,
  uint room_index_x,
  uint room_index_y,
  bool door_left,
  bool door_right,
  bool door_top,
  bool door_bottom,
  uint pattern) {
  uint top_left_y = room_index_y * TILES_PER_SCREEN_Y;
  uint top_left_x = room_index_x * TILES_PER_SCREEN_X;

  // Top wall
  uint vertical_door_x = TILES_PER_SCREEN_X / 2;
  for (int i = 0; i < TILES_PER_SCREEN_X; i++) {
    if (door_top && (i == vertical_door_x || i == vertical_door_x - 1)) {
      // Leave a door
    } else {
      set_tile(world, top_left_x + i, top_left_y, 1);
    }
  }

  // Bottom wall
  for (int i = 0; i < TILES_PER_SCREEN_X; i++) {
    if (door_bottom && (i == vertical_door_x || i == vertical_door_x - 1)) {
      // Leave a door
    } else {
      set_tile(world, top_left_x + i, top_left_y + TILES_PER_SCREEN_Y - 1, 1);
    }
  }

  // Left wall
  uint horizontal_door_y = TILES_PER_SCREEN_Y / 2;
  for (int i = 0; i < TILES_PER_SCREEN_Y; i++) {
    if (door_left && i == horizontal_door_y) {
      // Leave a door
    } else {
      set_tile(world, top_left_x, top_left_y + i, 1);
    }
  }

  // Right wall
  for (int i = 0; i < TILES_PER_SCREEN_Y; i++) {
    if (door_right && i == horizontal_door_y) {
      // Leave a door
    } else {
      set_tile(world, top_left_x + TILES_PER_SCREEN_X - 1, top_left_y + i, 1);
    }
  }

  // A pattern in the center
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
      uint hash = pattern + i + 10 * j;
      if (hash % 3 == 0 || hash % 4 == 0 || hash % 5 == 0) {
        set_tile(world, top_left_x + vertical_door_x - 1 + i, top_left_y + horizontal_door_y - 1 + j, 1);
      }
    }
  }
}

void fill_world_with_rooms(world_t* world) {
  uint room_count_y = 5;
  uint room_count_x = 5;

  for (uint row = 0; row < room_count_x; row++) {
    for (uint col = 0; col < room_count_y; col++) {
      bool has_top_door = row > 0;
      bool has_bottom_door = row < room_count_y - 1;
      bool has_left_door = col > 0;
      bool has_right_door = col < room_count_x - 1;
      uint pattern = row * 3 + col * 7;

      create_room(world, col, row, has_left_door, has_right_door, has_top_door, has_bottom_door, pattern);
    }
  }
}

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

  game_state.world.tile_chunks = make_tile_chunks();
  fill_world_with_rooms(&game_state.world);

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
