#include "assembler.h"

#include <utility>
#include <cassert>
#include <vector>

void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const R64&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const R32&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const R64&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const R32&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R64&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R64&,const R64&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R32&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R32&,const R32&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M8&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M8&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M64&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M64&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M64&,const R64&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M64&,const R32&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M32&,const uint8_t) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M32&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M32&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M32&,const R64&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M32&,const R32&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M256&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M256&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M16&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M16&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M128&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t,const M128&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const Ymm&,const uint8_t) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const Ymm&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const Xmm&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const M64&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const M32&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const M256&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Ymm&,const M128&,const Ymm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const Xmm&,const uint8_t) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const Xmm&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const R64&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const R32&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const M8&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const M64&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const M32&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const M16&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const Xmm&,const M128&,const Xmm&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R64&,const R64&,const uint8_t) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R64&,const R64&,const R64&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R64&,const M64&,const uint8_t) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R64&,const M64&,const R64&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R32&,const R32&,const uint8_t) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R32&,const R32&,const R32&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R32&,const M32&,const uint8_t) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const R32&,const M32&,const R32&) {}
void Assembler::vex(const uint8_t,const uint8_t,const uint8_t,const uint8_t,const uint8_t) {}
void Assembler::rex(const Xmm&,const R64&,const uint8_t) {}
void Assembler::rex(const Xmm&,const R32&,const uint8_t) {}
void Assembler::rex(const Xmm&,const Mm&,const uint8_t) {}
void Assembler::rex(const St&,const uint8_t) {}
void Assembler::rex(const R64&,const Sreg&,const uint8_t) {}
void Assembler::rex(const R64&,const Mm&,const uint8_t) {}
void Assembler::rex(const R32&,const Mm&,const uint8_t) {}
void Assembler::rex(const R16&,const Sreg&,const uint8_t) {}
void Assembler::rex(const Mm&,const uint8_t) {}
void Assembler::rex(const Mm&,const Xmm&,const uint8_t) {}
void Assembler::rex(const Mm&,const R64&,const uint8_t) {}
void Assembler::rex(const Mm&,const R32&,const uint8_t) {}
void Assembler::rex(const Mm&,const Mm&,const uint8_t) {}
void Assembler::rex(const M80Fp&,const uint8_t) {}
void Assembler::rex(const M80Bcd&,const uint8_t) {}
void Assembler::rex(const M8&,const Xmm&,const uint8_t) {}
void Assembler::rex(const M8&,const Rh&,const uint8_t) {}
void Assembler::rex(const M64Int&,const uint8_t) {}
void Assembler::rex(const M64Fp&,const uint8_t) {}
void Assembler::rex(const M64&,const Sreg&,const uint8_t) {}
void Assembler::rex(const M64&,const Mm&,const uint8_t) {}
void Assembler::rex(const M512Byte&,const uint8_t) {}
void Assembler::rex(const M32Int&,const uint8_t) {}
void Assembler::rex(const M32Fp&,const uint8_t) {}
void Assembler::rex(const M32&,const Mm&,const uint8_t) {}
void Assembler::rex(const M2Byte&,const uint8_t) {}
void Assembler::rex(const M28Byte&,const uint8_t) {}
void Assembler::rex(const M16Int&,const uint8_t) {}
void Assembler::rex(const M16&,const Xmm&,const uint8_t) {}
void Assembler::rex(const M16&,const Sreg&,const uint8_t) {}
void Assembler::rex(const M16&,const Mm&,const uint8_t) {}
void Assembler::rex(const M128&,const uint8_t) {}
void Assembler::rex(const M128&,const Xmm&,const uint8_t) {}
void Assembler::rex(const M128&,const R64&,const uint8_t) {}
void Assembler::rex(const M128&,const Mm&,const uint8_t) {}
void Assembler::rex(const M108Byte&,const uint8_t) {}
void Assembler::rex(const FarPtr1664&,const uint8_t) {}
void Assembler::rex(const FarPtr1664&,const R64&,const uint8_t) {}
void Assembler::rex(const FarPtr1632&,const uint8_t) {}
void Assembler::rex(const FarPtr1632&,const R32&,const uint8_t) {}
void Assembler::rex(const FarPtr1616&,const uint8_t) {}
void Assembler::rex(const FarPtr1616&,const R16&,const uint8_t) {}
void Assembler::pref_group4(const M80Fp&) {}
void Assembler::pref_group4(const M80Bcd&) {}
void Assembler::pref_group4(const M64Int&) {}
void Assembler::pref_group4(const M64Fp&) {}
void Assembler::pref_group4(const M512Byte&) {}
void Assembler::pref_group4(const M32Int&) {}
void Assembler::pref_group4(const M32Fp&) {}
void Assembler::pref_group4(const M2Byte&) {}
void Assembler::pref_group4(const M28Byte&) {}
void Assembler::pref_group4(const M256&) {}
void Assembler::pref_group4(const M16Int&) {}
void Assembler::pref_group4(const M128&) {}
void Assembler::pref_group4(const M108Byte&) {}
void Assembler::pref_group4(const FarPtr1664&) {}
void Assembler::pref_group4(const FarPtr1632&) {}
void Assembler::pref_group4(const FarPtr1616&) {}
void Assembler::pref_group2(const M80Fp&) {}
void Assembler::pref_group2(const M80Bcd&) {}
void Assembler::pref_group2(const M64Int&) {}
void Assembler::pref_group2(const M64Fp&) {}
void Assembler::pref_group2(const M512Byte&) {}
void Assembler::pref_group2(const M32Int&) {}
void Assembler::pref_group2(const M32Fp&) {}
void Assembler::pref_group2(const M2Byte&) {}
void Assembler::pref_group2(const M28Byte&) {}
void Assembler::pref_group2(const M256&) {}
void Assembler::pref_group2(const M16Int&) {}
void Assembler::pref_group2(const M128&) {}
void Assembler::pref_group2(const M108Byte&) {}
void Assembler::pref_group2(const Hint&) {}
void Assembler::pref_group2(const FarPtr1664&) {}
void Assembler::pref_group2(const FarPtr1632&) {}
void Assembler::pref_group2(const FarPtr1616&) {}
void Assembler::pref_group1(const uint8_t) {}
void Assembler::pref_fwait(const uint8_t) {}
void Assembler::mod_rm_sib(const FarPtr1664&,const uint8_t) {}
void Assembler::mod_rm_sib(const FarPtr1632&,const uint8_t) {}
void Assembler::mod_rm_sib(const FarPtr1616&,const uint8_t) {}
void Assembler::mod_rm_sib(const M512Byte&,const uint8_t) {}
void Assembler::mod_rm_sib(const M128&,const uint8_t) {}
void Assembler::mod_rm_sib(const M108Byte&,const uint8_t) {}
void Assembler::mod_rm_sib(const M80Fp&,const uint8_t) {}
void Assembler::mod_rm_sib(const M80Bcd&,const uint8_t) {}
void Assembler::mod_rm_sib(const M64Int&,const uint8_t) {}
void Assembler::mod_rm_sib(const M64Fp&,const uint8_t) {}
void Assembler::mod_rm_sib(const M32Int&,const uint8_t) {}
void Assembler::mod_rm_sib(const M32Fp&,const uint8_t) {}
void Assembler::mod_rm_sib(const M28Byte&,const uint8_t) {}
void Assembler::mod_rm_sib(const M16Int&,const uint8_t) {}
void Assembler::mod_rm_sib(const Ymm&,const uint8_t) {}
void Assembler::mod_rm_sib(const Rh&,const uint8_t) {}
void Assembler::mod_rm_sib(const Mm&,const uint8_t) {}
void Assembler::mod_rm_sib(const M2Byte&,const uint8_t) {}
void Assembler::mod_rm_sib(const Ymm&,const Ymm&) {}
void Assembler::mod_rm_sib(const Xmm&,const Ymm&) {}
void Assembler::mod_rm_sib(const M8&,const Ymm&) {}
void Assembler::mod_rm_sib(const M64&,const Ymm&) {}
void Assembler::mod_rm_sib(const M32&,const Ymm&) {}
void Assembler::mod_rm_sib(const M256&,const Ymm&) {}
void Assembler::mod_rm_sib(const M16&,const Ymm&) {}
void Assembler::mod_rm_sib(const M128&,const Ymm&) {}
void Assembler::mod_rm_sib(const Ymm&,const Xmm&) {}
void Assembler::mod_rm_sib(const Mm&,const Xmm&) {}
void Assembler::mod_rm_sib(const M256&,const Xmm&) {}
void Assembler::mod_rm_sib(const R64&,const Sreg&) {}
void Assembler::mod_rm_sib(const R16&,const Sreg&) {}
void Assembler::mod_rm_sib(const M64&,const Sreg&) {}
void Assembler::mod_rm_sib(const M16&,const Sreg&) {}
void Assembler::mod_rm_sib(const Ymm&,const R64&) {}
void Assembler::mod_rm_sib(const Xmm&,const R64&) {}
void Assembler::mod_rm_sib(const Mm&,const R64&) {}
void Assembler::mod_rm_sib(const M128&,const R64&) {}
void Assembler::mod_rm_sib(const FarPtr1664&,const R64&) {}
void Assembler::mod_rm_sib(const Ymm&,const R32&) {}
void Assembler::mod_rm_sib(const Xmm&,const R32&) {}
void Assembler::mod_rm_sib(const Rh&,const R32&) {}
void Assembler::mod_rm_sib(const Mm&,const R32&) {}
void Assembler::mod_rm_sib(const FarPtr1632&,const R32&) {}
void Assembler::mod_rm_sib(const Rh&,const R16&) {}
void Assembler::mod_rm_sib(const FarPtr1616&,const R16&) {}
void Assembler::mod_rm_sib(const Xmm&,const Mm&) {}
void Assembler::mod_rm_sib(const R64&,const Mm&) {}
void Assembler::mod_rm_sib(const R32&,const Mm&) {}
void Assembler::mod_rm_sib(const Mm&,const Mm&) {}
void Assembler::mod_rm_sib(const M64&,const Mm&) {}
void Assembler::mod_rm_sib(const M32&,const Mm&) {}
void Assembler::mod_rm_sib(const M16&,const Mm&) {}
void Assembler::mod_rm_sib(const M128&,const Mm&) {}
void Assembler::disp_imm(const Zero&,const Imm16&) {}
void Assembler::disp_imm(const Ymm&) {}
void Assembler::disp_imm(const Xmm&) {}
void Assembler::disp_imm(const One&,const Imm16&) {}
void Assembler::disp_imm(const Moffs8&) {}
void Assembler::disp_imm(const Moffs64&) {}
void Assembler::disp_imm(const Moffs32&) {}
void Assembler::disp_imm(const Moffs16&) {}
void Assembler::disp_imm(const Imm8&,const Imm16&) {}





