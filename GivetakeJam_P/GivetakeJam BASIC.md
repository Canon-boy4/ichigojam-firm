# GivetakeJam BASIC 1.6.1 Specification

- IchigoJam BASIC 1.6.1 command:  full compatible
- Program storage: 25 slots
- Program size (LIST): 4096 bytes
- Extended array variables:
  - [0..101]  : VAR (#800-#8CA)
  - [102..357]: VAR2 (#C00-#DFE)
- CLV/CLEAR also clears VAR2 correctly
- Built-in HELP memory map updated:
  #000 CHAR
  #700 PCG
  #800 VAR
  #900 VRAM
  #C00 VAR2
  #E00 LIST
