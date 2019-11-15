#include "P01_ClocksReset.h"
#include "Gameboy.h"

/*
union { bool DIV_00; bool DIV_00; };
union { bool DIV_01; bool DIV_01; bool CLK_256K; };
union { bool DIV_02; bool DIV_02; };
union { bool DIV_03; bool DIV_03; bool CLK_64K; };
union { bool DIV_04; bool DIV_04; };
union { bool DIV_05; bool DIV_05; bool TAMA_16Kn; };
union { bool DIV_06; bool DIV_06; };
union { bool DIV_07; bool DIV_07; };
union { bool DIV_08; bool DIV_08; };
union { bool DIV_09; bool DIV_09; };
union { bool DIV_10; bool DIV_10; };
union { bool DIV_11; bool DIV_11; };
union { bool DIV_12; bool DIV_12; };
union { bool DIV_13; bool DIV_13; };
union { bool DIV_14; bool DIV_14; };
union { bool DIV_15; bool DIV_15; };
*/

//-----------------------------------------------------------------------------
// This file should contain the schematics as directly translated to C,
// no modifications or simplifications.

const std::vector<SignalData> P01_ClocksReset::signals() {
  return
  {
    SignalData("-----Clocks-----"),
    SignalData("PHASE_ABCD", offsetof(P01_ClocksReset, CLK_ABCDxxxx1)),
    SignalData("PHASE_BCDE", offsetof(P01_ClocksReset, CLK_xBCDExxx1)),
    SignalData("PHASE_CDEF", offsetof(P01_ClocksReset, CLK_xxCDEFxx1)),
    SignalData("PHASE_DEFG", offsetof(P01_ClocksReset, CLK_xxxDEFGx1)),
    SignalData("COKE", offsetof(P01_ClocksReset, COKE)),

    /*
    SignalData("-----DIV-----"),
    SignalData("DIV_00",    offsetof(P01_ClocksReset, DIV_00)),
    SignalData("DIV_01",    offsetof(P01_ClocksReset, DIV_01)),
    SignalData("DIV_02",    offsetof(P01_ClocksReset, DIV_02)),
    SignalData("DIV_03",    offsetof(P01_ClocksReset, DIV_03)),
    SignalData("DIV_04",    offsetof(P01_ClocksReset, DIV_04)),
    SignalData("DIV_05",    offsetof(P01_ClocksReset, DIV_05)),
    SignalData("DIV_06",    offsetof(P01_ClocksReset, DIV_06)),
    SignalData("DIV_07",    offsetof(P01_ClocksReset, DIV_07)),
    SignalData("DIV_08",    offsetof(P01_ClocksReset, DIV_08)),
    SignalData("DIV_09",    offsetof(P01_ClocksReset, DIV_09)),
    SignalData("DIV_10",    offsetof(P01_ClocksReset, DIV_10)),
    SignalData("DIV_11",    offsetof(P01_ClocksReset, DIV_11)),
    SignalData("DIV_12",    offsetof(P01_ClocksReset, DIV_12)),
    SignalData("DIV_13",    offsetof(P01_ClocksReset, DIV_13)),
    SignalData("DIV_14",    offsetof(P01_ClocksReset, DIV_14)),
    SignalData("DIV_15",    offsetof(P01_ClocksReset, DIV_15)),
    */

    /*
    SignalData("----------"),
    SignalData("BARA",       offsetof(P01_ClocksReset, BARA)),
    SignalData("CARU",       offsetof(P01_ClocksReset, CARU)),
    SignalData("BYLU",       offsetof(P01_ClocksReset, BYLU)),

    SignalData("----------"),
    SignalData("ATYK",       offsetof(P01_ClocksReset, ATYK)),
    SignalData("AVOK",       offsetof(P01_ClocksReset, AVOK)),
    SignalData("JESO",       offsetof(P01_ClocksReset, JESO)),
    */
  };
}

//-----------------------------------------------------------------------------