template <unsigned S>
void Assembler::disp_imm (const Imm<S> & x) { emit (x.value); }


void Assembler::disp_label8(Label & l) {
  const size_t diff = l.value() - size () - 1;
  emit ((uint8_t) diff);
}
void Assembler::disp_label32(Label&) {}

void Assembler::disp_imm(const Rel8& x) {
  emit((uint8_t) (x.value - 1));
}
void Assembler::disp_imm(const Rel32& x) {
  emit(x.value - 5);
}


void Assembler::opcode (uint8_t x) {
  emit (x);
}

void Assembler::opcode (uint8_t x, uint8_t y) {
  emit (x, y);
}

template <unsigned S>
void Assembler::opcode (uint8_t x, const SReg<S> & r) {
  if (r.code < 8)
  {
    emit ((uint8_t) (x + r.code));
  } else
  {
    emit ((uint8_t) (x + r.code - 8));
  }
}

template <unsigned S>
void Assembler::opcode (uint8_t a, uint8_t b, const SReg<S> & r)
{
  emit   (a);
  opcode (b, r);
}

void Assembler::opcode(const uint8_t,const uint8_t,const uint8_t) {}
void Assembler::opcode(const uint8_t,const uint8_t,const St&) {}
void Assembler::opcode(const uint8_t,const Rh&) {}



