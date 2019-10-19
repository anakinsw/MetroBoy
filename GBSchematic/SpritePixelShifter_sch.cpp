// This file should contain the schematics as directly translated to C, no modifications or simplifications

#include "Schematics.h"
#include "Clocks.h"
#include "ExtBus.h"
#include "VramBus.h"
#include "PPU.h"

//----------
// inputs

extern bool XONO;
extern bool XADO;
extern bool PUCO;

//----------
// outputs

//----------
// registers

reg PUDU;
reg RAMU;
reg SELE;
reg SAJA;
reg MYTU;
reg MOFO;
reg SUNY;
reg SUTO;
reg PEFO;
reg REWO;
reg RYDU;
reg SEGA;
reg PEBA;
reg ROKA;
reg RAMA;
reg SEMO;

reg NURO;
reg MASO;
reg LEFE;
reg LESU;
reg WYHO;
reg WORA;
reg VAFO;
reg WUFY;

reg NYLU;
reg PEFU;
reg NATY;
reg PYJO;
reg VARE;
reg WEBA;
reg VANU;
reg VUPY;

//-----------------------------------------------------------------------------

void tick_spritepixelshifter() {
  // sprite x flip
  bool POBE = mux2(vram.MD4, vram.MD3, XONO);
  bool PACY = mux2(vram.MD3, vram.MD4, XONO);
  bool PONO = mux2(vram.MD5, vram.MD2, XONO);
  bool PUGU = mux2(vram.MD2, vram.MD5, XONO);
  bool PUTE = mux2(vram.MD7, vram.MD0, XONO);
  bool PULY = mux2(vram.MD0, vram.MD7, XONO);
  bool PELO = mux2(vram.MD6, vram.MD1, XONO);
  bool PAWE = mux2(vram.MD1, vram.MD6, XONO);

  // the schematic layout is slightly inconsistent, might have some of these swapped

  bool PUDU_Q = PUDU.latch(XADO, POBE);
  bool RAMU_Q = RAMU.latch(PUCO, POBE);
  bool SELE_Q = SELE.latch(PUCO, PACY);
  bool SAJA_Q = SAJA.latch(XADO, PACY);
  bool MYTU_Q = MYTU.latch(XADO, PONO);
  bool MOFO_Q = MOFO.latch(PUCO, PONO);
  bool SUNY_Q = SUNY.latch(XADO, PUGU);
  bool SUTO_Q = SUTO.latch(PUCO, PUGU);
  bool PEFO_Q = PEFO.latch(XADO, PUTE);
  bool REWO_Q = REWO.latch(PUCO, PUTE);
  bool RYDU_Q = RYDU.latch(PUCO, PULY);
  bool SEGA_Q = RYDU.latch(XADO, PULY);
  bool PEBA_Q = PEBA.latch(XADO, PELO);
  bool ROKA_Q = ROKA.latch(PUCO, PELO);
  bool RAMA_Q = RAMA.latch(PUCO, PAWE);
  bool SEMO_Q = SEMO.latch(XADO, PAWE);

  bool LUBO = not(PUDU_Q);
  bool SOLO = not(RAMU_Q);
  bool VOBY = not(RAMU_Q);
  bool WERY = not(RAMU_Q);
  bool LUMO = not(RAMU_Q);
  bool LASE = not(RAMU_Q);
  bool WURA = not(RAMU_Q);
  bool WYCO = not(RAMU_Q);
  bool LOZA = not(RAMU_Q);
  bool RATA = not(RAMU_Q);
  bool SELU = not(RAMU_Q);
  bool WAMY = not(RAMU_Q);
  bool NUCA = not(RAMU_Q);
  bool SYBO = not(RAMU_Q);
  bool SERY = not(RAMU_Q);
  bool SULU = not(RAMU_Q);

  bool LUFY = nand(LUBO,   ppu.ROBY);
  bool MAME = nand(PUDU_Q, ppu.ROBY);
  bool REHU = nand(SOLO,   ppu.ROBY);
  bool RANO = nand(RAMU_Q, ppu.ROBY);

  bool WAXO = nand(VOBY,   ppu.TYTA);
  bool TYGA = nand(SELE_Q, ppu.TYTA);
  bool XATO = nand(WERY,   ppu.TYTA);
  bool VEXU = nand(SAJA_Q, ppu.TYTA);

  bool MAJO = nand(LUMO,   ppu.LYKU);
  bool MYXA = nand(MYTU_Q, ppu.LYKU);
  bool LYDE = nand(LASE,   ppu.LYKU);
  bool LELA = nand(MOFO_Q, ppu.LYKU);

  bool XEXU = nand(WURA,   ppu.TYCO);
  bool VABY = nand(SUNY_Q, ppu.TYCO);
  bool XOLE = nand(WYCO,   ppu.TYCO);
  bool VUME = nand(SUTO_Q, ppu.TYCO);

  bool MOFY = nand(LOZA,   ppu.LESY);
  bool MEZU = nand(PEFO_Q, ppu.LESY);
  bool PYZU = nand(RATA,   ppu.LESY);
  bool PABE = nand(REWO_Q, ppu.LESY);

  bool TULA = nand(SELU,   ppu.XOVU);
  bool TESO = nand(RYDU_Q, ppu.XOVU);
  bool XYVE = nand(WAMY,   ppu.XOVU);
  bool VUNE = nand(SEGA_Q, ppu.XOVU);

  bool MADA = nand(NUCA,   ppu.LOTA);
  bool MYTO = nand(PEBA_Q, ppu.LOTA);
  bool RUCA = nand(SYBO,   ppu.LOTA);
  bool RUSY = nand(ROKA_Q, ppu.LOTA);

  bool TABY = nand(SERY,   ppu.SOKA);
  bool TAPO = nand(RAMA_Q, ppu.SOKA);
  bool TUPE = nand(SULU,   ppu.SOKA);
  bool TUXA = nand(SEMO_Q, ppu.SOKA);

  bool NURO_Q = NURO.srtock(clk.CLKPIPE, PABE, PYZU, ext.P10_B);
  bool MASO_Q = MASO.srtock(clk.CLKPIPE, MYTO, MADA, NURO_Q);
  bool LEFE_Q = LEFE.srtock(clk.CLKPIPE, LELA, LYDE, MASO_Q);
  bool LESU_Q = LESU.srtock(clk.CLKPIPE, MAME, LUFY, LEFE_Q);
  bool WYHO_Q = WYHO.srtock(clk.CLKPIPE, VEXU, XATO, LESU_Q);
  bool WORA_Q = WORA.srtock(clk.CLKPIPE, VABY, XEXU, WYHO_Q);
  bool VAFO_Q = VAFO.srtock(clk.CLKPIPE, TUXA, TUPE, WORA_Q);
  bool WUFY_Q = WUFY.srtock(clk.CLKPIPE, VUNE, XYVE, VAFO_Q);

  bool NYLU_Q = NYLU.srtock(clk.CLKPIPE, MEZU, MOFY, ext.P10_B);
  bool PEFU_Q = PEFU.srtock(clk.CLKPIPE, RUSY, RUCA, NYLU_Q);
  bool NATY_Q = NATY.srtock(clk.CLKPIPE, MYXA, MAJO, PEFU_Q);
  bool PYJO_Q = PYJO.srtock(clk.CLKPIPE, RANO, REHU, NATY_Q);
  bool VARE_Q = VARE.srtock(clk.CLKPIPE, TYGA, WAXO, PYJO_Q);
  bool WEBA_Q = WEBA.srtock(clk.CLKPIPE, VUME, XOLE, VARE_Q);
  bool VANU_Q = VANU.srtock(clk.CLKPIPE, TAPO, TABY, WEBA_Q);
  bool VUPY_Q = VUPY.srtock(clk.CLKPIPE, TESO, TULA, VANU_Q);

  ppu.SPR_PIX_B_0 = NURO_Q;
  ppu.SPR_PIX_B_1 = MASO_Q;
  ppu.SPR_PIX_B_2 = LEFE_Q;
  ppu.SPR_PIX_B_3 = LESU_Q;
  ppu.SPR_PIX_B_4 = WYHO_Q;
  ppu.SPR_PIX_B_5 = WORA_Q;
  ppu.SPR_PIX_B_6 = VAFO_Q;
  ppu.SPR_PIX_B_7 = WUFY_Q;

  ppu.SPR_PIX_A_0 = NYLU_Q;
  ppu.SPR_PIX_A_1 = PEFU_Q;
  ppu.SPR_PIX_A_2 = NATY_Q;
  ppu.SPR_PIX_A_3 = PYJO_Q;
  ppu.SPR_PIX_A_4 = VARE_Q;
  ppu.SPR_PIX_A_5 = WEBA_Q;
  ppu.SPR_PIX_A_6 = VANU_Q;
  ppu.SPR_PIX_A_7 = VUPY_Q;
}