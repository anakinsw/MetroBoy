#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <string>
#include <vector>

#include "Timer.h"
#include "TraceViewer.h"
#include "../src/TextPainter.h"
#include "pages/Gameboy.h"

#ifdef _MSC_VER
#include <include/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#if 0
void blah() {
  //----------
  // center right, generating the external read/write signals to the cart

  bool A13=0,A14=0,A15=0;
  bool FROM_CPU3=0,FROM_CPU4=0,FROM_CPU5=0;
  bool CPU_RAW_RD=0;
  //bool T1T2n=0,T1nT2=0;
  bool CPU_WR_SYNC=0;
  bool LUMA=0; // this is the "dma reading" signal


  if (LUMA) {
    bool WR_A = 0;
    bool WR_D = 0;
    bool RD_A = 1;
    bool RD_D = 1;
  }
  else {
    /*
    bool NOT_VRAM = 1;

    bool WR_A = and(CPU_WR_SYNC, FROM_CPU4, NOT_VRAM);
    bool WR_D = and(CPU_WR_SYNC, FROM_CPU4, NOT_VRAM);

    wire LAGU = unk3(CPU_RAW_RD, and(FROM_CPU4, NOT_VRAM), FROM_CPU3);
    
    bool RD_A = nand(LAGU, FROM_CPU4, NOT_VRAM);
    bool RD_D = nand(LAGU, FROM_CPU4, NOT_VRAM);
    */

    // LAVO drives the external data pins onto the data bus
    wire LAVO = nand(CPU_RAW_RD, and(FROM_CPU4, or(A13, A14, not(A15))), FROM_CPU5);


    // TAGY drives DIV onto the data bus
    //c.TAGY = and(FF04, not(CPU_RAW_RD));

    /*

    CPU_RD = not(CPU_RAW_RD);

    SORA = and(CPU_RD, FF07)
    if (SORA) dbus = TAC
    */


    /*
    // this doesn't seem right... polarity wrong
    ASOP = not(and(CPU_RD, FF10))
    if (ASOP) dbus = NR10
    */

    /*
    // something in the path to SEBY has to be wrong

    SOTO_Q = SOTO.flip(SYCY, in.RESET6);
    TUTO = 0;

    TEXO = and(in.FROM_CPU4, or(in.A13, in.A14, not(in.A15)));

    ABUZ = unk3(CLK_ABCD_Q, CLK_ABGH, in.FROM_CPU4);

    TUCA = and(A15, FEXXFFXXn, not(TEXO), ABUZ);

    BYHA = unk3(ANEL_Q, not(CATU), not(in.RESET_VIDEO2n));
    ANOM = nor(RESET_VIDEO2n, not(BYHA));

    AVAP = nor(DOBA_Q, not(ANOM), !BYBA_Q);

    WEGO = or(not(in.RESET_VIDEO), VOGA_Q);
    XYMU = or(WEGO, AVAP);


    SERE = and(mux2(0, not(in.MCS_IN), TUCA), not(XYMU));

    wire SEBY = and(SERE, not(CPU_RAW_RD), FROM_CPU5);
    if (SEBY) dbus <= vdbus;
    */
  }

}
#endif

//-----------------------------------------------------------------------------

template<typename ... Args>
void sprintf(std::string& out, const char* format, Args ... args)
{
  char source_buf[1024];
  snprintf(source_buf, 1024, format, args ...);
  out.append(source_buf);
}

//-----------------------------------------------------------------------------

int render_labels(TextPainter& tp, int x, int y, const std::vector<SignalData>& signals, int depth) {
  int cx = x;
  int cy = y;

  for (const SignalData& s : signals) {

    if (!s.children.empty()) {
      tp.render_text(cx + 6 * depth, cy, s.name);
      cy += 12;
      cy += render_labels(tp, cx, cy, s.children, depth + 1);
    }
    else {
      tp.render_text(cx + 6 * depth, cy, s.name);
      if (s.bit_width == 0) {
        cy += 12;
      }
      else {
        cy += s.bit_width * 12;
      }
    }

  }

  return cy - y;
}

//-----------------------------------------------------------------------------