void Assembler::pref_group3() { emit ((uint8_t) 0x66); }


uint8_t Assembler::rex_w ()
{
  return 0x8;
}

void Assembler::rex (uint8_t n)
{
  assert (n == rex_w());

  if (n == rex_w())
    emit ((unsigned char) 0x48);
}

template <unsigned S>
void Assembler::rex (const SReg<S> & x, uint8_t n)
{
  assert (n == 0 || n == rex_w());
  uint8_t code = 0x40;

  if (n == rex_w())
    code += 8;

  if (x.code >= 8)
    code += 1;

  if (code == 0x40 && !(S == 8 && x.code > 4))
    return;

  emit(code);
}
template <unsigned Sa, unsigned Sb>
void Assembler::rex (const SReg<Sa> & a, const SReg<Sb> & b, uint8_t n)
{
  assert (n == 0 || n == rex_w());
  uint8_t code = 0x40;

  if (n == rex_w())
    code += 8;

  if (a.code >= 8) code += 1;
  if (b.code >= 8) code += 4;

  if (code == 0x40 && !(Sa == 8 && Sb == 8 && (a.code > 4 || b.code > 4)))
    return;

  emit (code);
}
template <unsigned S>
void Assembler::rex (const Mem<S> & m, uint8_t n) { rex (m.reg, n); }

