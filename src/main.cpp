#include "main.h"

#include "Audio.h"
#include "MetroBoy.h"
#include "Assembler.h"

#include "test_codegen.h"
#include "test_micro.h"
#include "test_mooneye.h"
#include "test_wpol.h"
#include "test_screenshot.h"

#ifdef _MSC_VER
#include <include/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#pragma warning(disable : 4996)
#pragma warning(disable : 4702)
#pragma warning(disable : 4100)

extern const uint32_t gb_colors[];
extern uint8_t rom_buf[];

void run_test(const std::string& prefix, const std::string& name);

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  printf("Hello Metroboy\n");

  MetroBoyApp* app = new MetroBoyApp();
  int ret = app->main_(argc, argv);
  delete app;

  return ret;
}

//-----------------------------------------------------------------------------

int MetroBoyApp::main_(int /*argc*/, char** /*argv*/) {
  //run_microtests();
  //run_screenshot_tests();
  //run_mooneye_acceptance();
  //run_wpol_acceptance();
  //run_mealybug_tests();
  //return 0;

  //---------

  //load("oh"); // broken eye
  //load("pocket");
  //load("gejmboj");
  //load("LinksAwakening");
  //load("Prehistorik Man (U)");
  //load("SML");
  //load("tetris");

  //load("cpu_instrs");
  //load("instr_timing");

  //load("microtests/build/dmg", "poweron_000_div");

  load("microtests/build/dmg", "minimal");

  runmode = STEP_CYCLE;
  //runmode = RUN_FAST;
  //runmode = RUN_VSYNC;

  //----------

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  window = SDL_CreateWindow("MetroBoy Gameboy Simulator", 100, 100, fb_width, fb_height, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  fb_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, fb_width, fb_height);
  terminus_surface = SDL_LoadBMP("terminus2.bmp");
  terminus_font = (uint8_t*)terminus_surface->pixels;
  keyboard_state = SDL_GetKeyboardState(nullptr);
  freq = SDL_GetPerformanceFrequency();

  audio_init();

  background = new uint32_t[fb_width * fb_height];

  for (int y = 0; y < fb_height; y++) {
    for (int x = 0; x < fb_width; x++) {
      int c = ((x ^ y) & 0x20) ? 0x10101010 : 0x15151515;
      background[x + y * fb_width] = c;
    }
  }

  while (!quit) loop();

  audio_stop();

  SDL_Quit();
  return 0;
}

//-----------------------------------------------------------------------------