void dump(void* blob, int size) {
  uint8_t* src = (uint8_t*)blob;
  for (int i = 0; i < size; i++) {
    printf("%c", src[i] ? '+' : '-');
  }
  printf("\n");
}

//-----------------------------------------------------------------------------

void step_forwards(void* blobA, void* blobB) {
  //----------
  // old state

  Gameboy a = *(Gameboy*)blobA;

  //----------
  // old state + new inputs

  Gameboy b = a;
  b.timestamp = a.timestamp + 1;

  //b.cpu.CPU_RD = false;
  //b.cpu.CPU_WR = false;
  b.cpu.FROM_CPU3 = false;
  b.cpu.FROM_CPU4 = false;
  b.cpu.FROM_CPU5 = false;


  b.chip.RESET   = b.timestamp < 20;
  b.chip.CLKIN_A = true;
  b.chip.CLKIN_B = b.timestamp & 1;
  b.T1nT2   = false;
  b.T1nT2n  = true;
  b.T1T2n   = false;

  //----------
  // haxxxxx

  if (b.timestamp == 30) {
    b.p03.SOPU_0 = true;
    b.p03.SAMY_1 = false;
    b.p03.SABO_2 = true;

    b.p03.SABU_0 = 1;
    b.p03.NYKE_1 = 1;
    b.p03.MURU_2 = 1;
    b.p03.TYVA_3 = 1;
  }

  //----------
  // unmerged signals

  //----------
  // destination state
  Gameboy c = b;

  for (int rep = 0; rep < 40; rep++) {
    P01_ClocksReset::tick(a, b, c);
    P02_Interrupts::tick(a, b, c);
    P03_Timer::tick(a, b, c);
    P04_DMA::tick(a, b, c);
    P05_JoypadIO::tick(a, b, c);
    P06_SerialLink::tick(a, b, c);
    P07_SysDecode::tick(a, b, c);

    if (memcmp(&b, &c, sizeof(Gameboy)) == 0) break;
    a = b;
    b = c;
  }

  *(Gameboy*)blobB = b;
}

const std::vector<SignalData> sample_signals =
{
  SignalData("RESET",   offsetof(Gameboy, chip.RESET)),
  SignalData("CLKIN_B", offsetof(Gameboy, chip.CLKIN_B)),

  SignalData("p01",  offsetof(Gameboy, p01), P01_ClocksReset::signals()),
  SignalData("p03",  offsetof(Gameboy, p03), P03_Timer::signals()),
};

//-----------------------------------------------------------------------------

#if 0
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  printf("Hello World Again\n");

  Sample sampleA = {};
  Sample sampleB = {};

  step_forwards(&sampleA, &sampleB);

  return 0;
}
#endif

//-----------------------------------------------------------------------------

