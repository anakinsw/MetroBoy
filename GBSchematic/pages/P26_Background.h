#pragma once

struct Gameboy;

struct P26_Background {
public:

  static void tick(const Gameboy& a, const Gameboy& b, Gameboy& c);

  bool VAVA;

public:

  bool AXAD,ASUL,ACEN,BEJE,BAFY;
  bool FAFO_S,EMUX_S,ECAB_S,ETAM_S,DOTO_S,DABA_S,EFYK_S,EJOK_S;
  bool FAFO_C,EMUX_C,ECAB_C,ETAM_C,DOTO_C,DABA_C,EFYK_C,EJOK_C;
  bool ASUM,EVAD,DAHU,DODE,DUHO,CASE,CYPO,CETA,DAFE;

  bool ATAD_S,BEHU_S,APYH_S,BABE_S,ABOD_S,BEWY_S,BYCA_S,ACUL_S;
  bool ATAD_C,BEHU_C,APYH_C,BABE_C,ABOD_C,BEWY_C,BYCA_C,ACUL_C;
  bool AMUV,COXO,COVE,AXEP,AFEB,ALEL,COLY,AJAN;

  bool XOGA,XURA,TAJO,XENU,XYKE,XABA,TAFU,XUHO;
  bool TEDE,XALA,TYRA,XYRU,XUKU,XELY,TYKO,TUWU;
  bool WOKA,WEDE,TUFO,WEVO,WEDY,WUJA,TENA,WUBU;
  bool VEZO,WURU,VOSA,WYFU,XETE,WODA,VUMO;
};



