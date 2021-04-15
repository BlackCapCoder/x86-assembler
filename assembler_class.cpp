#include "assembler.h"
#include <cstdio>

#define p_2_4(A) pref_group2(A); pref_group4(A);

#define rex0(...) rex(__VA_ARGS__, (uint8_t)0x00);
#define rexw(...) rex(__VA_ARGS__, rex_w());
#define _rexw() rex(rex_w());
#define opimm(I,...) opcode(__VA_ARGS__); disp_imm(I);
#define F2() pref_group1((uint8_t) 0xf2);
#define F3() pref_group1((uint8_t) 0xf3);

  // rex(arg1,arg0,rex_w());


inline void Assembler::meta (const Al&, const Imm8& imm, const uint8_t code)
{
  opcode   (code);
  disp_imm (imm);
}
inline void Assembler::meta (const Eax&, const Imm8& imm, const uint8_t code)
{
  opcode   (code);
  disp_imm (imm);
}
inline void Assembler::meta (const Eax&, const Imm32& imm, const uint8_t code)
{
  opcode   (code);
  disp_imm (imm);
}
inline void Assembler::meta (const Rax&, const Imm32& imm, const uint8_t code)
{
  _rexw();
  opcode   (code);
  disp_imm (imm);
}
inline void Assembler::meta (const Ax&, const Imm8& imm, const uint8_t code)
{
  pref_group3();
  opcode   (code);
  disp_imm (imm);
}
inline void Assembler::meta (const Ax&, const Imm16& imm, const uint8_t code)
{
  pref_group3();
  opcode   (code);
  disp_imm (imm);
}

// #include <iostream>
template <unsigned Sra, unsigned Srb, class...Cs>
inline void Assembler::meta(const SReg<Sra>& arg0, const SReg<Srb>& arg1, const Cs...cs)
{
// std::cout << "Hello" << std::endl;
  if constexpr (Sra == 16) pref_group3();
  if constexpr (Sra <  64) rex0(arg0,arg1);
  if constexpr (Sra >= 64) rexw(arg0,arg1);
  opcode(cs...);
  mod_rm_sib(arg0,arg1);
}

template <unsigned Sr, unsigned Si, class...Cs>
inline void Assembler::meta(const SReg<Sr>& arg0, const Imm<Si>& arg1, const uint8_t ix, const Cs...cs)
{
  if constexpr (Sr == 16) pref_group3();
  if constexpr (Sr <  64) rex0(arg0);
  if constexpr (Sr >= 64) rexw(arg0);
  opcode(cs...);
  mod_rm_sib(arg0,Constants::r64s()[ix]);
  disp_imm(arg1);
}

template <unsigned Sm, unsigned Sr, class...Cs>
inline void Assembler::meta (const Mem<Sm>& arg0, const SReg<Sr>& arg1, const Cs...cs)
{
  p_2_4(arg0);
  meta (arg0.reg, arg1, cs...);
}

template <unsigned Sm, unsigned Si, class...Cs>
inline void Assembler::meta(const Mem<Sm>& arg0, const Imm<Si>& arg1, const uint8_t ix, const Cs...cs)
{
  p_2_4(arg0);
  meta (arg0.reg, arg1, ix, cs...);
}





void Assembler::addpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  addpd(arg0, arg1.reg);
}

void Assembler::addpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x58);
  mod_rm_sib(arg1,arg0);
}

void Assembler::addps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  addps(arg0, arg1.reg);
}

void Assembler::addps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x58);
  mod_rm_sib(arg1,arg0);
}

void Assembler::addsd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  addsd(arg0, arg1.reg);
}

void Assembler::addsd(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x58);
  mod_rm_sib(arg1,arg0);
}

void Assembler::addss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  addss(arg0, arg1.reg);
}

void Assembler::addss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x58);
  mod_rm_sib(arg1,arg0);
}

void Assembler::addsubpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  addsubpd(arg0, arg1.reg);
}

void Assembler::addsubpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::addsubps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  addsubps(arg0, arg1.reg);
}

void Assembler::addsubps(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0xd0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::aesdec(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  aesdec(arg0, arg1.reg);
}

void Assembler::aesdec(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0xde);
  mod_rm_sib(arg1,arg0);
}

void Assembler::aesdeclast(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  aesdeclast(arg0, arg1.reg);
}

void Assembler::aesdeclast(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0xdf);
  mod_rm_sib(arg1,arg0);
}

void Assembler::aesenc(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  aesenc(arg0, arg1.reg);
}

void Assembler::aesenc(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0xdc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::aesenclast(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  aesenclast(arg0, arg1.reg);
}

void Assembler::aesenclast(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0xdd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::aesimc(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  aesimc(arg0, arg1.reg);
}

void Assembler::aesimc(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0xdb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::aeskeygenassist(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  aeskeygenassist(arg0, arg1.reg, arg2);
}

void Assembler::aeskeygenassist(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0xdf);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::andn(const R32& arg0, const R32& arg1, const M32& arg2) {
  p_2_4(arg2);
  andn(arg0, arg1, arg2.reg);
}

void Assembler::andn(const R32& arg0, const R32& arg1, const R32& arg2) {
  vex(0x02,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0xf2);
  mod_rm_sib(arg2,arg0);
}

void Assembler::andn(const R64& arg0, const R64& arg1, const M64& arg2) {
  p_2_4(arg2);
  andn(arg0, arg1, arg2.reg);
}

void Assembler::andn(const R64& arg0, const R64& arg1, const R64& arg2) {
  vex(0x02,0x0,0x0,0x1,arg1,arg2,arg0);
  opcode(0xf2);
  mod_rm_sib(arg2,arg0);
}

void Assembler::andnpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  andnpd(arg0, arg1.reg);
}

void Assembler::andnpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x55);
  mod_rm_sib(arg1,arg0);
}

void Assembler::andnps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  andnps(arg0, arg1.reg);
}

void Assembler::andnps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x55);
  mod_rm_sib(arg1,arg0);
}

void Assembler::andpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  andpd(arg0, arg1.reg);
}

void Assembler::andpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x54);
  mod_rm_sib(arg1,arg0);
}

void Assembler::andps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  andps(arg0, arg1.reg);
}

void Assembler::andps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x54);
  mod_rm_sib(arg1,arg0);
}

void Assembler::bextr(const R32& arg0, const M32& arg1, const R32& arg2) {
  p_2_4(arg1);
  bextr(arg0, arg1.reg, arg2);
}

void Assembler::bextr(const R32& arg0, const R32& arg1, const R32& arg2) {
  vex(0x02,0x0,0x0,0x0,arg2,arg1,arg0);
  opcode(0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::bextr(const R64& arg0, const M64& arg1, const R64& arg2) {
  p_2_4(arg1);
  bextr(arg0, arg1.reg, arg2);
}

void Assembler::bextr(const R64& arg0, const R64& arg1, const R64& arg2) {
  vex(0x02,0x0,0x0,0x1,arg2,arg1,arg0);
  opcode(0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::blendpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  blendpd(arg0, arg1.reg, arg2);
}

void Assembler::blendpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0d);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::blendps(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  blendps(arg0, arg1.reg, arg2);
}

void Assembler::blendps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0c);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::blendvpd(const Xmm& arg0, const M128& arg1, const Xmm0& arg2) {
  p_2_4(arg1);
  blendvpd(arg0, arg1.reg, arg2);
}

void Assembler::blendvpd(const Xmm& arg0, const Xmm& arg1, const Xmm0& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x15);
  mod_rm_sib(arg1,arg0);
}

void Assembler::blendvps(const Xmm& arg0, const M128& arg1, const Xmm0& arg2) {
  p_2_4(arg1);
  blendvps(arg0, arg1.reg, arg2);
}

void Assembler::blendvps(const Xmm& arg0, const Xmm& arg1, const Xmm0& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x14);
  mod_rm_sib(arg1,arg0);
}

void Assembler::blsi(const R32& arg0, const M32& arg1) {
  p_2_4(arg1);
  blsi(arg0, arg1.reg);
}

void Assembler::blsi(const R32& arg0, const R32& arg1) {
  vex(0x02,0x0,0x0,0x0,arg0,arg1,Constants::r64s()[3]);
  opcode(0xf3);
  mod_rm_sib(arg1,Constants::r64s()[3]);
}

void Assembler::blsi(const R64& arg0, const M64& arg1) {
  p_2_4(arg1);
  blsi(arg0, arg1.reg);
}

void Assembler::blsi(const R64& arg0, const R64& arg1) {
  vex(0x02,0x0,0x0,0x1,arg0,arg1,Constants::r64s()[3]);
  opcode(0xf3);
  mod_rm_sib(arg1,Constants::r64s()[3]);
}

void Assembler::blsmsk(const R32& arg0, const M32& arg1) {
  p_2_4(arg1);
  blsmsk(arg0, arg1.reg);
}

void Assembler::blsmsk(const R32& arg0, const R32& arg1) {
  vex(0x02,0x0,0x0,0x0,arg0,arg1,Constants::r64s()[2]);
  opcode(0xf3);
  mod_rm_sib(arg1,Constants::r64s()[2]);
}

void Assembler::blsmsk(const R64& arg0, const M64& arg1) {
  p_2_4(arg1);
  blsmsk(arg0, arg1.reg);
}

void Assembler::blsmsk(const R64& arg0, const R64& arg1) {
  vex(0x02,0x0,0x0,0x1,arg0,arg1,Constants::r64s()[2]);
  opcode(0xf3);
  mod_rm_sib(arg1,Constants::r64s()[2]);
}

void Assembler::blsr(const R32& arg0, const M32& arg1) {
  p_2_4(arg1);
  blsr(arg0, arg1.reg);
}

void Assembler::blsr(const R32& arg0, const R32& arg1) {
  vex(0x02,0x0,0x0,0x0,arg0,arg1,Constants::r64s()[1]);
  opcode(0xf3);
  mod_rm_sib(arg1,Constants::r64s()[1]);
}

void Assembler::blsr(const R64& arg0, const M64& arg1) {
  p_2_4(arg1);
  blsr(arg0, arg1.reg);
}

void Assembler::blsr(const R64& arg0, const R64& arg1) {
  vex(0x02,0x0,0x0,0x1,arg0,arg1,Constants::r64s()[1]);
  opcode(0xf3);
  mod_rm_sib(arg1,Constants::r64s()[1]);
}

void Assembler::bswap(const R32& arg0) {
  rex0(arg0);
  opcode(0x0f,0xc8,arg0);
}

void Assembler::bswap(const R64& arg0) {
  rexw(arg0);
  opcode(0x0f,0xc8,arg0);
}

void Assembler::bzhi(const R32& arg0, const M32& arg1, const R32& arg2) {
  p_2_4(arg1);
  bzhi(arg0, arg1.reg, arg2);
}

