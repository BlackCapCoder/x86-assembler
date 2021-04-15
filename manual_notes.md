Instruction format:

Name  Prefixes | (REX, VEX, EVEX) Prefix | Opcode | ModR/M | SIB | Displacement | Immediate
Size  1/each   | (1,   2/3, 4   )        | 1/2/3  | 1      | 1   | 1/2/4        | 1/2/4

An instruction cannot exceed 15 bytes in total.

REX Prefix:
  - Only available in IA-32E (64-bit) mode

Displacement and Immediate are represented as
little endian encoded signed integers.


### Prefixes

AMP = Also used as mandatory prefix
#   = Prefix group

| # | Code   | Name            | AMP        |
| - | ------ | --------------- | ---------- |
| 1 | F0     | LOCK            |            |
| 1 | F2  *1 | REPNE/REPNZ     | yes        |
| 1 | F3  *1 | REP/REPE/REPZ   | yes *2     |
| 1 | F2  *3 | BND             |            |
|---|--------|-----------------|------------|
|              Segment overrides            |
|---|--------|-----------------|------------|
| # | Code   | Register        | AMP        |
|---|--------|-----------------|------------|
| 2 | 2E     | CS              |            |
| 2 | 36     | SS              |            |
| 2 | 3E     | DS              |            |
| 2 | 26     | ES              |            |
| 2 | 64     | FS              |            |
| 2 | 65     | GS              |            |
|---|--------|-----------------|------------|
|              Branch hints                 |
|     (only used with Jcc instruction)      |
|---|--------|-----------------|------------|
| # | Code   | Brach taken?    | AMP        |
|---|--------|-----------------|------------|
| 2 | 2E     | Not taken       |            |
| 2 | 3E     | Taken           |            |
|---|--------|-----------------|------------|
|               Size overrides              |
|---|--------|-----------------|------------|
| # | Code   | Override what?  | AMP        |
| - |--------|------------------------------|
| 3 | 66     | Operand-size    | yes *4     |
| 4 | 67     | Address-size    |            |

*1
  Only applies to string and IO instructions
  (MOVS, CMPS, SCAS, LODS, STOS, INS, OUTS)

*2
  popcnt, lzcnt, adox

*3
  Requires that:
    * CPUID.(EAX=07, ECX=0):EAX.MPX[bit 14] is set
    * BNDCFGU.EN and/or IA32_BNDCFGS.EN is set
    * When the F2 prefix precedes one of:
      - near CALL
      - near RET
      - near JMP
      - near Jcc
      - short Jcc

*4:
  "Some" SSE2/SSE3/SSSE3/SSE4 instructions, or instructions
  using a three-byte opcode


### REX Prefix

Used to:
  - Specify GPRs and SSE registers
  - Specify 64-bit operand size
  - Specify extended control registers

Fields:

Name    Padding | W | R | X | B
Size    4       | 1 | 1 | 1 | 1


* `Padding` is always 0100

* `W` specifies addressing-size:

| 66 prefix | W | Description                     |
|-----------|---|---------------------------------|
| no        | 0 | Operand size determined by CS.D |
| no        | 1 | 64-bit operand size             |
| yes       | 0 | 16-bit operand size             |
| yes       | 1 | 64-bit operand size             |


* R, X and B extend other fields:

| SIB used | Addressing         | R          | X         | B          |
|----------|--------------------|------------|-----------|------------|
| no       | Memory             | ModR/M reg | -         | ModR/M r/m |
| no       | Reg-Reg            | ModR/M reg | -         | ModR/M r/m |
| yes      | Memory             | ModR/M reg | SIB index | SIB base   |
| -        | Reg in opcode byte | -          | -         | Opcode reg |

! In some special cases these are ignored !


### VEX Prefix

* Will #UD if used with a LOCK, 66, F2, F3 or REX prefix,

2 bytes: First byte is C5
3 bytes: First byte is C4



### Opcodes

* 1-3 bytes
* 3-bit opcode field in ModR/M byte may be used

Format for general-purpose and SIMD instructions:

  [Mandatory prefix 66/F2/F3] (Escape opcode 0F) (1 or 2 opcodes)

? Always 0F if 2 or 3 bytes?



### ModR/M

ModR/M = addressing-form specifier byte


Fields:

NAME  Mod | Reg/Opcode | R/M
SIZE  2   | 3          | 3

* Reg/Opcode
Specifies either a register number, or three more bits of
opcode information.
The purpose of this field is specified in the primary opcode.

* Mod
Combines with r/m field to form 32 possible values:
  8 regisers and 24 addressing modes

* R/M
Either:
  - Specifies a register as an operand, or
  - combines with mod field to encode an addressing mode

Some combinations of mod + r/m are used to
express opcode information for some instructions.



### SIB

Extension addressing-byte of ModR/M, required for 32-bit:
  - base-plus-index
  - scale-plus-index

Fields:

NAME  Scale | Index | Base
SIZE  2     | 3     | 3

| Name  | Description                           |
|-------|---------------------------------------|
| Scale | Scale factor                          |
| Index | Register number of the index register |
| Base  | Register number of the base register  |



### 64-bit mics

- moffset
- 64-bit immediates
- RIP-relative addressing

Default 64-bit operand size:
- near branches
- all instructions, except far branches, that implicitly referene the RSP