void MetroBoyApp::loop() {
  frame_begin = SDL_GetPerformanceCounter();

  //----------------------------------------
  // Process events

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) switch (event.key.keysym.sym) {
    case SDLK_f:      runmode = RUN_FAST; break;
    case SDLK_v:      runmode = RUN_VSYNC; break;
    case SDLK_s:      runmode = STEP_FRAME; break;
    case SDLK_o:      overlay_mode = (overlay_mode + 1) % 3; break;
    case SDLK_RIGHT:  {
      if (keyboard_state[SDL_SCANCODE_LCTRL]) {
        step_forward += 10;
      } else {
        step_forward++;
      }
      break;
    }
    case SDLK_LEFT:   {
      if (keyboard_state[SDL_SCANCODE_LCTRL]) {
        step_backward += 10; 
      } else {
        step_backward++; 
      }
      break;
    }
    case SDLK_UP:     step_up = true; break;
    case SDLK_DOWN:   step_down = true; break;

    case SDLK_r:      reset = true; break;
    case SDLK_F1:     load_dump = true; break;
    case SDLK_F4:     save_dump = true; break;
    case SDLK_ESCAPE: quit = true; break;
    }

    if (event.type == SDL_QUIT) quit = true;

    if (event.type == SDL_DROPFILE) {
      metroboy.load_rom(event.drop.file, false);
      rom_loaded = true;
      runmode = RUN_VSYNC;
      SDL_free(event.drop.file);
    }
  }

  //----------------------------------------
  // Button input
  int64_t cycles_begin = metroboy.total_tcycles();

  buttons = 0;
  if (keyboard_state[SDL_SCANCODE_RIGHT])  buttons |= 0x01;
  if (keyboard_state[SDL_SCANCODE_LEFT])   buttons |= 0x02;
  if (keyboard_state[SDL_SCANCODE_UP])     buttons |= 0x04;
  if (keyboard_state[SDL_SCANCODE_DOWN])   buttons |= 0x08;
  if (keyboard_state[SDL_SCANCODE_X])      buttons |= 0x10;
  if (keyboard_state[SDL_SCANCODE_Z])      buttons |= 0x20;
  if (keyboard_state[SDL_SCANCODE_RSHIFT]) buttons |= 0x40;
  if (keyboard_state[SDL_SCANCODE_RETURN]) buttons |= 0x80;

  // probably too fast for debugging
  //if (keyboard_state[SDL_SCANCODE_RIGHT])  step_forward++;
  //if (keyboard_state[SDL_SCANCODE_LEFT])   step_backward++;

  //----------------------------------------
  // Handle sim controls

  if (reset) {
    metroboy.reset(0x0100);
    reset = false;
  }

  if (load_dump) {
    metroboy.load_dump();
    load_dump = false;
    rom_loaded = true;
  }

  if (save_dump) {
    metroboy.save_dump();
    save_dump = false;
  }

  if (step_up) {
    if (runmode == STEP_CYCLE) runmode = STEP_LINE;
    else if (runmode == STEP_LINE) runmode = STEP_FRAME;
  }

  if (step_down) {
    if (runmode == STEP_FRAME) runmode = STEP_LINE;
    else if (runmode == STEP_LINE) runmode = STEP_CYCLE;
  }

  //----------------------------------------
  // Run simulation


  if (runmode == RUN_FAST) {
    fast_cycles += (16.0 - 1000 * (double(frame_time) / double(freq))) * 100;
    metroboy.run_fast(buttons, (int)fast_cycles);
    //metroboy.run_fast(buttons, 100000);
  }
  else if (runmode == RUN_VSYNC) {
    //printf("%d --------\n", frame_count);
    metroboy.run_vsync(buttons);
  }
  else if (runmode == STEP_CYCLE) {
    while (step_forward--) {
      if (keyboard_state[SDL_SCANCODE_LSHIFT]) {
        metroboy.step_over();
      }
      else {
        metroboy.step_cycle();
      }
    }
    while (step_backward--) {
      metroboy.unstep_cycle();
    }
  }
  else if (runmode == STEP_FRAME) {
    while (step_forward--)  metroboy.step_frame();
    while (step_backward--) metroboy.unstep_frame();
  }
  else if (runmode == STEP_LINE) {
    while (step_forward--)  metroboy.step_line();
    while (step_backward--) metroboy.unstep_line();
  }

  step_forward = 0;
  step_backward = 0;
  step_up = false;
  step_down = false;

  int64_t cycles_end = metroboy.total_tcycles();
  //----------------------------------------
  // Clear screen

  SDL_LockTexture(fb_tex, NULL, (void**)(&framebuffer), &pitch);
  memcpy(framebuffer, background, fb_width * fb_height * 4);
  tp.begin_frame(framebuffer, fb_width, fb_height);

  //----------------------------------------
  // Left column text

  Gameboy& gameboy = metroboy.gb();
  Framebuffer& fb = metroboy.fb();

  int spacing = 192 + 32;

  gameboy.dump1(text_buf);
  //gameboy.get_oam().dump(text_buf);
  tp.render_text(spacing * 0 + 4, 4, text_buf.c_str());
  text_buf.clear();

  sprintf(text_buf, "\003--------------PPU--------------\001\n");
  
  gameboy.get_ppu().dump(text_buf);

  tp.render_text(spacing * 1 + 4, 4, text_buf.c_str());
  text_buf.clear();

  gameboy.dump3(text_buf);
  tp.render_text(spacing * 2 + 4, 4, text_buf.c_str());
  text_buf.clear();

  sprintf(text_buf, "\003--------------SPU--------------\001\n");
  gameboy.get_spu().dump(text_buf);
  tp.render_text(spacing * 2 + 4, 640 + 4, text_buf.c_str());
  text_buf.clear();

  gameboy.dump4(text_buf);
  tp.render_text(spacing * 3 + 4, 4, text_buf.c_str());
  text_buf.clear();

  //gameboy.get_spu().dump(text_buf);
  //render_text(280, 4, text_buf.c_str());
  //text_buf.clear();

  //----------------------------------------
  // Wave thingy

  /*
  for (int i = 0; i < 16; i++) {
    uint8_t a = (gameboy.get_spu().get_wave()[i] & 0x0F) >> 0;
    uint8_t b = (gameboy.get_spu().get_wave()[i] & 0xF0) >> 4;
    uint32_t color = 0xFFFFFFFF;
  
    framebuffer[(512 + 2 * i + 0) + (100 + b) * fb_width] = color;
    framebuffer[(512 + 2 * i + 1) + (100 + a) * fb_width] = color;
  }
  */

  //----------------------------------------
  // Gameboy screen

  //const int gb_screenx = 32 * 27 - 16;
  const int gb_screenx = fb_width - 288 - 288 - 288;
  const int gb_screeny = 32 * 10;

  if (overlay_mode == 0 || overlay_mode == 1) {
    for (int y = 0; y < 144; y++) {
      uint32_t* line1 = &framebuffer[(y * 2 + gb_screeny + 0) * fb_width + gb_screenx];
      uint32_t* lineM2 = &framebuffer[(y * 2 + gb_screeny + 1) * fb_width + gb_screenx];
      for (int x = 0; x < 160; x++) {
        uint32_t c = gb_colors[fb.buf[x + (y * 160)] & 7];
        *line1++ = c; *line1++ = c;
        *lineM2++ = c; *lineM2++ = c;
      }
    }
  }

  tp.draw_bbox(gb_screenx - 2, gb_screeny - 2, 320 + 3, 288 + 3, 0x505050);
  tp.draw_bbox(gb_screenx - 1, gb_screeny - 1, 320+1, 288+1, 0x101010);

  //----------------------------------------
  // Reference image

  if (overlay_mode == 2) {
    for (int y = 0; y < 144; y++) {
      uint32_t* line1 = &framebuffer[(y * 2 + gb_screeny + 0) * fb_width + gb_screenx];
      uint32_t* lineM2 = &framebuffer[(y * 2 + gb_screeny + 1) * fb_width + gb_screenx];
      for (int x = 0; x < 160; x++) {
        uint32_t c = gb_colors[golden[x + y * 160]];
        c += 0x100000;
        *line1++ = c; *line1++ = c;
        *lineM2++ = c; *lineM2++ = c;
      }
    }
  }

  //----------------------------------------
  // Diff overlay

  if (overlay_mode == 1) {
    for (int y = 0; y < 144; y++) {
      uint32_t* line1 = &framebuffer[(y * 2 + gb_screeny + 0) * fb_width + gb_screenx];
      uint32_t* lineM2 = &framebuffer[(y * 2 + gb_screeny + 1) * fb_width + gb_screenx];
      for (int x = 0; x < 160; x++) {
        int c = fb.buf[x + (y * 160)];
        if (c != golden[x + y * 160]) {
          *line1++ += 0x808000;
          *line1++ += 0x808000;
          *lineM2++ += 0x808000;
          *lineM2++ += 0x808000;
        }
        else {
          line1++;
          line1++;
          lineM2++;
          lineM2++;
        }
      }
    }
  }

  //----------------------------------------
  // Step highlight

  if (runmode == STEP_LINE || runmode == STEP_CYCLE) {

    int px = metroboy.gb_out.x;
    int py = metroboy.gb_out.y;

    for (int x = 0; x < 320; x++) {
      int color = (px == (x / 2)) ? 0x00606000 : 0x00600000;
      framebuffer[(gb_screenx + x) + (gb_screeny + py * 2 + 0) * fb_width] += color;
      framebuffer[(gb_screenx + x) + (gb_screeny + py * 2 + 1) * fb_width] += color;
    }
  }

  //----------------------------------------
  // Stat bar

  const char* mode_names[] = {
    "RUN_FAST",
    "RUN_VSYNC",
    "STEP_FRAME",
    "STEP_LINE",
    "STEP_CYCLE",
  };

  sprintf(text_buf, "%s %d", mode_names[runmode], (int)(metroboy.gb().get_tcycle() & 3));
  tp.render_text(gb_screenx, 32 * 20, text_buf.c_str());
  text_buf.clear();

  //----------------------------------------
  // VRAM dump

  gameboy.get_ppu().dump_tiles(framebuffer,  fb_width, fb_width - 288,             32,  2, gameboy.get_vram());
  gameboy.get_ppu().draw_bg_map(framebuffer, fb_width, fb_width - 288 - 288 - 288, 32,  1, gameboy.get_vram());
  gameboy.get_ppu().draw_wm_map(framebuffer, fb_width, fb_width - 288 - 288,       32, 1, gameboy.get_vram());

  //----------------------------------------
  // Trace buffer

  int trace_sx = gb_screenx;
  int trace_sy = 32 * 22;

  for (int y = 0; y < 154; y++) {
    for (int x = 0; x < 456; x++) {
      framebuffer[(trace_sx + x) + (trace_sy + y) * fb_width] = metroboy.get_trace()[x + y * 456];
    }
  }

  //----------------------------------------
  // Perf timer

  static double smoothed_frame_time = 0;
  smoothed_frame_time *= 0.98;
  smoothed_frame_time += (1000.0 * double(frame_time) / double(freq)) * 0.02;

  {
    char source_buf[1024];
    snprintf(source_buf, 1024, "frame time %2.2f msec, %6d cyc/frame\n", (double)smoothed_frame_time, (int)(cycles_end - cycles_begin) / 4);
    text_buf.append(source_buf);
  }
  
  tp.render_text(fb_width - 256, fb_height - 12, text_buf.c_str());
  text_buf.clear();

  //----------------------------------------
  // Console

  //render_console(256 - 32, fb_height - glyph_height * console_height - 4, terminus_font);

  //----------------------------------------
  // Swap

  SDL_UnlockTexture(fb_tex);
  framebuffer = nullptr;
  SDL_RenderCopy(renderer, fb_tex, NULL, NULL);

  frame_end = SDL_GetPerformanceCounter();
  frame_time = frame_end - frame_begin;

  SDL_RenderPresent(renderer);
  frame_count++;
}