void Assembler::bzhi(const R32& arg0, const R32& arg1, const R32& arg2) {
  vex(0x02,0x0,0x0,0x0,arg2,arg1,arg0);
  opcode(0xf5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::bzhi(const R64& arg0, const M64& arg1, const R64& arg2) {
  p_2_4(arg1);
  bzhi(arg0, arg1.reg, arg2);
}

void Assembler::bzhi(const R64& arg0, const R64& arg1, const R64& arg2) {
  vex(0x02,0x0,0x0,0x1,arg2,arg1,arg0);
  opcode(0xf5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::call(const FarPtr1616& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::call(const FarPtr1632& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::call(const FarPtr1664& arg0) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::call(Label& arg0) {
  opcode(0xe8);
  disp_label32(arg0);
}

void Assembler::call(const M64& arg0) {
  p_2_4(arg0);
  call(arg0.reg);
}

void Assembler::call(const R64& arg0) {
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::call(const Rel32& arg0) {
  opcode(0xe8);
  disp_imm(arg0);
}

void Assembler::cbw() {
  pref_group3();
  opcode(0x98);
}

void Assembler::cdq() {
  opcode(0x99);
}

void Assembler::cdqe() {
  _rexw();
  opcode(0x98);
}

void Assembler::clc() {
  opcode(0xf8);
}

void Assembler::cld() {
  opcode(0xfc);
}

void Assembler::clflush(const M8& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::cli() {
  opcode(0xfa);
}

void Assembler::cmc() {
  opcode(0xf5);
}

void Assembler::cmppd(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  cmppd(arg0, arg1.reg, arg2);
}

void Assembler::cmppd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xc2);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::cmpps(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  cmpps(arg0, arg1.reg, arg2);
}

void Assembler::cmpps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  rex0(arg1,arg0);
  opcode(0x0f,0xc2);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::cmps(const M16& arg0, const M16& arg1) {
  p_2_4(arg0);
  pref_group3();
  opcode(0xa7);
}

void Assembler::cmps(const M32& arg0, const M32& arg1) {
  p_2_4(arg0);
  opcode(0xa7);
}

void Assembler::cmps(const M64& arg0, const M64& arg1) {
  p_2_4(arg0);
  _rexw();
  opcode(0xa7);
}

void Assembler::cmps(const M8& arg0, const M8& arg1) {
  p_2_4(arg0);
  opcode(0xa6);
}

void Assembler::cmpsb() {
  opcode(0xa6);
}

void Assembler::cmpsd() {
  opcode(0xa7);
}

void Assembler::cmpsd(const Xmm& arg0, const M64& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  cmpsd(arg0, arg1.reg, arg2);
}

void Assembler::cmpsd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0xc2);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::cmpsq() {
  _rexw();
  opcode(0xa7);
}

void Assembler::cmpss(const Xmm& arg0, const M32& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  cmpss(arg0, arg1.reg, arg2);
}

void Assembler::cmpss(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xc2);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::cmpsw() {
  pref_group3();
  opcode(0xa7);
}

void Assembler::cmpxchg16b(const M128& arg0) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0x0f,0xc7);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::cmpxchg8b(const M64& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0xc7);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::comisd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::comisd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::comiss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x2f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::comiss(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x2f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cpuid() {
  opcode(0x0f,0xa2);
}

void Assembler::cqo() {
  _rexw();
  opcode(0x99);
}

void Assembler::crc32(const R32& arg0, const M16& arg1) {
  p_2_4(arg1);
  crc32(arg0, arg1.reg);
}

void Assembler::crc32(const R32& arg0, const M32& arg1) {
  p_2_4(arg1);
  crc32(arg0, arg1.reg);
}

void Assembler::crc32(const R32& arg0, const M8& arg1) {
  p_2_4(arg1);
  crc32(arg0, arg1.reg);
}

void Assembler::crc32(const R32& arg0, const R16& arg1) {
  pref_group3();
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0xf1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::crc32(const R32& arg0, const R32& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0xf1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::crc32(const R32& arg0, const R8& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0xf0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::crc32(const R32& arg0, const Rh& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0xf0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::crc32(const R64& arg0, const M64& arg1) {
  p_2_4(arg1);
  crc32(arg0, arg1.reg);
}

void Assembler::crc32(const R64& arg0, const M8& arg1) {
  p_2_4(arg1);
  crc32(arg0, arg1.reg);
}

void Assembler::crc32(const R64& arg0, const R64& arg1) {
  F2();
  rexw(arg1,arg0);
  opcode(0x0f,0x38,0xf1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::crc32(const R64& arg0, const R8& arg1) {
  F2();
  rexw(arg1,arg0);
  opcode(0x0f,0x38,0xf0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtdq2pd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtdq2pd(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtdq2ps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  cvtdq2ps(arg0, arg1.reg);
}

void Assembler::cvtdq2ps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtpd2dq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  cvtpd2dq(arg0, arg1.reg);
}

void Assembler::cvtpd2dq(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtpd2pi(const Mm& arg0, const M128& arg1) {
  p_2_4(arg1);
  cvtpd2pi(arg0, arg1.reg);
}

void Assembler::cvtpd2pi(const Mm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtpd2ps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  cvtpd2ps(arg0, arg1.reg);
}

void Assembler::cvtpd2ps(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtpi2pd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtpi2pd(const Xmm& arg0, const Mm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtpi2ps(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtpi2ps(const Xmm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtps2dq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  cvtps2dq(arg0, arg1.reg);
}

void Assembler::cvtps2dq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtps2pd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtps2pd(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtps2pi(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  cvtps2pi(arg0, arg1.reg);
}

void Assembler::cvtps2pi(const Mm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtsd2si(const R32& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtsd2si(const R32& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtsd2si(const R64& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rexw(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtsd2si(const R64& arg0, const Xmm& arg1) {
  F2();
  rexw(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtsd2ss(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  cvtsd2ss(arg0, arg1.reg);
}

void Assembler::cvtsd2ss(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtsi2sd(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  cvtsi2sd(arg0, arg1.reg);
}

void Assembler::cvtsi2sd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  cvtsi2sd(arg0, arg1.reg);
}

void Assembler::cvtsi2sd(const Xmm& arg0, const R32& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtsi2sd(const Xmm& arg0, const R64& arg1) {
  F2();
  rexw(arg1,arg0);
  opcode(0x0f,0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtsi2ss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  cvtsi2ss(arg0, arg1.reg);
}

void Assembler::cvtsi2ss(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  cvtsi2ss(arg0, arg1.reg);
}

void Assembler::cvtsi2ss(const Xmm& arg0, const R32& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtsi2ss(const Xmm& arg0, const R64& arg1) {
  F3();
  rexw(arg1,arg0);
  opcode(0x0f,0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtss2sd(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtss2sd(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtss2si(const R32& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtss2si(const R32& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtss2si(const R64& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rexw(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvtss2si(const R64& arg0, const Xmm& arg1) {
  F3();
  rexw(arg1,arg0);
  opcode(0x0f,0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttpd2dq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  cvttpd2dq(arg0, arg1.reg);
}

void Assembler::cvttpd2dq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttpd2pi(const Mm& arg0, const M128& arg1) {
  p_2_4(arg1);
  cvttpd2pi(arg0, arg1.reg);
}

void Assembler::cvttpd2pi(const Mm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttps2dq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  cvttps2dq(arg0, arg1.reg);
}

void Assembler::cvttps2dq(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttps2pi(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttps2pi(const Mm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttsd2si(const R32& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttsd2si(const R32& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttsd2si(const R64& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rexw(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttsd2si(const R64& arg0, const Xmm& arg1) {
  F2();
  rexw(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttss2si(const R32& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttss2si(const R32& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttss2si(const R64& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rexw(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cvttss2si(const R64& arg0, const Xmm& arg1) {
  F3();
  rexw(arg1,arg0);
  opcode(0x0f,0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::cwd() {
  pref_group3();
  opcode(0x99);
}

void Assembler::cwde() {
  opcode(0x98);
}

void Assembler::dec(const M16& arg0) {
  p_2_4(arg0);
  dec(arg0.reg);
}

void Assembler::dec(const M32& arg0) {
  p_2_4(arg0);
  dec(arg0.reg);
}

void Assembler::dec(const M64& arg0) {
  p_2_4(arg0);
  dec(arg0.reg);
}

void Assembler::dec(const M8& arg0) {
  p_2_4(arg0);
  dec(arg0.reg);
}

void Assembler::dec(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::dec(const R32& arg0) {
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::dec(const R64& arg0) {
  rexw(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::dec(const R8& arg0) {
  rex0(arg0);
  opcode(0xfe);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::dec(const Rh& arg0) {
  rex0(arg0);
  opcode(0xfe);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::div(const M16& arg0) {
  p_2_4(arg0);
  div(arg0.reg);
}

void Assembler::div(const M32& arg0) {
  p_2_4(arg0);
  div(arg0.reg);
}

void Assembler::div(const M64& arg0) {
  p_2_4(arg0);
  div(arg0.reg);
}

void Assembler::div(const M8& arg0) {
  p_2_4(arg0);
  div(arg0.reg);
}

void Assembler::div(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::div(const R32& arg0) {
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::div(const R64& arg0) {
  rexw(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::div(const R8& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::div(const Rh& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::divpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  divpd(arg0, arg1.reg);
}

void Assembler::divpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::divps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  divps(arg0, arg1.reg);
}

void Assembler::divps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x5e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::divsd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x5e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::divsd(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x5e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::divss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::divss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::dppd(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  dppd(arg0, arg1.reg, arg2);
}

void Assembler::dppd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x41);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::dpps(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  dpps(arg0, arg1.reg, arg2);
}

void Assembler::dpps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x40);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::emms() {
  opcode(0x0f,0x77);
}

void Assembler::enter(const Imm8& arg0, const Imm16& arg1) {
  opcode(0xc8);
  disp_imm(arg0,arg1);
}

void Assembler::enter(const One& arg0, const Imm16& arg1) {
  opcode(0xc8);
  disp_imm(arg0,arg1);
}

void Assembler::enter(const Zero& arg0, const Imm16& arg1) {
  opcode(0xc8);
  disp_imm(arg0,arg1);
}

void Assembler::extractps(const M32& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  extractps(arg0.reg, arg1, arg2);
}

void Assembler::extractps(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x3a,0x17);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::extractps(const R64& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x3a,0x17);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::f2xm1() {
  opcode(0xd9,0xf0);
}

void Assembler::fabs() {
  opcode(0xd9,0xe1);
}

void Assembler::fadd(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd8);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::fadd(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdc);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::fadd(const St& arg0, const St0& arg1) {
  opcode(0xdc,0xc0,arg0);
}

void Assembler::fadd(const St0& arg0, const St& arg1) {
  opcode(0xd8,0xc0,arg1);
}

void Assembler::faddp() {
  opcode(0xde,0xc1);
}

void Assembler::faddp(const St& arg0, const St0& arg1) {
  opcode(0xde,0xc0,arg0);
}

void Assembler::fbld(const M80Bcd& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdf);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::fbstp(const M80Bcd& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdf);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::fchs() {
  opcode(0xd9,0xe0);
}

void Assembler::fclex() {
  pref_fwait(0x9b);
  opcode(0xdb,0xe2);
}

void Assembler::fcmovb(const St0& arg0, const St& arg1) {
  opcode(0xda,0xc0,arg1);
}

void Assembler::fcmovbe(const St0& arg0, const St& arg1) {
  opcode(0xda,0xd0,arg1);
}

void Assembler::fcmove(const St0& arg0, const St& arg1) {
  opcode(0xda,0xc8,arg1);
}

void Assembler::fcmovnb(const St0& arg0, const St& arg1) {
  opcode(0xdb,0xc0,arg1);
}

void Assembler::fcmovnbe(const St0& arg0, const St& arg1) {
  opcode(0xdb,0xd0,arg1);
}

void Assembler::fcmovne(const St0& arg0, const St& arg1) {
  opcode(0xdb,0xc8,arg1);
}

void Assembler::fcmovnu(const St0& arg0, const St& arg1) {
  opcode(0xdb,0xd8,arg1);
}

void Assembler::fcmovu(const St0& arg0, const St& arg1) {
  opcode(0xda,0xd8,arg1);
}

void Assembler::fcom() {
  opcode(0xd8,0xd1);
}

void Assembler::fcom(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd8);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::fcom(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdc);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::fcom(const St& arg0) {
  rex0(arg0);
  opcode(0xd8,0xd0,arg0);
}

void Assembler::fcomi(const St0& arg0, const St& arg1) {
  opcode(0xdb,0xf0,arg1);
}

void Assembler::fcomip(const St0& arg0, const St& arg1) {
  opcode(0xdf,0xf0,arg1);
}

void Assembler::fcomp() {
  opcode(0xd8,0xd9);
}

void Assembler::fcomp(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd8);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::fcomp(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdc);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::fcomp(const St& arg0) {
  rex0(arg0);
  opcode(0xd8,0xd8,arg0);
}

void Assembler::fcompp() {
  opcode(0xde,0xd9);
}

void Assembler::fcos() {
  opcode(0xd9,0xff);
}

void Assembler::fdecstp() {
  opcode(0xd9,0xf6);
}

void Assembler::fdiv(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd8);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::fdiv(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdc);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::fdiv(const St& arg0, const St0& arg1) {
  opcode(0xdc,0xf0,arg0);
}

void Assembler::fdiv(const St0& arg0, const St& arg1) {
  opcode(0xd8,0xf0,arg1);
}

void Assembler::fdivp() {
  opcode(0xde,0xf1);
}

void Assembler::fdivp(const St& arg0, const St0& arg1) {
  opcode(0xde,0xf0,arg0);
}

void Assembler::fdivr(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd8);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fdivr(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdc);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fdivr(const St& arg0, const St0& arg1) {
  opcode(0xdc,0xf8,arg0);
}

void Assembler::fdivr(const St0& arg0, const St& arg1) {
  opcode(0xd8,0xf8,arg1);
}

void Assembler::fdivrp() {
  opcode(0xde,0xf9);
}

void Assembler::fdivrp(const St& arg0, const St0& arg1) {
  opcode(0xde,0xf8,arg0);
}

void Assembler::ffree(const St& arg0) {
  rex0(arg0);
  opcode(0xdd,0xc0,arg0);
}

void Assembler::fiadd(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xde);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::fiadd(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xda);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::ficom(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xde);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::ficom(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xda);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::ficomp(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xde);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::ficomp(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xda);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::fidiv(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xde);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::fidiv(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xda);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::fidivr(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xde);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fidivr(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xda);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fild(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdf);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::fild(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdb);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::fild(const M64Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdf);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::fimul(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xde);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::fimul(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xda);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::fincstp() {
  opcode(0xd9,0xf7);
}

void Assembler::finit() {
  pref_fwait(0x9b);
  opcode(0xdb,0xe3);
}

void Assembler::fist(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdf);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::fist(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdb);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::fistp(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdf);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::fistp(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdb);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::fistp(const M64Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdf);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fisttp(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdf);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::fisttp(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdb);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::fisttp(const M64Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdd);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::fisub(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xde);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::fisub(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xda);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::fisubr(const M16Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xde);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::fisubr(const M32Int& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xda);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::fld(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd9);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::fld(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdd);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::fld(const M80Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdb);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::fld(const St& arg0) {
  rex0(arg0);
  opcode(0xd9,0xc0,arg0);
}

void Assembler::fld1() {
  opcode(0xd9,0xe8);
}

void Assembler::fldcw(const M2Byte& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd9);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::fldenv(const M28Byte& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd9);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::fldl2e() {
  opcode(0xd9,0xea);
}

void Assembler::fldl2t() {
  opcode(0xd9,0xe9);
}

void Assembler::fldlg2() {
  opcode(0xd9,0xec);
}

void Assembler::fldln2() {
  opcode(0xd9,0xed);
}

void Assembler::fldpi() {
  opcode(0xd9,0xeb);
}

void Assembler::fldz() {
  opcode(0xd9,0xee);
}

void Assembler::fmul(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd8);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::fmul(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdc);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::fmul(const St& arg0, const St0& arg1) {
  opcode(0xdc,0xc8,arg0);
}

void Assembler::fmul(const St0& arg0, const St& arg1) {
  opcode(0xd8,0xc8,arg1);
}

void Assembler::fmulp() {
  opcode(0xde,0xc9);
}

void Assembler::fmulp(const St& arg0, const St0& arg1) {
  opcode(0xde,0xc8,arg0);
}

void Assembler::fnclex() {
  opcode(0xdb,0xe2);
}

void Assembler::fninit() {
  opcode(0xdb,0xe3);
}

void Assembler::fnop() {
  opcode(0xd9,0xd0);
}

void Assembler::fnsave(const M108Byte& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdd);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::fnstcw(const M2Byte& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd9);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fnstenv(const M28Byte& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd9);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::fnstsw(const Ax& arg0) {
  opcode(0xdf,0xe0);
}

void Assembler::fnstsw(const M2Byte& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdd);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fpatan() {
  opcode(0xd9,0xf3);
}

void Assembler::fprem() {
  opcode(0xd9,0xf8);
}

void Assembler::fprem1() {
  opcode(0xd9,0xf5);
}

void Assembler::fptan() {
  opcode(0xd9,0xf2);
}

void Assembler::frndint() {
  opcode(0xd9,0xfc);
}

void Assembler::frstor(const M108Byte& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdd);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::fsave(const M108Byte& arg0) {
  pref_fwait(0x9b);
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdd);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::fscale() {
  opcode(0xd9,0xfd);
}

void Assembler::fsin() {
  opcode(0xd9,0xfe);
}

void Assembler::fsincos() {
  opcode(0xd9,0xfb);
}

void Assembler::fsqrt() {
  opcode(0xd9,0xfa);
}

void Assembler::fst(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd9);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::fst(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdd);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::fst(const St& arg0) {
  rex0(arg0);
  opcode(0xdd,0xd0,arg0);
}

void Assembler::fstcw(const M2Byte& arg0) {
  pref_fwait(0x9b);
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd9);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fstenv(const M28Byte& arg0) {
  pref_fwait(0x9b);
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd9);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::fstp(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd9);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::fstp(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdd);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::fstp(const M80Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdb);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fstp(const St& arg0) {
  rex0(arg0);
  opcode(0xdd,0xd8,arg0);
}

void Assembler::fstsw(const Ax& arg0) {
  pref_fwait(0x9b);
  opcode(0xdf,0xe0);
}

void Assembler::fstsw(const M2Byte& arg0) {
  pref_fwait(0x9b);
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdd);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::fsub(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd8);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::fsub(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdc);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::fsub(const St& arg0, const St0& arg1) {
  opcode(0xdc,0xe0,arg0);
}

void Assembler::fsub(const St0& arg0, const St& arg1) {
  opcode(0xd8,0xe0,arg1);
}

void Assembler::fsubp() {
  opcode(0xde,0xe1);
}

void Assembler::fsubp(const St& arg0, const St0& arg1) {
  opcode(0xde,0xe0,arg0);
}

void Assembler::fsubr(const M32Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd8);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::fsubr(const M64Fp& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xdc);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::fsubr(const St& arg0, const St0& arg1) {
  opcode(0xdc,0xe8,arg0);
}

void Assembler::fsubr(const St0& arg0, const St& arg1) {
  opcode(0xd8,0xe8,arg1);
}

void Assembler::fsubrp() {
  opcode(0xde,0xe9);
}

void Assembler::fsubrp(const St& arg0, const St0& arg1) {
  opcode(0xde,0xe8,arg0);
}

void Assembler::ftst() {
  opcode(0xd9,0xe4);
}

void Assembler::fucom() {
  opcode(0xdd,0xe1);
}

void Assembler::fucom(const St& arg0) {
  rex0(arg0);
  opcode(0xdd,0xe0,arg0);
}

void Assembler::fucomi(const St0& arg0, const St& arg1) {
  opcode(0xdb,0xe8,arg1);
}

void Assembler::fucomip(const St0& arg0, const St& arg1) {
  opcode(0xdf,0xe8,arg1);
}

void Assembler::fucomp() {
  opcode(0xdd,0xe9);
}

void Assembler::fucomp(const St& arg0) {
  rex0(arg0);
  opcode(0xdd,0xe8,arg0);
}

void Assembler::fucompp() {
  opcode(0xda,0xe9);
}

void Assembler::fwait() {
  pref_fwait(0x9b);
}

void Assembler::fxam() {
  opcode(0xd9,0xe5);
}

void Assembler::fxch() {
  opcode(0xd9,0xc9);
}

void Assembler::fxch(const St& arg0) {
  rex0(arg0);
  opcode(0xd9,0xc8,arg0);
}

void Assembler::fxrstor(const M512Byte& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::fxrstor64(const M512Byte& arg0) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::fxsave(const M512Byte& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::fxsave64(const M512Byte& arg0) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::fxtract() {
  opcode(0xd9,0xf4);
}

void Assembler::fyl2x() {
  opcode(0xd9,0xf1);
}

void Assembler::fyl2xp1() {
  opcode(0xd9,0xf9);
}

void Assembler::haddpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  haddpd(arg0, arg1.reg);
}

void Assembler::haddpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x7c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::haddps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  haddps(arg0, arg1.reg);
}

void Assembler::haddps(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x7c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::hsubpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  hsubpd(arg0, arg1.reg);
}

void Assembler::hsubpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x7d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::hsubps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  hsubps(arg0, arg1.reg);
}

void Assembler::hsubps(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x7d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::idiv(const M16& arg0) {
  p_2_4(arg0);
  idiv(arg0.reg);
}

void Assembler::idiv(const M32& arg0) {
  p_2_4(arg0);
  idiv(arg0.reg);
}

void Assembler::idiv(const M64& arg0) {
  p_2_4(arg0);
  idiv(arg0.reg);
}

void Assembler::idiv(const M8& arg0) {
  p_2_4(arg0);
  idiv(arg0.reg);
}

void Assembler::idiv(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::idiv(const R32& arg0) {
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::idiv(const R64& arg0) {
  rexw(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::idiv(const R8& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::idiv(const Rh& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::imul(const M16& arg0) {
  p_2_4(arg0);
  imul(arg0.reg);
}

void Assembler::imul(const M32& arg0) {
  p_2_4(arg0);
  imul(arg0.reg);
}

void Assembler::imul(const M64& arg0) {
  p_2_4(arg0);
  imul(arg0.reg);
}

void Assembler::imul(const M8& arg0) {
  p_2_4(arg0);
  imul(arg0.reg);
}

void Assembler::imul(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::imul(const R16& arg0, const M16& arg1, const Imm16& arg2) {
  p_2_4(arg1);
  imul(arg0, arg1.reg, arg2);
}

void Assembler::imul(const R16& arg0, const M16& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  imul(arg0, arg1.reg, arg2);
}

void Assembler::imul(const R16& arg0, const R16& arg1, const Imm16& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x69);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::imul(const R16& arg0, const R16& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x6b);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::imul(const R32& arg0) {
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::imul(const R32& arg0, const M32& arg1, const Imm32& arg2) {
  p_2_4(arg1);
  imul(arg0, arg1.reg, arg2);
}

void Assembler::imul(const R32& arg0, const M32& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  imul(arg0, arg1.reg, arg2);
}

void Assembler::imul(const R32& arg0, const R32& arg1, const Imm32& arg2) {
  rex0(arg1,arg0);
  opcode(0x69);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::imul(const R32& arg0, const R32& arg1, const Imm8& arg2) {
  rex0(arg1,arg0);
  opcode(0x6b);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::imul(const R64& arg0) {
  rexw(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::imul(const R64& arg0, const M64& arg1, const Imm32& arg2) {
  p_2_4(arg1);
  imul(arg0, arg1.reg, arg2);
}

void Assembler::imul(const R64& arg0, const M64& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  imul(arg0, arg1.reg, arg2);
}

void Assembler::imul(const R64& arg0, const R64& arg1, const Imm32& arg2) {
  rexw(arg1,arg0);
  opcode(0x69);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::imul(const R64& arg0, const R64& arg1, const Imm8& arg2) {
  rexw(arg1,arg0);
  opcode(0x6b);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::imul(const R8& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::imul(const Rh& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::in(const Al& arg0, const Dx& arg1) {
  opcode(0xec);
}

void Assembler::in(const Ax& arg0, const Dx& arg1) {
  pref_group3();
  opcode(0xed);
}

void Assembler::in(const Eax& arg0, const Dx& arg1) {
  opcode(0xed);
}

void Assembler::inc(const M16& arg0) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::inc(const M32& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::inc(const M64& arg0) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::inc(const M8& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xfe);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}


void Assembler::inc(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::inc(const R32& arg0) {
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::inc(const R64& arg0) {
  rexw(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::inc(const R8& arg0) {
  rex0(arg0);
  opcode(0xfe);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::inc(const Rh& arg0) {
  rex0(arg0);
  opcode(0xfe);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::ins(const M16& arg0, const Dx& arg1) {
  p_2_4(arg0);
  pref_group3();
  opcode(0x6d);
}

void Assembler::ins(const M32& arg0, const Dx& arg1) {
  p_2_4(arg0);
  opcode(0x6d);
}

void Assembler::ins(const M8& arg0, const Dx& arg1) {
  p_2_4(arg0);
  opcode(0x6c);
}

void Assembler::insb() {
  opcode(0x6c);
}

void Assembler::insd() {
  opcode(0x6d);
}

void Assembler::insertps(const Xmm& arg0, const M32& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x21);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::insertps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x21);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::insw() {
  pref_group3();
  opcode(0x6d);
}

void Assembler::int_(const Imm8& arg0) {
  opcode(0xcd);
  disp_imm(arg0);
}

void Assembler::int_(const Three& arg0) {
  opcode(0xcc);
}

void Assembler::iret() {
  pref_group3();
  opcode(0xcf);
}

void Assembler::iretd() {
  opcode(0xcf);
}

void Assembler::iretq() {
  _rexw();
  opcode(0xcf);
}

void Assembler::ja(Label& arg0) {
  opcode(0x77);
  disp_label8(arg0);
}

void Assembler::ja_1(Label& arg0) {
  opcode(0x0f,0x87);
  disp_label32(arg0);
}

void Assembler::ja(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x77);
  disp_label8(arg0);
}

void Assembler::ja_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x87);
  disp_label32(arg0);
}

void Assembler::ja(const Rel32& arg0) {
  opcode(0x0f,0x87);
  disp_imm(arg0);
}

void Assembler::ja(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x87);
  disp_imm(arg0);
}

void Assembler::ja(const Rel8& arg0) {
  opcode(0x77);
  disp_imm(arg0);
}

void Assembler::ja(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x77);
  disp_imm(arg0);
}

void Assembler::jae(Label& arg0) {
  opcode(0x73);
  disp_label8(arg0);
}

void Assembler::jae_1(Label& arg0) {
  opcode(0x0f,0x83);
  disp_label32(arg0);
}

void Assembler::jae(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x73);
  disp_label8(arg0);
}

void Assembler::jae_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x83);
  disp_label32(arg0);
}

void Assembler::jae(const Rel32& arg0) {
  opcode(0x0f,0x83);
  disp_imm(arg0);
}

void Assembler::jae(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x83);
  disp_imm(arg0);
}

void Assembler::jae(const Rel8& arg0) {
  opcode(0x73);
  disp_imm(arg0);
}

void Assembler::jae(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x73);
  disp_imm(arg0);
}

void Assembler::jb(Label& arg0) {
  opcode(0x72);
  disp_label8(arg0);
}

void Assembler::jb_1(Label& arg0) {
  opcode(0x0f,0x82);
  disp_label32(arg0);
}

void Assembler::jb(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x72);
  disp_label8(arg0);
}

void Assembler::jb_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x82);
  disp_label32(arg0);
}

void Assembler::jb(const Rel32& arg0) {
  opcode(0x0f,0x82);
  disp_imm(arg0);
}

void Assembler::jb(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x82);
  disp_imm(arg0);
}

void Assembler::jb(const Rel8& arg0) {
  opcode(0x72);
  disp_imm(arg0);
}

void Assembler::jb(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x72);
  disp_imm(arg0);
}

void Assembler::jbe(Label& arg0) {
  opcode(0x76);
  disp_label8(arg0);
}

void Assembler::jbe_1(Label& arg0) {
  opcode(0x0f,0x86);
  disp_label32(arg0);
}

void Assembler::jbe(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x76);
  disp_label8(arg0);
}

void Assembler::jbe_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x86);
  disp_label32(arg0);
}

void Assembler::jbe(const Rel32& arg0) {
  opcode(0x0f,0x86);
  disp_imm(arg0);
}

void Assembler::jbe(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x86);
  disp_imm(arg0);
}

void Assembler::jbe(const Rel8& arg0) {
  opcode(0x76);
  disp_imm(arg0);
}

void Assembler::jbe(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x76);
  disp_imm(arg0);
}

void Assembler::jc(Label& arg0) {
  opcode(0x72);
  disp_label8(arg0);
}

void Assembler::jc_1(Label& arg0) {
  opcode(0x0f,0x82);
  disp_label32(arg0);
}

void Assembler::jc(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x72);
  disp_label8(arg0);
}

void Assembler::jc_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x82);
  disp_label32(arg0);
}

void Assembler::jc(const Rel32& arg0) {
  opcode(0x0f,0x82);
  disp_imm(arg0);
}

void Assembler::jc(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x82);
  disp_imm(arg0);
}

void Assembler::jc(const Rel8& arg0) {
  opcode(0x72);
  disp_imm(arg0);
}

void Assembler::jc(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x72);
  disp_imm(arg0);
}

void Assembler::je(Label& arg0) {
  opcode(0x74);
  disp_label8(arg0);
}

void Assembler::je_1(Label& arg0) {
  opcode(0x0f,0x84);
  disp_label32(arg0);
}

void Assembler::je(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x74);
  disp_label8(arg0);
}

void Assembler::je_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x84);
  disp_label32(arg0);
}

void Assembler::je(const Rel32& arg0) {
  opcode(0x0f,0x84);
  disp_imm(arg0);
}

void Assembler::je(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x84);
  disp_imm(arg0);
}

void Assembler::je(const Rel8& arg0) {
  opcode(0x74);
  disp_imm(arg0);
}

void Assembler::je(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x74);
  disp_imm(arg0);
}

void Assembler::jecxz(Label& arg0) {
  opcode(0xe3);
  disp_label8(arg0);
}

void Assembler::jecxz(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0xe3);
  disp_label8(arg0);
}

void Assembler::jecxz(const Rel8& arg0) {
  opcode(0xe3);
  disp_imm(arg0);
}

void Assembler::jecxz(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0xe3);
  disp_imm(arg0);
}

void Assembler::jg(Label& arg0) {
  opcode(0x7f);
  disp_label8(arg0);
}

void Assembler::jg_1(Label& arg0) {
  opcode(0x0f,0x8f);
  disp_label32(arg0);
}

void Assembler::jg(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7f);
  disp_label8(arg0);
}

void Assembler::jg_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8f);
  disp_label32(arg0);
}

void Assembler::jg(const Rel32& arg0) {
  opcode(0x0f,0x8f);
  disp_imm(arg0);
}

void Assembler::jg(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8f);
  disp_imm(arg0);
}

void Assembler::jg(const Rel8& arg0) {
  opcode(0x7f);
  disp_imm(arg0);
}

void Assembler::jg(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7f);
  disp_imm(arg0);
}

void Assembler::jge(Label& arg0) {
  opcode(0x7d);
  disp_label8(arg0);
}

void Assembler::jge_1(Label& arg0) {
  opcode(0x0f,0x8d);
  disp_label32(arg0);
}

void Assembler::jge(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7d);
  disp_label8(arg0);
}

void Assembler::jge_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8d);
  disp_label32(arg0);
}

void Assembler::jge(const Rel32& arg0) {
  opcode(0x0f,0x8d);
  disp_imm(arg0);
}

void Assembler::jge(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8d);
  disp_imm(arg0);
}

void Assembler::jge(const Rel8& arg0) {
  opcode(0x7d);
  disp_imm(arg0);
}

void Assembler::jge(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7d);
  disp_imm(arg0);
}

void Assembler::jl(Label& arg0) {
  opcode(0x7c);
  disp_label8(arg0);
}

void Assembler::jl_1(Label& arg0) {
  opcode(0x0f,0x8c);
  disp_label32(arg0);
}

void Assembler::jl(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7c);
  disp_label8(arg0);
}

void Assembler::jl_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8c);
  disp_label32(arg0);
}

void Assembler::jl(const Rel32& arg0) {
  opcode(0x0f,0x8c);
  disp_imm(arg0);
}

void Assembler::jl(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8c);
  disp_imm(arg0);
}

void Assembler::jl(const Rel8& arg0) {
  opcode(0x7c);
  disp_imm(arg0);
}

void Assembler::jl(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7c);
  disp_imm(arg0);
}

void Assembler::jle(Label& arg0) {
  opcode(0x7e);
  disp_label8(arg0);
}

void Assembler::jle_1(Label& arg0) {
  opcode(0x0f,0x8e);
  disp_label32(arg0);
}

void Assembler::jle(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7e);
  disp_label8(arg0);
}

void Assembler::jle_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8e);
  disp_label32(arg0);
}

void Assembler::jle(const Rel32& arg0) {
  opcode(0x0f,0x8e);
  disp_imm(arg0);
}

void Assembler::jle(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8e);
  disp_imm(arg0);
}

void Assembler::jle(const Rel8& arg0) {
  opcode(0x7e);
  disp_imm(arg0);
}

void Assembler::jle(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7e);
  disp_imm(arg0);
}

void Assembler::jmp(const FarPtr1616& arg0) {
  p_2_4(arg0);
  opcode(0xff);
}

void Assembler::jmp(const FarPtr1632& arg0) {
  p_2_4(arg0);
  opcode(0xff);
}

void Assembler::jmp(const FarPtr1664& arg0) {
  p_2_4(arg0);
  _rexw();
  opcode(0xff);
}

void Assembler::jmp(Label& arg0) {
  opcode(0xeb);
  disp_label8(arg0);
}

void Assembler::jmp_1(Label& arg0) {
  opcode(0xe9);
  disp_label32(arg0);
}

void Assembler::jmp(const M64& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::jmp(const R64& arg0) {
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::jmp(const Rel32& arg0) {
  opcode(0xe9);
  disp_imm(arg0);
}

void Assembler::jmp(const Rel8& arg0) {
  opcode(0xeb);
  disp_imm(arg0);
}

void Assembler::jna(Label& arg0) {
  opcode(0x76);
  disp_label8(arg0);
}

void Assembler::jna_1(Label& arg0) {
  opcode(0x0f,0x86);
  disp_label32(arg0);
}

void Assembler::jna(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x76);
  disp_label8(arg0);
}

void Assembler::jna_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x86);
  disp_label32(arg0);
}

void Assembler::jna(const Rel32& arg0) {
  opcode(0x0f,0x86);
  disp_imm(arg0);
}

void Assembler::jna(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x86);
  disp_imm(arg0);
}

void Assembler::jna(const Rel8& arg0) {
  opcode(0x76);
  disp_imm(arg0);
}

void Assembler::jna(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x76);
  disp_imm(arg0);
}

void Assembler::jnae(Label& arg0) {
  opcode(0x72);
  disp_label8(arg0);
}

void Assembler::jnae_1(Label& arg0) {
  opcode(0x0f,0x82);
  disp_label32(arg0);
}

void Assembler::jnae(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x72);
  disp_label8(arg0);
}

void Assembler::jnae_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x82);
  disp_label32(arg0);
}

void Assembler::jnae(const Rel32& arg0) {
  opcode(0x0f,0x82);
  disp_imm(arg0);
}

void Assembler::jnae(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x82);
  disp_imm(arg0);
}

void Assembler::jnae(const Rel8& arg0) {
  opcode(0x72);
  disp_imm(arg0);
}

void Assembler::jnae(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x72);
  disp_imm(arg0);
}

void Assembler::jnb(Label& arg0) {
  opcode(0x73);
  disp_label8(arg0);
}

void Assembler::jnb_1(Label& arg0) {
  opcode(0x0f,0x83);
  disp_label32(arg0);
}

void Assembler::jnb(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x73);
  disp_label8(arg0);
}

void Assembler::jnb_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x83);
  disp_label32(arg0);
}

void Assembler::jnb(const Rel32& arg0) {
  opcode(0x0f,0x83);
  disp_imm(arg0);
}

void Assembler::jnb(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x83);
  disp_imm(arg0);
}

void Assembler::jnb(const Rel8& arg0) {
  opcode(0x73);
  disp_imm(arg0);
}

void Assembler::jnb(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x73);
  disp_imm(arg0);
}

void Assembler::jnbe(Label& arg0) {
  opcode(0x77);
  disp_label8(arg0);
}

void Assembler::jnbe_1(Label& arg0) {
  opcode(0x0f,0x87);
  disp_label32(arg0);
}

void Assembler::jnbe(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x77);
  disp_label8(arg0);
}

void Assembler::jnbe_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x87);
  disp_label32(arg0);
}

void Assembler::jnbe(const Rel32& arg0) {
  opcode(0x0f,0x87);
  disp_imm(arg0);
}

void Assembler::jnbe(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x87);
  disp_imm(arg0);
}

void Assembler::jnbe(const Rel8& arg0) {
  opcode(0x77);
  disp_imm(arg0);
}

void Assembler::jnbe(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x77);
  disp_imm(arg0);
}

void Assembler::jnc(Label& arg0) {
  opcode(0x73);
  disp_label8(arg0);
}

void Assembler::jnc_1(Label& arg0) {
  opcode(0x0f,0x83);
  disp_label32(arg0);
}

void Assembler::jnc(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x73);
  disp_label8(arg0);
}

void Assembler::jnc_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x83);
  disp_label32(arg0);
}

void Assembler::jnc(const Rel32& arg0) {
  opcode(0x0f,0x83);
  disp_imm(arg0);
}

void Assembler::jnc(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x83);
  disp_imm(arg0);
}

void Assembler::jnc(const Rel8& arg0) {
  opcode(0x73);
  disp_imm(arg0);
}

void Assembler::jnc(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x73);
  disp_imm(arg0);
}

void Assembler::jne(Label& arg0) {
  opcode(0x75);
  disp_label8(arg0);
}

void Assembler::jne_1(Label& arg0) {
  opcode(0x0f,0x85);
  disp_label32(arg0);
}

void Assembler::jne(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x75);
  disp_label8(arg0);
}

void Assembler::jne_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x85);
  disp_label32(arg0);
}

void Assembler::jne(const Rel32& arg0) {
  opcode(0x0f,0x85);
  disp_imm(arg0);
}

void Assembler::jne(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x85);
  disp_imm(arg0);
}

void Assembler::jne(const Rel8& arg0) {
  opcode(0x75);
  disp_imm(arg0);
}

void Assembler::jne(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x75);
  disp_imm(arg0);
}

void Assembler::jng(Label& arg0) {
  opcode(0x7e);
  disp_label8(arg0);
}

void Assembler::jng_1(Label& arg0) {
  opcode(0x0f,0x8e);
  disp_label32(arg0);
}

void Assembler::jng(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7e);
  disp_label8(arg0);
}

void Assembler::jng_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8e);
  disp_label32(arg0);
}

void Assembler::jng(const Rel32& arg0) {
  opcode(0x0f,0x8e);
  disp_imm(arg0);
}

void Assembler::jng(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8e);
  disp_imm(arg0);
}

void Assembler::jng(const Rel8& arg0) {
  opcode(0x7e);
  disp_imm(arg0);
}

void Assembler::jng(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7e);
  disp_imm(arg0);
}

void Assembler::jnge(Label& arg0) {
  opcode(0x7c);
  disp_label8(arg0);
}

void Assembler::jnge_1(Label& arg0) {
  opcode(0x0f,0x8c);
  disp_label32(arg0);
}

void Assembler::jnge(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7c);
  disp_label8(arg0);
}

void Assembler::jnge_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8c);
  disp_label32(arg0);
}

void Assembler::jnge(const Rel32& arg0) {
  opcode(0x0f,0x8c);
  disp_imm(arg0);
}

void Assembler::jnge(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8c);
  disp_imm(arg0);
}

void Assembler::jnge(const Rel8& arg0) {
  opcode(0x7c);
  disp_imm(arg0);
}

void Assembler::jnge(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7c);
  disp_imm(arg0);
}

void Assembler::jnl(Label& arg0) {
  opcode(0x7d);
  disp_label8(arg0);
}

void Assembler::jnl_1(Label& arg0) {
  opcode(0x0f,0x8d);
  disp_label32(arg0);
}

void Assembler::jnl(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7d);
  disp_label8(arg0);
}

void Assembler::jnl_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8d);
  disp_label32(arg0);
}

void Assembler::jnl(const Rel32& arg0) {
  opcode(0x0f,0x8d);
  disp_imm(arg0);
}

void Assembler::jnl(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8d);
  disp_imm(arg0);
}

void Assembler::jnl(const Rel8& arg0) {
  opcode(0x7d);
  disp_imm(arg0);
}

void Assembler::jnl(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7d);
  disp_imm(arg0);
}

void Assembler::jnle(Label& arg0) {
  opcode(0x7f);
  disp_label8(arg0);
}

void Assembler::jnle_1(Label& arg0) {
  opcode(0x0f,0x8f);
  disp_label32(arg0);
}

void Assembler::jnle(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7f);
  disp_label8(arg0);
}

void Assembler::jnle_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8f);
  disp_label32(arg0);
}

void Assembler::jnle(const Rel32& arg0) {
  opcode(0x0f,0x8f);
  disp_imm(arg0);
}

void Assembler::jnle(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8f);
  disp_imm(arg0);
}

void Assembler::jnle(const Rel8& arg0) {
  opcode(0x7f);
  disp_imm(arg0);
}

void Assembler::jnle(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7f);
  disp_imm(arg0);
}

void Assembler::jno(Label& arg0) {
  opcode(0x71);
  disp_label8(arg0);
}

void Assembler::jno_1(Label& arg0) {
  opcode(0x0f,0x81);
  disp_label32(arg0);
}

void Assembler::jno(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x71);
  disp_label8(arg0);
}

void Assembler::jno_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x81);
  disp_label32(arg0);
}

void Assembler::jno(const Rel32& arg0) {
  opcode(0x0f,0x81);
  disp_imm(arg0);
}

void Assembler::jno(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x81);
  disp_imm(arg0);
}

void Assembler::jno(const Rel8& arg0) {
  opcode(0x71);
  disp_imm(arg0);
}

void Assembler::jno(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x71);
  disp_imm(arg0);
}

void Assembler::jnp(Label& arg0) {
  opcode(0x7b);
  disp_label8(arg0);
}

void Assembler::jnp_1(Label& arg0) {
  opcode(0x0f,0x8b);
  disp_label32(arg0);
}

void Assembler::jnp(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7b);
  disp_label8(arg0);
}

void Assembler::jnp_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8b);
  disp_label32(arg0);
}

void Assembler::jnp(const Rel32& arg0) {
  opcode(0x0f,0x8b);
  disp_imm(arg0);
}

void Assembler::jnp(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8b);
  disp_imm(arg0);
}

void Assembler::jnp(const Rel8& arg0) {
  opcode(0x7b);
  disp_imm(arg0);
}

void Assembler::jnp(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7b);
  disp_imm(arg0);
}

void Assembler::jns(Label& arg0) {
  opcode(0x79);
  disp_label8(arg0);
}

void Assembler::jns_1(Label& arg0) {
  opcode(0x0f,0x89);
  disp_label32(arg0);
}

void Assembler::jns(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x79);
  disp_label8(arg0);
}

void Assembler::jns_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x89);
  disp_label32(arg0);
}

void Assembler::jns(const Rel32& arg0) {
  opcode(0x0f,0x89);
  disp_imm(arg0);
}

void Assembler::jns(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x89);
  disp_imm(arg0);
}

void Assembler::jns(const Rel8& arg0) {
  opcode(0x79);
  disp_imm(arg0);
}

void Assembler::jns(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x79);
  disp_imm(arg0);
}

void Assembler::jnz(Label& arg0) {
  opcode(0x75);
  disp_label8(arg0);
}

void Assembler::jnz_1(Label& arg0) {
  opcode(0x0f,0x85);
  disp_label32(arg0);
}

void Assembler::jnz(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x75);
  disp_label8(arg0);
}

void Assembler::jnz_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x85);
  disp_label32(arg0);
}

void Assembler::jnz(const Rel32& arg0) {
  opcode(0x0f,0x85);
  disp_imm(arg0);
}

void Assembler::jnz(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x85);
  disp_imm(arg0);
}

void Assembler::jnz(const Rel8& arg0) {
  opcode(0x75);
  disp_imm(arg0);
}

void Assembler::jnz(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x75);
  disp_imm(arg0);
}