void blah(const Gameboy& a, const Gameboy& b, Gameboy& c) {
  if (b.p07.MODE_DBG1 || b.p07.MODE_DBG2) {
    c.cpu.CPU_RESET   = 1;
    c.p01.RESET_REG   = 0;
    c.p01.SYS_RESETn1 = 0;
  }
  else if (!b.chip.RST && !b.p01.CPUCLK_REQn && !b.p01.CLK_BAD1) {
    c.p01.CPU_RESET   = 0;
    c.p01.RESET_REG   = tock_pos(a.p01.RESET_CLK, b.p01.RESET_CLK, 1, b.p01.RESET_REG, 0);
    c.p01.SYS_RESETn1 = not(b.p01.RESET_REG);
  }
  else {
    c.p01.CPU_RESET   = b.p01.DIV_15;
    c.p01.RESET_REG   = tock_pos(a.p01.RESET_CLK, b.p01.RESET_CLK, 1, b.p01.RESET_REG, b.chip.RST || b.p01.DIV_15);
    c.p01.SYS_RESETn1 = not(b.p01.RESET_REG) && not(b.chip.RST) && not(b.p01.DIV_15);
  }
}

//-----------------------------------------------------------------------------

void P01_ClocksReset::tick(const Gameboy& a, const Gameboy& b, Gameboy& c) {

  //----------
  // CPU reset

  c.p01.UPYF        = or(b.chip.RST, b.p01.CLK_BAD1);
  c.p01.TUBO        = or(b.p01.CPUCLK_REQn, b.p01.UPYF);
  c.p01.UNUT        = and(b.p01.TUBO, b.p01.DIV_15);
  c.p01.CPU_RESET   = or(b.p07.MODE_DBG2, b.p07.MODE_DBG1, b.p01.UNUT);
  c.cpu.CPU_RESET   = b.p01.CPU_RESET;
  c.p01.CPU_RESETn  = not(b.p01.CPU_RESET);

  //----------
  // SYS reset tree

  c.p01.AFAR        = nor(b.p01.CPU_RESETn, b.chip.RST);
  c.p01.ASOL        = or (b.p01.AFAR, b.chip.RST);
  /*AFER*/ c.p01.RESET_REG   = tock_pos(a.p01.RESET_CLK, b.p01.RESET_CLK, b.p07.MODE_PROD, b.p01.RESET_REG, b.p01.ASOL);
  c.p01.AVOR        = or(b.p01.RESET_REG, b.p01.ASOL);
  
  /*ALUR*/ c.p01.SYS_RESETn1 = not(b.p01.AVOR);

  /*DULA*/ c.p01.SYS_RESET1  = not(b.p01.SYS_RESETn1);
  /*P09.HAPO*/ c.p01.SYS_RESET2  = not(b.p01.SYS_RESETn1);

  /*CUNU*/ c.p01.SYS_RESETn2 = not(b.p01.SYS_RESET1);
  /*P09.GUFO*/ c.p01.SYS_RESETn3 = not(b.p01.SYS_RESET2);

  /*XORE*/ c.p01.SYS_RESET3  = not(b.p01.SYS_RESETn2);

  /*XEBE*/ c.p01.SYS_RESETn4 = not(b.p01.SYS_RESET3);
  /*WALU*/ c.p01.SYS_RESETn5 = not(b.p01.SYS_RESET3);
  /*WESY*/ c.p01.SYS_RESETn6 = not(b.p01.SYS_RESET3);
  /*XARE*/ c.p01.SYS_RESETn7 = not(b.p01.SYS_RESET3);

  //----------
  // VID reset tree

  /*XODO*/ c.p01.VID_RESET1  = and(b.p01.SYS_RESETn4, b.p23.LCD_ON); // polarity?
  /*XAPO*/ c.p01.VID_RESETn1 = not(b.p01.VID_RESET1);

  /*LYHA*/ c.p01.VID_RESET2  = not(b.p01.VID_RESETn1);
  /*TOFU*/ c.p01.VID_RESET3  = not(b.p01.VID_RESETn1);
  /*PYRY*/ c.p01.VID_RESET4  = not(b.p01.VID_RESETn1);
  /*ROSY*/ c.p01.VID_RESET5  = not(b.p01.VID_RESETn1);
  /*ATAR*/ c.p01.VID_RESET6  = not(b.p01.VID_RESETn1);
  /*AMYG*/ c.p01.VID_RESET7  = not(b.p01.VID_RESETn1);

  /*LYFE*/ c.p01.VID_RESETn2 = not(b.p01.VID_RESET2);
  /*ABEZ*/ c.p01.VID_RESETn3 = not(b.p01.VID_RESET6);

  //----------
  // APU reset tree

  /*P09.JYRO*/ c.p01.APU_RST_00  = or(b.p01.SYS_RESET2, !b.p09.ALL_SOUND_ON);

  /*P09.KEBA*/ c.p09.APU_RESET1  = not(b.p09.APU_RESETn7);
  /*P09.AGUR*/ c.p09.APU_RESETn  = not(b.p09.APU_RESET1);
  /*P09.AFAT*/ c.p09.APU_RESETn2 = not(b.p09.APU_RESET1);
  /*P09.ATYV*/ c.p09.APU_RESETn3 = not(b.p09.APU_RESET1);
  /*P09.DAPA*/ c.p09.APU_RESETn4 = not(b.p09.APU_RESET1);
  /*P09.KAME*/ c.p09.APU_RESETn5 = not(b.p09.APU_RESET1);
  /*P09.KEPY*/ c.p09.APU_RESETn6 = not(b.p01.APU_RST_00);
  /*P09.KUBY*/ c.p09.APU_RESETn7 = not(b.p01.APU_RST_00);

  /*p01.BOPO*/ c.p01.APU_RESETn1 = not(b.p09.APU_RESET1);
  /*p01.ATUS*/ c.p01.APU_RESETn2 = not(b.p09.APU_RESET1);
  /*p01.BELA*/ c.p01.APU_RESETn3 = not(b.p09.APU_RESET1);
  /*p14.HUDE*/ c.p14.APU_RESETn1 = not(b.p09.APU_RESET1);

  /*p11.CEPO*/ c.p11.CEPO = not(b.p09.APU_RESET1);
  /*p11.HATO*/ c.p11.HATO = not(b.p09.APU_RESET1);
  /*p11.CAMY*/ c.p11.CAMY = not(b.p09.APU_RESET1);
  /*p13.KADO*/ c.p13.KADO = not(b.p09.APU_RESET1);
  /*p13.ERUM*/ c.p13.ERUM = not(b.p09.APU_RESET1);
  /*p13.DUKA*/ c.p13.DUKA = not(b.p09.APU_RESET1);
  /*p14.JYBU*/ c.p14.JYBU = not(b.p09.APU_RESET1);
  /*p14.KYPU*/ c.p14.KYPU = not(b.p09.APU_RESET1);
  /*p14.FAZO*/ c.p14.FAZO = not(b.p09.APU_RESET1);
  /*p15.KATY*/ c.p15.KATY = not(b.p09.APU_RESET1);
  /*p15.CYWU*/ c.p15.CYWU = not(b.p09.APU_RESET1);
  /*p15.CEXE*/ c.p15.CEXE = not(b.p09.APU_RESET1);
  /*p15.BUWE*/ c.p15.BUWE = not(b.p09.APU_RESET1);
  /*p16.GOVE*/ c.p16.GOVE = not(b.p09.APU_RESET1);
  /*p16.GAZE*/ c.p16.GAZE = not(b.p09.APU_RESET1);
  /*p16.GOMA*/ c.p16.GOMA = not(b.p09.APU_RESET1);
  /*p16.KUHA*/ c.p16.KUHA = not(b.p09.APU_RESET1);
  /*p16.HEKY*/ c.p16.HEKY = not(b.p09.APU_RESET1);
  /*p16.KOPY*/ c.p16.KOPY = not(b.p09.APU_RESET1);
  /*p16.GURO*/ c.p16.GURO = not(b.p09.APU_RESET1);
  /*p17.BAMA*/ c.p17.BAMA = not(b.p09.APU_RESET1);
  /*p17.ACOR*/ c.p17.ACOR = not(b.p09.APU_RESET1);
  /*p18.CALU*/ c.p18.CALU = not(b.p09.APU_RESET1);
  /*p19.FEXO*/ c.p19.FEXO = not(b.p09.APU_RESET1);
  /*p19.HYNE*/ c.p19.HYNE = not(b.p09.APU_RESET1);
  /*p19.CABE*/ c.p19.CABE = not(b.p09.APU_RESET1);
  /*p20.BOKY*/ c.p20.BOKY = not(b.p09.APU_RESET1);
  /*p20.GASO*/ c.p20.GASO = not(b.p09.APU_RESET1);
  /*p20.FEBY*/ c.p20.FEBY = not(b.p09.APU_RESET1);

  //----------
  // Clock control

  /*p01.ABOL*/ c.p01.CPUCLK_REQn = not(b.cpu.CPUCLK_REQ);
  /*p01.BUTY*/ c.p01.CPUCLK_REQ  = not(b.p01.CPUCLK_REQn);
  /*p01.UCOB*/ c.p01.CLK_BAD1   = not(b.chip.CLKIN_A);
  /*p01.ATEZ*/ c.p01.CLK_BAD2   = not(b.chip.CLKIN_A);

  //----------
  // Clock tree

  /*p01.ARYS*/ c.p01.CLK_AxCxExGx5 = not(b.chip.CLKIN_B);
  /*p01.ANOS*/ c.p01.CLK_AxCxExGx6 = nand(b.chip.CLKIN_B,      b.p01.CLK_xBxDxFxH3);

  /*p01.APUV*/ c.p01.CLK_AxCxExGx1  = not(b.p01.CLK_xBxDxFxH1);
  /*p01.ARYF*/ c.p01.CLK_AxCxExGx2  = not(b.p01.CLK_xBxDxFxH1);
  /*p01.ALET*/ c.p01.CLK_AxCxExGx4  = not(b.p01.CLK_xBxDxFxH2);
  /*p01.ATAL*/ c.p01.CLK_AxCxExGx3  = not(b.p01.CLK_xBxDxFxH3);
  /*p01.ATAG*/ c.p01.CLK_AxCxExGx8  = not(b.p01.CLK_xBxDxFxH4);
  /*p01.ZAXY*/ c.p01.CLK_AxCxExGx9  = not(b.p01.CLK_xBxDxFxH4);
  /*p01.TAVA*/ c.p01.CLK_AxCxExGx10 = not(b.p01.CLK_xBxDxFxH5);

  /*p01.AMUK*/ c.p01.CLK_xBxDxFxH1 = not(b.p01.CLK_AxCxExGx8);
  /*p01.ZEME*/ c.p01.CLK_xBxDxFxH2 = not(b.p01.CLK_AxCxExGx9);
  /*p01.AVET*/ c.p01.CLK_xBxDxFxH3 = nand(b.p01.CLK_AxCxExGx6, b.p01.CLK_AxCxExGx5);
  /*p01.AZOF*/ c.p01.CLK_xBxDxFxH4 = not(b.p01.CLK_AxCxExGx3);
  /*p01.LAPE*/ c.p01.CLK_xBxDxFxH5 = not(b.p01.CLK_AxCxExGx4);

  /*p17.ARUC*/ c.p17.ARUC = not(b.p01.CLK_xBxDxFxH1);
  /*p17.COZY*/ c.p17.COZY = not(b.p01.CLK_xBxDxFxH1);


  //----------
  // Phase generator. These registers tick on _BOTH_EDGES_ of the master clock.

  /*p01.AFUR*/ c.p01.CLK_ABCDxxxx1 = tock_duo(a.p01.CLK_AxCxExGx3, b.p01.CLK_AxCxExGx3, b.p07.MODE_PROD, b.p01.CLK_ABCDxxxx1, !b.p01.CLK_xxxDEFGx1);
  /*p01.ALEF*/ c.p01.CLK_xBCDExxx1 = tock_duo(a.p01.CLK_AxCxExGx3, b.p01.CLK_AxCxExGx3, b.p07.MODE_PROD, b.p01.CLK_xBCDExxx1, b.p01.CLK_ABCDxxxx1);
  /*p01.APUK*/ c.p01.CLK_xxCDEFxx1 = tock_duo(a.p01.CLK_AxCxExGx3, b.p01.CLK_AxCxExGx3, b.p07.MODE_PROD, b.p01.CLK_xxCDEFxx1, b.p01.CLK_xBCDExxx1);
  /*p01.ADYK*/ c.p01.CLK_xxxDEFGx1 = tock_duo(a.p01.CLK_AxCxExGx3, b.p01.CLK_AxCxExGx3, b.p07.MODE_PROD, b.p01.CLK_xxxDEFGx1, b.p01.CLK_xxCDEFxx1);
  /*p01.ATYP*/ c.p01.CLK_xxxxEFGH1 = not(b.p01.CLK_ABCDxxxx1);
  /*p01.AFEP*/ c.p01.CLK_AxxxxFGH1 = not(b.p01.CLK_xBCDExxx1);
  /*p01.AROV*/ c.p01.CLK_ABxxxxGH1 = not(b.p01.CLK_xxCDEFxx1);
  /*p01.ADAR*/ c.p01.CLK_ABCxxxxH1 = not(b.p01.CLK_xxxDEFGx1);

  /*p01.BUGO*/ c.p01.CLK_xBCDExxx2 = not(b.p01.CLK_AxxxxFGH1);
  /*p01.AFAS*/ c.p01.CLK_xxxDxxxx1 = nor(b.p01.CLK_ABCxxxxH1, b.p01.CLK_xxxxEFGH1);
  /*p01.AJAX*/ c.p01.CLK_ABCDxxxx3 = not(b.p01.CLK_xxxxEFGH1);
  
  //----------
  
  /*p01.AREV*/ c.p01.CPU_WR_SYNCn = nand(b.cpu.CPU_RAW_WR, b.p01.CLK_xxxDxxxx1);
  /*p01.APOV*/ c.p01.CPU_WR_SYNC  = not(b.p01.CPU_WR_SYNCn);

  /*p01.AGUT*/ c.p01.AGUT         = and(or(b.p01.CLK_ABCDxxxx3, b.p01.CLK_ABxxxxGH1), b.cpu.ADDR_VALID);
  /*p01.AWOD*/ c.p01.CPU_RD_SYNCn = or(b.p07.MODE_DBG2, b.p01.AGUT);
  /*p01.ABUZ*/ c.p01.CPU_RD_SYNC  = not(b.p01.CPU_RD_SYNCn);

  // debug override of CPU_RD/CPU_WR

  c.p07.CPU_RD_MUX = mux2(b.chip.RD_C, b.cpu.CPU_RAW_RD,  b.p07.MODE_DBG2);
  c.p07.CPU_WR_MUX = mux2(b.chip.WR_C, b.p01.CPU_WR_SYNC, b.p07.MODE_DBG2);

  c.p07.CPU_RD     = not(b.p07.CPU_RD_MUX);
  c.p07.CPU_RDn    = not(b.p07.CPU_RD);
  c.p07.CPU_RD2    = not(b.p07.CPU_RDn);

  c.p07.CPU_WR     = not(b.p07.CPU_WR_MUX);
  c.p07.CPU_WRn    = not(b.p07.CPU_WR);
  c.p07.CPU_WR2    = not(b.p07.CPU_WRn);
  c.p09.CPU_RDn    = not(b.p07.CPU_RD);
  c.p09.CPU_RD1    = not(b.p09.CPU_RDn);
  c.p08.CPU_RDn    = not(b.p07.CPU_RD);
  c.p11.CPU_RDn    = not(b.p07.CPU_RD);

  //----------
  // CPU clocks

  /*BATE*/ c.p01.CPUCLK_xxxxxFxx2 = nor(b.p01.CPUCLK_REQn, b.p01.CLK_xBCDExxx2, b.p01.CLK_ABxxxxGH1);
  /*BAPY*/ c.p01.CPUCLK_xxDExxxx1 = nor(b.p01.CPUCLK_REQn, b.p01.CLK_ABxxxxGH1, b.p01.CLK_xxxxEFGH1);
  /*NULE*/ c.p01.CPUCLK_ABCDxxxx1 = nor(b.p01.CPUCLK_REQn, b.p01.CLK_xxxxEFGH1);

  /*BASU*/ c.p01.CPUCLK_xxxxxFxxn = not(b.p01.CPUCLK_xxxxxFxx2);
  /*BUKE*/ c.p01.CPUCLK_xxxxxFxx1 = not(b.p01.CPUCLK_xxxxxFxxn);

  /*BERU*/ c.p01.CPUCLK_ABxxEFGH1 = not(b.p01.CPUCLK_xxDExxxx1);
  /*BUFA*/ c.p01.CPUCLK_xxDExxxx2 = not(b.p01.CPUCLK_ABxxEFGH1);
  /*BYLY*/ c.p01.CPUCLK_xxDExxxx3 = not(b.p01.CPUCLK_ABxxEFGH1);
  /*BOLO*/ c.p01.CPUCLK_ABxxEFGH2 = not(b.p01.CPUCLK_xxDExxxx2);
  /*BYDA*/ c.p01.CPUCLK_ABxxEFGH3 = not(b.p01.CPUCLK_xxDExxxx3);

  /*BYRY*/ c.p01.CPUCLK_xxxxEFGH1 = not(b.p01.CPUCLK_ABCDxxxx1);
  /*BUDE*/ c.p01.CPUCLK_ABCDxxxx2 = not(b.p01.CPUCLK_xxxxEFGH1);
  /*BEKO*/ c.p01.CPUCLK_xxxxEFGH2 = not(b.p01.CPUCLK_ABCDxxxx2);

  /*BEVA*/ c.p01.CPUCLK_ABCDxxxx3 = not(b.p01.CPUCLK_xxxxEFGH1);
  /*BAVY*/ c.p01.CPUCLK_xxxxEFGH3 = not(b.p01.CPUCLK_ABCDxxxx3);

  /*BEJA*/ c.p01.CPUCLK_ABCDxxxx4 = nand(b.p01.CPUCLK_ABxxEFGH2,
                                         b.p01.CPUCLK_ABxxEFGH3,
                                         b.p01.CPUCLK_xxxxEFGH2,
                                         b.p01.CPUCLK_xxxxEFGH3);

  c.p01.CPUCLK_xxxxEFGH4 = not(b.p01.CPUCLK_ABCDxxxx4);
  c.p01.CPUCLK_ABCDxxxx5 = not(b.p01.CPUCLK_xxxxEFGH4);
  c.p01.CPUCLK_xxxxEFGH5 = not(b.p01.CPUCLK_ABCDxxxx5);

  c.p17.ABUR = not(b.p01.CPUCLK_xxxxxFxx1);

  // The CPUCLK_REQ cancels out here, CLK_ABCDExxx3 is not affected by it.
  c.p01.CLK_ABCDExxx3 = nand(b.p01.CLK_AxxxxFGH1, b.p01.CLK_xxxxEFGH1, b.p01.CPUCLK_xxxxEFGH5);

  c.p01.CLK_xxxxxFGH1 = not(b.p01.CLK_ABCDExxx3);

  /*p01.BYJU*/ c.p01.BYJU      = nor(b.p01.CLK_xxxxxFGH1, b.p01.CLK_BAD2);
  /*p01.BALY*/ c.p01.BALY      = not(b.p01.BYJU);
  /*p01.BOGA*/ c.p01.DIV_CLK   = not(b.p01.BALY);
  /*p01.BOMA*/ c.p01.RESET_CLK = not(b.p01.DIV_CLK);

  //----------
  // Scavenged clocks from elsewhere

  /*p15.AZEG*/ c.p15.AZEG = not(b.p01.CLK_xBxDxFxH1);
  /*p16.FABO*/ c.p16.FABO = not(b.p01.CLK_ABxxEFxx1);
  /*p27.MOXE*/ c.p27.MOXE = not(b.p01.CLK_AxCxExGx4);
  /*p27.MEHE*/ c.p27.MEHE = not(b.p01.CLK_AxCxExGx4);
  /*p27.MYVO*/ c.p27.MYVO = not(b.p01.CLK_AxCxExGx4);
  /*p27.TAVA*/ c.p27.TAVA = not(b.p01.CLK_xBxDxFxH5);
  /*p29.LAPE*/ c.p29.LAPE = not(b.p01.CLK_AxCxExGx4);
  
  /*p29.XYVA*/ c.p29.XYVA = not(b.p01.CLK_xBxDxFxH2);
  /*p29.XOTA*/ c.p29.XOTA = not(b.p29.XYVA);
  /*p29.XYFY*/ c.p29.XYFY = not(b.p29.XOTA);

  /*p29.WUVU*/ c.p29.WUVU = tock_pos(a.p29.XOTA, b.p29.XOTA, b.p01.VID_RESETn1, b.p29.WUVU, !b.p29.WUVU);
  /*p29.XUPY*/ c.p29.XUPY = not(!b.p29.WUVU);

  /*p21.VENA*/ c.p21.VENA = tock_pos(!a.p29.WUVU, !b.p29.WUVU, b.p01.VID_RESETn1, b.p21.VENA, !b.p21.VENA);
  /*p21.TALU*/ c.p21.TALU = not(!b.p21.VENA);
  /*p21.SONO*/ c.p21.SONO = not(b.p21.TALU);

  //----------
  // TO_CPU

  c.p01.BUVU = and(b.p01.CPUCLK_REQ, b.p01.BALY);
  c.p01.BYXO = not(b.p01.BUVU);
  c.p01.CPUCLK_xxxxxFGH2 = not(b.p01.BYXO);
  c.p01.CPUCLK_ABCDExxx2 = not(b.p01.CPUCLK_xxxxxFGH2);

  //----------
  // Cartridge clock

  c.p01.CPUCLK_xxxxEFGH8 = not(b.p01.CPUCLK_ABCDxxxx3);
  c.p01.CPUCLK_xxxxEFGH9 = not(b.p01.CPUCLK_ABCDxxxx3);
  c.p04.CPUCLK_ABCDxxxx9 = not(b.p01.CPUCLK_xxxxEFGH8);

  //----------
  // FF04 DIV

  c.p01.DIV_RSTn = nor(b.p01.CLK_BAD1, b.chip.RST, b.p01.DIV_WR);
  c.p01.DIV_WR   = and(b.p07.CPU_WR, b.p03.FF04_FF07, b.p08.A1n, b.p03.A0n);
  c.p01.DIV_RD   = and(b.p07.CPU_RD, b.p03.FF04_FF07, b.p08.A1n, b.p03.A0n);

  c.p01.DIV_00 = tock_pos(a.p01.DIV_CLK,  b.p01.DIV_CLK,  b.p01.DIV_RSTn, b.p01.DIV_00, !b.p01.DIV_00);
  c.p01.DIV_01 = tock_pos(!a.p01.DIV_00, !b.p01.DIV_00, b.p01.DIV_RSTn, b.p01.DIV_01, !b.p01.DIV_01);
  c.p01.DIV_02 = tock_pos(!a.p01.DIV_01, !b.p01.DIV_01, b.p01.DIV_RSTn, b.p01.DIV_02, !b.p01.DIV_02);
  c.p01.DIV_03 = tock_pos(!a.p01.DIV_02, !b.p01.DIV_02, b.p01.DIV_RSTn, b.p01.DIV_03, !b.p01.DIV_03);
  c.p01.DIV_04 = tock_pos(!a.p01.DIV_03, !b.p01.DIV_03, b.p01.DIV_RSTn, b.p01.DIV_04, !b.p01.DIV_04);
  c.p01.DIV_05 = tock_pos(!a.p01.DIV_04, !b.p01.DIV_04, b.p01.DIV_RSTn, b.p01.DIV_05, !b.p01.DIV_05);

  c.p01.ULUR = mux2(b.p01.DIV_CLK, !b.p01.DIV_05, b.p07.FF60_1);

  c.p01.DIV_06 = tock_pos( a.p01.ULUR,     b.p01.ULUR,  b.p01.DIV_RSTn, b.p01.DIV_06, !b.p01.DIV_06);
  c.p01.DIV_07 = tock_pos(!a.p01.DIV_06, !b.p01.DIV_06, b.p01.DIV_RSTn, b.p01.DIV_07, !b.p01.DIV_07);
  c.p01.DIV_08 = tock_pos(!a.p01.DIV_07, !b.p01.DIV_07, b.p01.DIV_RSTn, b.p01.DIV_08, !b.p01.DIV_08);
  c.p01.DIV_09 = tock_pos(!a.p01.DIV_08, !b.p01.DIV_08, b.p01.DIV_RSTn, b.p01.DIV_09, !b.p01.DIV_09);
  c.p01.DIV_10 = tock_pos(!a.p01.DIV_09, !b.p01.DIV_09, b.p01.DIV_RSTn, b.p01.DIV_10, !b.p01.DIV_10);
  c.p01.DIV_11 = tock_pos(!a.p01.DIV_10, !b.p01.DIV_10, b.p01.DIV_RSTn, b.p01.DIV_11, !b.p01.DIV_11);
  c.p01.DIV_12 = tock_pos(!a.p01.DIV_11, !b.p01.DIV_11, b.p01.DIV_RSTn, b.p01.DIV_12, !b.p01.DIV_12);
  c.p01.DIV_13 = tock_pos(!a.p01.DIV_12, !b.p01.DIV_12, b.p01.DIV_RSTn, b.p01.DIV_13, !b.p01.DIV_13);
  c.p01.DIV_14 = tock_pos(!a.p01.DIV_13, !b.p01.DIV_13, b.p01.DIV_RSTn, b.p01.DIV_14, !b.p01.DIV_14);
  c.p01.DIV_15 = tock_pos(!a.p01.DIV_14, !b.p01.DIV_14, b.p01.DIV_RSTn, b.p01.DIV_15, !b.p01.DIV_15);

  c.p01.DIV_06n = not(b.p01.DIV_06);
  c.p01.DIV_07n = not(b.p01.DIV_07);
  c.p01.DIV_08n = not(b.p01.DIV_08);
  c.p01.DIV_09n = not(b.p01.DIV_09);
  c.p01.DIV_10n = not(b.p01.DIV_10);
  c.p01.DIV_11n = not(b.p01.DIV_11);
  c.p01.DIV_12n = not(b.p01.DIV_12);
  c.p01.DIV_13n = not(b.p01.DIV_13);

  c.p01.DIV_D0 = not(b.p01.DIV_06n);
  c.p01.DIV_D1 = not(b.p01.DIV_07n);
  c.p01.DIV_D2 = not(b.p01.DIV_08n);
  c.p01.DIV_D3 = not(b.p01.DIV_09n);
  c.p01.DIV_D4 = not(b.p01.DIV_10n);
  c.p01.DIV_D5 = not(b.p01.DIV_11n);
  c.p01.DIV_D6 = not(b.p01.DIV_12n);
  c.p01.DIV_D7 = not(b.p01.DIV_13n);

  if (b.p01.DIV_RD) {
    c.D0 = b.p01.DIV_D0;
    c.D1 = b.p01.DIV_D1;
    c.D2 = b.p01.DIV_D2;
    c.D3 = b.p01.DIV_D3;
    c.D4 = b.p01.DIV_D4;
    c.D5 = b.p01.DIV_D5;
    c.D6 = b.p01.DIV_D6;
    c.D7 = b.p01.DIV_D7;
  }

  c.p01.CLK_16K = not(b.p01.DIV_05);


  //----------
  // APU clocks

  c.p01.AJER_2M = tock_pos(a.p01.CLK_AxCxExGx1,
                           b.p01.CLK_AxCxExGx1,
                           b.p09.APU_RESETn3,
                           b.p01.AJER_2M,
                           !b.p01.AJER_2M);

  c.p09.BATA = not(b.p01.AJER_2M);
  c.p09.CALO = tock_pos(a.p09.BATA, b.p09.BATA, b.p09.APU_RESETn, b.p09.CALO, !b.p09.CALO);
  c.p01.DYFA_1M = not(!b.p09.CALO);

  c.p01.CLK_AxCxExGx7 = not(b.p01.CLK_xBxDxFxH1);
  c.p01.CLK_ABxxEFxx1 = tock_pos(a.p01.CLK_AxCxExGx7,
                                 b.p01.CLK_AxCxExGx7,
                                 b.p01.APU_RESETn3,
                                  b.p01.CLK_ABxxEFxx1,
                                 !b.p01.CLK_ABxxEFxx1);

  c.p01.COKE = not(b.p01.AJER_2M);
  c.p01.BARA = tock_pos( a.p01.COKE,  b.p01.COKE, b.p01.APU_RESETn2, b.p01.BARA,  b.p01.DIV_10n);
  c.p01.CARU = tock_pos( a.p01.BURE,  b.p01.BURE, b.p01.APU_RESETn2, b.p01.CARU, !b.p01.CARU);
  c.p01.BYLU = tock_pos(!a.p01.CARU, !b.p01.CARU, b.p01.APU_RESETn2, b.p01.BYLU, !b.p01.BYLU);

  c.p01.BURE = not(!b.p01.BARA);

  c.p01.FYNE = not(b.p01.BURE);
  c.p01.CULO = not(!b.p01.CARU);
  c.p01.APEF = not(!b.p01.BYLU);

  c.p01.GALE = mux2(b.p01.APUCLK_512Kn, b.p01.FYNE, b.p09.FERO);
  c.p01.BEZE = mux2(b.p01.APUCLK_512Kn, b.p01.CULO, b.p09.FERO);
  c.p01.BULE = mux2(b.p01.APUCLK_512Kn, b.p01.APEF, b.p09.FERO);

  c.p01.GEXY = not(b.p01.GALE);
  c.p01.COFU = not(b.p01.BEZE);
  c.p01.BARU = not(b.p01.BULE);

  c.p01.HORU_512 = not(b.p01.GEXY);
  c.p01.BUFY_256 = not(b.p01.COFU);
  c.p01.BYFE_128 = not(b.p01.BARU);

  c.p20.ALOP = not(b.p01.BYFE_128);

  c.p01.ATYK = tock_pos(a.p01.CLK_AxCxExGx2, b.p01.CLK_AxCxExGx2, b.p01.APU_RESETn1,  b.p01.ATYK, !b.p01.ATYK);
  c.p01.AVOK = tock_pos(a.p01.ATYK,          b.p01.ATYK,          b.p01.APU_RESETn1,  b.p01.AVOK, !b.p01.AVOK);
  c.p01.JESO_512K = tock_pos(a.p01.BAVU_1M,          b.p01.BAVU_1M,          b.p09.APU_RESETn5, b.p01.JESO_512K, !b.p01.JESO_512K);
  c.p01.BAVU_1M = not(b.p01.AVOK);
  c.p01.APUCLK_512Kn = not(!b.p01.JESO_512K);
}