//-----------------------------------------------------------------------------

void MetroBoyApp::load(const std::string& prefix, const std::string& name) {
  std::string gb_filename = prefix + "/" + name + ".gb";
  std::string golden_filename = prefix + "/" + name + ".bmp";
  SDL_Surface* golden_surface = SDL_LoadBMP(golden_filename.c_str());

  if (!golden_surface) {
    overlay_mode = 0;
    memset(golden, 0, 160 * 144);
  }

  if (golden_surface && golden_surface->format->format == SDL_PIXELFORMAT_INDEX8) {
    printf("Loaded i8 golden\n");
    uint8_t* src = (uint8_t*)golden_surface->pixels;
    uint32_t* pal = (uint32_t*)golden_surface->format->palette->colors;
    for (int y = 0; y < 144; y++) {
      for (int x = 0; x < 160; x++) {
        uint8_t a = pal[src[x + y * 160]] & 0xFF;

        if (a < 40) a = 3;
        else if (a < 128) a = 2;
        else if (a < 210) a = 1;
        else a = 0;

        golden[x + y * 160] = a;
      }
    }
    overlay_mode = 1;
  }

  else if (golden_surface && golden_surface->format->format == SDL_PIXELFORMAT_BGR24) {
    printf("Loaded argb golden\n");
    uint8_t* src = (uint8_t*)golden_surface->pixels;
    for (int y = 0; y < 144; y++) {
      for (int x = 0; x < 160; x++) {
        uint8_t a = src[x * 3 + y * golden_surface->pitch];

        if (a < 40) a = 3;
        else if (a < 128) a = 2;
        else if (a < 210) a = 1;
        else a = 0;

        golden[x + y * 160] = a;
      }
    }
    overlay_mode = 1;
  }

  printf("Loading rom %s\n", gb_filename.c_str());
  memset(rom_buf, 0, 1024 * 1024);
  metroboy.load_rom(gb_filename.c_str(), false);
  rom_loaded = true;
  runmode = RUN_VSYNC;
}

