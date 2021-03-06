#pragma once
#include <stdint.h>
#include "OAM.h"
#include "VRAM.h"
#include "Constants.h"

//-----------------------------------------------------------------------------

struct PPU {
  struct Out {
    Bus ppu_to_bus;
    Bus ppu_to_vram;
    Bus ppu_to_oam;

    int x;
    int y;
    int counter;
    uint8_t pix_out;
    bool pix_oe;

    bool stat1;
    bool stat2;
    bool vblank1;
    bool vblank2;
  };

  void reset(bool run_bootrom);
  Out  tick(const int tcycle_) const;
  void tock(const int tcycle_, const Bus bus_to_ppu_, const Bus vram_to_ppu_, const Bus oam_to_ppu_);
  void dump(std::string& out) const;

  uint8_t get_stat()       const { return stat; }

  void dump_tiles(uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* vram) const;
  void draw_sprite(OAM& oam, uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* vram, int sprite_index) const;
  void draw_map(uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* map, const uint8_t* tiles, uint8_t map_flip) const;
  void draw_bg_map(uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* vram) const;
  void draw_wm_map(uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* vram) const;

  bool get_old_stat_int1() const { return old_stat_int2; }
  bool get_old_stat_int2() const { return old_stat_int2; }

private:

  void tock_lcdoff(const int tcycle_, const Bus bus_to_ppu_, const Bus vram_to_ppu_, const Bus oam_to_ppu_);
  void emit_pixel(int tphase);
  void merge_tile(int tphase);

  //----------
  // Buses

  int tcycle;
  Bus bus_to_ppu;
  Bus vram_to_ppu;
  Bus oam_to_ppu;

  Bus ppu_to_bus;
  Bus ppu_to_vram;
  Bus ppu_to_oam;

  //----------
  // Timers and states

  int  frame_count;
  bool frame_start;
  bool frame_done;

  uint8_t line;
  uint8_t line_delay1;
  uint8_t line_delay2;
  uint8_t line_delay3;

  int state;

  uint16_t counter;
  uint16_t counter_delay1;
  uint16_t counter_delay2;
  uint16_t counter_delay3;

  uint8_t hblank_delay2;

  //----------
  // Registers

  uint8_t lcdc; // FF40
  uint8_t stat; // FF41
  uint8_t scy;  // FF42
  uint8_t scx;  // FF43
  uint8_t ly;   // FF44
  uint8_t lyc;  // FF45
  uint8_t dma;  // FF46
  uint8_t bgp;  // FF47
  uint8_t obp0; // FF48
  uint8_t obp1; // FF49
  uint8_t wy;   // FF4A
  uint8_t wx;   // FF4B

  uint8_t palettes[4];

  //----------
  // interrupt stuff

  int compare_line;

  uint8_t stat_int1;
  uint8_t stat_int2;

  bool old_stat_int1;
  bool old_stat_int2;

  uint8_t scx_latch;
  uint8_t win_y_latch;
  uint8_t win_y_counter;

  //----------
  // Sprites

  uint8_t sprite_count;
  int8_t sprite_index;
  int sprite_hit;

  uint8_t spriteY;
  uint8_t spriteX;
  uint8_t spriteP;
  uint8_t spriteF;

  uint8_t sprite_lo;
  uint8_t sprite_hi;

  uint8_t sprite_x[10]; // 80 bits
  uint8_t sprite_y[10]; // 80 bits
  uint8_t sprite_i[10]; // 60 bits?

  //----------
  // Vram Fetcher

  enum FetchType {
    FETCH_NONE,
    FETCH_BACKGROUND,
    FETCH_WINDOW,
    FETCH_SPRITE,
  };

  enum FetchState {
    FETCH_IDLE,
    
    FETCH_TILE_MAP,
    FETCH_TILE_LO,
    FETCH_TILE_HI,

    FETCH_SPRITE_MAP,
    FETCH_SPRITE_LO,
    FETCH_SPRITE_HI,
  };

  FetchType fetch_type;
  FetchState fetch_state;
  bool fetch_delay;

  bool in_window_old;
  bool in_window_new;
  bool in_window_early;
  bool in_window_late;

  bool win_retrig_old;
  bool win_retrig_new;

  //----------
  // Pixel pipe

  uint8_t map_x;

  uint8_t tile_map;
  uint8_t tile_lo;
  uint8_t tile_hi;
  bool    tile_latched;

  uint8_t pix_count;
  uint8_t pix_discard_scx;
  uint8_t pix_discard_pad;
  uint8_t pipe_count;

  uint8_t bg_pix_lo;
  uint8_t bg_pix_hi;
  uint8_t bg_pal_lo;
  uint8_t bg_pal_hi;

  uint8_t ob_pix_lo;
  uint8_t ob_pix_hi;
  uint8_t ob_pal_lo;
  uint8_t ob_pal_hi;

  uint8_t pix_out;
  bool pix_oe;

  bool stat1;
  bool stat2;
  bool vblank1;
  bool vblank2;
};

//-----------------------------------------------------------------------------