void Assembler::jo(Label& arg0) {
  opcode(0x70);
  disp_label8(arg0);
}

void Assembler::jo_1(Label& arg0) {
  opcode(0x0f,0x80);
  disp_label32(arg0);
}

void Assembler::jo(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x70);
  disp_label8(arg0);
}

void Assembler::jo_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x80);
  disp_label32(arg0);
}

void Assembler::jo(const Rel32& arg0) {
  opcode(0x0f,0x80);
  disp_imm(arg0);
}

void Assembler::jo(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x80);
  disp_imm(arg0);
}

void Assembler::jo(const Rel8& arg0) {
  opcode(0x70);
  disp_imm(arg0);
}

void Assembler::jo(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x70);
  disp_imm(arg0);
}

void Assembler::jp(Label& arg0) {
  opcode(0x7a);
  disp_label8(arg0);
}

void Assembler::jp_1(Label& arg0) {
  opcode(0x0f,0x8a);
  disp_label32(arg0);
}

void Assembler::jp(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7a);
  disp_label8(arg0);
}

void Assembler::jp_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8a);
  disp_label32(arg0);
}

void Assembler::jp(const Rel32& arg0) {
  opcode(0x0f,0x8a);
  disp_imm(arg0);
}

void Assembler::jp(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8a);
  disp_imm(arg0);
}

void Assembler::jp(const Rel8& arg0) {
  opcode(0x7a);
  disp_imm(arg0);
}

void Assembler::jp(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7a);
  disp_imm(arg0);
}

void Assembler::jpe(Label& arg0) {
  opcode(0x7a);
  disp_label8(arg0);
}

void Assembler::jpe_1(Label& arg0) {
  opcode(0x0f,0x8a);
  disp_label32(arg0);
}

void Assembler::jpe(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7a);
  disp_label8(arg0);
}

void Assembler::jpe_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8a);
  disp_label32(arg0);
}

void Assembler::jpe(const Rel32& arg0) {
  opcode(0x0f,0x8a);
  disp_imm(arg0);
}

void Assembler::jpe(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8a);
  disp_imm(arg0);
}

void Assembler::jpe(const Rel8& arg0) {
  opcode(0x7a);
  disp_imm(arg0);
}

void Assembler::jpe(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7a);
  disp_imm(arg0);
}

void Assembler::jpo(Label& arg0) {
  opcode(0x7b);
  disp_label8(arg0);
}

void Assembler::jpo_1(Label& arg0) {
  opcode(0x0f,0x8b);
  disp_label32(arg0);
}

void Assembler::jpo(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7b);
  disp_label8(arg0);
}

void Assembler::jpo_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8b);
  disp_label32(arg0);
}

void Assembler::jpo(const Rel32& arg0) {
  opcode(0x0f,0x8b);
  disp_imm(arg0);
}

void Assembler::jpo(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x8b);
  disp_imm(arg0);
}

void Assembler::jpo(const Rel8& arg0) {
  opcode(0x7b);
  disp_imm(arg0);
}

void Assembler::jpo(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x7b);
  disp_imm(arg0);
}

void Assembler::jrcxz(Label& arg0) {
  opcode(0xe3);
  disp_label8(arg0);
}

void Assembler::jrcxz(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0xe3);
  disp_label8(arg0);
}

void Assembler::jrcxz(const Rel8& arg0) {
  opcode(0xe3);
  disp_imm(arg0);
}

void Assembler::jrcxz(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0xe3);
  disp_imm(arg0);
}

void Assembler::js(Label& arg0) {
  opcode(0x78);
  disp_label8(arg0);
}

void Assembler::js_1(Label& arg0) {
  opcode(0x0f,0x88);
  disp_label32(arg0);
}

void Assembler::js(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x78);
  disp_label8(arg0);
}

void Assembler::js_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x88);
  disp_label32(arg0);
}

void Assembler::js(const Rel32& arg0) {
  opcode(0x0f,0x88);
  disp_imm(arg0);
}

void Assembler::js(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x88);
  disp_imm(arg0);
}

void Assembler::js(const Rel8& arg0) {
  opcode(0x78);
  disp_imm(arg0);
}

void Assembler::js(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x78);
  disp_imm(arg0);
}

void Assembler::jz(Label& arg0) {
  opcode(0x74);
  disp_label8(arg0);
}

void Assembler::jz_1(Label& arg0) {
  opcode(0x0f,0x84);
  disp_label32(arg0);
}

void Assembler::jz(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x74);
  disp_label8(arg0);
}

void Assembler::jz_1(Label& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x84);
  disp_label32(arg0);
}

void Assembler::jz(const Rel32& arg0) {
  opcode(0x0f,0x84);
  disp_imm(arg0);
}

void Assembler::jz(const Rel32& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x0f,0x84);
  disp_imm(arg0);
}

void Assembler::jz(const Rel8& arg0) {
  opcode(0x74);
  disp_imm(arg0);
}

void Assembler::jz(const Rel8& arg0, const Hint& arg1) {
  pref_group2(arg1);
  opcode(0x74);
  disp_imm(arg0);
}

void Assembler::lahf() {
  opcode(0x9f);
}

void Assembler::lddqu(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0xf0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::ldmxcsr(const M32& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::leave() {
  opcode(0xc9);
}

void Assembler::leave(const Pref66& arg0) {
  pref_group3();
  opcode(0xc9);
}

void Assembler::lfence() {
  opcode(0x0f,0xae,0xe8);
}

void Assembler::lfs(const R16& arg0, const FarPtr1616& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xb4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lfs(const R32& arg0, const FarPtr1632& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xb4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lfs(const R64& arg0, const FarPtr1664& arg1) {
  p_2_4(arg1);
  rexw(arg1,arg0);
  opcode(0x0f,0xb4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lgs(const R16& arg0, const FarPtr1616& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xb5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lgs(const R32& arg0, const FarPtr1632& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xb5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lgs(const R64& arg0, const FarPtr1664& arg1) {
  p_2_4(arg1);
  rexw(arg1,arg0);
  opcode(0x0f,0xb5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lock() {
  opcode(0xf0);
}

void Assembler::lods(const M16& arg0) {
  p_2_4(arg0);
  pref_group3();
  opcode(0xad);
}

void Assembler::lods(const M32& arg0) {
  p_2_4(arg0);
  opcode(0xad);
}

void Assembler::lods(const M64& arg0) {
  p_2_4(arg0);
  _rexw();
  opcode(0xad);
}

void Assembler::lods(const M8& arg0) {
  p_2_4(arg0);
  opcode(0xac);
}

void Assembler::lodsb() {
  opcode(0xac);
}

void Assembler::lodsd() {
  opcode(0xad);
}

void Assembler::lodsq() {
  _rexw();
  opcode(0xad);
}

void Assembler::lodsw() {
  pref_group3();
  opcode(0xad);
}

void Assembler::loop(Label& arg0) {
  opcode(0xe2);
  disp_label8(arg0);
}

void Assembler::loop(const Rel8& arg0) {
  opcode(0xe2);
  disp_imm(arg0);
}

void Assembler::loope(Label& arg0) {
  opcode(0xe0);
  disp_label8(arg0);
}

void Assembler::loope(const Rel8& arg0) {
  opcode(0xe0);
  disp_imm(arg0);
}

void Assembler::loopne(Label& arg0) {
  opcode(0xe0);
  disp_label8(arg0);
}

void Assembler::loopne(const Rel8& arg0) {
  opcode(0xe0);
  disp_imm(arg0);
}

void Assembler::lss(const R16& arg0, const FarPtr1616& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xb2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lss(const R32& arg0, const FarPtr1632& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xb2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lss(const R64& arg0, const FarPtr1664& arg1) {
  p_2_4(arg1);
  rexw(arg1,arg0);
  opcode(0x0f,0xb2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lzcnt(const R16& arg0, const R16& arg1) {
  pref_group3();
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xbd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lzcnt(const R32& arg0, const R32& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xbd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::lzcnt(const R64& arg0, const R64& arg1) {
  F3();
  rexw(arg1,arg0);
  opcode(0x0f,0xbd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::maskmovdqu(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::maskmovq(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::maxpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  maxpd(arg0, arg1.reg);
}

void Assembler::maxpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::maxps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  maxps(arg0, arg1.reg);
}

void Assembler::maxps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x5f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::maxsd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x5f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::maxsd(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x5f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::maxss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::maxss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::mfence() {
  opcode(0x0f,0xae,0xf0);
}

void Assembler::minpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  minpd(arg0, arg1.reg);
}

void Assembler::minpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::minps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  minps(arg0, arg1.reg);
}

void Assembler::minps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x5d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::minsd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x5d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::minsd(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x5d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::minss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::minss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::monitor() {
  opcode(0x0f,0x01,0xc8);
}

void Assembler::mov(const Al& arg0, const Moffs8& arg1) {
  opcode(0xa0);
  disp_imm(arg1);
}

void Assembler::mov(const Al& arg0, const Moffs8& arg1, const PrefRexW& arg2) {
  _rexw();
  opcode(0xa0);
  disp_imm(arg1);
}

void Assembler::mov(const Ax& arg0, const Moffs16& arg1) {
  pref_group3();
  opcode(0xa1);
  disp_imm(arg1);
}

void Assembler::mov(const Eax& arg0, const Moffs32& arg1) {
  opcode(0xa1);
  disp_imm(arg1);
}

void Assembler::mov(const M16& arg0, const Sreg& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x8c);
  mod_rm_sib(arg0,arg1);
}

void Assembler::mov(const M64& arg0, const Sreg& arg1) {
  p_2_4(arg0);
  rexw(arg0,arg1);
  opcode(0x8c);
  mod_rm_sib(arg0,arg1);
}

void Assembler::mov(const Moffs16& arg0, const Ax& arg1) {
  pref_group3();
  opcode(0xa3);
  disp_imm(arg0);
}

void Assembler::mov(const Moffs32& arg0, const Eax& arg1) {
  opcode(0xa3);
  disp_imm(arg0);
}

void Assembler::mov(const Moffs64& arg0, const Rax& arg1) {
  _rexw();
  opcode(0xa3);
  disp_imm(arg0);
}

void Assembler::mov(const Moffs8& arg0, const Al& arg1) {
  opcode(0xa2);
  disp_imm(arg0);
}

void Assembler::mov(const Moffs8& arg0, const Al& arg1, const PrefRexW& arg2) {
  _rexw();
  opcode(0xa2);
  disp_imm(arg0);
}

void Assembler::mov(const R16& arg0, const Imm16& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xb8,arg0);
  disp_imm(arg1);
}

void Assembler::mov(const R16& arg0, const Sreg& arg1) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x8c);
  mod_rm_sib(arg0,arg1);
}

void Assembler::mov(const R32& arg0, const Imm32& arg1) {
  rex0(arg0);
  opcode(0xb8,arg0);
  disp_imm(arg1);
}

void Assembler::mov(const R64& arg0, const Imm64& arg1) {
  rexw(arg0);
  opcode(0xb8,arg0);
  disp_imm(arg1);
}

void Assembler::mov(const R64& arg0, const Sreg& arg1) {
  rexw(arg0,arg1);
  opcode(0x8c);
  mod_rm_sib(arg0,arg1);
}

void Assembler::mov(const R8& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0xb0,arg0);
  disp_imm(arg1);
}

void Assembler::mov(const Rax& arg0, const Moffs64& arg1) {
  _rexw();
  opcode(0xa1);
  disp_imm(arg1);
}

void Assembler::mov(const Rh& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0xb0,arg0);
  disp_imm(arg1);
}

void Assembler::mov(const Sreg& arg0, const M16& arg1) {
  p_2_4(arg1);
  mov(arg0, arg1.reg);
}

void Assembler::mov(const Sreg& arg0, const M64& arg1) {
  p_2_4(arg1);
  mov(arg0, arg1.reg);
}

void Assembler::mov(const Sreg& arg0, const R16& arg1) {
  rex0(arg1,arg0);
  opcode(0x8e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::mov(const Sreg& arg0, const R64& arg1) {
  rexw(arg1,arg0);
  opcode(0x8e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movapd(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movapd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  movapd(arg0, arg1.reg);
}

void Assembler::movapd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x28);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movapd_1(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movaps(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  rex0(arg0,arg1);
  opcode(0x0f,0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movaps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  movaps(arg0, arg1.reg);
}

void Assembler::movaps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x28);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movaps_1(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg0,arg1);
  opcode(0x0f,0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movd(const M32& arg0, const Mm& arg1) {
  p_2_4(arg0);
  rex0(arg0,arg1);
  opcode(0x0f,0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movd(const M32& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movd(const Mm& arg0, const M32& arg1) {
  p_2_4(arg1);
  movd(arg0, arg1.reg);
}

void Assembler::movd(const Mm& arg0, const R32& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x6e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movd(const R32& arg0, const Mm& arg1) {
  rex0(arg0,arg1);
  opcode(0x0f,0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movd(const R32& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movd(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  movd(arg0, arg1.reg);
}

void Assembler::movd(const Xmm& arg0, const R32& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x6e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movddup(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movddup(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movdq2q(const Mm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0xd6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movdqa(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movdqa(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  movdqa(arg0, arg1.reg);
}

void Assembler::movdqa(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x6f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movdqa_1(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movdqu(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  F3();
  rex0(arg0,arg1);
  opcode(0x0f,0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movdqu(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  movdqu(arg0, arg1.reg);
}

void Assembler::movdqu(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x6f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movdqu_1(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg0,arg1);
  opcode(0x0f,0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movhlps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movhpd(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x17);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movhpd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x16);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movhps(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  rex0(arg0,arg1);
  opcode(0x0f,0x17);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movhps(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x16);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movlhps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x16);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movlpd(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x13);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movlpd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movlps(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  rex0(arg0,arg1);
  opcode(0x0f,0x13);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movlps(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movmskpd(const R32& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movmskpd(const R64& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movmskps(const R32& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movmskps(const R64& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movntdq(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0xe7);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movntdqa(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  movntdqa(arg0, arg1.reg);
}

void Assembler::movntpd(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x2b);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movntps(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  rex0(arg0,arg1);
  opcode(0x0f,0x2b);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movntq(const M64& arg0, const Mm& arg1) {
  p_2_4(arg0);
  rex0(arg0,arg1);
  opcode(0x0f,0xe7);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movq(const M64& arg0, const Mm& arg1) {
  p_2_4(arg0);
  rexw(arg0,arg1);
  opcode(0x0f,0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movq_1(const M64& arg0, const Mm& arg1) {
  p_2_4(arg0);
  rex0(arg0,arg1);
  opcode(0x0f,0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movq(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rexw(arg0,arg1);
  opcode(0x0f,0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movq_1(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0xd6);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movq(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  movq(arg0, arg1.reg);
}

void Assembler::movq_1(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  movq_1(arg0, arg1.reg);
}

void Assembler::movq(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x6f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movq_1(const Mm& arg0, const Mm& arg1) {
  rex0(arg0,arg1);
  opcode(0x0f,0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movq(const Mm& arg0, const R64& arg1) {
  rexw(arg1,arg0);
  opcode(0x0f,0x6e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movq(const R64& arg0, const Mm& arg1) {
  rexw(arg0,arg1);
  opcode(0x0f,0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movq(const R64& arg0, const Xmm& arg1) {
  pref_group3();
  rexw(arg0,arg1);
  opcode(0x0f,0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movq(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  movq(arg0, arg1.reg);
}

void Assembler::movq_1(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  movq_1(arg0, arg1.reg);
}

void Assembler::movq(const Xmm& arg0, const R64& arg1) {
  pref_group3();
  rexw(arg1,arg0);
  opcode(0x0f,0x6e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movq(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x7e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movq_1(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0xd6);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movq2dq(const Xmm& arg0, const Mm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movs(const M16& arg0, const M16& arg1) {
  p_2_4(arg0);
  pref_group3();
  opcode(0xa5);
}

void Assembler::movs(const M32& arg0, const M32& arg1) {
  p_2_4(arg0);
  opcode(0xa5);
}

void Assembler::movs(const M64& arg0, const M64& arg1) {
  p_2_4(arg0);
  _rexw();
  opcode(0xa5);
}

void Assembler::movs(const M8& arg0, const M8& arg1) {
  p_2_4(arg0);
  opcode(0xa4);
}

void Assembler::movsb() {
  opcode(0xa4);
}

void Assembler::movsd() {
  opcode(0xa5);
}

void Assembler::movsd(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  F2();
  rex0(arg0,arg1);
  opcode(0x0f,0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movsd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movsd(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movsd_1(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg0,arg1);
  opcode(0x0f,0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movshdup(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  movshdup(arg0, arg1.reg);
}

void Assembler::movshdup(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x16);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movsldup(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  movsldup(arg0, arg1.reg);
}

void Assembler::movsldup(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movsq() {
  _rexw();
  opcode(0xa5);
}

void Assembler::movss(const M32& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  F3();
  rex0(arg0,arg1);
  opcode(0x0f,0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movss_1(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg0,arg1);
  opcode(0x0f,0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movsw() {
  pref_group3();
  opcode(0xa5);
}

void Assembler::movupd(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movupd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  movupd(arg0, arg1.reg);
}

void Assembler::movupd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movupd_1(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movups(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  rex0(arg0,arg1);
  opcode(0x0f,0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::movups(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  movups(arg0, arg1.reg);
}

void Assembler::movups(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::movups_1(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg0,arg1);
  opcode(0x0f,0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::mpsadbw(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  mpsadbw(arg0, arg1.reg, arg2);
}

void Assembler::mpsadbw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x42);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::mul(const M16& arg0) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::mul(const M32& arg0) {
  p_2_4(arg0);
  mul(arg0.reg);
}

void Assembler::mul(const M64& arg0) {
  p_2_4(arg0);
  mul(arg0.reg);
}

void Assembler::mul(const M8& arg0) {
  p_2_4(arg0);
  mul(arg0.reg);
}

void Assembler::mul(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::mul(const R32& arg0) {
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::mul(const R64& arg0) {
  rexw(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::mul(const R8& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::mul(const Rh& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::mulpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  mulpd(arg0, arg1.reg);
}

void Assembler::mulpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::mulps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  mulps(arg0, arg1.reg);
}

void Assembler::mulps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::mulsd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::mulsd(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::mulss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::mulss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::mulx(const R32& arg0, const R32& arg1, const M32& arg2) {
  p_2_4(arg2);
  mulx(arg0, arg1, arg2.reg);
}

void Assembler::mulx(const R32& arg0, const R32& arg1, const R32& arg2) {
  vex(0x02,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0xf6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::mulx(const R64& arg0, const R64& arg1, const M64& arg2) {
  p_2_4(arg2);
  mulx(arg0, arg1, arg2.reg);
}

void Assembler::mulx(const R64& arg0, const R64& arg1, const R64& arg2) {
  vex(0x02,0x0,0x3,0x1,arg1,arg2,arg0);
  opcode(0xf6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::mwait() {
  opcode(0x0f,0x01,0xc9);
}

void Assembler::neg(const M16& arg0) {
  p_2_4(arg0);
  neg(arg0.reg);
}

void Assembler::neg(const M32& arg0) {
  p_2_4(arg0);
  neg(arg0.reg);
}

void Assembler::neg(const M64& arg0) {
  p_2_4(arg0);
  neg(arg0.reg);
}

void Assembler::neg(const M8& arg0) {
  p_2_4(arg0);
  neg(arg0.reg);
}

void Assembler::neg(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::neg(const R32& arg0) {
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::neg(const R64& arg0) {
  rexw(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::neg(const R8& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::neg(const Rh& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::nop() {
  opcode(0x90);
}

void Assembler::nop(const M16& arg0) {
  p_2_4(arg0);
  nop(arg0.reg);
}

void Assembler::nop(const M32& arg0) {
  p_2_4(arg0);
  nop(arg0.reg);
}

void Assembler::nop(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x1f);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::nop(const R32& arg0) {
  rex0(arg0);
  opcode(0x0f,0x1f);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::not_(const M16& arg0) {
  p_2_4(arg0);
  not_(arg0.reg);
}

void Assembler::not_(const M32& arg0) {
  p_2_4(arg0);
  not_(arg0.reg);
}

void Assembler::not_(const M64& arg0) {
  p_2_4(arg0);
  not_(arg0.reg);
}

void Assembler::not_(const M8& arg0) {
  p_2_4(arg0);
  not_(arg0.reg);
}

void Assembler::not_(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::not_(const R32& arg0) {
  rex0(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::not_(const R64& arg0) {
  rexw(arg0);
  opcode(0xf7);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::not_(const R8& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::not_(const Rh& arg0) {
  rex0(arg0);
  opcode(0xf6);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::orpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  orpd(arg0, arg1.reg);
}

void Assembler::orpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x56);
  mod_rm_sib(arg1,arg0);
}

void Assembler::orps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  orps(arg0, arg1.reg);
}

void Assembler::orps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x56);
  mod_rm_sib(arg1,arg0);
}

void Assembler::out(const Dx& arg0, const Al& arg1) {
  opcode(0xee);
}

void Assembler::out(const Dx& arg0, const Ax& arg1) {
  pref_group3();
  opcode(0xef);
}

void Assembler::out(const Dx& arg0, const Eax& arg1) {
  opcode(0xef);
}

void Assembler::out(const Imm8& arg0, const Al& arg1) {
  opcode(0xe6);
  disp_imm(arg0);
}

void Assembler::out(const Imm8& arg0, const Ax& arg1) {
  pref_group3();
  opcode(0xe7);
  disp_imm(arg0);
}

void Assembler::out(const Imm8& arg0, const Eax& arg1) {
  opcode(0xe7);
  disp_imm(arg0);
}

void Assembler::outs(const Dx& arg0, const M16& arg1) {
  p_2_4(arg1);
  pref_group3();
  opcode(0x6f);
}

void Assembler::outs(const Dx& arg0, const M32& arg1) {
  p_2_4(arg1);
  opcode(0x6f);
}

void Assembler::outs(const Dx& arg0, const M8& arg1) {
  p_2_4(arg1);
  opcode(0x6e);
}

void Assembler::outsb() {
  opcode(0x6e);
}

void Assembler::outsd() {
  opcode(0x6f);
}

void Assembler::outsw() {
  pref_group3();
  opcode(0x6f);
}

void Assembler::pabsb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x1c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pabsb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x1c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pabsb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pabsb(arg0, arg1.reg);
}

void Assembler::pabsb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x1c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pabsd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x1e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pabsd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x1e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pabsd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pabsd(arg0, arg1.reg);
}

void Assembler::pabsd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x1e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pabsw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x1d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pabsw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x1d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pabsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pabsw(arg0, arg1.reg);
}

void Assembler::pabsw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x1d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packssdw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x6b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packssdw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x6b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packssdw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  packssdw(arg0, arg1.reg);
}

void Assembler::packssdw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x6b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packsswb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x63);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packsswb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x63);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packsswb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  packsswb(arg0, arg1.reg);
}

void Assembler::packsswb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x63);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packusdw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  packusdw(arg0, arg1.reg);
}

void Assembler::packusdw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x2b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packuswb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x67);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packuswb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x67);
  mod_rm_sib(arg1,arg0);
}

void Assembler::packuswb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  packuswb(arg0, arg1.reg);
}

void Assembler::packuswb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x67);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xfc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xfc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  paddb(arg0, arg1.reg);
}

void Assembler::paddb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xfc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xfe);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xfe);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  paddd(arg0, arg1.reg);
}

void Assembler::paddd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xfe);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddq(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xd4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddq(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xd4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  paddq(arg0, arg1.reg);
}

void Assembler::paddq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddsb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xec);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddsb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xec);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddsb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  paddsb(arg0, arg1.reg);
}

void Assembler::paddsb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xec);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddsw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xed);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddsw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xed);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  paddsw(arg0, arg1.reg);
}

void Assembler::paddsw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xed);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddusb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xdc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddusb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xdc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddusb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  paddusb(arg0, arg1.reg);
}

void Assembler::paddusb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xdc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddusw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xdd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddusw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xdd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddusw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  paddusw(arg0, arg1.reg);
}

void Assembler::paddusw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xdd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xfd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xfd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::paddw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  paddw(arg0, arg1.reg);
}

void Assembler::paddw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xfd);
  mod_rm_sib(arg1,arg0);
}

void Assembler::palignr(const Mm& arg0, const M64& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0f);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::palignr(const Mm& arg0, const Mm& arg1, const Imm8& arg2) {
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0f);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::palignr(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  palignr(arg0, arg1.reg, arg2);
}

void Assembler::palignr(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0f);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pand(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xdb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pand(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xdb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pand(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pand(arg0, arg1.reg);
}

void Assembler::pand(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xdb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pandn(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xdf);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pandn(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xdf);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pandn(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pandn(arg0, arg1.reg);
}

void Assembler::pandn(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xdf);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pause() {
  F3();
  opcode(0x90);
}

void Assembler::pavgb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xe0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pavgb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xe0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pavgb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pavgb(arg0, arg1.reg);
}

void Assembler::pavgb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pavgw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xe3);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pavgw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xe3);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pavgw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pavgw(arg0, arg1.reg);
}

void Assembler::pavgw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe3);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pblendvb(const Xmm& arg0, const M128& arg1, const Xmm0& arg2) {
  p_2_4(arg1);
  pblendvb(arg0, arg1.reg, arg2);
}

void Assembler::pblendvb(const Xmm& arg0, const Xmm& arg1, const Xmm0& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pblendw(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pblendw(arg0, arg1.reg, arg2);
}

void Assembler::pblendw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0e);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pclmulqdq(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pclmulqdq(arg0, arg1.reg, arg2);
}

void Assembler::pclmulqdq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x44);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pcmpeqb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x74);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpeqb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x74);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpeqb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pcmpeqb(arg0, arg1.reg);
}

void Assembler::pcmpeqb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x74);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpeqd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x76);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpeqd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x76);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpeqd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pcmpeqd(arg0, arg1.reg);
}

void Assembler::pcmpeqd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x76);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpeqq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pcmpeqq(arg0, arg1.reg);
}

void Assembler::pcmpeqq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x29);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpeqw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x75);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpeqw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x75);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpeqw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pcmpeqw(arg0, arg1.reg);
}

void Assembler::pcmpeqw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x75);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpestri(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pcmpestri(arg0, arg1.reg, arg2);
}

void Assembler::pcmpestri(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x61);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pcmpestrm(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pcmpestrm(arg0, arg1.reg, arg2);
}

void Assembler::pcmpestrm(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x60);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pcmpgtb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x64);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpgtb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x64);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpgtb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pcmpgtb(arg0, arg1.reg);
}

void Assembler::pcmpgtb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x64);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpgtd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x66);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpgtd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x66);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpgtd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pcmpgtd(arg0, arg1.reg);
}

void Assembler::pcmpgtd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x66);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpgtq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pcmpgtq(arg0, arg1.reg);
}

void Assembler::pcmpgtq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x37);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpgtw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x65);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpgtw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x65);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpgtw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pcmpgtw(arg0, arg1.reg);
}

void Assembler::pcmpgtw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x65);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pcmpistri(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pcmpistri(arg0, arg1.reg, arg2);
}

void Assembler::pcmpistri(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x63);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pcmpistrm(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pcmpistrm(arg0, arg1.reg, arg2);
}

void Assembler::pcmpistrm(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x62);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pdep(const R32& arg0, const R32& arg1, const M32& arg2) {
  p_2_4(arg2);
  pdep(arg0, arg1, arg2.reg);
}

void Assembler::pdep(const R32& arg0, const R32& arg1, const R32& arg2) {
  vex(0x02,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0xf5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::pdep(const R64& arg0, const R64& arg1, const M64& arg2) {
  p_2_4(arg2);
  pdep(arg0, arg1, arg2.reg);
}

void Assembler::pdep(const R64& arg0, const R64& arg1, const R64& arg2) {
  vex(0x02,0x0,0x3,0x1,arg1,arg2,arg0);
  opcode(0xf5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::pext(const R32& arg0, const R32& arg1, const M32& arg2) {
  p_2_4(arg2);
  pext(arg0, arg1, arg2.reg);
}

void Assembler::pext(const R32& arg0, const R32& arg1, const R32& arg2) {
  vex(0x02,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0xf5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::pext(const R64& arg0, const R64& arg1, const M64& arg2) {
  p_2_4(arg2);
  pext(arg0, arg1, arg2.reg);
}

void Assembler::pext(const R64& arg0, const R64& arg1, const R64& arg2) {
  vex(0x02,0x0,0x2,0x1,arg1,arg2,arg0);
  opcode(0xf5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::pextrb(const M8& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x3a,0x14);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::pextrb(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x3a,0x14);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::pextrb(const R64& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x3a,0x14);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::pextrd(const M32& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  pextrd(arg0.reg, arg1, arg2);
}

void Assembler::pextrd(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x3a,0x16);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::pextrq(const M64& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  pextrq(arg0.reg, arg1, arg2);
}

void Assembler::pextrq(const R64& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rexw(arg0,arg1);
  opcode(0x0f,0x3a,0x16);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::pextrw(const M16& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x3a,0x15);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::pextrw(const R32& arg0, const Mm& arg1, const Imm8& arg2) {
  rex0(arg1,arg0);
  opcode(0x0f,0xc5);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pextrw(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xc5);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pextrw_1(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x3a,0x15);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::pextrw(const R64& arg0, const Mm& arg1, const Imm8& arg2) {
  rex0(arg1,arg0);
  opcode(0x0f,0xc5);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pextrw(const R64& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xc5);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pextrw_1(const R64& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0x3a,0x15);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::phaddd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x02);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phaddd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x02);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phaddd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  phaddd(arg0, arg1.reg);
}

void Assembler::phaddd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x02);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phaddsw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x03);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phaddsw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x03);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phaddsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  phaddsw(arg0, arg1.reg);
}

void Assembler::phaddsw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x03);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phaddw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x01);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phaddw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x01);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phaddw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  phaddw(arg0, arg1.reg);
}

void Assembler::phaddw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x01);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phminposuw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  phminposuw(arg0, arg1.reg);
}

void Assembler::phminposuw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x41);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phsubd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x06);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phsubd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x06);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phsubd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  phsubd(arg0, arg1.reg);
}

void Assembler::phsubd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x06);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phsubsw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x07);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phsubsw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x07);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phsubsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  phsubsw(arg0, arg1.reg);
}

void Assembler::phsubsw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x07);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phsubw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x05);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phsubw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x05);
  mod_rm_sib(arg1,arg0);
}

void Assembler::phsubw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  phsubw(arg0, arg1.reg);
}

void Assembler::phsubw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x05);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pinsrb(const Xmm& arg0, const M8& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x20);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pinsrb(const Xmm& arg0, const R32& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x20);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pinsrd(const Xmm& arg0, const M32& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pinsrd(arg0, arg1.reg, arg2);
}

void Assembler::pinsrd(const Xmm& arg0, const R32& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x22);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pinsrw(const Mm& arg0, const M16& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xc4);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pinsrw(const Mm& arg0, const R32& arg1, const Imm8& arg2) {
  rex0(arg1,arg0);
  opcode(0x0f,0xc4);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pinsrw(const Xmm& arg0, const M16& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xc4);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pinsrw(const Xmm& arg0, const R32& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xc4);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pmaddubsw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x04);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaddubsw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x04);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaddubsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmaddubsw(arg0, arg1.reg);
}

void Assembler::pmaddubsw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x04);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaddwd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xf5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaddwd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xf5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaddwd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmaddwd(arg0, arg1.reg);
}

void Assembler::pmaddwd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xf5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxsb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmaxsb(arg0, arg1.reg);
}

void Assembler::pmaxsb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x3c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxsd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmaxsd(arg0, arg1.reg);
}

void Assembler::pmaxsd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x3d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxsw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xee);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxsw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xee);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmaxsw(arg0, arg1.reg);
}

void Assembler::pmaxsw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xee);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxub(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xde);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxub(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xde);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxub(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmaxub(arg0, arg1.reg);
}

void Assembler::pmaxub(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xde);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxud(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmaxud(arg0, arg1.reg);
}

void Assembler::pmaxud(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x3f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmaxuw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmaxuw(arg0, arg1.reg);
}

void Assembler::pmaxuw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x3e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminsb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pminsb(arg0, arg1.reg);
}

