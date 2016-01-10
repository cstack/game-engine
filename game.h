#ifndef GAME_H
#define GAME_H

#include <iostream>

const uint SCREEN_WIDTH = 1024;
const uint SCREEN_HEIGHT = 576;

const uint TILE_MAP_WIDTH = 16;
const uint TILE_MAP_HEIGHT = 9;
const uint TILE_WIDTH = SCREEN_WIDTH / TILE_MAP_WIDTH;
const uint TILE_HEIGHT = SCREEN_HEIGHT / TILE_MAP_HEIGHT;
const color_t TILE_COLOR = rgb(100, 100, 100);

typedef bool tile_t;

struct tile_map_t {
  tile_t* tiles;
};

struct world_t {
  tile_map_t* tile_maps;
  uint width;
  uint height;
};

struct game_state_t {
  game_state_t() : initialized(false) {}
  bool initialized;

  // Which tile map is player on?
  uint tile_map_x;
  uint tile_map_y;

  // Where on current tile map is player?
  double player_x;
  double player_y;
};

#endif
