.include "header.inc"

main:
  ld a, $80
  ldh (TMA), a
  ld a, $FC
  ldh (TIMA), a
  ld a, %00000101
  ldh ($07), a
  xor a
  ldh ($04), a

  nops 6
  ldh a, (TIMA)
  add $D5
  
end:
  ld ($8000), a
  jr end