void Assembler::pminsb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x38);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminsd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pminsd(arg0, arg1.reg);
}

void Assembler::pminsd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x39);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminsw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xea);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminsw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xea);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pminsw(arg0, arg1.reg);
}

void Assembler::pminsw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xea);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminub(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xda);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminub(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xda);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminub(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pminub(arg0, arg1.reg);
}

void Assembler::pminub(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xda);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminud(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pminud(arg0, arg1.reg);
}

void Assembler::pminud(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x3b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pminuw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pminuw(arg0, arg1.reg);
}

void Assembler::pminuw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x3a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovmskb(const R32& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xd7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovmskb(const R32& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovmskb(const R64& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xd7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovmskb(const R64& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxbd(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x21);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxbd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x21);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxbq(const Xmm& arg0, const M16& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x22);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxbq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x22);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxbw(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x20);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxbw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x20);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxdq(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x25);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxdq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x25);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxwd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x23);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxwd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x23);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxwq(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x24);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovsxwq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x24);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxbd(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x31);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxbd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x31);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxbq(const Xmm& arg0, const M16& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x32);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxbq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x32);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxbw(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x30);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxbw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x30);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxdq(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x35);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxdq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x35);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxwd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x33);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxwd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x33);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxwq(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x34);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmovzxwq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x34);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmuldq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmuldq(arg0, arg1.reg);
}

void Assembler::pmuldq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x28);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulhrsw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x0b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulhrsw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x0b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulhrsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmulhrsw(arg0, arg1.reg);
}

void Assembler::pmulhrsw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x0b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulhuw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xe4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulhuw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xe4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulhuw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmulhuw(arg0, arg1.reg);
}

void Assembler::pmulhuw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulhw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xe5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulhw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xe5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulhw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmulhw(arg0, arg1.reg);
}

void Assembler::pmulhw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmulld(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmulld(arg0, arg1.reg);
}

void Assembler::pmulld(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x40);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmullw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xd5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmullw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xd5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmullw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmullw(arg0, arg1.reg);
}

void Assembler::pmullw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd5);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmuludq(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xf4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmuludq(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xf4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pmuludq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pmuludq(arg0, arg1.reg);
}

void Assembler::pmuludq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xf4);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pop(const Fs& arg0) {
  opcode(0x0f,0xa1);
}

void Assembler::pop(const Fs& arg0, const Pref66& arg1) {
  pref_group3();
  opcode(0x0f,0xa1);
}

void Assembler::pop(const Gs& arg0) {
  opcode(0x0f,0xa9);
}

void Assembler::pop(const Gs& arg0, const Pref66& arg1) {
  pref_group3();
  opcode(0x0f,0xa9);
}

void Assembler::pop(const M16& arg0) {
  p_2_4(arg0);
  pop(arg0.reg);
}

void Assembler::pop(const M64& arg0) {
  p_2_4(arg0);
  pop(arg0.reg);
}

void Assembler::pop(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0x8f);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::pop_1(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0x58,arg0);
}

void Assembler::pop(const R64& arg0) {
  rex0(arg0);
  opcode(0x8f);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::pop_1(const R64& arg0) {
  rex0(arg0);
  opcode(0x58,arg0);
}

void Assembler::popcnt(const R16& arg0, const R16& arg1) {
  pref_group3();
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xb8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::popcnt(const R32& arg0, const R32& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xb8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::popcnt(const R64& arg0, const R64& arg1) {
  F3();
  rexw(arg1,arg0);
  opcode(0x0f,0xb8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::popf() {
  pref_group3();
  opcode(0x9d);
}

void Assembler::popfq() {
  opcode(0x9d);
}

void Assembler::por(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xeb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::por(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xeb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::por(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  por(arg0, arg1.reg);
}

void Assembler::por(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xeb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::prefetchnta(const M8& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0x18);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::prefetcht0(const M8& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0x18);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::prefetcht1(const M8& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0x18);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::prefetcht2(const M8& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0x18);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::psadbw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xf6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psadbw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xf6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psadbw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psadbw(arg0, arg1.reg);
}

void Assembler::psadbw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xf6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pshufb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x00);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pshufb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x00);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pshufb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pshufb(arg0, arg1.reg);
}

void Assembler::pshufb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x00);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pshufd(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pshufd(arg0, arg1.reg, arg2);
}

void Assembler::pshufd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pshufhw(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pshufhw(arg0, arg1.reg, arg2);
}

void Assembler::pshufhw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pshuflw(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pshuflw(arg0, arg1.reg, arg2);
}

void Assembler::pshuflw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pshufw(const Mm& arg0, const M64& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::pshufw(const Mm& arg0, const Mm& arg1, const Imm8& arg2) {
  rex0(arg1,arg0);
  opcode(0x0f,0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::psignb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x08);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psignb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x08);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psignb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psignb(arg0, arg1.reg);
}

void Assembler::psignb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x08);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psignd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x0a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psignd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x0a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psignd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psignd(arg0, arg1.reg);
}

void Assembler::psignd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x0a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psignw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x09);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psignw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x09);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psignw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psignw(arg0, arg1.reg);
}

void Assembler::psignw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x09);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pslld(const Mm& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0x0f,0x72);
  mod_rm_sib(arg0,Constants::r64s()[6]);
  disp_imm(arg1);
}

void Assembler::pslld(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xf2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pslld(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xf2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pslld(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x72);
  mod_rm_sib(arg0,Constants::r64s()[6]);
  disp_imm(arg1);
}

void Assembler::pslld(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pslld(arg0, arg1.reg);
}

void Assembler::pslld(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xf2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pslldq(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x73);
  mod_rm_sib(arg0,Constants::r64s()[7]);
  disp_imm(arg1);
}

void Assembler::psllq(const Mm& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0x0f,0x73);
  mod_rm_sib(arg0,Constants::r64s()[6]);
  disp_imm(arg1);
}

void Assembler::psllq(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xf3);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psllq(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xf3);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psllq(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x73);
  mod_rm_sib(arg0,Constants::r64s()[6]);
  disp_imm(arg1);
}

void Assembler::psllq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psllq(arg0, arg1.reg);
}

void Assembler::psllq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xf3);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psllw(const Mm& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0x0f,0x71);
  mod_rm_sib(arg0,Constants::r64s()[6]);
  disp_imm(arg1);
}

void Assembler::psllw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xf1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psllw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xf1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psllw(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x71);
  mod_rm_sib(arg0,Constants::r64s()[6]);
  disp_imm(arg1);
}

void Assembler::psllw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psllw(arg0, arg1.reg);
}

void Assembler::psllw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xf1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrad(const Mm& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0x0f,0x72);
  mod_rm_sib(arg0,Constants::r64s()[4]);
  disp_imm(arg1);
}

void Assembler::psrad(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xe2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrad(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xe2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrad(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f, 0x72);
  mod_rm_sib(arg0,Constants::r64s()[4]);
  disp_imm(arg1);
}

void Assembler::psrad(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psrad(arg0, arg1.reg);
}

void Assembler::psrad(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psraw(const Mm& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0x0f,0x71);
  mod_rm_sib(arg0,Constants::r64s()[4]);
  disp_imm(arg1);
}

void Assembler::psraw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xe1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psraw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xe1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psraw(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x71);
  mod_rm_sib(arg0,Constants::r64s()[4]);
  disp_imm(arg1);
}

void Assembler::psraw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psraw(arg0, arg1.reg);
}

void Assembler::psraw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrld(const Mm& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0x0f,0x72);
  mod_rm_sib(arg0,Constants::r64s()[2]);
  disp_imm(arg1);
}

void Assembler::psrld(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xd2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrld(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xd2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrld(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x72);
  mod_rm_sib(arg0,Constants::r64s()[2]);
  disp_imm(arg1);
}

void Assembler::psrld(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psrld(arg0, arg1.reg);
}

void Assembler::psrld(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd2);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrldq(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x73);
  mod_rm_sib(arg0,Constants::r64s()[3]);
  disp_imm(arg1);
}

void Assembler::psrlq(const Mm& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0x0f,0x73);
  mod_rm_sib(arg0,Constants::r64s()[2]);
  disp_imm(arg1);
}

void Assembler::psrlq(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xd3);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrlq(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xd3);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrlq(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x73);
  mod_rm_sib(arg0,Constants::r64s()[2]);
  disp_imm(arg1);
}

void Assembler::psrlq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psrlq(arg0, arg1.reg);
}

void Assembler::psrlq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd3);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrlw(const Mm& arg0, const Imm8& arg1) {
  rex0(arg0);
  opcode(0x0f,0x71);
  mod_rm_sib(arg0,Constants::r64s()[2]);
  disp_imm(arg1);
}

void Assembler::psrlw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xd1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrlw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xd1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psrlw(const Xmm& arg0, const Imm8& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0x71);
  mod_rm_sib(arg0,Constants::r64s()[2]);
  disp_imm(arg1);
}

void Assembler::psrlw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psrlw(arg0, arg1.reg);
}

void Assembler::psrlw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd1);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xf8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xf8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psubb(arg0, arg1.reg);
}

void Assembler::psubb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xf8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xfa);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xfa);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psubd(arg0, arg1.reg);
}

void Assembler::psubd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xfa);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubq(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xfb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubq(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xfb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psubq(arg0, arg1.reg);
}

void Assembler::psubq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xfb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubsb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xe8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubsb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xe8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubsb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psubsb(arg0, arg1.reg);
}

void Assembler::psubsb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubsw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xe9);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubsw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xe9);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psubsw(arg0, arg1.reg);
}

void Assembler::psubsw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xe9);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubusb(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xd8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubusb(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xd8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubusb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psubusb(arg0, arg1.reg);
}

void Assembler::psubusb(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd8);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubusw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xd9);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubusw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xd9);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubusw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psubusw(arg0, arg1.reg);
}

void Assembler::psubusw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xd9);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xf9);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xf9);
  mod_rm_sib(arg1,arg0);
}

void Assembler::psubw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  psubw(arg0, arg1.reg);
}

void Assembler::psubw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xf9);
  mod_rm_sib(arg1,arg0);
}

void Assembler::ptest(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  ptest(arg0, arg1.reg);
}

void Assembler::ptest(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x38,0x17);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhbw(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x68);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhbw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x68);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhbw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  punpckhbw(arg0, arg1.reg);
}

void Assembler::punpckhbw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x68);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhdq(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x6a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhdq(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x6a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhdq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  punpckhdq(arg0, arg1.reg);
}

void Assembler::punpckhdq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x6a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhqdq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  punpckhqdq(arg0, arg1.reg);
}

void Assembler::punpckhqdq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x6d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhwd(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x69);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhwd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x69);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckhwd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  punpckhwd(arg0, arg1.reg);
}

void Assembler::punpckhwd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x69);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpcklbw(const Mm& arg0, const M32& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x60);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpcklbw(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x60);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpcklbw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  punpcklbw(arg0, arg1.reg);
}

void Assembler::punpcklbw(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x60);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckldq(const Mm& arg0, const M32& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x62);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckldq(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x62);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpckldq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  punpckldq(arg0, arg1.reg);
}

void Assembler::punpckldq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x62);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpcklqdq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  punpcklqdq(arg0, arg1.reg);
}

void Assembler::punpcklqdq(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x6c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpcklwd(const Mm& arg0, const M32& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x61);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpcklwd(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x61);
  mod_rm_sib(arg1,arg0);
}

void Assembler::punpcklwd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  punpcklwd(arg0, arg1.reg);
}

void Assembler::punpcklwd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x61);
  mod_rm_sib(arg1,arg0);
}

void Assembler::push(const Fs& arg0) {
  opcode(0x0f,0xa0);
}

void Assembler::push(const Gs& arg0) {
  opcode(0x0f,0xa8);
}

void Assembler::push(const M16& arg0) {
  p_2_4(arg0);
  push(arg0.reg);
}

void Assembler::push(const M64& arg0) {
  p_2_4(arg0);
  push(arg0.reg);
}

void Assembler::push(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::push_1(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0x50,arg0);
}

void Assembler::push(const R64& arg0) {
  rex0(arg0);
  opcode(0xff);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::push_1(const R64& arg0) {
  rex0(arg0);
  opcode(0x50,arg0);
}

void Assembler::pushf() {
  pref_group3();
  opcode(0x9c);
}

void Assembler::pushfq() {
  opcode(0x9c);
}

void Assembler::pushq(const Imm16& arg0) {
  pref_group3();
  opcode(0x68);
  disp_imm(arg0);
}

void Assembler::pushq(const Imm32& arg0) {
  opcode(0x68);
  disp_imm(arg0);
}

void Assembler::pushq(const Imm8& arg0) {
  opcode(0x6a);
  disp_imm(arg0);
}

void Assembler::pushw(const Imm16& arg0) {
  pref_group3();
  opcode(0x68);
  disp_imm(arg0);
}

void Assembler::pushw(const Imm8& arg0) {
  pref_group3();
  opcode(0x6a);
  disp_imm(arg0);
}

void Assembler::pxor(const Mm& arg0, const M64& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0xef);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pxor(const Mm& arg0, const Mm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0xef);
  mod_rm_sib(arg1,arg0);
}

void Assembler::pxor(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  pxor(arg0, arg1.reg);
}

void Assembler::pxor(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xef);
  mod_rm_sib(arg1,arg0);
}

void Assembler::rcl(const M16& arg0, const Cl& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const M16& arg0, const One& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const M32& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const M32& arg0, const One& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const M64& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const M64& arg0, const One& arg1) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const M8& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const M8& arg0, const One& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const R16& arg0, const Cl& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const R16& arg0, const One& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const R32& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const R32& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const R64& arg0, const Cl& arg1) {
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const R64& arg0, const One& arg1) {
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const R8& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const R8& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const Rh& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcl(const Rh& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::rcpps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  rcpps(arg0, arg1.reg);
}

void Assembler::rcpps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x53);
  mod_rm_sib(arg1,arg0);
}

void Assembler::rcpss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x53);
  mod_rm_sib(arg1,arg0);
}

void Assembler::rcpss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x53);
  mod_rm_sib(arg1,arg0);
}

void Assembler::rcr(const M16& arg0, const Cl& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const M16& arg0, const One& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const M32& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const M32& arg0, const One& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const M64& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const M64& arg0, const One& arg1) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const M8& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const M8& arg0, const One& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const R16& arg0, const Cl& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const R16& arg0, const One& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const R32& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const R32& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const R64& arg0, const Cl& arg1) {
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const R64& arg0, const One& arg1) {
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const R8& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const R8& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const Rh& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rcr(const Rh& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::rdfsbase(const R32& arg0) {
  F3();
  rex0(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rdfsbase(const R64& arg0) {
  F3();
  rexw(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rdgsbase(const R32& arg0) {
  F3();
  rex0(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::rdgsbase(const R64& arg0) {
  F3();
  rexw(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::rdrand(const R16& arg0) {
  pref_group3();
  rex0(arg0);
  opcode(0x0f,0xc7);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::rdrand(const R32& arg0) {
  rex0(arg0);
  opcode(0x0f,0xc7);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::rdrand(const R64& arg0) {
  rexw(arg0);
  opcode(0x0f,0xc7);
  mod_rm_sib(arg0,Constants::r64s()[6]);
}

void Assembler::rep_ins(const M16& arg0, const Dx& arg1) {
  p_2_4(arg0);
  pref_group3();
  F3();
  opcode(0x6d);
}

void Assembler::rep_ins(const M32& arg0, const Dx& arg1) {
  p_2_4(arg0);
  F3();
  opcode(0x6d);
}

void Assembler::rep_ins(const M64& arg0, const Dx& arg1) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0x6d);
}

void Assembler::rep_ins(const M8& arg0, const Dx& arg1) {
  p_2_4(arg0);
  F3();
  opcode(0x6c);
}

void Assembler::rep_ins_1(const M8& arg0, const Dx& arg1) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0x6c);
}

void Assembler::rep_lods(const Al& arg0) {
  F3();
  opcode(0xac);
}

void Assembler::rep_lods_1(const Al& arg0) {
  F3();
  _rexw();
  opcode(0xac);
}

void Assembler::rep_lods(const Ax& arg0) {
  pref_group3();
  F3();
  opcode(0xad);
}

void Assembler::rep_lods(const Eax& arg0) {
  F3();
  opcode(0xad);
}

void Assembler::rep_lods(const Rax& arg0) {
  F3();
  _rexw();
  opcode(0xad);
}

void Assembler::rep_movs(const M16& arg0, const M16& arg1) {
  p_2_4(arg0);
  pref_group3();
  F3();
  opcode(0xa5);
}

void Assembler::rep_movs(const M32& arg0, const M32& arg1) {
  p_2_4(arg0);
  F3();
  opcode(0xa5);
}

void Assembler::rep_movs(const M64& arg0, const M64& arg1) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0xa5);
}

void Assembler::rep_movs(const M8& arg0, const M8& arg1) {
  p_2_4(arg0);
  F3();
  opcode(0xa4);
}

void Assembler::rep_movs_1(const M8& arg0, const M8& arg1) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0xa4);
}

void Assembler::rep_outs(const Dx& arg0, const M16& arg1) {
  p_2_4(arg1);
  pref_group3();
  F3();
  opcode(0x6f);
}

void Assembler::rep_outs(const Dx& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  opcode(0x6f);
}

void Assembler::rep_outs(const Dx& arg0, const M64& arg1) {
  p_2_4(arg1);
  F3();
  _rexw();
  opcode(0x6f);
}

void Assembler::rep_outs(const Dx& arg0, const M8& arg1) {
  p_2_4(arg1);
  F3();
  opcode(0x6e);
}

void Assembler::rep_outs_1(const Dx& arg0, const M8& arg1) {
  p_2_4(arg1);
  F3();
  _rexw();
  opcode(0x6e);
}

void Assembler::rep_stos(const M16& arg0) {
  p_2_4(arg0);
  pref_group3();
  F3();
  opcode(0xab);
}

void Assembler::rep_stos(const M32& arg0) {
  p_2_4(arg0);
  F3();
  opcode(0xab);
}

void Assembler::rep_stos(const M64& arg0) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0xab);
}

void Assembler::rep_stos(const M8& arg0) {
  p_2_4(arg0);
  F3();
  opcode(0xaa);
}

void Assembler::rep_stos_1(const M8& arg0) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0xaa);
}

void Assembler::repe_cmps(const M16& arg0, const M16& arg1) {
  p_2_4(arg0);
  pref_group3();
  F3();
  opcode(0xa7);
}

void Assembler::repe_cmps(const M32& arg0, const M32& arg1) {
  p_2_4(arg0);
  F3();
  opcode(0xa7);
}

void Assembler::repe_cmps(const M64& arg0, const M64& arg1) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0xa7);
}

void Assembler::repe_cmps(const M8& arg0, const M8& arg1) {
  p_2_4(arg0);
  F3();
  opcode(0xa6);
}

void Assembler::repe_cmps_1(const M8& arg0, const M8& arg1) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0xa6);
}

void Assembler::repe_scas(const M16& arg0) {
  p_2_4(arg0);
  pref_group3();
  F3();
  opcode(0xaf);
}

void Assembler::repe_scas(const M32& arg0) {
  p_2_4(arg0);
  F3();
  opcode(0xaf);
}

void Assembler::repe_scas(const M64& arg0) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0xaf);
}

void Assembler::repe_scas(const M8& arg0) {
  p_2_4(arg0);
  F3();
  opcode(0xae);
}

void Assembler::repe_scas_1(const M8& arg0) {
  p_2_4(arg0);
  F3();
  _rexw();
  opcode(0xae);
}

void Assembler::repne_cmps(const M16& arg0, const M16& arg1) {
  p_2_4(arg0);
  pref_group3();
  F2();
  opcode(0xa7);
}

void Assembler::repne_cmps(const M32& arg0, const M32& arg1) {
  p_2_4(arg0);
  F2();
  opcode(0xa7);
}

void Assembler::repne_cmps(const M64& arg0, const M64& arg1) {
  p_2_4(arg0);
  F2();
  _rexw();
  opcode(0xa7);
}

void Assembler::repne_cmps(const M8& arg0, const M8& arg1) {
  p_2_4(arg0);
  F2();
  opcode(0xa6);
}

void Assembler::repne_cmps_1(const M8& arg0, const M8& arg1) {
  p_2_4(arg0);
  F2();
  _rexw();
  opcode(0xa6);
}

void Assembler::repne_scas(const M16& arg0) {
  p_2_4(arg0);
  pref_group3();
  F2();
  opcode(0xaf);
}

void Assembler::repne_scas(const M32& arg0) {
  p_2_4(arg0);
  F2();
  opcode(0xaf);
}

void Assembler::repne_scas(const M64& arg0) {
  p_2_4(arg0);
  F2();
  _rexw();
  opcode(0xaf);
}

void Assembler::repne_scas(const M8& arg0) {
  p_2_4(arg0);
  F2();
  opcode(0xae);
}

void Assembler::repne_scas_1(const M8& arg0) {
  p_2_4(arg0);
  F2();
  _rexw();
  opcode(0xae);
}

void Assembler::ret() {
  opcode(0xc3);
}

void Assembler::ret(const Far& arg0) {
  opcode(0xcb);
}

void Assembler::ret(const Imm16& arg0) {
  opcode(0xc2);
  disp_imm(arg0);
}

void Assembler::ret(const Imm16& arg0, const Far& arg1) {
  opcode(0xca);
  disp_imm(arg0);
}

void Assembler::rol(const M16& arg0, const Cl& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const M16& arg0, const One& arg1) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const M32& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const M32& arg0, const One& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const M64& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const M64& arg0, const One& arg1) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const M8& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const M8& arg0, const One& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const R16& arg0, const Cl& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const R16& arg0, const One& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const R32& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const R32& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const R64& arg0, const Cl& arg1) {
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const R64& arg0, const One& arg1) {
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const R8& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const R8& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const Rh& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::rol(const Rh& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::ror(const M16& arg0, const Cl& arg1) {
  p_2_4(arg0);
  ror(arg0.reg, arg1);
}

void Assembler::ror(const M16& arg0, const One& arg1) {
  p_2_4(arg0);
  ror(arg0.reg, arg1);
}

void Assembler::ror(const M32& arg0, const Cl& arg1) {
  p_2_4(arg0);
  ror(arg0.reg, arg1);
}

void Assembler::ror(const M32& arg0, const One& arg1) {
  p_2_4(arg0);
  ror(arg0.reg, arg1);
}

void Assembler::ror(const M64& arg0, const Cl& arg1) {
  p_2_4(arg0);
  ror(arg0.reg, arg1);
}

void Assembler::ror(const M64& arg0, const One& arg1) {
  p_2_4(arg0);
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const M8& arg0, const Cl& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const M8& arg0, const One& arg1) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const R16& arg0, const Cl& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}


void Assembler::ror(const R16& arg0, const One& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const R32& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const R32& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const R64& arg0, const Cl& arg1) {
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const R64& arg0, const One& arg1) {
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const R8& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const R8& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const Rh& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::ror(const Rh& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[1]);
}

void Assembler::rorx(const R32& arg0, const M32& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  rorx(arg0, arg1.reg, arg2);
}

void Assembler::rorx(const R32& arg0, const R32& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xf0);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::rorx(const R64& arg0, const M64& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  rorx(arg0, arg1.reg, arg2);
}

void Assembler::rorx(const R64& arg0, const R64& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x3,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0xf0);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::roundpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  roundpd(arg0, arg1.reg, arg2);
}