//-----------------------------------------------------------------------------

#if 0
void MetroBoyApp::render_text(int dst_x, int dst_y, const char* text) {
  int xcursor = 0;
  int ycursor = 0;

  uint32_t color = 0xFFCCCCCC;

  for (const char* c = text; *c; c++) {
    switch(*c) {
    case 1: color = 0xFFCCCCCC; continue;
    case 2: color = 0xFFFF8888; continue;
    case 3: color = 0xFF88FF88; continue;
    case 4: color = 0xFF8888FF; continue;
    case 5: color = 0xFF88FFFF; continue;
    case 6: color = 0xFFFF88FF; continue;
    case 7: color = 0xFFFFFF88; continue;
    case '\n':
      xcursor = 0;
      ycursor += glyph_height;
      continue;
    default:
      break;
    }

    int row = ((*c) >> 5) * 16 + 3;
    int col = ((*c) & 31) * 8;
    int src_cursor = col + (row * glyph_stride);
    int dst_cursor = (xcursor + dst_x) + (ycursor + dst_y) * fb_width;

    for (int y = 0; y < glyph_height; y++) {
      for (int x = 0; x < glyph_width; x++) {
        if (terminus_font[src_cursor]) framebuffer[dst_cursor] = color;
        src_cursor++;
        dst_cursor++;
      }
      src_cursor += (glyph_stride - glyph_width);
      dst_cursor += (fb_width - glyph_width);
    }

    xcursor += glyph_width;
    if (xcursor > (fb_width - glyph_width)) {
      xcursor = 0;
      ycursor += glyph_height;
    }
  }
}
#endif

//-----------------------------------------------------------------------------

void MetroBoyApp::printf_console(const char* format, ...) {

  char buffer[256];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  char* text = buffer;
  while (*text) {
    char c = *text;
    if (c == '\n') {
      for (int i = cursor_x; i < console_width; i++) {
        console_buf[i + cursor_y * console_width] = 0;
      }
      cursor_x = 0;
      cursor_y = (cursor_y + 1) % console_height;
    }
    else {
      console_buf[cursor_x + cursor_y * console_width] = c;
      cursor_x++;
      if (cursor_x == console_width) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y == console_height) {
          cursor_y = 0;
        }
      }
    }
    text++;
  }
}

//-----------------------------------------------------------------------------