#if 1
int main(int /*argc*/, char** /*argv*/) {
  const int fb_width = 1900;
  const int fb_height = 1000;
  const int gb_width = 160 * 2;
  const int gb_height = 144 * 2;

  SDL_Window* window = SDL_CreateWindow("MetroBoy Trace Debugger", 4, 35, fb_width, fb_height, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  SDL_Texture* fb_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, fb_width, fb_height);
  const uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);

  uint32_t* background = new uint32_t[fb_width * fb_height];
  for (int y = 0; y < fb_height; y++) {
    for (int x = 0; x < fb_width; x++) {
      int c = ((x ^ y) & 0x20) ? 0x10101010 : 0x15151515;
      background[x + y * fb_width] = c;
    }
  }

  TextPainter tp;

  //----------
  // Generate trace

  const int timer_count = 64 * 1024;
  Gameboy* samples = new Gameboy[timer_count];
  memset(samples, 0xCD, timer_count * sizeof(Gameboy));

  Gameboy reset_sample = {};
  reset_sample.timestamp = -1;

  uint64_t timeA = SDL_GetPerformanceCounter();


  step_forwards(&reset_sample, &samples[0]);

  for (int i = 1; i < timer_count; i++) {
    samples[i] = samples[i-1];
    step_forwards(&samples[i-1], &samples[i]);
  }

  uint64_t timeB = SDL_GetPerformanceCounter();

  printf("%f\n", double(timeB - timeA) / double(SDL_GetPerformanceFrequency()));

  //----------

  int64_t cycle_cursor = 0;
  uint64_t frame_begin, frame_end, frame_time = 0;
  uint64_t freq = SDL_GetPerformanceFrequency();

  //double center = 512.0;
  //int zoom = -8;
  double center = 8191;
  int zoom = -8;

  bool quit = false;
  while (!quit) {
    frame_begin = SDL_GetPerformanceCounter();

    int step_count = 0;
    int step_size = 1;

    //----------------------------------------
    // Process events

    if (keyboard_state[SDL_SCANCODE_LCTRL]) {
      step_size = 16;
    }

    double span = 4096.0 * pow(2.0, float(zoom) / 4);

    bool debug_cycle = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_RIGHT:  {
          int delta = int(span / 8.0f);
          if (delta == 0) delta = 1;
          center += delta;
          if (center > timer_count) center = timer_count;
          break;
        }
        case SDLK_LEFT:   {
          int delta = int(span / 8.0f);
          if (delta == 0) delta = 1;
          center -= delta;
          if (center < 0) center = 0;
          break;
        }
        case SDLK_UP:   {
          zoom--;
          if (zoom < -40) zoom = -40;
          break;
        }
        case SDLK_DOWN: {
          zoom++;
          if (zoom > 40) zoom = 40;
          break;
        }
        case SDLK_ESCAPE: {
          quit = true;
          break;
        }
        case SDLK_d: debug_cycle = true; break;
        }
      }
      if (event.type == SDL_QUIT) quit = true;
    }

    span = 4096.0 * pow(2.0, float(zoom) / 4);

    //----------------------------------------
    // Clear screen

    uint32_t* framebuffer = nullptr;
    int pitch = 0;
    SDL_LockTexture(fb_tex, NULL, (void**)(&framebuffer), &pitch);
    memcpy(framebuffer, background, fb_width * fb_height * 4);
    tp.begin_frame(framebuffer, fb_width, fb_height);

    //----------------------------------------
    // Run sim

    /*
    if (center > 0 && center < timer_count) {
      Sample sample = samples[int(center) - 1];
      step_forwards(&sample);
    }
    */

    if (debug_cycle) {
      int cursor = (int)floor(center);
      if (cursor > 0) {
        printf("debugging sample %d\n", cursor);
        Gameboy sampleA = samples[cursor-1];
        Gameboy sampleB = samples[cursor];

        Gameboy temp = {};
        step_forwards(&sampleA, &temp);
      }

      //Sample sampleA = 
    }

    //----------------------------------------
    // Render trace

    cycle_cursor += step_count;
    step_count = 0;

    TraceViewer tv;

    tv.framebuffer = framebuffer;
    tv.fb_width = fb_width;
    tv.fb_height = fb_height;

    tv.screen_x = 200;
    tv.screen_y = 16;

    render_labels(tp, 100, 16, sample_signals, 0);

    tv.render(samples, sizeof(Gameboy), timer_count, sample_signals, center, span);

    std::string temp;
    temp.clear();
    sprintf(temp, "Cycle %lld, center %f, zoom %d, span %f", cycle_cursor, center, zoom, span);
    tp.render_text(3,3, temp.c_str());

    //----------------------------------------
    // Render stats

    static double smoothed_frame_time = 0;
    smoothed_frame_time *= 0.98;
    smoothed_frame_time += (1000.0 * double(frame_time) / double(freq)) * 0.02;

    {
      std::string temp2;
      sprintf(temp2, "frame time %2.2f msec, %6d cyc/frame\n", (double)smoothed_frame_time, 0);
      tp.render_text(fb_width - 256, fb_height - 12, temp2.c_str());
    }
  
    //----------------------------------------
    // Swap

    SDL_UnlockTexture(fb_tex);
    SDL_RenderCopy(renderer, fb_tex, NULL, NULL);

    frame_end = SDL_GetPerformanceCounter();
    frame_time = frame_end - frame_begin;

    SDL_RenderPresent(renderer);

  }

  return 0;
}
#endif
//-----------------------------------------------------------------------------