void Assembler::roundpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x09);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::roundps(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  roundps(arg0, arg1.reg, arg2);
}

void Assembler::roundps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x08);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::roundsd(const Xmm& arg0, const M64& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0b);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::roundsd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0b);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::roundss(const Xmm& arg0, const M32& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0a);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::roundss(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x3a,0x0a);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::rsqrtps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  rsqrtps(arg0, arg1.reg);
}

void Assembler::rsqrtps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x52);
  mod_rm_sib(arg1,arg0);
}

void Assembler::rsqrtss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x52);
  mod_rm_sib(arg1,arg0);
}

void Assembler::rsqrtss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x52);
  mod_rm_sib(arg1,arg0);
}

void Assembler::sahf() {
  opcode(0x9e);
}

void Assembler::sal(const M16& arg0, const Cl& arg1) {
  p_2_4(arg0);
  sal(arg0.reg, arg1);
}

void Assembler::sal(const M16& arg0, const One& arg1) {
  p_2_4(arg0);
  sal(arg0.reg, arg1);
}

void Assembler::sal(const M32& arg0, const Cl& arg1) {
  p_2_4(arg0);
  sal(arg0.reg, arg1);
}

void Assembler::sal(const M32& arg0, const One& arg1) {
  p_2_4(arg0);
  sal(arg0.reg, arg1);
}

void Assembler::sal(const M64& arg0, const Cl& arg1) {
  p_2_4(arg0);
  sal(arg0.reg, arg1);
}

void Assembler::sal(const M64& arg0, const One& arg1) {
  p_2_4(arg0);
  sal(arg0.reg, arg1);
}

void Assembler::sal(const M8& arg0, const Cl& arg1) {
  p_2_4(arg0);
  sal(arg0.reg, arg1);
}

void Assembler::sal(const M8& arg0, const One& arg1) {
  p_2_4(arg0);
  sal(arg0.reg, arg1);
}

void Assembler::sal(const R16& arg0, const Cl& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sal(const R16& arg0, const One& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sal(const R32& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sal(const R32& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sal(const R64& arg0, const Cl& arg1) {
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sal(const R64& arg0, const One& arg1) {
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sal(const R8& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sal(const R8& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sal(const Rh& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sal(const Rh& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::sar(const M16& arg0, const Cl& arg1) {
  p_2_4(arg0);
  sar(arg0.reg, arg1);
}

void Assembler::sar(const M16& arg0, const One& arg1) {
  p_2_4(arg0);
  sar(arg0.reg, arg1);
}

void Assembler::sar(const M32& arg0, const Cl& arg1) {
  p_2_4(arg0);
  sar(arg0.reg, arg1);
}

void Assembler::sar(const M32& arg0, const One& arg1) {
  p_2_4(arg0);
  sar(arg0.reg, arg1);
}

void Assembler::sar(const M64& arg0, const Cl& arg1) {
  p_2_4(arg0);
  sar(arg0.reg, arg1);
}

void Assembler::sar(const M64& arg0, const One& arg1) {
  p_2_4(arg0);
  sar(arg0.reg, arg1);
}

void Assembler::sar(const M8& arg0, const Cl& arg1) {
  p_2_4(arg0);
  sar(arg0.reg, arg1);
}

void Assembler::sar(const M8& arg0, const One& arg1) {
  p_2_4(arg0);
  sar(arg0.reg, arg1);
}

void Assembler::sar(const R16& arg0, const Cl& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sar(const R16& arg0, const One& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sar(const R32& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sar(const R32& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sar(const R64& arg0, const Cl& arg1) {
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sar(const R64& arg0, const One& arg1) {
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sar(const R8& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sar(const R8& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sar(const Rh& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sar(const Rh& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[7]);
}

void Assembler::sarx(const R32& arg0, const M32& arg1, const R32& arg2) {
  p_2_4(arg1);
  sarx(arg0, arg1.reg, arg2);
}

void Assembler::sarx(const R32& arg0, const R32& arg1, const R32& arg2) {
  vex(0x02,0x0,0x2,0x0,arg2,arg1,arg0);
  opcode(0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::sarx(const R64& arg0, const M64& arg1, const R64& arg2) {
  p_2_4(arg1);
  sarx(arg0, arg1.reg, arg2);
}

void Assembler::sarx(const R64& arg0, const R64& arg1, const R64& arg2) {
  vex(0x02,0x0,0x2,0x1,arg2,arg1,arg0);
  opcode(0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::scas(const M16& arg0) {
  p_2_4(arg0);
  pref_group3();
  opcode(0xaf);
}

void Assembler::scas(const M32& arg0) {
  p_2_4(arg0);
  opcode(0xaf);
}

void Assembler::scas(const M64& arg0) {
  p_2_4(arg0);
  _rexw();
  opcode(0xaf);
}

void Assembler::scas(const M8& arg0) {
  p_2_4(arg0);
  opcode(0xae);
}

void Assembler::scasb() {
  opcode(0xae);
}

void Assembler::scasd() {
  opcode(0xaf);
}

void Assembler::scasq() {
  _rexw();
  opcode(0xaf);
}

void Assembler::scasw() {
  pref_group3();
  opcode(0xaf);
}

void Assembler::seta(const M8& arg0) {
  p_2_4(arg0);
  seta(arg0.reg);
}

void Assembler::seta(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x97);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::seta(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x97);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setae(const M8& arg0) {
  p_2_4(arg0);
  setae(arg0.reg);
}

void Assembler::setae(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x93);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setae(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x93);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setb(const M8& arg0) {
  p_2_4(arg0);
  setb(arg0.reg);
}

void Assembler::setb(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x92);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setb(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x92);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setbe(const M8& arg0) {
  p_2_4(arg0);
  setbe(arg0.reg);
}

void Assembler::setbe(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x96);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setbe(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x96);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setc(const M8& arg0) {
  p_2_4(arg0);
  setc(arg0.reg);
}

void Assembler::setc(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x92);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setc(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x92);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::sete(const M8& arg0) {
  p_2_4(arg0);
  sete(arg0.reg);
}

void Assembler::sete(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x94);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::sete(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x94);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setg(const M8& arg0) {
  p_2_4(arg0);
  setg(arg0.reg);
}

void Assembler::setg(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9f);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setg(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9f);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setge(const M8& arg0) {
  p_2_4(arg0);
  setge(arg0.reg);
}

void Assembler::setge(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9d);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setge(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9d);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setl(const M8& arg0) {
  p_2_4(arg0);
  setl(arg0.reg);
}

void Assembler::setl(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9c);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setl(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9c);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setle(const M8& arg0) {
  p_2_4(arg0);
  setle(arg0.reg);
}

void Assembler::setle(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9e);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setle(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9e);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setna(const M8& arg0) {
  p_2_4(arg0);
  setna(arg0.reg);
}

void Assembler::setna(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x96);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setna(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x96);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnae(const M8& arg0) {
  p_2_4(arg0);
  setnae(arg0.reg);
}

void Assembler::setnae(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x92);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnae(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x92);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnb(const M8& arg0) {
  p_2_4(arg0);
  setnb(arg0.reg);
}

void Assembler::setnb(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x93);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnb(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x93);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnbe(const M8& arg0) {
  p_2_4(arg0);
  setnbe(arg0.reg);
}

void Assembler::setnbe(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x97);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnbe(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x97);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnc(const M8& arg0) {
  p_2_4(arg0);
  setnc(arg0.reg);
}

void Assembler::setnc(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x93);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnc(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x93);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setne(const M8& arg0) {
  p_2_4(arg0);
  setne(arg0.reg);
}

void Assembler::setne(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x95);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setne(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x95);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setng(const M8& arg0) {
  p_2_4(arg0);
  setng(arg0.reg);
}

void Assembler::setng(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9e);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setng(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9e);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnge(const M8& arg0) {
  p_2_4(arg0);
  setnge(arg0.reg);
}

void Assembler::setnge(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9c);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnge(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9c);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnl(const M8& arg0) {
  p_2_4(arg0);
  setnl(arg0.reg);
}

void Assembler::setnl(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9d);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnl(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9d);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnle(const M8& arg0) {
  p_2_4(arg0);
  setnle(arg0.reg);
}

void Assembler::setnle(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9f);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnle(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9f);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setno(const M8& arg0) {
  p_2_4(arg0);
  setno(arg0.reg);
}

void Assembler::setno(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x91);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setno(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x91);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnp(const M8& arg0) {
  p_2_4(arg0);
  setnp(arg0.reg);
}

void Assembler::setnp(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9b);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnp(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9b);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setns(const M8& arg0) {
  p_2_4(arg0);
  setns(arg0.reg);
}

void Assembler::setns(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x99);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setns(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x99);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnz(const M8& arg0) {
  p_2_4(arg0);
  setnz(arg0.reg);
}

void Assembler::setnz(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x95);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setnz(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x95);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::seto(const M8& arg0) {
  p_2_4(arg0);
  seto(arg0.reg);
}

void Assembler::seto(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x90);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::seto(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x90);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setp(const M8& arg0) {
  p_2_4(arg0);
  setp(arg0.reg);
}

void Assembler::setp(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9a);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setp(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9a);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setpe(const M8& arg0) {
  p_2_4(arg0);
  setpe(arg0.reg);
}

void Assembler::setpe(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9a);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setpe(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9a);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setpo(const M8& arg0) {
  p_2_4(arg0);
  setpo(arg0.reg);
}

void Assembler::setpo(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9b);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setpo(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x9b);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::sets(const M8& arg0) {
  p_2_4(arg0);
  sets(arg0.reg);
}

void Assembler::sets(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x98);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::sets(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x98);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setz(const M8& arg0) {
  p_2_4(arg0);
  setz(arg0.reg);
}

void Assembler::setz(const R8& arg0) {
  rex0(arg0);
  opcode(0x0f,0x94);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::setz(const Rh& arg0) {
  rex0(arg0);
  opcode(0x0f,0x94);
  mod_rm_sib(arg0,Constants::r64s()[0]);
}

void Assembler::sfence() {
  opcode(0x0f,0xae,0xf8);
}

void Assembler::shl(const M16& arg0, const Cl& arg1) {
  p_2_4(arg0);
  shl(arg0.reg, arg1);
}

void Assembler::shl(const M16& arg0, const One& arg1) {
  p_2_4(arg0);
  shl(arg0.reg, arg1);
}

void Assembler::shl(const M32& arg0, const Cl& arg1) {
  p_2_4(arg0);
  shl(arg0.reg, arg1);
}

void Assembler::shl(const M32& arg0, const One& arg1) {
  p_2_4(arg0);
  shl(arg0.reg, arg1);
}

void Assembler::shl(const M64& arg0, const Cl& arg1) {
  p_2_4(arg0);
  shl(arg0.reg, arg1);
}

void Assembler::shl(const M64& arg0, const One& arg1) {
  p_2_4(arg0);
  shl(arg0.reg, arg1);
}

void Assembler::shl(const M8& arg0, const Cl& arg1) {
  p_2_4(arg0);
  shl(arg0.reg, arg1);
}

void Assembler::shl(const M8& arg0, const One& arg1) {
  p_2_4(arg0);
  shl(arg0.reg, arg1);
}

void Assembler::shl(const R16& arg0, const Cl& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shl(const R16& arg0, const One& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shl(const R32& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shl(const R32& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shl(const R64& arg0, const Cl& arg1) {
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shl(const R64& arg0, const One& arg1) {
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shl(const R8& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shl(const R8& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shl(const Rh& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shl(const Rh& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::shld(const M16& arg0, const R16& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0xa4);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::shld(const M32& arg0, const R32& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  rex0(arg0,arg1);
  opcode(0x0f,0xa4);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::shld(const M64& arg0, const R64& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  rexw(arg0,arg1);
  opcode(0x0f,0xa4);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}




void Assembler::shlx(const R32& arg0, const M32& arg1, const R32& arg2) {
  p_2_4(arg1);
  shlx(arg0, arg1.reg, arg2);
}

void Assembler::shlx(const R32& arg0, const R32& arg1, const R32& arg2) {
  vex(0x02,0x0,0x1,0x0,arg2,arg1,arg0);
  opcode(0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::shlx(const R64& arg0, const M64& arg1, const R64& arg2) {
  p_2_4(arg1);
  shlx(arg0, arg1.reg, arg2);
}

void Assembler::shlx(const R64& arg0, const R64& arg1, const R64& arg2) {
  vex(0x02,0x0,0x1,0x1,arg2,arg1,arg0);
  opcode(0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::shr(const M16& arg0, const Cl& arg1) {
  p_2_4(arg0);
  shr(arg0.reg, arg1);
}

void Assembler::shr(const M16& arg0, const One& arg1) {
  p_2_4(arg0);
  shr(arg0.reg, arg1);
}

void Assembler::shr(const M32& arg0, const Cl& arg1) {
  p_2_4(arg0);
  shr(arg0.reg, arg1);
}

void Assembler::shr(const M32& arg0, const One& arg1) {
  p_2_4(arg0);
  shr(arg0.reg, arg1);
}

void Assembler::shr(const M64& arg0, const Cl& arg1) {
  p_2_4(arg0);
  shr(arg0.reg, arg1);
}

void Assembler::shr(const M64& arg0, const One& arg1) {
  p_2_4(arg0);
  shr(arg0.reg, arg1);
}

void Assembler::shr(const M8& arg0, const Cl& arg1) {
  p_2_4(arg0);
  shr(arg0.reg, arg1);
}

void Assembler::shr(const M8& arg0, const One& arg1) {
  p_2_4(arg0);
  shr(arg0.reg, arg1);
}

void Assembler::shr(const R16& arg0, const Cl& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shr(const R16& arg0, const One& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shr(const R32& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shr(const R32& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shr(const R64& arg0, const Cl& arg1) {
  rexw(arg0);
  opcode(0xd3);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shr(const R64& arg0, const One& arg1) {
  rexw(arg0);
  opcode(0xd1);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shr(const R8& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shr(const R8& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shr(const Rh& arg0, const Cl& arg1) {
  rex0(arg0);
  opcode(0xd2);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shr(const Rh& arg0, const One& arg1) {
  rex0(arg0);
  opcode(0xd0);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::shrd(const M16& arg0, const R16& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  shrd(arg0.reg, arg1, arg2);
}

void Assembler::shrd(const M32& arg0, const R32& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  shrd(arg0.reg, arg1, arg2);
}

void Assembler::shrd(const M64& arg0, const R64& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  shrd(arg0.reg, arg1, arg2);
}

void Assembler::shrd(const R16& arg0, const R16& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg0,arg1);
  opcode(0x0f,0xac);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::shrd(const R32& arg0, const R32& arg1, const Imm8& arg2) {
  rex0(arg0,arg1);
  opcode(0x0f,0xac);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::shrd(const R64& arg0, const R64& arg1, const Imm8& arg2) {
  rexw(arg0,arg1);
  opcode(0x0f,0xac);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::shrx(const R32& arg0, const M32& arg1, const R32& arg2) {
  p_2_4(arg1);
  shrx(arg0, arg1.reg, arg2);
}

void Assembler::shrx(const R32& arg0, const R32& arg1, const R32& arg2) {
  vex(0x02,0x0,0x3,0x0,arg2,arg1,arg0);
  opcode(0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::shrx(const R64& arg0, const M64& arg1, const R64& arg2) {
  p_2_4(arg1);
  shrx(arg0, arg1.reg, arg2);
}

void Assembler::shrx(const R64& arg0, const R64& arg1, const R64& arg2) {
  vex(0x02,0x0,0x3,0x1,arg2,arg1,arg0);
  opcode(0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::shufpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  shufpd(arg0, arg1.reg, arg2);
}

void Assembler::shufpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0xc6);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::shufps(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  shufps(arg0, arg1.reg, arg2);
}

void Assembler::shufps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  rex0(arg1,arg0);
  opcode(0x0f,0xc6);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::sqrtpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  sqrtpd(arg0, arg1.reg);
}

void Assembler::sqrtpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::sqrtps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  sqrtps(arg0, arg1.reg);
}

void Assembler::sqrtps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::sqrtsd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::sqrtsd(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::sqrtss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::sqrtss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::stc() {
  opcode(0xf9);
}

void Assembler::std_() {
  opcode(0xfd);
}

void Assembler::sti() {
  opcode(0xfb);
}

void Assembler::stmxcsr(const M32& arg0) {
  p_2_4(arg0);
  rex0(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::stos(const M16& arg0) {
  p_2_4(arg0);
  pref_group3();
  opcode(0xab);
}

void Assembler::stos(const M32& arg0) {
  p_2_4(arg0);
  opcode(0xab);
}

void Assembler::stos(const M64& arg0) {
  p_2_4(arg0);
  _rexw();
  opcode(0xab);
}

void Assembler::stos(const M8& arg0) {
  p_2_4(arg0);
  opcode(0xaa);
}

void Assembler::stosb() {
  opcode(0xaa);
}

void Assembler::stosd() {
  opcode(0xab);
}

void Assembler::stosq() {
  _rexw();
  opcode(0xab);
}

void Assembler::stosw() {
  pref_group3();
  opcode(0xab);
}

void Assembler::sub(const R32& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0x83, 5);
}

void Assembler::sub(const R64& arg0, const Imm32& arg1) {
  meta(arg0, arg1, 0x81, 5);
}

void Assembler::sub(const R64& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0x83, 5);
}

void Assembler::sub(const R8& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0x80, 5);
}

void Assembler::sub(const Rh& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0x80, 5);
}

void Assembler::subpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  subpd(arg0, arg1.reg);
}

void Assembler::subpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::subps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  subps(arg0, arg1.reg);
}

void Assembler::subps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x5c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::subsd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x5c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::subsd(const Xmm& arg0, const Xmm& arg1) {
  F2();
  rex0(arg1,arg0);
  opcode(0x0f,0x5c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::subss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::subss(const Xmm& arg0, const Xmm& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0x5c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::swapgs() {
  opcode(0x0f,0x01,0xf8);
}

void Assembler::syscall() {
  opcode(0x0f,0x05);
}

void Assembler::sysenter() {
  opcode(0x0f,0x34);
}

void Assembler::sysexit() {
  opcode(0x0f,0x35);
}

void Assembler::sysexit(const PrefRexW& arg0) {
  _rexw();
  opcode(0x0f,0x35);
}

void Assembler::sysret() {
  opcode(0x0f,0x07);
}

void Assembler::sysret(const PrefRexW& arg0) {
  _rexw();
  opcode(0x0f,0x07);
}

void Assembler::test(const R16& arg0, const Imm16& arg1) {
  meta(arg0, arg1, 0xf7, 0);
}

void Assembler::test(const R32& arg0, const Imm32& arg1) {
  meta(arg0, arg1, 0xf7, 0);
}

void Assembler::test(const R64& arg0, const Imm32& arg1) {
  meta(arg0, arg1, 0xf7, 0);
}

void Assembler::test(const R8& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0xf6, 0);
}

void Assembler::test(const Rh& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0xf6, 0);
}

void Assembler::tzcnt(const R16& arg0, const R16& arg1) {
  pref_group3();
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xbc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::tzcnt(const R32& arg0, const R32& arg1) {
  F3();
  rex0(arg1,arg0);
  opcode(0x0f,0xbc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::tzcnt(const R64& arg0, const R64& arg1) {
  F3();
  rexw(arg1,arg0);
  opcode(0x0f,0xbc);
  mod_rm_sib(arg1,arg0);
}

void Assembler::ucomisd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::ucomisd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x2e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::ucomiss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  rex0(arg1,arg0);
  opcode(0x0f,0x2e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::ucomiss(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x2e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::ud2() {
  opcode(0x0f,0x0b);
}

void Assembler::unpckhpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  unpckhpd(arg0, arg1.reg);
}

void Assembler::unpckhpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x15);
  mod_rm_sib(arg1,arg0);
}

void Assembler::unpckhps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  unpckhps(arg0, arg1.reg);
}

void Assembler::unpckhps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x15);
  mod_rm_sib(arg1,arg0);
}

void Assembler::unpcklpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  unpcklpd(arg0, arg1.reg);
}

void Assembler::unpcklpd(const Xmm& arg0, const Xmm& arg1) {
  pref_group3();
  rex0(arg1,arg0);
  opcode(0x0f,0x14);
  mod_rm_sib(arg1,arg0);
}

void Assembler::unpcklps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  unpcklps(arg0, arg1.reg);
}

void Assembler::unpcklps(const Xmm& arg0, const Xmm& arg1) {
  rex0(arg1,arg0);
  opcode(0x0f,0x14);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vaddpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vaddpd(arg0, arg1, arg2.reg);
}

void Assembler::vaddpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vaddps(arg0, arg1, arg2.reg);
}

void Assembler::vaddps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x58);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddsubpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vaddsubpd(arg0, arg1, arg2.reg);
}

void Assembler::vaddsubpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd0);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddsubpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd0);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddsubpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd0);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddsubps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vaddsubps(arg0, arg1, arg2.reg);
}

void Assembler::vaddsubps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0xd0);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddsubps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x3,0x0,arg1,arg2,arg0);
  opcode(0xd0);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaddsubps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x3,0x0,arg1,arg2,arg0);
  opcode(0xd0);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaesdec(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vaesdec(arg0, arg1, arg2.reg);
}

void Assembler::vaesdec(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xde);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaesdeclast(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vaesdeclast(arg0, arg1, arg2.reg);
}

void Assembler::vaesdeclast(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaesenc(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vaesenc(arg0, arg1, arg2.reg);
}

void Assembler::vaesenc(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaesenclast(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vaesenclast(arg0, arg1, arg2.reg);
}

void Assembler::vaesenclast(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vaesimc(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vaesimc(arg0, arg1.reg);
}

void Assembler::vaesimc(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xdb);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vaeskeygenassist(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vaeskeygenassist(arg0, arg1.reg, arg2);
}

void Assembler::vaeskeygenassist(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xdf);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vandnpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vandnpd(arg0, arg1, arg2.reg);
}

void Assembler::vandnpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x55);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandnpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x55);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandnpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x55);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandnps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vandnps(arg0, arg1, arg2.reg);
}

void Assembler::vandnps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x55);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandnps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x55);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandnps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x55);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vandpd(arg0, arg1, arg2.reg);
}

void Assembler::vandpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x54);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x54);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x54);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vandps(arg0, arg1, arg2.reg);
}

void Assembler::vandps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x54);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x54);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vandps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x54);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vblendpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vblendpd(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vblendpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0d);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0d);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0d);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vblendps(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vblendps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0c);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0c);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0c);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendvpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Xmm& arg3) {
  p_2_4(arg2);
  vblendvpd(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vblendvpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Xmm& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x4b);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendvpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Ymm& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x4b);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendvpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Ymm& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x4b);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendvps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Xmm& arg3) {
  p_2_4(arg2);
  vblendvps(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vblendvps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Xmm& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x4a);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendvps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Ymm& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x4a);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vblendvps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Ymm& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x4a);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vbroadcastf128(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vbroadcasti128(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vbroadcastsd(const Ymm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x19);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vbroadcastsd(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x19);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vbroadcastss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x18);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vbroadcastss(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x18);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vbroadcastss(const Ymm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x18);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vbroadcastss(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x18);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcmppd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vcmppd(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vcmppd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcmppd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcmppd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcmpps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vcmpps(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vcmpps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcmpps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcmpps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcmpsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcmpsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcmpss(const Xmm& arg0, const Xmm& arg1, const M32& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcmpss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0xc2);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vcomisd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcomisd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcomiss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcomiss(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtdq2pd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtdq2pd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtdq2pd(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vcvtdq2pd(arg0, arg1.reg);
}

void Assembler::vcvtdq2pd(const Ymm& arg0, const Xmm& arg1) {
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtdq2ps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vcvtdq2ps(arg0, arg1.reg);
}

void Assembler::vcvtdq2ps(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtdq2ps(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtdq2ps(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtpd2dq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vcvtpd2dq(arg0, arg1.reg);
}

void Assembler::vcvtpd2dq(const Xmm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtpd2dq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtpd2dq(const Xmm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtpd2ps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vcvtpd2ps(arg0, arg1.reg);
}

void Assembler::vcvtpd2ps(const Xmm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtpd2ps(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtpd2ps(const Xmm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtph2ps(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x13);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtph2ps(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x13);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtph2ps(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vcvtph2ps(arg0, arg1.reg);
}

void Assembler::vcvtph2ps(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x13);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtps2dq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vcvtps2dq(arg0, arg1.reg);
}

void Assembler::vcvtps2dq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtps2dq(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtps2dq(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtps2pd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtps2pd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtps2pd(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vcvtps2pd(arg0, arg1.reg);
}

void Assembler::vcvtps2pd(const Ymm& arg0, const Xmm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtps2ph(const M128& arg0, const Ymm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  vcvtps2ph(arg0.reg, arg1, arg2);
}

void Assembler::vcvtps2ph(const M64& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x1d);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vcvtps2ph(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x1d);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vcvtps2ph(const Xmm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x1d);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vcvtsd2si(const R32& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtsd2si(const R32& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtsd2si(const R64& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x3,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtsd2si(const R64& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x3,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtsd2ss(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtsd2ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtsi2sd(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x2a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtsi2sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x1,arg1,arg2,arg0);
  opcode(0x2a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtsi2sd(const Xmm& arg0, const Xmm& arg1, const R32& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x2a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtsi2sd(const Xmm& arg0, const Xmm& arg1, const R64& arg2) {
  vex(0x01,0x0,0x3,0x1,arg1,arg2,arg0);
  opcode(0x2a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtsi2ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x2a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtsi2ss(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x1,arg1,arg2,arg0);
  opcode(0x2a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtsi2ss(const Xmm& arg0, const Xmm& arg1, const R32& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x2a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtsi2ss(const Xmm& arg0, const Xmm& arg1, const R64& arg2) {
  vex(0x01,0x0,0x2,0x1,arg1,arg2,arg0);
  opcode(0x2a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtss2sd(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtss2sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vcvtss2si(const R32& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtss2si(const R32& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtss2si(const R64& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x2,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvtss2si(const R64& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x2d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttpd2dq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vcvttpd2dq(arg0, arg1.reg);
}

void Assembler::vcvttpd2dq(const Xmm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttpd2dq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttpd2dq(const Xmm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xe6);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttps2dq(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vcvttps2dq(arg0, arg1.reg);
}

void Assembler::vcvttps2dq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttps2dq(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttps2dq(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x5b);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttsd2si(const R32& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttsd2si(const R32& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttsd2si(const R64& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x3,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttsd2si(const R64& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x3,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttss2si(const R32& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttss2si(const R32& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttss2si(const R64& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x2,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vcvttss2si(const R64& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x2c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vdivpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vdivpd(arg0, arg1, arg2.reg);
}

void Assembler::vdivpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdivpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdivpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdivps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vdivps(arg0, arg1, arg2.reg);
}

void Assembler::vdivps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdivps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdivps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdivsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdivsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdivss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdivss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vdppd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vdppd(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vdppd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x41);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vdpps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vdpps(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vdpps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x40);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vdpps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x40);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vdpps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x40);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::verr(const M16& arg0) {
  p_2_4(arg0);
  verr(arg0.reg);
}

void Assembler::verr(const R16& arg0) {
  rex0(arg0);
  opcode(0x0f,0x00);
  mod_rm_sib(arg0,Constants::r64s()[4]);
}

void Assembler::verw(const M16& arg0) {
  p_2_4(arg0);
  verw(arg0.reg);
}

void Assembler::verw(const R16& arg0) {
  rex0(arg0);
  opcode(0x0f,0x00);
  mod_rm_sib(arg0,Constants::r64s()[5]);
}

void Assembler::vextractf128(const M128& arg0, const Ymm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  vextractf128(arg0.reg, arg1, arg2);
}

void Assembler::vextractf128(const Xmm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x19);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vextracti128(const M128& arg0, const Ymm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  vextracti128(arg0.reg, arg1, arg2);
}

void Assembler::vextracti128(const Xmm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x39);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vextractps(const M32& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  vextractps(arg0.reg, arg1, arg2);
}

void Assembler::vextractps(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x17);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vfmadd132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmadd132pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmadd132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x98);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x98);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x98);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmadd132ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmadd132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x98);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x98);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x98);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd132sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x99);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd132sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x99);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd132ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x99);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd132ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x99);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmadd213pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmadd213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmadd213ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmadd213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd213ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmadd231pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmadd231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmadd231ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmadd231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmadd231ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmaddsub132pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmaddsub132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x96);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x96);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x96);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmaddsub132ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmaddsub132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x96);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x96);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x96);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmaddsub213pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmaddsub213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmaddsub213ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmaddsub213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmaddsub231pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmaddsub231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmaddsub231ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmaddsub231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmaddsub231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsub132pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmsub132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsub132ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmsub132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub132ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsub213pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmsub213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xaa);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xaa);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xaa);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsub213ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmsub213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xaa);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xaa);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xaa);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xab);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xab);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xab);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub213ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xab);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsub231pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmsub231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xba);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xba);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xba);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsub231ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmsub231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xba);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xba);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xba);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsub231ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsubadd132pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmsubadd132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x97);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x97);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x97);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsubadd132ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmsubadd132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x97);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x97);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x97);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsubadd213pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmsubadd213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xa7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsubadd213ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmsubadd213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xa7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsubadd231pd(arg0, arg1, arg2.reg);
}

void Assembler::vfmsubadd231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xb7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfmsubadd231ps(arg0, arg1, arg2.reg);
}

void Assembler::vfmsubadd231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfmsubadd231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xb7);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmadd132pd(arg0, arg1, arg2.reg);
}

void Assembler::vfnmadd132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmadd132ps(arg0, arg1, arg2.reg);
}

void Assembler::vfnmadd132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd132ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmadd213pd(arg0, arg1, arg2.reg);
}

void Assembler::vfnmadd213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xac);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xac);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xac);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmadd213ps(arg0, arg1, arg2.reg);
}

void Assembler::vfnmadd213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xac);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xac);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xac);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xad);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xad);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xad);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd213ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xad);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmadd231pd(arg0, arg1, arg2.reg);
}