template <unsigned Sm, unsigned Sr>
void Assembler::rex (const Mem<Sm> & a, const SReg<Sr> & b, uint8_t n) { rex (a.reg, b, n); }

void Assembler::rex (const Xmm & x, uint8_t n)
{
  assert (n == 0);
}
void Assembler::rex (const Xmm & a, const Xmm & b, uint8_t n)
{
  // assert(false);
  // emit((uint8_t) 0x01);
}

void Assembler::rex(const M32&,const Xmm&,const uint8_t) {
  emit((uint8_t) 0x41);
}
void Assembler::rex(const R64&,const Xmm&,const uint8_t) {
  emit((uint8_t) 0x41);
}
void Assembler::rex(const R32&,const Xmm&,const uint8_t) {
  emit((uint8_t) 0x41);
}
void Assembler::rex(const M64&,const Xmm&,const uint8_t) {
  emit((uint8_t) 0x41);
}

void Assembler::rex(const Rh&,const uint8_t) {}
void Assembler::rex(const Rh&,const Rh&,const uint8_t) {}
void Assembler::rex(const Rh&,const R8&,const uint8_t) {}
void Assembler::rex(const Rh&,const R32&,const uint8_t) {}
void Assembler::rex(const Rh&,const R16&,const uint8_t) {}
void Assembler::rex(const R8&,const Rh& x,const uint8_t) {
}


// void Assembler::movd(const Xmm& arg0, const M32& arg1) {
//   pref_group2(arg1);
//   pref_group4(arg1);
//   pref_group3();
//   rex(arg1,arg0,(uint8_t)0x00);
//   opcode(0x0f,0x6e);
//   mod_rm_sib(arg1,arg0);
// }
// 676641 0f6e 30


void Assembler::__mod_rm_sib (uint8_t a, uint8_t b)
{
  emit ((uint8_t) (a + (b << 3)));
}

template <unsigned S>
void Assembler::mod_rm_sib(const Mem<S> & m, uint8_t c)
{
  __mod_rm_sib (m.reg.code - 8, c);
}
template <unsigned Sm, unsigned Sr>
void Assembler::mod_rm_sib (const Mem<Sm> & m, const SReg<Sr> & r)
{
  mod_rm_sib (m, r.code - 8);
}
template <unsigned S>
void Assembler::mod_rm_sib(const SReg<S> & reg, uint8_t c)
{
  uint8_t r = 0xb8 + reg.code;
  if (reg.code < 8) r += 8;
  __mod_rm_sib (r, c);
}

void Assembler::mod_rm_sib(const Xmm & reg, uint8_t c) { __mod_rm_sib (0xc0 + reg.code, c); }
void Assembler::mod_rm_sib(const Xmm & a, const Xmm & b) { mod_rm_sib(a, b.code); }
template <unsigned Sa, unsigned Sb>
void Assembler::mod_rm_sib(const SReg<Sa> & a, const SReg<Sb> & b)
{
  __mod_rm_sib (0xc0 + (a.code & 0xF7), b.code & 0xF7);


  // if (a.code < 8 && b.code < 8)
  //   __mod_rm_sib (0xd8 + a.code, b.code);
  // else
  //   __mod_rm_sib (0x78 + a.code, b.code);
}

template <unsigned S>
void Assembler::mod_rm_sib(const Mem<S> & m, const Xmm & r) {
  if (S == 128)
    __mod_rm_sib (m.reg.code, r.code);

  if (S < 128)
    mod_rm_sib (m, r.code);
}

void Assembler::mod_rm_sib(const R64& a,const Xmm& b) {
  __mod_rm_sib (0xc0 + b.code, a.code - 8); // ?
}
void Assembler::mod_rm_sib(const R32& a,const Xmm& b) {
  __mod_rm_sib (0xc0 + b.code, a.code - 8);
}

void Assembler::mod_rm_sib(const Rh& b,const Rh& a) {
  __mod_rm_sib (0xe4 + b.code, a.code);
}
void Assembler::mod_rm_sib(const R8& a,const Rh& b) {
  __mod_rm_sib (0xe0 + a.code, b.code);
}
void Assembler::mod_rm_sib(const M8&,const Rh&) {}
void Assembler::mod_rm_sib(const Rh& a,const R8& b) {
  __mod_rm_sib (0xc4 + a.code, b.code);
}


template <unsigned S>
void Assembler::pref_group2 (const Mem<S> & m)
{
  if (S == 16) {}
  if (S == 32) emit ((uint8_t) 0x67);
}
template <unsigned S>
void Assembler::pref_group4 (const Mem<S> & m)
{
}

