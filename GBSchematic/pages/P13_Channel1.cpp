#include "P13_Channel1.h"
#include "../Schematics.h"
#include "Gameboy.h"

//-----------------------------------------------------------------------------
// This file should contain the schematics as directly translated to C,
// no modifications or simplifications.

void P13_Channel1::tick(const Gameboy& a, const Gameboy& b, Gameboy& c) {

  //----------
  // Waveform counter

  c.p13.CALA = not(b.p11.CH1_FREQ_10);
  c.p13.CH1_PHASE_CLKnb = tock_pos(a.p13.CALA, b.p13.CALA, b.p13.DYRU, b.p13.CH1_PHASE_CLKnb, !b.p13.CH1_PHASE_CLKnb);
  
  c.p13.DOKA = and(b.p13.CH1_PHASE_CLKnb, b.p01.DYFA_1M);
  c.p13.DYRU = nor(b.apu.APU_RESET1, b.p13.FEKU, b.p13.DOKA);
  
  c.p13.CH1_PHASE_CLKb = not(b.p13.CH1_PHASE_CLKnb);
  c.p13.CH1_PHASE_CLKna = not(b.p13.CH1_PHASE_CLKb);
  c.p13.CH1_PHASE_CLKa = not(b.p13.CH1_PHASE_CLKna);

  c.p13.CH1_PHASE_0 = tock_pos( a.p13.CH1_PHASE_CLKa,  b.p13.CH1_PHASE_CLKa, b.p11.CEPO, c.p13.CH1_PHASE_0, !c.p13.CH1_PHASE_0);
  c.p13.CH1_PHASE_1 = tock_pos(!a.p13.CH1_PHASE_0,   !b.p13.CH1_PHASE_0,   b.p11.CEPO, c.p13.CH1_PHASE_1, !c.p13.CH1_PHASE_1);
  c.p13.CH1_PHASE_2 = tock_pos(!a.p13.CH1_PHASE_1,   !b.p13.CH1_PHASE_1,   b.p11.CEPO, c.p13.CH1_PHASE_2, !c.p13.CH1_PHASE_2);

  // ENEK    = 00000011
  // EZOZ    = 00000010
  // PHASE_2 = 00001111
  // CODO    = 11111100

  c.p13.DUVO = not(b.p13.CH1_PHASE_0);
  c.p13.ENEK = and(b.p13.DUVO, b.p13.EZOZ);
  c.p13.EZOZ = and(b.p13.CH1_PHASE_1, b.p13.CH1_PHASE_2);
  c.p13.CODO = not(b.p13.EZOZ);

  c.p13.DUTY_0 = nor( b.p11.CH1_DUTY_0,  b.p11.CH1_DUTY_1);
  c.p13.DUTY_1 = nor(!b.p11.CH1_DUTY_0,  b.p11.CH1_DUTY_1);
  c.p13.DUTY_2 = nor( b.p11.CH1_DUTY_0, !b.p11.CH1_DUTY_1);
  c.p13.DUTY_3 = nor(!b.p11.CH1_DUTY_0, !b.p11.CH1_DUTY_1);

  c.p13.CH1_RAW_BIT = amux4(b.p13.ENEK,        b.p13.DUTY_0,
                            b.p13.EZOZ,        b.p13.DUTY_1,
                            b.p13.CH1_PHASE_2, b.p13.DUTY_2,
                            b.p13.CODO,        b.p13.DUTY_3);

  //----------
  // Length timer

  c.p13.FF11_WRna = nand(b.apu.APU_WR, b.apu.ADDR_FF11);
  c.p13.FF11_WRa = not(b.p13.FF11_WRna);
  c.p13.FF11_WRb = not(b.p13.FF11_WRna);
  c.p13.FF11_WRc = not(b.p13.FF11_WRna);
  c.p13.CORY = nor(b.p13.FEKU, b.apu.APU_RESET1, b.p13.FF11_WRb);



  c.p13.CH1_LEN_CLKn = nor(b.p01.CLK_256a, !b.p11.NR14_STOP, b.p13.CH1_LEN_DONE); // use_len polarity?
  c.p13.CH1_LEN_CLK = not(b.p13.CH1_LEN_CLKn);

  c.p13.NR11_LEN0 = count_pos(a.p13.CH1_LEN_CLK, b.p13.CH1_LEN_CLK, b.p13.FF11_WRc, b.p13.NR11_LEN0, b.D0);
  c.p13.NR11_LEN1 = count_pos(a.p13.NR11_LEN0,   b.p13.NR11_LEN0,   b.p13.FF11_WRc, b.p13.NR11_LEN1, b.D1);
  c.p13.NR11_LEN2 = count_pos(a.p13.NR11_LEN1,   b.p13.NR11_LEN1,   b.p13.FF11_WRc, b.p13.NR11_LEN2, b.D2);
  c.p13.NR11_LEN3 = count_pos(a.p13.NR11_LEN2,   b.p13.NR11_LEN2,   b.p13.FF11_WRc, b.p13.NR11_LEN3, b.D3);
  c.p13.NR11_LEN3n = not(!b.p13.NR11_LEN3);
  c.p13.NR11_LEN4 = count_pos(a.p13.NR11_LEN3n,  b.p13.NR11_LEN3n,  b.p13.FF11_WRa, b.p13.NR11_LEN4, b.D4);
  c.p13.NR11_LEN5 = count_pos(a.p13.NR11_LEN4,   b.p13.NR11_LEN4,   b.p13.FF11_WRa, b.p13.NR11_LEN5, b.D5);

  c.p13.CH1_LEN_DONE = tock_pos(!a.p13.NR11_LEN5, !b.p13.NR11_LEN5, b.p13.CORY, c.p13.CH1_LEN_DONE, !c.p13.CH1_LEN_DONE);

  c.p13.CH1_SWEEP_STOP = not(b.p12.CH1_SWEEP_STOPn);
  c.p13.CH1_AMP_ENn = nor(b.p11.CH1_ENV_DIR, b.p11.CH1_VOL_0, b.p11.CH1_VOL_1, b.p11.CH1_VOL_2, b.p11.CH1_VOL_3);

  /*p13.CYFA*/ c.p13.CH1_LEN_STOP = and(b.p13.CH1_LEN_DONE, b.p11.NR14_STOP);
  /*p13.BERY*/ c.p13.CH1_STOP = or(b.p13.CH1_SWEEP_STOP, b.apu.APU_RESET1, b.p13.CH1_LEN_STOP, b.p13.CH1_AMP_ENn);

  //----------
  // EG timer

  c.p13.KAZA = nor(b.p13.FEKU, b.p13.KOZY);
  c.p13.SWEEP_RST = not(b.p13.KAZA);

  c.p13.CLK_128n = not(b.p01.CLK_128a);
  
  c.p13.CLK_64a  = tock_pos(a.p13.CLK_128n, b.p13.CLK_128n, b.p13.KADO, b.p13.CLK_64a, !b.p13.CLK_64a);
  c.p13.CLK_64nb = not(b.p13.CLK_64a);
  c.p13.CLK_64b  = not(b.p13.CLK_64nb);

  c.p13.SWEEP_CNT0 = count_pos(a.p13.CLK_64b,    b.p13.CLK_64b,    b.p13.SWEEP_RST, b.p13.SWEEP_CNT0, !b.p11.CH1_SWEEP_0);
  c.p13.SWEEP_CNT1 = count_pos(a.p13.SWEEP_CNT0, b.p13.SWEEP_CNT0, b.p13.SWEEP_RST, b.p13.SWEEP_CNT1, !b.p11.CH1_SWEEP_1);
  c.p13.SWEEP_CNT2 = count_pos(a.p13.SWEEP_CNT1, b.p13.SWEEP_CNT1, b.p13.SWEEP_RST, b.p13.SWEEP_CNT2, !b.p11.CH1_SWEEP_2);

  
  c.p13.CH1_NO_SWEEP = nor(b.p11.CH1_SWEEP_0, b.p11.CH1_SWEEP_1, b.p11.CH1_SWEEP_2);

  c.p13.KOTE = and(b.p13.SWEEP_CNT0, b.p13.SWEEP_CNT1, b.p13.SWEEP_CNT2);

  c.p13.KORO = nor(b.p13.KUKU, b.p13.CH1_NO_SWEEP);

  c.p13.KOZY = tock_pos(a.p01.CLK_512a, b.p01.CLK_512a, b.p13.KORO, b.p13.KOZY, b.p13.KOTE);
  c.p13.KYNO = tock_pos(a.p13.KOZY,     b.p13.KOZY,     b.p13.KORU, b.p13.KYNO, b.p13.CH1_ENV_MAX);


  c.p13.KURY = not(b.p13.KOZY);
  c.p13.KUKU = nor(b.p01.CPUCLK_REQn, b.p13.KURY);

  c.p13.FEKU = tock_pos(a.p01.DYFA_1M, b.p01.DYFA_1M, b.p13.EGET, b.p13.FEKU, b.p13.FYFO);
  c.p13.FARE = tock_pos(a.p01.DYFA_1M, b.p01.DYFA_1M, b.p13.ERUM, b.p13.FARE, b.p13.FEKU);
  c.p13.FYTE = tock_pos(a.p01.DYFA_1M, b.p01.DYFA_1M, b.p13.ERUM, b.p13.FYTE, b.p13.FARE);

  c.p13.KORU = nor(b.p13.FEKU, b.apu.APU_RESET1);

  c.p13.FEMY = nor(b.p13.CH1_AMP_ENn, b.apu.APU_RESET1);
  c.p13.GEPU = not(b.p13.FYTE);
  c.p13.GEXU = unk2(b.p13.FEMY, b.p13.GEPU);

  c.p13.EGET = nor(b.apu.APU_RESET1, b.p13.FARE);
  c.p13.GEFE = not(b.p13.EGET);
  c.p13.FYFO = or(b.p13.GEFE, b.p13.CH1_RUNNING); // unk2





  /*p13.KEKO*/ c.p13.KEKO = or(b.apu.APU_RESET1, b.p13.FEKU);
  /*p13.KABA*/ c.p13.KABA = or(b.apu.APU_RESET1, b.p13.FEKU);
  /*p13.KYLY*/ c.p13.KYLY = not(b.p13.KABA);
  /*p13.KEZU*/ c.p13.KEZU = or(b.p13.KYNO, b.p13.KEKO); // unk2
  /*p13.KAKE*/ c.p13.KAKE = and(b.p13.KOZY, b.p13.CH1_NO_SWEEP, b.p13.KEZU);

  // these are probably flipped or something
  c.p13.CH1_ACTIVE = or(b.p13.FEKU, b.p13.CH1_STOP);
  c.p13.CH1_ACTIVEn = not(b.p13.CH1_ACTIVE);

  /*p13.DUWO*/ c.p13.DUWO = tock_pos(a.p13.CH1_PHASE_CLKna, b.p13.CH1_PHASE_CLKna, b.p11.CEPO, b.p13.DUWO, b.p13.CH1_RAW_BIT);
  /*p13.COWE*/ c.p13.COWE = and(b.p13.CH1_ACTIVE, b.p13.DUWO);
  c.p13.CH1_BIT = or(b.p13.COWE, b.apu.DBG_APU);

  // I'm not entirely sure how this works...

  /*p13.HESU*/ c.p13.CH1_ENV_DELTA0 = amux2(b.p11.CH1_ENV_DIR, b.p13.CH1_ENV1, !b.p13.CH1_ENV1, !b.p11.CH1_ENV_DIR);
  /*p13.HETO*/ c.p13.CH1_ENV_DELTA1 = amux2(b.p11.CH1_ENV_DIR, b.p13.CH1_ENV2, !b.p13.CH1_ENV2, !b.p11.CH1_ENV_DIR);
  /*p13.HYTO*/ c.p13.CH1_ENV_DELTA2 = amux2(b.p11.CH1_ENV_DIR, b.p13.CH1_ENV3, !b.p13.CH1_ENV3, !b.p11.CH1_ENV_DIR);
  /*p13.JUFY*/ c.p13.CH1_ENV_DELTA3 = amux2(b.p11.CH1_ENV_DIR, b.p13.KAKE,  b.p13.KAKE, !b.p11.CH1_ENV_DIR);

  /*p13.HEVO*/ c.p13.CH1_ENV0 = count_pos(a.p13.CH1_ENV_DELTA0, b.p13.CH1_ENV_DELTA0, b.p13.FEKU, b.p13.CH1_ENV0, b.p11.CH1_VOL_0);
  /*p13.HOKO*/ c.p13.CH1_ENV1 = count_pos(a.p13.CH1_ENV_DELTA1, b.p13.CH1_ENV_DELTA1, b.p13.FEKU, b.p13.CH1_ENV1, b.p11.CH1_VOL_1);
  /*p13.HEMY*/ c.p13.CH1_ENV2 = count_pos(a.p13.CH1_ENV_DELTA2, b.p13.CH1_ENV_DELTA2, b.p13.FEKU, b.p13.CH1_ENV2, b.p11.CH1_VOL_2);
  /*p13.HAFO*/ c.p13.CH1_ENV3 = count_pos(a.p13.CH1_ENV_DELTA3, b.p13.CH1_ENV_DELTA3, b.p13.FEKU, b.p13.CH1_ENV3, b.p11.CH1_VOL_3);

  /*p13.AMOP*/ c.p13.CH1_OUT0 = and(b.p13.CH1_ENV0, b.p13.CH1_BIT);
  /*p13.ASON*/ c.p13.CH1_OUT1 = and(b.p13.CH1_ENV1, b.p13.CH1_BIT);
  /*p13.AGOF*/ c.p13.CH1_OUT2 = and(b.p13.CH1_ENV2, b.p13.CH1_BIT);
  /*p13.ACEG*/ c.p13.CH1_OUT3 = and(b.p13.CH1_ENV3, b.p13.CH1_BIT);

  /*p13.HUFU*/ c.p13.CH1_ENV_TOPn = nand(b.p11.CH1_ENV_DIR, b.p13.CH1_ENV0, b.p13.CH1_ENV1, b.p13.CH1_ENV2, b.p13.CH1_ENV3);
  /*p13.HANO*/ c.p13.CH1_ENV_BOT = nor(b.p11.CH1_ENV_DIR,  b.p13.CH1_ENV0, b.p13.CH1_ENV1, b.p13.CH1_ENV2, b.p13.CH1_ENV3);
  /*p13.HAKE*/ c.p13.CH1_ENV_TOP = not(b.p13.CH1_ENV_TOPn);
  /*p13.JADE*/ c.p13.CH1_ENV_MAX = or(b.p13.CH1_ENV_TOP, b.p13.CH1_ENV_BOT);

  //----------
  // Sweep timer

  /*p13.DAFA*/ c.p13.DAFA = nor(b.p13.BEXA, b.p13.FEKU);
  /*p13.CYMU*/ c.p13.CYMU = not(b.p13.DAFA);
  /*p13.BAVE*/ c.p13.BAVE = and(b.p11.NR10_SWEEP_TIME_0, b.p11.NR10_SWEEP_TIME_1, b.p11.NR10_SWEEP_TIME_2);

  // this is some debug thing
  /*p09.BAZA*/ c.apu.DBG_SWEEP_CLK = tock_pos(a.p01.AJER_2M, b.p01.AJER_2M, b.apu.APU_RESETn3, b.apu.DBG_SWEEP_CLK, b.apu.DBG_SWEEP);
  /*p09.CELY*/ c.apu.CELY = mux2(b.apu.DBG_SWEEP_CLK, b.p01.CLK_128a, b.apu.DBG_APU);
  /*p09.CONE*/ c.apu.CONE = not(b.apu.CELY);
  /*p09.CATE*/ c.apu.CATE = not(b.apu.CONE);
  /*p13.CUPO*/ c.p13.CUPO = count_pos(a.apu.CATE, b.apu.CATE, b.p13.CYMU, b.p13.CUPO, b.p11.NR10_SWEEP_TIME_0);
  /*p13.CYPU*/ c.p13.CYPU = count_pos(a.p13.CUPO, b.p13.CUPO, b.p13.CYMU, b.p13.CYPU, b.p11.NR10_SWEEP_TIME_1);
  /*p13.CAXY*/ c.p13.CAXY = count_pos(a.p13.CYPU, b.p13.CYPU, b.p13.CYMU, b.p13.CAXY, b.p11.NR10_SWEEP_TIME_2);
  /*p13.BURY*/ c.p13.BURY = nor(b.p13.BAVE, b.apu.APU_RESET1);
  /*p13.COZE*/ c.p13.COZE = and(b.p13.CAXY, b.p13.CYPU, b.p13.CUPO);
  /*p13.BEXA*/ c.p13.BEXA = tock_pos(a.p01.AJER_2M, b.p01.AJER_2M, b.p13.BURY, b.p13.BEXA, b.p13.COZE);

  //----------
  // Sweep shift counter

  /*p13.DACU*/ c.p13.DACU = nor(b.p13.FEKU, b.p13.BEXA);
  /*p13.CYLU*/ c.p13.CYLU = not(b.p13.DACU);
  /*p13.BUGE*/ c.p13.BUGE = nand(b.p11.NR10_SWEEP_SHIFT_0, b.p11.NR10_SWEEP_SHIFT_1, b.p11.NR10_SWEEP_SHIFT_2);
  /*p13.CELE*/ c.p13.CELE = not(b.p13.BUGE);
  /*p13.ADAD*/ c.p13.ADAD = not(!b.p13.BYTE);
  /*p13.EPUK*/ c.p13.EPUK = nor(b.p13.ADAD, b.apu.APU_RESET1);
  /*p13.EVOL*/ c.p13.EVOL = nor(b.p13.BEXA, b.p13.FYTE);
  /*p13.FEMU*/ c.p13.FEMU = unk2(b.p13.EPUK, b.p13.EVOL);
  /*p13.EGYP*/ c.p13.EGYP = nor(b.p01.DYFA_1M, b.p13.FEMU);
  /*p13.DODY*/ c.p13.DODY = nor(b.p13.EGYP, b.p13.CELE);
  /*p13.EGOR*/ c.p13.EGOR = and(b.ch4.NR41_LEN3b, b.p13.DODY); // wtf? probably debug something
  /*p13.DAPU*/ c.p13.DAPU = not(b.p13.EGOR);
  /*p13.COPA*/ c.p13.COPA = count_pos(a.p13.DAPU, b.p13.DAPU, b.p13.CYLU, b.p13.COPA, b.p11.NR10_SWEEP_SHIFT_0);
  /*p13.CAJA*/ c.p13.CAJA = count_pos(a.p13.COPA, b.p13.COPA, b.p13.CYLU, b.p13.CAJA, b.p11.NR10_SWEEP_SHIFT_1);
  /*p13.BYRA*/ c.p13.BYRA = count_pos(a.p13.CAJA, b.p13.CAJA, b.p13.CYLU, b.p13.BYRA, b.p11.NR10_SWEEP_SHIFT_2);
  /*p13.COPY*/ c.p13.COPY = and(b.p13.COPA, b.p13.CAJA, b.p13.BYRA);
  /*p13.ATAT*/ c.p13.ATAT = nor(b.apu.APU_RESET1, b.p13.BEXA);
  /*p13.BYTE*/ c.p13.BYTE = tock_pos(a.p01.AJER_2M, b.p01.AJER_2M, b.p13.ATAT, b.p13.BYTE, b.p13.COPY);
  /*p13.ATUV*/ c.p13.ATUV = and(b.p13.BEXA, b.p12.CH1_SWEEP_STOPn);
  /*p13.BOJE*/ c.p13.BOJE = and(b.p13.ATUV, b.p13.BUGE);
  /*p13.BUSO*/ c.p13.BUSO = or(b.p13.BUGE, b.p12.CH1_SWEEP_STOPn, b.p13.BEXA);
  /*p13.KALA*/ c.p13.KALA = nor(b.p13.BEXA, b.p13.FEKU);
}