void Assembler::vfnmadd231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmadd231ps(arg0, arg1, arg2.reg);
}

void Assembler::vfnmadd231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmadd231ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmsub132pd(arg0, arg1, arg2.reg);
}

void Assembler::vfnmsub132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmsub132ps(arg0, arg1, arg2.reg);
}

void Assembler::vfnmsub132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x9f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub132ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x9f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmsub213pd(arg0, arg1, arg2.reg);
}

void Assembler::vfnmsub213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xae);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xae);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xae);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmsub213ps(arg0, arg1, arg2.reg);
}

void Assembler::vfnmsub213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xae);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xae);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xae);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xaf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xaf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xaf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub213ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xaf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmsub231pd(arg0, arg1, arg2.reg);
}

void Assembler::vfnmsub231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbe);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbe);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbe);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vfnmsub231ps(arg0, arg1, arg2.reg);
}

void Assembler::vfnmsub231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbe);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbe);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbe);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0xbf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vfnmsub231ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xbf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vgatherdpd(const Xmm& arg0, const M32& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x1,arg2,arg1,arg0);
  opcode(0x92);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vgatherdpd(const Ymm& arg0, const M32& arg1, const Ymm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x1,arg2,arg1,arg0);
  opcode(0x92);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vgatherdps(const Xmm& arg0, const M32& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,arg2,arg1,arg0);
  opcode(0x92);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vgatherdps(const Ymm& arg0, const M32& arg1, const Ymm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,arg2,arg1,arg0);
  opcode(0x92);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vgatherqpd(const Xmm& arg0, const M64& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x1,arg2,arg1,arg0);
  opcode(0x93);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vgatherqpd(const Ymm& arg0, const M64& arg1, const Ymm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x1,arg2,arg1,arg0);
  opcode(0x93);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vgatherqps(const Xmm& arg0, const M64& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,arg2,arg1,arg0);
  opcode(0x93);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vgatherqps_1(const Xmm& arg0, const M64& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,arg2,arg1,arg0);
  opcode(0x93);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vhaddpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vhaddpd(arg0, arg1, arg2.reg);
}

void Assembler::vhaddpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x7c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhaddpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x7c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhaddpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x7c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhaddps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vhaddps(arg0, arg1, arg2.reg);
}

void Assembler::vhaddps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x7c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhaddps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x3,0x0,arg1,arg2,arg0);
  opcode(0x7c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhaddps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x3,0x0,arg1,arg2,arg0);
  opcode(0x7c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhsubpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vhsubpd(arg0, arg1, arg2.reg);
}

void Assembler::vhsubpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x7d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhsubpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x7d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhsubpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x7d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhsubps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vhsubps(arg0, arg1, arg2.reg);
}

void Assembler::vhsubps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x7d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhsubps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x3,0x0,arg1,arg2,arg0);
  opcode(0x7d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vhsubps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x3,0x0,arg1,arg2,arg0);
  opcode(0x7d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vinsertf128(const Ymm& arg0, const Ymm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vinsertf128(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vinsertf128(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x18);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vinserti128(const Ymm& arg0, const Ymm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vinserti128(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vinserti128(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x38);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vinsertps(const Xmm& arg0, const Xmm& arg1, const M32& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x21);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vinsertps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x21);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vlddqu(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xf0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vlddqu(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xf0);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vldmxcsr(const M32& arg0) {
  p_2_4(arg0);
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg0,Constants::r64s()[2]);
  opcode(0xae);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::vmaskmovdqu(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xf7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmaskmovpd(const M128& arg0, const Xmm& arg1, const Xmm& arg2) {
  p_2_4(arg0);
  vex(0x02,0x0,0x1,0x0,arg1,arg0,arg2);
  opcode(0x2f);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vmaskmovpd(const M256& arg0, const Ymm& arg1, const Ymm& arg2) {
  p_2_4(arg0);
  vex(0x02,0x1,0x1,0x0,arg1,arg0,arg2);
  opcode(0x2f);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vmaskmovpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x2d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaskmovpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x2d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaskmovps(const M128& arg0, const Xmm& arg1, const Xmm& arg2) {
  p_2_4(arg0);
  vex(0x02,0x0,0x1,0x0,arg1,arg0,arg2);
  opcode(0x2e);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vmaskmovps(const M256& arg0, const Ymm& arg1, const Ymm& arg2) {
  p_2_4(arg0);
  vex(0x02,0x1,0x1,0x0,arg1,arg0,arg2);
  opcode(0x2e);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vmaskmovps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x2c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaskmovps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x2c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vmaxpd(arg0, arg1, arg2.reg);
}

void Assembler::vmaxpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vmaxps(arg0, arg1, arg2.reg);
}

void Assembler::vmaxps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmaxss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vminpd(arg0, arg1, arg2.reg);
}

void Assembler::vminpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vminps(arg0, arg1, arg2.reg);
}

void Assembler::vminps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vminss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmovapd(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vmovapd(arg0.reg, arg1);
}

void Assembler::vmovapd(const M256& arg0, const Ymm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovapd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vmovapd(arg0, arg1.reg);
}

void Assembler::vmovapd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x28);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovapd_1(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovapd(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x28);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovapd(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x28);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovapd_1(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovaps(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vmovaps(arg0.reg, arg1);
}

void Assembler::vmovaps(const M256& arg0, const Ymm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovaps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vmovaps(arg0, arg1.reg);
}

void Assembler::vmovaps(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x28);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovaps_1(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovaps(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x28);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovaps(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x28);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovaps_1(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x29);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovd(const M32& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vmovd(arg0.reg, arg1);
}

void Assembler::vmovd(const R32& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovd(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vmovd(arg0, arg1.reg);
}

void Assembler::vmovd(const Xmm& arg0, const R32& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x6e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovddup(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovddup(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovddup(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovddup(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovdqa(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vmovdqa(arg0.reg, arg1);
}

void Assembler::vmovdqa(const M256& arg0, const Ymm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovdqa(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vmovdqa(arg0, arg1.reg);
}

void Assembler::vmovdqa(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x6f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovdqa_1(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovdqa(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x6f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovdqa(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x6f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovdqa_1(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovdqu(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vmovdqu(arg0.reg, arg1);
}

void Assembler::vmovdqu(const M256& arg0, const Ymm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovdqu(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vmovdqu(arg0, arg1.reg);
}

void Assembler::vmovdqu(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x6f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovdqu_1(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovdqu(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x6f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovdqu(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x6f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovdqu_1(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x7f);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovhlps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x12);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmovhpd(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x17);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovhpd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x16);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmovhps(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x17);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovhps(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x16);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmovlhps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x16);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmovlpd(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x13);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovlpd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x12);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmovlps(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x13);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovlps(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x12);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmovmskpd(const R32& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovmskpd(const R32& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovmskpd(const R64& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovmskpd(const R64& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovmskps(const R32& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovmskps(const R32& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovmskps(const R64& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovmskps(const R64& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x50);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovntdq(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0xe7);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovntdq(const M256& arg0, const Ymm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0xe7);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovntdqa(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovntdqa(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2a);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovntpd(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x2b);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovntpd(const M256& arg0, const Ymm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x2b);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovntps(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x2b);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovntps(const M256& arg0, const Ymm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x2b);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovq(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x1,0x1,Constants::xmm0(),arg0,arg1);
  opcode(0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovq_1(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0xd6);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovq(const R64& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x1,Constants::xmm0(),arg0,arg1);
  opcode(0x7e);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovq(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x1,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x6e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovq_1(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x7e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovq(const Xmm& arg0, const R64& arg1) {
  vex(0x01,0x0,0x1,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x6e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x7e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovq_1(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0xd6);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovsd(const M64& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovsd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x10);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmovsd_1(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg0,arg2);
  opcode(0x11);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vmovshdup(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vmovshdup(arg0, arg1.reg);
}

void Assembler::vmovshdup(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x16);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovshdup(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x16);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovshdup(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x16);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovsldup(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vmovsldup(arg0, arg1.reg);
}

void Assembler::vmovsldup(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovsldup(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovsldup(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x12);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovss(const M32& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x10);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmovss_1(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg0,arg2);
  opcode(0x11);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vmovupd(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vmovupd(arg0.reg, arg1);
}

void Assembler::vmovupd(const M256& arg0, const Ymm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovupd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vmovupd(arg0, arg1.reg);
}

void Assembler::vmovupd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovupd_1(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovupd(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovupd(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovupd_1(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovups(const M128& arg0, const Xmm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovups(const M256& arg0, const Ymm& arg1) {
  p_2_4(arg0);
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovups(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vmovups(arg0, arg1.reg);
}

void Assembler::vmovups(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovups_1(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmovups(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovups(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x10);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vmovups_1(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x11);
  mod_rm_sib(arg0,arg1);
}

void Assembler::vmpsadbw(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vmpsadbw(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vmpsadbw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x42);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vmpsadbw(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x42);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vmpsadbw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x42);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vmulpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vmulpd(arg0, arg1, arg2.reg);
}

void Assembler::vmulpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmulpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmulpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmulps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vmulps(arg0, arg1, arg2.reg);
}

void Assembler::vmulps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmulps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmulps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmulsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmulsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmulss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vmulss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x59);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vorpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vorpd(arg0, arg1, arg2.reg);
}

void Assembler::vorpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x56);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vorpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x56);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vorpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x56);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vorps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vorps(arg0, arg1, arg2.reg);
}

void Assembler::vorps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x56);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vorps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x56);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vorps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x56);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpabsb(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vpabsb(arg0, arg1.reg);
}

void Assembler::vpabsb(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpabsb(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpabsb(const Ymm& arg0, const Ymm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1c);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpabsd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vpabsd(arg0, arg1.reg);
}

void Assembler::vpabsd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpabsd(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpabsd(const Ymm& arg0, const Ymm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpabsw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vpabsw(arg0, arg1.reg);
}

void Assembler::vpabsw(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpabsw(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpabsw(const Ymm& arg0, const Ymm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x1d);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpackssdw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpackssdw(arg0, arg1, arg2.reg);
}

void Assembler::vpackssdw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpackssdw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpackssdw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpacksswb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpacksswb(arg0, arg1, arg2.reg);
}

void Assembler::vpacksswb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x63);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpacksswb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x63);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpacksswb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x63);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpackusdw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpackusdw(arg0, arg1, arg2.reg);
}

void Assembler::vpackusdw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x2b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpackusdw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x2b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpackusdw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x2b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpackuswb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpackuswb(arg0, arg1, arg2.reg);
}

void Assembler::vpackuswb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x67);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpackuswb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x67);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpackuswb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x67);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpaddb(arg0, arg1, arg2.reg);
}

void Assembler::vpaddb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpaddd(arg0, arg1, arg2.reg);
}

void Assembler::vpaddd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfe);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfe);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfe);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpaddq(arg0, arg1, arg2.reg);
}

void Assembler::vpaddq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd4);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd4);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd4);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddsb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpaddsb(arg0, arg1, arg2.reg);
}

void Assembler::vpaddsb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xec);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddsb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xec);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddsb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xec);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpaddsw(arg0, arg1, arg2.reg);
}

void Assembler::vpaddsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xed);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xed);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xed);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddusb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpaddusb(arg0, arg1, arg2.reg);
}

void Assembler::vpaddusb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddusb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddusb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdc);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddusw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpaddusw(arg0, arg1, arg2.reg);
}

void Assembler::vpaddusw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddusw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddusw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpaddw(arg0, arg1, arg2.reg);
}

void Assembler::vpaddw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpaddw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfd);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpalignr(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vpalignr(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vpalignr(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0f);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpalignr(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0f);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpalignr(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0f);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpand(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpand(arg0, arg1, arg2.reg);
}

void Assembler::vpand(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpand(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpand(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpandn(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpandn(arg0, arg1, arg2.reg);
}

void Assembler::vpandn(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpandn(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpandn(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xdf);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpavgb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpavgb(arg0, arg1, arg2.reg);
}

void Assembler::vpavgb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe0);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpavgb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe0);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpavgb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe0);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpavgw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpavgw(arg0, arg1, arg2.reg);
}

void Assembler::vpavgw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe3);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpavgw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe3);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpavgw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe3);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpblendd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vpblendd(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vpblendd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x02);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpblendd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x02);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpblendd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x02);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpblendvb(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Xmm& arg3) {
  p_2_4(arg2);
  vpblendvb(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vpblendvb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Xmm& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x4c);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpblendvb(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Ymm& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x4c);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpblendvb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Ymm& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x4c);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpblendw(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vpblendw(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vpblendw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0e);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpblendw(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0e);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpblendw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0e);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpbroadcastb(const Xmm& arg0, const M8& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x78);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastb(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x78);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastb(const Ymm& arg0, const M8& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x78);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastb(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x78);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastd(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x58);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x58);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastd(const Ymm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x58);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastd(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x58);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastq(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastq(const Ymm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastq(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x59);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastw(const Xmm& arg0, const M16& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x79);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastw(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x79);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastw(const Ymm& arg0, const M16& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x79);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpbroadcastw(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x79);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpclmulqdq(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vpclmulqdq(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vpclmulqdq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x44);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpcmpeqb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpcmpeqb(arg0, arg1, arg2.reg);
}

void Assembler::vpcmpeqb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x74);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x74);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x74);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpcmpeqd(arg0, arg1, arg2.reg);
}

void Assembler::vpcmpeqd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x76);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x76);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x76);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpcmpeqq(arg0, arg1, arg2.reg);
}

void Assembler::vpcmpeqq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x29);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x29);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x29);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpcmpeqw(arg0, arg1, arg2.reg);
}

void Assembler::vpcmpeqw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x75);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x75);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpeqw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x75);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpestri(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vpcmpestri(arg0, arg1.reg, arg2);
}

void Assembler::vpcmpestri(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x61);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpcmpestrm(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vpcmpestrm(arg0, arg1.reg, arg2);
}

void Assembler::vpcmpestrm(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x60);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpcmpgtb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpcmpgtb(arg0, arg1, arg2.reg);
}

void Assembler::vpcmpgtb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x64);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x64);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x64);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpcmpgtd(arg0, arg1, arg2.reg);
}

void Assembler::vpcmpgtd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x66);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x66);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x66);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpcmpgtq(arg0, arg1, arg2.reg);
}

void Assembler::vpcmpgtq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x37);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x37);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x37);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpcmpgtw(arg0, arg1, arg2.reg);
}

void Assembler::vpcmpgtw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x65);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x65);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpgtw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x65);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpcmpistri(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vpcmpistri(arg0, arg1.reg, arg2);
}

void Assembler::vpcmpistri(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x63);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpcmpistrm(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vpcmpistrm(arg0, arg1.reg, arg2);
}

void Assembler::vpcmpistrm(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x62);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vperm2f128(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x06);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vperm2f128(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x06);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vperm2i128(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x46);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vperm2i128(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x03,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x46);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpermd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x36);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x36);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermilpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vpermilpd(arg0, arg1.reg, arg2);
}

void Assembler::vpermilpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x05);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpermilpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpermilpd(arg0, arg1, arg2.reg);
}

void Assembler::vpermilpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermilpd(const Ymm& arg0, const M256& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x05);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpermilpd(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x05);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpermilpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermilpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermilps(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vpermilps(arg0, arg1.reg, arg2);
}

void Assembler::vpermilps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x04);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpermilps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpermilps(arg0, arg1, arg2.reg);
}

void Assembler::vpermilps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermilps(const Ymm& arg0, const M256& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x04);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpermilps(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x04);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpermilps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermilps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermpd(const Ymm& arg0, const M256& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vex(0x03,0x1,0x1,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x01);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpermpd(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x03,0x1,0x1,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x01);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpermps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x16);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x16);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpermq(const Ymm& arg0, const M256& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vex(0x03,0x1,0x1,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x00);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpermq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x03,0x1,0x1,0x1,Constants::xmm0(),arg1,arg0);
  opcode(0x00);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpextrb(const M8& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x14);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vpextrb(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x14);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vpextrb(const R64& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x14);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vpextrd(const M32& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  vpextrd(arg0.reg, arg1, arg2);
}

void Assembler::vpextrd(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x16);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vpextrq(const M64& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  vex(0x03,0x0,0x1,0x1,Constants::xmm0(),arg0,arg1);
  opcode(0x16);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vpextrq(const R64& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x1,Constants::xmm0(),arg0,arg1);
  opcode(0x16);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vpextrw(const M16& arg0, const Xmm& arg1, const Imm8& arg2) {
  p_2_4(arg0);
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x15);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vpextrw(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xc5);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpextrw_1(const R32& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x15);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vpextrw(const R64& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xc5);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpextrw_1(const R64& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg0,arg1);
  opcode(0x15);
  mod_rm_sib(arg0,arg1);
  disp_imm(arg2);
}

void Assembler::vpgatherdd(const Xmm& arg0, const M32& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,arg2,arg1,arg0);
  opcode(0x90);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpgatherdd(const Ymm& arg0, const M32& arg1, const Ymm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,arg2,arg1,arg0);
  opcode(0x90);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpgatherdq(const Xmm& arg0, const M32& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x1,arg2,arg1,arg0);
  opcode(0x90);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpgatherdq(const Ymm& arg0, const M32& arg1, const Ymm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x1,arg2,arg1,arg0);
  opcode(0x90);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpgatherqd(const Xmm& arg0, const M64& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,arg2,arg1,arg0);
  opcode(0x91);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpgatherqd_1(const Xmm& arg0, const M64& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,arg2,arg1,arg0);
  opcode(0x91);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpgatherqq(const Xmm& arg0, const M64& arg1, const Xmm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x1,arg2,arg1,arg0);
  opcode(0x91);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpgatherqq(const Ymm& arg0, const M64& arg1, const Ymm& arg2) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x1,arg2,arg1,arg0);
  opcode(0x91);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vphaddd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vphaddd(arg0, arg1, arg2.reg);
}

void Assembler::vphaddd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x02);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphaddd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x02);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphaddd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x02);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphaddsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vphaddsw(arg0, arg1, arg2.reg);
}

void Assembler::vphaddsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x03);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphaddsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x03);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphaddsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x03);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphaddw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vphaddw(arg0, arg1, arg2.reg);
}

void Assembler::vphaddw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x01);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphaddw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x01);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphaddw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x01);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphminposuw(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vphminposuw(arg0, arg1.reg);
}

void Assembler::vphminposuw(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x41);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vphsubd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vphsubd(arg0, arg1, arg2.reg);
}

void Assembler::vphsubd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x06);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphsubd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x06);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphsubd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x06);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphsubsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vphsubsw(arg0, arg1, arg2.reg);
}

void Assembler::vphsubsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x07);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphsubsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x07);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphsubsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x07);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphsubw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vphsubw(arg0, arg1, arg2.reg);
}

void Assembler::vphsubw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x05);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphsubw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x05);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vphsubw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x05);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpinsrb(const Xmm& arg0, const Xmm& arg1, const M8& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x20);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpinsrb(const Xmm& arg0, const Xmm& arg1, const R32& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x20);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpinsrd(const Xmm& arg0, const Xmm& arg1, const M32& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vpinsrd(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vpinsrd(const Xmm& arg0, const Xmm& arg1, const R32& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x22);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpinsrq(const Xmm& arg0, const Xmm& arg1, const M64& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vpinsrq(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vpinsrq(const Xmm& arg0, const Xmm& arg1, const R64& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x22);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpinsrw(const Xmm& arg0, const Xmm& arg1, const M16& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xc4);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpinsrw(const Xmm& arg0, const Xmm& arg1, const R32& arg2, const Imm8& arg3) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xc4);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vpmaddubsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmaddubsw(arg0, arg1, arg2.reg);
}

void Assembler::vpmaddubsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x04);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaddubsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x04);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaddubsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x04);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaddwd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmaddwd(arg0, arg1, arg2.reg);
}

void Assembler::vpmaddwd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaddwd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaddwd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaskmovd(const M128& arg0, const Xmm& arg1, const Xmm& arg2) {
  p_2_4(arg0);
  vex(0x02,0x0,0x1,0x0,arg1,arg0,arg2);
  opcode(0x8e);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vpmaskmovd(const M256& arg0, const Ymm& arg1, const Ymm& arg2) {
  p_2_4(arg0);
  vex(0x02,0x1,0x1,0x0,arg1,arg0,arg2);
  opcode(0x8e);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vpmaskmovd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x8c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaskmovd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x8c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaskmovq(const M128& arg0, const Xmm& arg1, const Xmm& arg2) {
  p_2_4(arg0);
  vex(0x02,0x0,0x1,0x1,arg1,arg0,arg2);
  opcode(0x8e);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vpmaskmovq(const M256& arg0, const Ymm& arg1, const Ymm& arg2) {
  p_2_4(arg0);
  vex(0x02,0x1,0x1,0x1,arg1,arg0,arg2);
  opcode(0x8e);
  mod_rm_sib(arg0,arg2);
}

void Assembler::vpmaskmovq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x8c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaskmovq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x8c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxsb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmaxsb(arg0, arg1, arg2.reg);
}

void Assembler::vpmaxsb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxsb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxsb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxsd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmaxsd(arg0, arg1, arg2.reg);
}

void Assembler::vpmaxsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxsd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxsd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmaxsw(arg0, arg1, arg2.reg);
}

void Assembler::vpmaxsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xee);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xee);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xee);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxub(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmaxub(arg0, arg1, arg2.reg);
}

void Assembler::vpmaxub(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xde);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxub(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xde);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxub(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xde);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxud(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmaxud(arg0, arg1, arg2.reg);
}

void Assembler::vpmaxud(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxud(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxud(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3f);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxuw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmaxuw(arg0, arg1, arg2.reg);
}

void Assembler::vpmaxuw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxuw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmaxuw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3e);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminsb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpminsb(arg0, arg1, arg2.reg);
}

void Assembler::vpminsb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x38);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminsb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x38);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminsb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x38);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminsd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpminsd(arg0, arg1, arg2.reg);
}

void Assembler::vpminsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x39);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminsd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x39);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminsd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x39);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpminsw(arg0, arg1, arg2.reg);
}

void Assembler::vpminsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xea);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminub(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpminub(arg0, arg1, arg2.reg);
}

void Assembler::vpminub(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xda);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminub(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xda);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminub(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xda);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminud(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpminud(arg0, arg1, arg2.reg);
}

void Assembler::vpminud(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminud(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminud(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminuw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpminuw(arg0, arg1, arg2.reg);
}

void Assembler::vpminuw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminuw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpminuw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x3a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmovmskb(const R32& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xd7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovmskb(const R32& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xd7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovmskb(const R64& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xd7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovmskb(const R64& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0xd7);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbd(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x21);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x21);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbd(const Ymm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x21);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbd(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x21);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbq(const Xmm& arg0, const M16& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x22);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x22);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbq(const Ymm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x22);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbq(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x22);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbw(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x20);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbw(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x20);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxbw(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vpmovsxbw(arg0, arg1.reg);
}

void Assembler::vpmovsxbw(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x20);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxdq(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x25);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxdq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x25);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxdq(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vpmovsxdq(arg0, arg1.reg);
}

void Assembler::vpmovsxdq(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x25);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxwd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x23);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxwd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x23);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxwd(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vpmovsxwd(arg0, arg1.reg);
}

void Assembler::vpmovsxwd(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x23);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxwq(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x24);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxwq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x24);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxwq(const Ymm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x24);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovsxwq(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x24);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbd(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x31);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x31);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbd(const Ymm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x31);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbd(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x31);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbq(const Xmm& arg0, const M16& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x32);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x32);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbq(const Ymm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x32);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbq(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x32);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbw(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x30);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbw(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x30);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxbw(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vpmovzxbw(arg0, arg1.reg);
}

void Assembler::vpmovzxbw(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x30);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxdq(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x35);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxdq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x35);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxdq(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vpmovzxdq(arg0, arg1.reg);
}

void Assembler::vpmovzxdq(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x35);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxwd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x33);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxwd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x33);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxwd(const Ymm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vpmovzxwd(arg0, arg1.reg);
}

void Assembler::vpmovzxwd(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x33);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxwq(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x34);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxwq(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x34);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxwq(const Ymm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x34);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmovzxwq(const Ymm& arg0, const Xmm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x34);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpmuldq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmuldq(arg0, arg1, arg2.reg);
}

void Assembler::vpmuldq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x28);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmuldq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x28);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmuldq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x28);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulhrsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmulhrsw(arg0, arg1, arg2.reg);
}

void Assembler::vpmulhrsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulhrsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulhrsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0b);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulhuw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmulhuw(arg0, arg1, arg2.reg);
}

void Assembler::vpmulhuw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe4);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulhuw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe4);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulhuw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe4);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulhw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmulhw(arg0, arg1, arg2.reg);
}

void Assembler::vpmulhw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulhw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulhw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulld(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmulld(arg0, arg1, arg2.reg);
}

void Assembler::vpmulld(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x40);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulld(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x40);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmulld(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x40);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmullw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmullw(arg0, arg1, arg2.reg);
}

void Assembler::vpmullw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmullw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmullw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd5);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmuludq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpmuludq(arg0, arg1, arg2.reg);
}

void Assembler::vpmuludq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf4);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmuludq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf4);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpmuludq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf4);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpor(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpor(arg0, arg1, arg2.reg);
}

void Assembler::vpor(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xeb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpor(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xeb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpor(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xeb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsadbw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsadbw(arg0, arg1, arg2.reg);
}

void Assembler::vpsadbw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsadbw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsadbw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf6);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpshufb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpshufb(arg0, arg1, arg2.reg);
}

