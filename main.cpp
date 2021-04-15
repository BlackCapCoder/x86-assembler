#include "assembler_ex.h"
#include "registers.h"

#include <utility>
#include <cstdio>
#include <iostream>
#include <cassert>
#include <time.h>
#include <functional>


template <class R>
class SmartReg : public R
{
  Assembler & owner;

public:
  SmartReg (Assembler & as) : R (), owner {as}
  {}

  template <unsigned S>
  void operator = (const Imm<S> im) {
    if (im.value == 0)
      owner.xor_ (*this, *this);
    else
      owner.mov (*this, im);
  }
  void operator = (const uint32_t im) {
    operator = (Imm<32> { im });
  }

  template <unsigned S>
  void operator += (const Imm<S> im) {
    owner.add (*this, im);
  }
  void operator += (const uint32_t im) {
    operator += (Imm<32> { im });
  }

  template <unsigned S>
  void operator = (const SReg<S> & r) {
    owner.mov ((const R &) *this, (const SReg<S> &) r);
  }

  void operator ++ () {
    owner.add (*this, Imm8 { 1 });
  }
  void operator -- () {
    owner.sub (*this, Imm8 { 1 });
  }


};


int main (void)
{
  using namespace Registers;

  AssemblerEx as {};

  as.xor_(rax, rax);
  as.shr(esi, Imm<8>{3});
  as.align(16);
  // auto loop = as.label();
  // as.xor_ (rax, Mem<64>(arg1));
  // as.add(arg1, Imm<8>{1});
  // as.sub(esi, Imm<8>{1});
  // as.jnz(loop);
  as.ret();


  std::cout << as.to_hex() << std::endl;

  return 0;
}