void Assembler::vpshufb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x00);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpshufb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x00);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpshufb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x00);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpshufd(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vpshufd(arg0, arg1.reg, arg2);
}

void Assembler::vpshufd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpshufd(const Ymm& arg0, const M256& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpshufd(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpshufhw(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vpshufhw(arg0, arg1.reg, arg2);
}

void Assembler::vpshufhw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpshufhw(const Ymm& arg0, const M256& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpshufhw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x2,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpshuflw(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vpshuflw(arg0, arg1.reg, arg2);
}

void Assembler::vpshuflw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpshuflw(const Ymm& arg0, const M256& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vex(0x01,0x1,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpshuflw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x3,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x70);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vpsignb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsignb(arg0, arg1, arg2.reg);
}

void Assembler::vpsignb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x08);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsignd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsignd(arg0, arg1, arg2.reg);
}

void Assembler::vpsignd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsignw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsignw(arg0, arg1, arg2.reg);
}

void Assembler::vpsignw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x09);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpslld(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[6]);
  opcode(0x72);
  mod_rm_sib(arg1,Constants::r64s()[6]);
  disp_imm(arg2);
}

void Assembler::vpslld(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpslld(arg0, arg1, arg2.reg);
}

void Assembler::vpslld(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf2);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpslld(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[6]);
  opcode(0x72);
  mod_rm_sib(arg1,Constants::r64s()[6]);
  disp_imm(arg2);
}

void Assembler::vpslld(const Ymm& arg0, const Ymm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpslld(arg0, arg1, arg2.reg);
}

void Assembler::vpslld(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf2);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpslldq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[7]);
  opcode(0x73);
  mod_rm_sib(arg1,Constants::r64s()[7]);
  disp_imm(arg2);
}

void Assembler::vpslldq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[7]);
  opcode(0x73);
  mod_rm_sib(arg1,Constants::r64s()[7]);
  disp_imm(arg2);
}

void Assembler::vpsllq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[6]);
  opcode(0x73);
  mod_rm_sib(arg1,Constants::r64s()[6]);
  disp_imm(arg2);
}

void Assembler::vpsllq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsllq(arg0, arg1, arg2.reg);
}

void Assembler::vpsllq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf3);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsllq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[6]);
  opcode(0x73);
  mod_rm_sib(arg1,Constants::r64s()[6]);
  disp_imm(arg2);
}

void Assembler::vpsllq(const Ymm& arg0, const Ymm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsllq(arg0, arg1, arg2.reg);
}

void Assembler::vpsllq(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf3);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsllvd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsllvd(arg0, arg1, arg2.reg);
}

void Assembler::vpsllvd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x47);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsllvd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x47);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsllvd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x47);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsllvq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsllvq(arg0, arg1, arg2.reg);
}

void Assembler::vpsllvq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x47);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsllvq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x47);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsllvq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x47);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsllw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[6]);
  opcode(0x71);
  mod_rm_sib(arg1,Constants::r64s()[6]);
  disp_imm(arg2);
}

void Assembler::vpsllw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsllw(arg0, arg1, arg2.reg);
}

void Assembler::vpsllw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf1);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsllw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[6]);
  opcode(0x71);
  mod_rm_sib(arg1,Constants::r64s()[6]);
  disp_imm(arg2);
}

void Assembler::vpsllw(const Ymm& arg0, const Ymm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsllw(arg0, arg1, arg2.reg);
}

void Assembler::vpsllw(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf1);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrad(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[4]);
  opcode(0x72);
  mod_rm_sib(arg1,Constants::r64s()[4]);
  disp_imm(arg2);
}

void Assembler::vpsrad(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrad(arg0, arg1, arg2.reg);
}

void Assembler::vpsrad(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe2);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrad(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[4]);
  opcode(0x72);
  mod_rm_sib(arg1,Constants::r64s()[4]);
  disp_imm(arg2);
}

void Assembler::vpsrad(const Ymm& arg0, const Ymm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrad(arg0, arg1, arg2.reg);
}

void Assembler::vpsrad(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe2);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsravd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsravd(arg0, arg1, arg2.reg);
}

void Assembler::vpsravd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x46);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsravd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x46);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsravd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x46);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsraw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[4]);
  opcode(0x71);
  mod_rm_sib(arg1,Constants::r64s()[4]);
  disp_imm(arg2);
}

void Assembler::vpsraw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsraw(arg0, arg1, arg2.reg);
}

void Assembler::vpsraw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe1);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsraw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[4]);
  opcode(0x71);
  mod_rm_sib(arg1,Constants::r64s()[4]);
  disp_imm(arg2);
}

void Assembler::vpsraw(const Ymm& arg0, const Ymm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsraw(arg0, arg1, arg2.reg);
}

void Assembler::vpsraw(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe1);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrld(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[2]);
  opcode(0x72);
  mod_rm_sib(arg1,Constants::r64s()[2]);
  disp_imm(arg2);
}

void Assembler::vpsrld(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrld(arg0, arg1, arg2.reg);
}

void Assembler::vpsrld(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd2);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrld(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[2]);
  opcode(0x72);
  mod_rm_sib(arg1,Constants::r64s()[2]);
  disp_imm(arg2);
}

void Assembler::vpsrld(const Ymm& arg0, const Ymm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrld(arg0, arg1, arg2.reg);
}

void Assembler::vpsrld(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd2);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrldq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[3]);
  opcode(0x73);
  mod_rm_sib(arg1,Constants::r64s()[3]);
  disp_imm(arg2);
}

void Assembler::vpsrldq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[3]);
  opcode(0x73);
  mod_rm_sib(arg1,Constants::r64s()[3]);
  disp_imm(arg2);
}

void Assembler::vpsrlq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[2]);
  opcode(0x73);
  mod_rm_sib(arg1,Constants::r64s()[2]);
  disp_imm(arg2);
}

void Assembler::vpsrlq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrlq(arg0, arg1, arg2.reg);
}

void Assembler::vpsrlq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd3);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrlq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[2]);
  opcode(0x73);
  mod_rm_sib(arg1,Constants::r64s()[2]);
  disp_imm(arg2);
}

void Assembler::vpsrlq(const Ymm& arg0, const Ymm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrlq(arg0, arg1, arg2.reg);
}

void Assembler::vpsrlq(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd3);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrlvd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrlvd(arg0, arg1, arg2.reg);
}

void Assembler::vpsrlvd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x45);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrlvd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x45);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrlvd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x45);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrlvq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrlvq(arg0, arg1, arg2.reg);
}

void Assembler::vpsrlvq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x02,0x0,0x1,0x1,arg1,arg2,arg0);
  opcode(0x45);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrlvq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x45);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrlvq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x02,0x1,0x1,0x1,arg1,arg2,arg0);
  opcode(0x45);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrlw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x01,0x0,0x1,0x0,arg0,arg1,Constants::r64s()[2]);
  opcode(0x71);
  mod_rm_sib(arg1,Constants::r64s()[2]);
  disp_imm(arg2);
}

void Assembler::vpsrlw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrlw(arg0, arg1, arg2.reg);
}

void Assembler::vpsrlw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd1);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsrlw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x01,0x1,0x1,0x0,arg0,arg1,Constants::r64s()[2]);
  opcode(0x71);
  mod_rm_sib(arg1,Constants::r64s()[2]);
  disp_imm(arg2);
}

void Assembler::vpsrlw(const Ymm& arg0, const Ymm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsrlw(arg0, arg1, arg2.reg);
}

void Assembler::vpsrlw(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd1);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsubb(arg0, arg1, arg2.reg);
}

void Assembler::vpsubb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsubd(arg0, arg1, arg2.reg);
}

void Assembler::vpsubd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfa);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfa);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfa);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsubq(arg0, arg1, arg2.reg);
}

void Assembler::vpsubq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xfb);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubsb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsubsb(arg0, arg1, arg2.reg);
}

void Assembler::vpsubsb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubsb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubsb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsubsw(arg0, arg1, arg2.reg);
}

void Assembler::vpsubsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xe9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubusb(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsubusb(arg0, arg1, arg2.reg);
}

void Assembler::vpsubusb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubusb(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubusb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd8);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubusw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsubusw(arg0, arg1, arg2.reg);
}

void Assembler::vpsubusw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubusw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubusw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xd9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpsubw(arg0, arg1, arg2.reg);
}

void Assembler::vpsubw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpsubw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xf9);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vptest(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vptest(arg0, arg1.reg);
}

void Assembler::vptest(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x17);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vptest(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x17);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vptest(const Ymm& arg0, const Ymm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x17);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vpunpckhbw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpunpckhbw(arg0, arg1, arg2.reg);
}

void Assembler::vpunpckhbw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x68);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhbw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x68);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhbw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x68);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhdq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpunpckhdq(arg0, arg1, arg2.reg);
}

void Assembler::vpunpckhdq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhdq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhdq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6a);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhqdq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpunpckhqdq(arg0, arg1, arg2.reg);
}

void Assembler::vpunpckhqdq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhqdq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhqdq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6d);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhwd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpunpckhwd(arg0, arg1, arg2.reg);
}

void Assembler::vpunpckhwd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x69);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhwd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x69);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckhwd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x69);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpcklbw(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpunpcklbw(arg0, arg1, arg2.reg);
}

void Assembler::vpunpcklbw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x60);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpcklbw(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x60);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpcklbw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x60);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckldq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpunpckldq(arg0, arg1, arg2.reg);
}

void Assembler::vpunpckldq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x62);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckldq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x62);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpckldq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x62);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpcklqdq(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpunpcklqdq(arg0, arg1, arg2.reg);
}

void Assembler::vpunpcklqdq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpcklqdq(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpcklqdq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x6c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpcklwd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpunpcklwd(arg0, arg1, arg2.reg);
}

void Assembler::vpunpcklwd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x61);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpcklwd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x61);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpunpcklwd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x61);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpxor(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vpxor(arg0, arg1, arg2.reg);
}

void Assembler::vpxor(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xef);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpxor(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xef);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vpxor(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xef);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vrcpps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vrcpps(arg0, arg1.reg);
}

void Assembler::vrcpps(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x53);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vrcpps(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x53);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vrcpps(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x53);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vrcpss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x53);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vrcpss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x53);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vroundpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vroundpd(arg0, arg1.reg, arg2);
}

void Assembler::vroundpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x09);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vroundpd(const Ymm& arg0, const M256& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x09);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vroundpd(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x09);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vroundps(const Xmm& arg0, const M128& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vroundps(arg0, arg1.reg, arg2);
}

void Assembler::vroundps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2) {
  vex(0x03,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x08);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vroundps(const Ymm& arg0, const M256& arg1, const Imm8& arg2) {
  p_2_4(arg1);
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x08);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vroundps(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2) {
  vex(0x03,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x08);
  mod_rm_sib(arg1,arg0);
  disp_imm(arg2);
}

void Assembler::vroundsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0b);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vroundsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0b);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vroundss(const Xmm& arg0, const Xmm& arg1, const M32& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0a);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vroundss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x03,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x0a);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vrsqrtps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vrsqrtps(arg0, arg1.reg);
}

void Assembler::vrsqrtps(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x52);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vrsqrtps(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x52);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vrsqrtps(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x52);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vrsqrtss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x52);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vrsqrtss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x52);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vshufpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vshufpd(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vshufpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0xc6);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vshufpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xc6);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vshufpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0xc6);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vshufps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vshufps(arg0, arg1, arg2.reg, arg3);
}

void Assembler::vshufps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0xc6);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vshufps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0xc6);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vshufps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0xc6);
  mod_rm_sib(arg2,arg0);
  disp_imm(arg3);
}

void Assembler::vsqrtpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vsqrtpd(arg0, arg1.reg);
}

void Assembler::vsqrtpd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vsqrtpd(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vsqrtpd(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vsqrtps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vsqrtps(arg0, arg1.reg);
}

void Assembler::vsqrtps(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vsqrtps(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vsqrtps(const Ymm& arg0, const Ymm& arg1) {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x51);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vsqrtsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x51);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsqrtsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x51);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsqrtss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x51);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsqrtss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x51);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vstmxcsr(const M32& arg0) {
  p_2_4(arg0);
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg0,Constants::r64s()[3]);
  opcode(0xae);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::vsubpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vsubpd(arg0, arg1, arg2.reg);
}

void Assembler::vsubpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsubpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsubpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsubps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vsubps(arg0, arg1, arg2.reg);
}

void Assembler::vsubps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsubps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsubps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsubsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsubsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x3,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsubss(const Xmm& arg0, const Xmm& arg1, const M32& arg2) {
  p_2_4(arg2);
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vsubss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x2,0x0,arg1,arg2,arg0);
  opcode(0x5c);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vtestpd(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vtestpd(arg0, arg1.reg);
}

void Assembler::vtestpd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x0f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vtestpd(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x0f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vtestpd(const Ymm& arg0, const Ymm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x0f);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vtestps(const Xmm& arg0, const M128& arg1) {
  p_2_4(arg1);
  vtestps(arg0, arg1.reg);
}

void Assembler::vtestps(const Xmm& arg0, const Xmm& arg1) {
  vex(0x02,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x0e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vtestps(const Ymm& arg0, const M256& arg1) {
  p_2_4(arg1);
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x0e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vtestps(const Ymm& arg0, const Ymm& arg1) {
  vex(0x02,0x1,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x0e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vucomisd(const Xmm& arg0, const M64& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vucomisd(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x1,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vucomiss(const Xmm& arg0, const M32& arg1) {
  p_2_4(arg1);
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vucomiss(const Xmm& arg0, const Xmm& arg1) {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0(),arg1,arg0);
  opcode(0x2e);
  mod_rm_sib(arg1,arg0);
}

void Assembler::vunpckhpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vunpckhpd(arg0, arg1, arg2.reg);
}

void Assembler::vunpckhpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x15);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpckhpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x15);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpckhpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x15);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpckhps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vunpckhps(arg0, arg1, arg2.reg);
}

void Assembler::vunpckhps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x15);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpckhps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x15);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpckhps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x15);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpcklpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vunpcklpd(arg0, arg1, arg2.reg);
}

void Assembler::vunpcklpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x14);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpcklpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x14);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpcklpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x14);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpcklps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vunpcklps(arg0, arg1, arg2.reg);
}

void Assembler::vunpcklps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x14);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpcklps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x14);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vunpcklps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x14);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vxorpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vxorpd(arg0, arg1, arg2.reg);
}

void Assembler::vxorpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x1,0x0,arg1,arg2,arg0);
  opcode(0x57);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vxorpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x57);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vxorpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x1,0x0,arg1,arg2,arg0);
  opcode(0x57);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vxorps(const Xmm& arg0, const Xmm& arg1, const M128& arg2) {
  p_2_4(arg2);
  vxorps(arg0, arg1, arg2.reg);
}

void Assembler::vxorps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2) {
  vex(0x01,0x0,0x0,0x0,arg1,arg2,arg0);
  opcode(0x57);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vxorps(const Ymm& arg0, const Ymm& arg1, const M256& arg2) {
  p_2_4(arg2);
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x57);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vxorps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2) {
  vex(0x01,0x1,0x0,0x0,arg1,arg2,arg0);
  opcode(0x57);
  mod_rm_sib(arg2,arg0);
}

void Assembler::vzeroall() {
  vex(0x01,0x1,0x0,0x0,Constants::xmm0());
  opcode(0x77);
}

void Assembler::vzeroupper() {
  vex(0x01,0x0,0x0,0x0,Constants::xmm0());
  opcode(0x77);
}

void Assembler::wait() {
  pref_fwait(0x9b);
}

void Assembler::wrfsbase(const R32& arg0) {
  F3();
  rex0(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::wrfsbase(const R64& arg0) {
  F3();
  rexw(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[2]);
}

void Assembler::wrgsbase(const R32& arg0) {
  F3();
  rex0(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::wrgsbase(const R64& arg0) {
  F3();
  rexw(arg0);
  opcode(0x0f,0xae);
  mod_rm_sib(arg0,Constants::r64s()[3]);
}

void Assembler::xabort(const Imm8& arg0) {
  opcode(0xc6,0xf8);
  disp_imm(arg0);
}

void Assembler::xacquire() {
  F2();
}

void Assembler::xor_(const R16& arg0, const Imm16& arg1) {
  meta(arg0, arg1, 0x81, 6);
}

void Assembler::xor_(const R16& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0x83, 6);
}

void Assembler::xor_(const R32& arg0, const Imm32& arg1) {
  meta(arg0, arg1, 0x81, 6);
}

void Assembler::xor_(const R32& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0x83, 6);
}

void Assembler::xor_(const R64& arg0, const Imm32& arg1) {
  meta(arg0, arg1, 0x81, 6);
}

void Assembler::xor_(const R64& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0x83, 6);
}

void Assembler::xor_(const R8& arg0, const Imm8& arg1) {
  meta(arg0, arg1, 0x80, 6);
}

void Assembler::xbegin(Label& arg0) {
  opcode(0xc7,0xf8);
  disp_label32(arg0);
}

void Assembler::xbegin(const Rel32& arg0) {
  opcode(0xc7,0xf8);
  disp_imm(arg0);
}

void Assembler::xchg(const Ax& arg0, const R16& arg1) {
  pref_group3();
  rex0(arg1);
  opcode(0x90,arg1);
}

void Assembler::xchg(const Eax& arg0, const R32& arg1) {
  rex0(arg1);
  opcode(0x90,arg1);
}

void Assembler::xchg(const R16& arg0, const Ax& arg1) {
  pref_group3();
  rex0(arg0);
  opcode(0x90,arg0);
}

void Assembler::xchg(const R32& arg0, const Eax& arg1) {
  rex0(arg0);
  opcode(0x90,arg0);
}

void Assembler::xchg(const R64& arg0, const Rax& arg1) {
  rexw(arg0);
  opcode(0x90,arg0);
}

void Assembler::xchg(const Rax& arg0, const R64& arg1) {
  rexw(arg1);
  opcode(0x90,arg1);
}



void Assembler::add     (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x00);               }
void Assembler::add     (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x00);               }
void Assembler::add     (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x00);               }
void Assembler::add     (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x00);               }
void Assembler::add     (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x00);               }
void Assembler::add     (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x00);               }
void Assembler::add     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x01);               }
void Assembler::add     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x01);               }
void Assembler::add     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x01);               }
void Assembler::add     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x01);               }
void Assembler::add     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x01);               }
void Assembler::add     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x01);               }
void Assembler::add     (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x02);               }
void Assembler::add_1   (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x02);               }
void Assembler::add_1   (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x02);               }
void Assembler::add     (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x02);               }
void Assembler::add_1   (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x02);               }
void Assembler::add_1   (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x02);               }
void Assembler::add     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x03);               }
void Assembler::add_1   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x03);               }
void Assembler::add     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x03);               }
void Assembler::add_1   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x03);               }
void Assembler::add     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x03);               }
void Assembler::add_1   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x03);               }
void Assembler::add     (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0x04);               }
void Assembler::add     (const Ax& arg0, const Imm16& arg1)                { meta(arg0 , arg1 , 0x05);               }
void Assembler::add     (const Eax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x05);               }
void Assembler::add     (const Rax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x05);               }
void Assembler::or_     (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x08);               }
void Assembler::or_     (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x08);               }
void Assembler::or_     (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x08);               }
void Assembler::or_     (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x08);               }
void Assembler::or_     (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x08);               }
void Assembler::or_     (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x08);               }
void Assembler::or_     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x09);               }
void Assembler::or_     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x09);               }
void Assembler::or_     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x09);               }
void Assembler::or_     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x09);               }
void Assembler::or_     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x09);               }
void Assembler::or_     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x09);               }
void Assembler::or_     (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x0a);               }
void Assembler::or__1   (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x0a);               }
void Assembler::or__1   (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x0a);               }
void Assembler::or_     (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x0a);               }
void Assembler::or__1   (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x0a);               }
void Assembler::or__1   (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x0a);               }
void Assembler::or_     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0b);               }
void Assembler::or__1   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0b);               }
void Assembler::or_     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0b);               }
void Assembler::or__1   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0b);               }
void Assembler::or_     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0b);               }
void Assembler::or__1   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0b);               }
void Assembler::or_     (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0x0c);               }
void Assembler::or_     (const Ax& arg0, const Imm16& arg1)                { meta(arg0 , arg1 , 0x0d);               }
void Assembler::or_     (const Eax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x0d);               }
void Assembler::or_     (const Rax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x0d);               }
void Assembler::lar     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x02);        }
void Assembler::lar     (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x02);        }
void Assembler::lar     (const R32& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x02);        }
void Assembler::lar     (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x02);        }
void Assembler::lar     (const R64& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x02);        }
void Assembler::lar     (const R64& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x02);        }
void Assembler::lsl     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x03);        }
void Assembler::lsl     (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x03);        }
void Assembler::lsl     (const R32& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x03);        }
void Assembler::lsl     (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x03);        }
void Assembler::lsl     (const R64& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x03);        }
void Assembler::lsl     (const R64& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x03);        }
void Assembler::invpcid (const R64& arg0, const M128& arg1)                { meta(arg1 , arg0 , 0x0f , 0x38 , 0x82); }
void Assembler::movbe   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x38 , 0xf0); }
void Assembler::movbe   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x38 , 0xf0); }
void Assembler::movbe   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x38 , 0xf0); }
void Assembler::movbe   (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0x38 , 0xf1); }
void Assembler::movbe   (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0x38 , 0xf1); }
void Assembler::movbe   (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0x38 , 0xf1); }
void Assembler::cmovo   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x40);        }
void Assembler::cmovo   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x40);        }
void Assembler::cmovo   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x40);        }
void Assembler::cmovo   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x40);        }
void Assembler::cmovo   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x40);        }
void Assembler::cmovo   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x40);        }
void Assembler::cmovno  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x41);        }
void Assembler::cmovno  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x41);        }
void Assembler::cmovno  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x41);        }
void Assembler::cmovno  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x41);        }
void Assembler::cmovno  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x41);        }
void Assembler::cmovno  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x41);        }
void Assembler::cmovb   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovb   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovb   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovb   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovb   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovb   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovc   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovc   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovc   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovc   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovc   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovc   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovnae (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovnae (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovnae (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovnae (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovnae (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovnae (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x42);        }
void Assembler::cmovae  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovae  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovae  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovae  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovae  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovae  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnb  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnb  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnb  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnb  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnb  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnb  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnc  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnc  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnc  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnc  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnc  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmovnc  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x43);        }
void Assembler::cmove   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmove   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmove   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmove   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmove   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmove   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmovz   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmovz   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmovz   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmovz   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmovz   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmovz   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x44);        }
void Assembler::cmovne  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovne  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovne  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovne  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovne  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovne  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovnz  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovnz  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovnz  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovnz  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovnz  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovnz  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x45);        }
void Assembler::cmovbe  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovbe  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovbe  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovbe  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovbe  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovbe  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovna  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovna  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovna  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovna  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovna  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmovna  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x46);        }
void Assembler::cmova   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmova   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmova   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmova   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmova   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmova   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmovnbe (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmovnbe (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmovnbe (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmovnbe (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmovnbe (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmovnbe (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x47);        }
void Assembler::cmovs   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x48);        }
void Assembler::cmovs   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x48);        }
void Assembler::cmovs   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x48);        }
void Assembler::cmovs   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x48);        }
void Assembler::cmovs   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x48);        }
void Assembler::cmovs   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x48);        }
void Assembler::cmovns  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x49);        }
void Assembler::cmovns  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x49);        }
void Assembler::cmovns  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x49);        }
void Assembler::cmovns  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x49);        }
void Assembler::cmovns  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x49);        }
void Assembler::cmovns  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x49);        }
void Assembler::cmovp   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovp   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovp   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovp   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovp   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovp   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovpe  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovpe  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovpe  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovpe  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovpe  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovpe  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4a);        }
void Assembler::cmovnp  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovnp  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovnp  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovnp  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovnp  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovnp  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovpo  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovpo  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovpo  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovpo  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovpo  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovpo  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4b);        }
void Assembler::cmovl   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovl   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovl   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovl   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovl   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovl   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovnge (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovnge (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovnge (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovnge (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovnge (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovnge (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4c);        }
void Assembler::cmovge  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovge  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovge  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovge  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovge  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovge  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovnl  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovnl  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovnl  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovnl  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovnl  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovnl  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4d);        }
void Assembler::cmovle  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovle  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovle  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovle  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovle  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovle  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovng  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovng  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovng  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovng  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovng  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovng  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4e);        }
void Assembler::cmovg   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovg   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovg   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovg   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovg   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovg   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovnle (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovnle (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovnle (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovnle (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovnle (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::cmovnle (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0x4f);        }
void Assembler::bt      (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xa3);        }
void Assembler::bt      (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xa3);        }
void Assembler::bt      (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xa3);        }
void Assembler::bt      (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xa3);        }
void Assembler::bt      (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xa3);        }
void Assembler::bt      (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xa3);        }
void Assembler::shld    (const M16& arg0, const R16& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xa5);        }
void Assembler::shld    (const M32& arg0, const R32& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xa5);        }
void Assembler::shld    (const M64& arg0, const R64& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xa5);        }
void Assembler::shld    (const R16& arg0, const R16& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xa5);        }
void Assembler::shld    (const R32& arg0, const R32& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xa5);        }
void Assembler::shld    (const R64& arg0, const R64& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xa5);        }
void Assembler::bts     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xab);        }
void Assembler::bts     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xab);        }
void Assembler::bts     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xab);        }
void Assembler::bts     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xab);        }
void Assembler::bts     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xab);        }
void Assembler::bts     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xab);        }
void Assembler::shrd    (const M16& arg0, const R16& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xad);        }
void Assembler::shrd    (const M32& arg0, const R32& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xad);        }
void Assembler::shrd    (const M64& arg0, const R64& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xad);        }
void Assembler::shrd    (const R16& arg0, const R16& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xad);        }
void Assembler::shrd    (const R32& arg0, const R32& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xad);        }
void Assembler::shrd    (const R64& arg0, const R64& arg1, const Cl& arg2) { meta(arg0 , arg1 , 0x0f , 0xad);        }
void Assembler::imul    (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xaf);        }
void Assembler::imul    (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xaf);        }
void Assembler::imul    (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xaf);        }
void Assembler::imul    (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xaf);        }
void Assembler::imul    (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xaf);        }
void Assembler::imul    (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xaf);        }
void Assembler::cmpxchg (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xb0);        }
void Assembler::cmpxchg (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xb0);        }
void Assembler::cmpxchg (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xb0);        }
void Assembler::cmpxchg (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xb0);        }
void Assembler::cmpxchg (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xb0);        }
void Assembler::cmpxchg (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xb0);        }
void Assembler::cmpxchg (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb1);        }
void Assembler::cmpxchg (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb1);        }
void Assembler::cmpxchg (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb1);        }
void Assembler::cmpxchg (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb1);        }
void Assembler::cmpxchg (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb1);        }
void Assembler::cmpxchg (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb1);        }
void Assembler::btr     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb3);        }
void Assembler::btr     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb3);        }
void Assembler::btr     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb3);        }
void Assembler::btr     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb3);        }
void Assembler::btr     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb3);        }
void Assembler::btr     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xb3);        }
void Assembler::movzx   (const R16& arg0, const M8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xb6);        }
void Assembler::movzx   (const R16& arg0, const R8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xb6);        }
void Assembler::movzx   (const R16& arg0, const Rh& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xb6);        }
void Assembler::movzx   (const R32& arg0, const M8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xb6);        }
void Assembler::movzx   (const R32& arg0, const R8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xb6);        }
void Assembler::movzx   (const R32& arg0, const Rh& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xb6);        }
void Assembler::movzx   (const R64& arg0, const M8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xb6);        }
void Assembler::movzx   (const R64& arg0, const R8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xb6);        }
void Assembler::movzx   (const R32& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xb7);        }
void Assembler::movzx   (const R32& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xb7);        }
void Assembler::movzx   (const R64& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xb7);        }
void Assembler::movzx   (const R64& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xb7);        }
void Assembler::popcnt  (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xb8);        }
void Assembler::popcnt  (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xb8);        }
void Assembler::popcnt  (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xb8);        }
void Assembler::bt      (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x0f , 0xba); }
void Assembler::bt      (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x0f , 0xba); }
void Assembler::bt      (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x0f , 0xba); }
void Assembler::bt      (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x0f , 0xba); }
void Assembler::bt      (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x0f , 0xba); }
void Assembler::bt      (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x0f , 0xba); }
void Assembler::btc     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x0f , 0xba); }
void Assembler::btc     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x0f , 0xba); }
void Assembler::btc     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x0f , 0xba); }
void Assembler::btc     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x0f , 0xba); }
void Assembler::btc     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x0f , 0xba); }
void Assembler::btc     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x0f , 0xba); }
void Assembler::btr     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 6    , 0x0f , 0xba); }
void Assembler::btr     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 6    , 0x0f , 0xba); }
void Assembler::btr     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 6    , 0x0f , 0xba); }
void Assembler::btr     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 6    , 0x0f , 0xba); }
void Assembler::btr     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 6    , 0x0f , 0xba); }
void Assembler::btr     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 6    , 0x0f , 0xba); }
void Assembler::bts     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x0f , 0xba); }
void Assembler::bts     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x0f , 0xba); }
void Assembler::bts     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x0f , 0xba); }
void Assembler::bts     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x0f , 0xba); }
void Assembler::bts     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x0f , 0xba); }
void Assembler::bts     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x0f , 0xba); }
void Assembler::btc     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xbb);        }
void Assembler::btc     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xbb);        }
void Assembler::btc     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xbb);        }
void Assembler::btc     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xbb);        }
void Assembler::btc     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xbb);        }
void Assembler::btc     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xbb);        }
void Assembler::bsf     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbc);        }
void Assembler::bsf     (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbc);        }
void Assembler::bsf     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbc);        }
void Assembler::bsf     (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbc);        }
void Assembler::bsf     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbc);        }
void Assembler::bsf     (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbc);        }
void Assembler::tzcnt   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbc);        }
void Assembler::tzcnt   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbc);        }
void Assembler::tzcnt   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbc);        }
void Assembler::bsr     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbd);        }
void Assembler::bsr     (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbd);        }
void Assembler::bsr     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbd);        }
void Assembler::bsr     (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbd);        }
void Assembler::bsr     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbd);        }
void Assembler::bsr     (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbd);        }
void Assembler::lzcnt   (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbd);        }
void Assembler::lzcnt   (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbd);        }
void Assembler::lzcnt   (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbd);        }
void Assembler::movsx   (const R16& arg0, const M8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xbe);        }
void Assembler::movsx   (const R16& arg0, const R8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xbe);        }
void Assembler::movsx   (const R16& arg0, const Rh& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xbe);        }
void Assembler::movsx   (const R32& arg0, const M8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xbe);        }
void Assembler::movsx   (const R32& arg0, const R8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xbe);        }
void Assembler::movsx   (const R32& arg0, const Rh& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xbe);        }
void Assembler::movsx   (const R64& arg0, const M8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xbe);        }
void Assembler::movsx   (const R64& arg0, const R8& arg1)                  { meta(arg1 , arg0 , 0x0f , 0xbe);        }
void Assembler::movsx   (const R32& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbf);        }
void Assembler::movsx   (const R32& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbf);        }
void Assembler::movsx   (const R64& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbf);        }
void Assembler::movsx   (const R64& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x0f , 0xbf);        }
void Assembler::xadd    (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xc0);        }
void Assembler::xadd    (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xc0);        }
void Assembler::xadd    (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xc0);        }
void Assembler::xadd    (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xc0);        }
void Assembler::xadd    (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xc0);        }
void Assembler::xadd    (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x0f , 0xc0);        }
void Assembler::xadd    (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xc1);        }
void Assembler::xadd    (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xc1);        }
void Assembler::xadd    (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xc1);        }
void Assembler::xadd    (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xc1);        }
void Assembler::xadd    (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xc1);        }
void Assembler::xadd    (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xc1);        }
void Assembler::movnti  (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xc3);        }
void Assembler::movnti  (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x0f , 0xc3);        }
void Assembler::adc     (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x10);               }
void Assembler::adc     (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x10);               }
void Assembler::adc     (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x10);               }
void Assembler::adc     (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x10);               }
void Assembler::adc     (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x10);               }
void Assembler::adc     (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x10);               }
void Assembler::adc     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x11);               }
void Assembler::adc     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x11);               }
void Assembler::adc     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x11);               }
void Assembler::adc     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x11);               }
void Assembler::adc     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x11);               }
void Assembler::adc     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x11);               }
void Assembler::adc     (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x12);               }
void Assembler::adc_1   (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x12);               }
void Assembler::adc_1   (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x12);               }
void Assembler::adc     (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x12);               }
void Assembler::adc_1   (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x12);               }
void Assembler::adc_1   (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x12);               }
void Assembler::adc     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x13);               }
void Assembler::adc_1   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x13);               }
void Assembler::adc     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x13);               }
void Assembler::adc_1   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x13);               }
void Assembler::adc     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x13);               }
void Assembler::adc_1   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x13);               }
void Assembler::adc     (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0x14);               }
void Assembler::adc     (const Ax& arg0, const Imm16& arg1)                { meta(arg0 , arg1 , 0x15);               }
void Assembler::adc     (const Eax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x15);               }
void Assembler::adc     (const Rax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x15);               }
void Assembler::sbb     (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x18);               }
void Assembler::sbb     (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x18);               }
void Assembler::sbb     (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x18);               }
void Assembler::sbb     (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x18);               }
void Assembler::sbb     (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x18);               }
void Assembler::sbb     (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x18);               }
void Assembler::sbb     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x19);               }
void Assembler::sbb     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x19);               }
void Assembler::sbb     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x19);               }
void Assembler::sbb     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x19);               }
void Assembler::sbb     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x19);               }
void Assembler::sbb     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x19);               }
void Assembler::sbb     (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x1a);               }
void Assembler::sbb_1   (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x1a);               }
void Assembler::sbb_1   (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x1a);               }
void Assembler::sbb     (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x1a);               }
void Assembler::sbb_1   (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x1a);               }
void Assembler::sbb_1   (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x1a);               }
void Assembler::sbb     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x1b);               }
void Assembler::sbb_1   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x1b);               }
void Assembler::sbb     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x1b);               }
void Assembler::sbb_1   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x1b);               }
void Assembler::sbb     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x1b);               }
void Assembler::sbb_1   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x1b);               }
void Assembler::sbb     (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0x1c);               }
void Assembler::sbb     (const Ax& arg0, const Imm16& arg1)                { meta(arg0 , arg1 , 0x1d);               }
void Assembler::sbb     (const Eax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x1d);               }
void Assembler::sbb     (const Rax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x1d);               }
void Assembler::and_    (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x20);               }
void Assembler::and_    (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x20);               }
void Assembler::and_    (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x20);               }
void Assembler::and_    (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x20);               }
void Assembler::and_    (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x20);               }
void Assembler::and_    (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x20);               }
void Assembler::and_    (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x21);               }
void Assembler::and_    (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x21);               }
void Assembler::and_    (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x21);               }
void Assembler::and_    (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x21);               }
void Assembler::and_    (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x21);               }
void Assembler::and_    (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x21);               }
void Assembler::and_    (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x22);               }
void Assembler::and__1  (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x22);               }
void Assembler::and__1  (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x22);               }
void Assembler::and_    (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x22);               }
void Assembler::and__1  (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x22);               }
void Assembler::and__1  (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x22);               }
void Assembler::and_    (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x23);               }
void Assembler::and__1  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x23);               }
void Assembler::and_    (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x23);               }
void Assembler::and__1  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x23);               }
void Assembler::and_    (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x23);               }
void Assembler::and__1  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x23);               }
void Assembler::and_    (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0x24);               }
void Assembler::and_    (const Ax& arg0, const Imm16& arg1)                { meta(arg0 , arg1 , 0x25);               }
void Assembler::and_    (const Eax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x25);               }
void Assembler::and_    (const Rax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x25);               }
void Assembler::sub     (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x28);               }
void Assembler::sub     (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x28);               }
void Assembler::sub     (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x28);               }
void Assembler::sub     (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x28);               }
void Assembler::sub     (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x28);               }
void Assembler::sub     (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x28);               }
void Assembler::sub     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x29);               }
void Assembler::sub     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x29);               }
void Assembler::sub     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x29);               }
void Assembler::sub     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x29);               }
void Assembler::sub     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x29);               }
void Assembler::sub     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x29);               }
void Assembler::sub     (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x2a);               }
void Assembler::sub_1   (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x2a);               }
void Assembler::sub_1   (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x2a);               }
void Assembler::sub     (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x2a);               }
void Assembler::sub_1   (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x2a);               }
void Assembler::sub_1   (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x2a);               }
void Assembler::sub     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x2b);               }
void Assembler::sub_1   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x2b);               }
void Assembler::sub     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x2b);               }
void Assembler::sub_1   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x2b);               }
void Assembler::sub     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x2b);               }
void Assembler::sub_1   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x2b);               }
void Assembler::sub     (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0x2c);               }
void Assembler::sub     (const Ax& arg0, const Imm16& arg1)                { meta(arg0 , arg1 , 0x2d);               }
void Assembler::sub     (const Eax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x2d);               }
void Assembler::sub     (const Rax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x2d);               }
void Assembler::xor_    (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x30);               }
void Assembler::xor_    (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x30);               }
void Assembler::xor_    (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x30);               }
void Assembler::xor_    (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x30);               }
void Assembler::xor_    (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x30);               }
void Assembler::xor_    (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x30);               }
void Assembler::xor_    (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x31);               }
void Assembler::xor_    (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x31);               }
void Assembler::xor_    (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x31);               }
void Assembler::xor_    (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x31);               }
void Assembler::xor_    (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x31);               }
void Assembler::xor_    (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x31);               }
void Assembler::xor_    (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x32);               }
void Assembler::xor__1  (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x32);               }
void Assembler::xor__1  (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x32);               }
void Assembler::xor_    (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x32);               }
void Assembler::xor__1  (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x32);               }
void Assembler::xor__1  (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x32);               }
void Assembler::xor_    (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x33);               }
void Assembler::xor__1  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x33);               }
void Assembler::xor_    (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x33);               }
void Assembler::xor__1  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x33);               }
void Assembler::xor_    (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x33);               }
void Assembler::xor__1  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x33);               }
void Assembler::xor_    (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0x34);               }
void Assembler::xor_    (const Ax& arg0, const Imm16& arg1)                { meta(arg0 , arg1 , 0x35);               }
void Assembler::xor_    (const Eax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x35);               }
void Assembler::xor_    (const Rax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x35);               }
void Assembler::cmp     (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x38);               }
void Assembler::cmp     (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x38);               }
void Assembler::cmp     (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x38);               }
void Assembler::cmp     (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x38);               }
void Assembler::cmp     (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x38);               }
void Assembler::cmp     (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x38);               }
void Assembler::cmp     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x39);               }
void Assembler::cmp     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x39);               }
void Assembler::cmp     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x39);               }
void Assembler::cmp     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x39);               }
void Assembler::cmp     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x39);               }
void Assembler::cmp     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x39);               }
void Assembler::cmp     (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x3a);               }
void Assembler::cmp_1   (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x3a);               }
void Assembler::cmp_1   (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x3a);               }
void Assembler::cmp     (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x3a);               }
void Assembler::cmp_1   (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x3a);               }
void Assembler::cmp_1   (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x3a);               }
void Assembler::cmp     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x3b);               }
void Assembler::cmp_1   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x3b);               }
void Assembler::cmp     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x3b);               }
void Assembler::cmp_1   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x3b);               }
void Assembler::cmp     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x3b);               }
void Assembler::cmp_1   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x3b);               }
void Assembler::cmp     (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0x3c);               }
void Assembler::cmp     (const Ax& arg0, const Imm16& arg1)                { meta(arg0 , arg1 , 0x3d);               }
void Assembler::cmp     (const Eax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x3d);               }
void Assembler::cmp     (const Rax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0x3d);               }
void Assembler::movsxd  (const R64& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x63);               }
void Assembler::movsxd  (const R64& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x63);               }
void Assembler::adc     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 2    , 0x80);        }
void Assembler::adc     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 2    , 0x80);        }
void Assembler::adc     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 2    , 0x80);        }
void Assembler::add     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0x80);        }
void Assembler::add     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0x80);        }
void Assembler::add     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0x80);        }
void Assembler::and_    (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 4    , 0x80);        }
void Assembler::and_    (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 4    , 0x80);        }
void Assembler::and_    (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 4    , 0x80);        }
void Assembler::cmp     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 7    , 0x80);        }
void Assembler::cmp     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 7    , 0x80);        }
void Assembler::cmp     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 7    , 0x80);        }
void Assembler::or_     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 1    , 0x80);        }
void Assembler::or_     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 1    , 0x80);        }
void Assembler::or_     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 1    , 0x80);        }
void Assembler::sbb     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 3    , 0x80);        }
void Assembler::sbb     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 3    , 0x80);        }
void Assembler::sbb     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 3    , 0x80);        }
void Assembler::sub     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 5    , 0x80);        }
void Assembler::xor_    (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 6    , 0x80);        }
void Assembler::xor_    (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 6    , 0x80);        }
void Assembler::adc     (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 2    , 0x81);        }
void Assembler::adc     (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 2    , 0x81);        }
void Assembler::adc     (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 2    , 0x81);        }
void Assembler::adc     (const R16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 2    , 0x81);        }
void Assembler::adc     (const R32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 2    , 0x81);        }
void Assembler::adc     (const R64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 2    , 0x81);        }
void Assembler::add     (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 0    , 0x81);        }
void Assembler::add     (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0x81);        }
void Assembler::add     (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0x81);        }
void Assembler::add     (const R16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 0    , 0x81);        }
void Assembler::add     (const R32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0x81);        }
void Assembler::add     (const R64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0x81);        }
void Assembler::and_    (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 4    , 0x81);        }
void Assembler::and_    (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 4    , 0x81);        }
void Assembler::and_    (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 4    , 0x81);        }
void Assembler::and_    (const R16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 4    , 0x81);        }
void Assembler::and_    (const R32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 4    , 0x81);        }
void Assembler::and_    (const R64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 4    , 0x81);        }
void Assembler::cmp     (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 7    , 0x81);        }
void Assembler::cmp     (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 7    , 0x81);        }
void Assembler::cmp     (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 7    , 0x81);        }
void Assembler::cmp     (const R16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 7    , 0x81);        }
void Assembler::cmp     (const R32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 7    , 0x81);        }
void Assembler::cmp     (const R64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 7    , 0x81);        }
void Assembler::or_     (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 1    , 0x81);        }
void Assembler::or_     (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 1    , 0x81);        }
void Assembler::or_     (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 1    , 0x81);        }
void Assembler::or_     (const R16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 1    , 0x81);        }
void Assembler::or_     (const R32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 1    , 0x81);        }
void Assembler::or_     (const R64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 1    , 0x81);        }
void Assembler::sbb     (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 3    , 0x81);        }
void Assembler::sbb     (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 3    , 0x81);        }
void Assembler::sbb     (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 3    , 0x81);        }
void Assembler::sbb     (const R16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 3    , 0x81);        }
void Assembler::sbb     (const R32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 3    , 0x81);        }
void Assembler::sbb     (const R64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 3    , 0x81);        }
void Assembler::sub     (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 5    , 0x81);        }
void Assembler::sub     (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 5    , 0x81);        }
void Assembler::sub     (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 5    , 0x81);        }
void Assembler::sub     (const R16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 5    , 0x81);        }
void Assembler::sub     (const R32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 5    , 0x81);        }
void Assembler::xor_    (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 6    , 0x81);        }
void Assembler::xor_    (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 6    , 0x81);        }
void Assembler::xor_    (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 6    , 0x81);        }
void Assembler::adc     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0x83);        }
void Assembler::adc     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0x83);        }
void Assembler::adc     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0x83);        }
void Assembler::adc     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0x83);        }
void Assembler::adc     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0x83);        }
void Assembler::adc     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0x83);        }
void Assembler::add     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0x83);        }
void Assembler::add     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0x83);        }
void Assembler::add     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0x83);        }
void Assembler::add     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0x83);        }
void Assembler::add     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0x83);        }
void Assembler::add     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0x83);        }
void Assembler::and_    (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x83);        }
void Assembler::and_    (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x83);        }
void Assembler::and_    (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x83);        }
void Assembler::and_    (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x83);        }
void Assembler::and_    (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x83);        }
void Assembler::and_    (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0x83);        }
void Assembler::cmp     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x83);        }
void Assembler::cmp     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x83);        }
void Assembler::cmp     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x83);        }
void Assembler::cmp     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x83);        }
void Assembler::cmp     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x83);        }
void Assembler::cmp     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0x83);        }
void Assembler::or_     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0x83);        }
void Assembler::or_     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0x83);        }
void Assembler::or_     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0x83);        }
void Assembler::or_     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0x83);        }
void Assembler::or_     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0x83);        }
void Assembler::or_     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0x83);        }
void Assembler::sbb     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0x83);        }
void Assembler::sbb     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0x83);        }
void Assembler::sbb     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0x83);        }
void Assembler::sbb     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0x83);        }
void Assembler::sbb     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0x83);        }
void Assembler::sbb     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0x83);        }
void Assembler::sub     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x83);        }
void Assembler::sub     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x83);        }
void Assembler::sub     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x83);        }
void Assembler::sub     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0x83);        }
void Assembler::xor_    (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 6    , 0x83);        }
void Assembler::xor_    (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 6    , 0x83);        }
void Assembler::xor_    (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 6    , 0x83);        }
void Assembler::test    (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x84);               }
void Assembler::test    (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x84);               }
void Assembler::test    (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x84);               }
void Assembler::test    (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x84);               }
void Assembler::test    (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x84);               }
void Assembler::test    (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x84);               }
void Assembler::test    (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x85);               }
void Assembler::test    (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x85);               }
void Assembler::test    (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x85);               }
void Assembler::test    (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x85);               }
void Assembler::test    (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x85);               }
void Assembler::test    (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x85);               }
void Assembler::xchg    (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x86);               }
void Assembler::xchg    (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x86);               }
void Assembler::xchg    (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x86);               }
void Assembler::xchg    (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x86);               }
void Assembler::xchg_1  (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x86);               }
void Assembler::xchg    (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x86);               }
void Assembler::xchg_1  (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x86);               }
void Assembler::xchg    (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x86);               }
void Assembler::xchg    (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x86);               }
void Assembler::xchg_1  (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x86);               }
void Assembler::xchg    (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x86);               }
void Assembler::xchg_1  (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x86);               }
void Assembler::xchg    (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x87);               }
void Assembler::xchg    (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x87);               }
void Assembler::xchg    (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x87);               }
void Assembler::xchg    (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x87);               }
void Assembler::xchg    (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x87);               }
void Assembler::xchg_1  (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x87);               }
void Assembler::xchg    (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x87);               }
void Assembler::xchg    (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x87);               }
void Assembler::xchg_1  (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x87);               }
void Assembler::xchg    (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x87);               }
void Assembler::xchg    (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x87);               }
void Assembler::xchg_1  (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x87);               }
void Assembler::mov     (const M8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x88);               }
void Assembler::mov     (const M8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x88);               }
void Assembler::mov     (const R8& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x88);               }
void Assembler::mov     (const R8& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x88);               }
void Assembler::mov     (const Rh& arg0, const R8& arg1)                   { meta(arg0 , arg1 , 0x88);               }
void Assembler::mov     (const Rh& arg0, const Rh& arg1)                   { meta(arg0 , arg1 , 0x88);               }
void Assembler::mov     (const M16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x89);               }
void Assembler::mov     (const M32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x89);               }
void Assembler::mov     (const M64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x89);               }
void Assembler::mov     (const R16& arg0, const R16& arg1)                 { meta(arg0 , arg1 , 0x89);               }
void Assembler::mov     (const R32& arg0, const R32& arg1)                 { meta(arg0 , arg1 , 0x89);               }
void Assembler::mov     (const R64& arg0, const R64& arg1)                 { meta(arg0 , arg1 , 0x89);               }
void Assembler::mov     (const R8& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x8a);               }
void Assembler::mov_1   (const R8& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x8a);               }
void Assembler::mov_1   (const R8& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x8a);               }
void Assembler::mov     (const Rh& arg0, const M8& arg1)                   { meta(arg1 , arg0 , 0x8a);               }
void Assembler::mov_1   (const Rh& arg0, const R8& arg1)                   { meta(arg1 , arg0 , 0x8a);               }
void Assembler::mov_1   (const Rh& arg0, const Rh& arg1)                   { meta(arg1 , arg0 , 0x8a);               }
void Assembler::mov     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x8b);               }
void Assembler::mov_1   (const R16& arg0, const R16& arg1)                 { meta(arg1 , arg0 , 0x8b);               }
void Assembler::mov     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x8b);               }
void Assembler::mov_1   (const R32& arg0, const R32& arg1)                 { meta(arg1 , arg0 , 0x8b);               }
void Assembler::mov     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x8b);               }
void Assembler::mov_1   (const R64& arg0, const R64& arg1)                 { meta(arg1 , arg0 , 0x8b);               }
void Assembler::lea     (const R16& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x8d);               }
void Assembler::lea     (const R16& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x8d);               }
void Assembler::lea     (const R16& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x8d);               }
void Assembler::lea     (const R32& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x8d);               }
void Assembler::lea     (const R32& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x8d);               }
void Assembler::lea     (const R32& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x8d);               }
void Assembler::lea     (const R64& arg0, const M16& arg1)                 { meta(arg1 , arg0 , 0x8d);               }
void Assembler::lea     (const R64& arg0, const M32& arg1)                 { meta(arg1 , arg0 , 0x8d);               }
void Assembler::lea     (const R64& arg0, const M64& arg1)                 { meta(arg1 , arg0 , 0x8d);               }
void Assembler::test    (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0xa8);               }
void Assembler::test    (const Ax& arg0, const Imm16& arg1)                { meta(arg0 , arg1 , 0xa9);               }
void Assembler::test    (const Eax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0xa9);               }
void Assembler::test    (const Rax& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0xa9);               }
void Assembler::rcl     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 2    , 0xc0);        }
void Assembler::rcl     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 2    , 0xc0);        }
void Assembler::rcl     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 2    , 0xc0);        }
void Assembler::rcr     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 3    , 0xc0);        }
void Assembler::rcr     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 3    , 0xc0);        }
void Assembler::rcr     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 3    , 0xc0);        }
void Assembler::rol     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0xc0);        }
void Assembler::rol     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0xc0);        }
void Assembler::rol     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0xc0);        }
void Assembler::ror     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 1    , 0xc0);        }
void Assembler::ror     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 1    , 0xc0);        }
void Assembler::ror     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 1    , 0xc0);        }
void Assembler::sal     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 4    , 0xc0);        }
void Assembler::sal     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 4    , 0xc0);        }
void Assembler::sal     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 4    , 0xc0);        }
void Assembler::sar     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 7    , 0xc0);        }
void Assembler::sar     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 7    , 0xc0);        }
void Assembler::sar     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 7    , 0xc0);        }
void Assembler::shl     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 4    , 0xc0);        }
void Assembler::shl     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 4    , 0xc0);        }
void Assembler::shl     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 4    , 0xc0);        }
void Assembler::shr     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 5    , 0xc0);        }
void Assembler::shr     (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 5    , 0xc0);        }
void Assembler::shr     (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 5    , 0xc0);        }
void Assembler::rcl     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0xc1);        }
void Assembler::rcl     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0xc1);        }
void Assembler::rcl     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0xc1);        }
void Assembler::rcl     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0xc1);        }
void Assembler::rcl     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0xc1);        }
void Assembler::rcl     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 2    , 0xc1);        }
void Assembler::rcr     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0xc1);        }
void Assembler::rcr     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0xc1);        }
void Assembler::rcr     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0xc1);        }
void Assembler::rcr     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0xc1);        }
void Assembler::rcr     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0xc1);        }
void Assembler::rcr     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 3    , 0xc1);        }
void Assembler::rol     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0xc1);        }
void Assembler::rol     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0xc1);        }
void Assembler::rol     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0xc1);        }
void Assembler::rol     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0xc1);        }
void Assembler::rol     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0xc1);        }
void Assembler::rol     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0    , 0xc1);        }
void Assembler::ror     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0xc1);        }
void Assembler::ror     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0xc1);        }
void Assembler::ror     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0xc1);        }
void Assembler::ror     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0xc1);        }
void Assembler::ror     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0xc1);        }
void Assembler::ror     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 1    , 0xc1);        }
void Assembler::sal     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::sal     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::sal     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::sal     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::sal     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::sal     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::sar     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0xc1);        }
void Assembler::sar     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0xc1);        }
void Assembler::sar     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0xc1);        }
void Assembler::sar     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0xc1);        }
void Assembler::sar     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0xc1);        }
void Assembler::sar     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 7    , 0xc1);        }
void Assembler::shl     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::shl     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::shl     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::shl     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::shl     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::shl     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 4    , 0xc1);        }
void Assembler::shr     (const M16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0xc1);        }
void Assembler::shr     (const M32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0xc1);        }
void Assembler::shr     (const M64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0xc1);        }
void Assembler::shr     (const R16& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0xc1);        }
void Assembler::shr     (const R32& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0xc1);        }
void Assembler::shr     (const R64& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 5    , 0xc1);        }
void Assembler::mov     (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0xc6);        }
void Assembler::mov_1   (const R8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0xc6);        }
void Assembler::mov_1   (const Rh& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0xc6);        }
void Assembler::mov     (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 0    , 0xc7);        }
void Assembler::mov     (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0xc7);        }
void Assembler::mov     (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0xc7);        }
void Assembler::mov_1   (const R16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 0    , 0xc7);        }
void Assembler::mov_1   (const R32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0xc7);        }
void Assembler::mov     (const R64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0xc7);        }
void Assembler::in      (const Al& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0xe4);               }
void Assembler::in      (const Ax& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0xe5);               }
void Assembler::in      (const Eax& arg0, const Imm8& arg1)                { meta(arg0 , arg1 , 0xe5);               }
void Assembler::test    (const M8& arg0, const Imm8& arg1)                 { meta(arg0 , arg1 , 0    , 0xf6);        }
void Assembler::test    (const M16& arg0, const Imm16& arg1)               { meta(arg0 , arg1 , 0    , 0xf7);        }
void Assembler::test    (const M32& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0xf7);        }
void Assembler::test    (const M64& arg0, const Imm32& arg1)               { meta(arg0 , arg1 , 0    , 0xf7);        }

void Assembler::shld    (const R16& arg0, const R16& arg1, const Imm8& arg2) { meta(arg0,arg1, 0x0f,0xa4); disp_imm(arg2); }
void Assembler::shld    (const R32& arg0, const R32& arg1, const Imm8& arg2) { meta(arg0,arg1, 0x0f,0xa4); disp_imm(arg2); }
void Assembler::shld    (const R64& arg0, const R64& arg1, const Imm8& arg2) { meta(arg0,arg1, 0x0f,0xa4); disp_imm(arg2); }
