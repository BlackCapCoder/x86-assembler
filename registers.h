#ifndef REGISTERS_H
#define REGISTERS_H


#include <stdint.h>
#include <utility>


template <unsigned> struct uint_impl     { using type = uint64_t; };
template <>         struct uint_impl<8>  { using type = uint8_t; };
template <>         struct uint_impl<16> { using type = uint16_t; };
template <>         struct uint_impl<32> { using type = uint32_t; };
template <>         struct uint_impl<64> { using type = uint64_t; };

template <unsigned S>
using UInt = typename uint_impl<S>::type;



template <unsigned S>
struct SReg
{
  static constexpr unsigned size = S;
  const            uint8_t  code;
  const               bool  high;

protected:
  constexpr SReg<S> (const uint8_t c, const bool h)
    : code {c}
    , high {h}
  {}

  // the char argument is there to prevent implicit casts from numbers
  // in favor of other types
};


using R8  = SReg<8>;
using R16 = SReg<16>;
using R32 = SReg<32>;
using R64 = SReg<64>;
using Xmm = SReg<128>;


#define mkreg(N,S,C)\
  struct N : SReg<S> {\
    constexpr N () : SReg<S>{C, 0} {}\
  }

  mkreg (Al,   8, 0x0);
  mkreg (Cl,   8, 0x1);
  mkreg (Dl,   8, 0x2);
  mkreg (Bl,   8, 0x3);
  mkreg (Spl,  8, 0x4);
  mkreg (Bpl,  8, 0x5);
  mkreg (Sil,  8, 0x6);
  mkreg (Dil,  8, 0x7);
  mkreg (R8b,  8, 0x8);
  mkreg (R9b,  8, 0x9);
  mkreg (R10b, 8, 0xa);
  mkreg (R11b, 8, 0xb);
  mkreg (R12b, 8, 0xc);
  mkreg (R13b, 8, 0xd);
  mkreg (R14b, 8, 0xe);
  mkreg (R15b, 8, 0xf);

  mkreg (Ax,   16, 0x0);
  mkreg (Cx,   16, 0x1);
  mkreg (Dx,   16, 0x2);
  mkreg (Bx,   16, 0x3);
  mkreg (Sp,   16, 0x4);
  mkreg (Bp,   16, 0x5);
  mkreg (Si,   16, 0x6);
  mkreg (Di,   16, 0x7);
  mkreg (R8w,  16, 0x8);
  mkreg (R9w,  16, 0x9);
  mkreg (R10w, 16, 0xa);
  mkreg (R11w, 16, 0xb);
  mkreg (R12w, 16, 0xc);
  mkreg (R13w, 16, 0xd);
  mkreg (R14w, 16, 0xe);
  mkreg (R15w, 16, 0xf);

  mkreg (Eax,  32, 0x0);
  mkreg (Ecx,  32, 0x1);
  mkreg (Edx,  32, 0x2);
  mkreg (Ebx,  32, 0x3);
  mkreg (Esp,  32, 0x4);
  mkreg (Ebp,  32, 0x5);
  mkreg (Esi,  32, 0x6);
  mkreg (Edi,  32, 0x7);
  mkreg (R8d,  32, 0x8);
  mkreg (R9d,  32, 0x9);
  mkreg (R10d, 32, 0xa);
  mkreg (R11d, 32, 0xb);
  mkreg (R12d, 32, 0xc);
  mkreg (R13d, 32, 0xd);
  mkreg (R14d, 32, 0xe);
  mkreg (R15d, 32, 0xf);

  mkreg (Rax, 64, 0x0);
  mkreg (Rcx, 64, 0x1);
  mkreg (Rdx, 64, 0x2);
  mkreg (Rbx, 64, 0x3);
  mkreg (Rsp, 64, 0x4);
  mkreg (Rbp, 64, 0x5);
  mkreg (Rsi, 64, 0x6);
  mkreg (Rdi, 64, 0x7);
  mkreg (R8_, 64, 0x8);
  mkreg (R9,  64, 0x9);
  mkreg (R10, 64, 0xa);
  mkreg (R11, 64, 0xb);
  mkreg (R12, 64, 0xc);
  mkreg (R13, 64, 0xd);
  mkreg (R14, 64, 0xe);
  mkreg (R15, 64, 0xf);
#undef mkreg

struct Xmm0 : Xmm { constexpr Xmm0 () : Xmm {0, 0} {}; };
struct Xmm1 : Xmm { constexpr Xmm1 () : Xmm {1, 0} {}; };
struct Xmm2 : Xmm { constexpr Xmm2 () : Xmm {2, 0} {}; };
struct Xmm3 : Xmm { constexpr Xmm3 () : Xmm {3, 0} {}; };
struct Xmm4 : Xmm { constexpr Xmm4 () : Xmm {4, 0} {}; };
struct Xmm5 : Xmm { constexpr Xmm5 () : Xmm {5, 0} {}; };
struct Xmm6 : Xmm { constexpr Xmm6 () : Xmm {6, 0} {}; };
struct Xmm7 : Xmm { constexpr Xmm7 () : Xmm {7, 0} {}; };

struct Rh : SReg<8> {
protected:
  constexpr Rh (const uint8_t c, bool h)
    : SReg { c, h }
  {}
};

struct Ah : Rh { constexpr Ah () : Rh {0, 1} {} };
struct Ch : Rh { constexpr Ch () : Rh {1, 1} {} };
struct Dh : Rh { constexpr Dh () : Rh {2, 1} {} };
struct Bh : Rh { constexpr Bh () : Rh {3, 1} {} };


template <unsigned S>
struct Mem
{
  const SReg<S> & reg;

  constexpr Mem<S> (const SReg<S> & r)
    : reg {r}
  {};
};

using M8   = Mem<8>;
using M16  = Mem<16>;
using M32  = Mem<32>;
using M64  = Mem<64>;
using M128 = Mem<128>;


constexpr unsigned pack (const uint64_t n)
{
  if (n > UINT32_MAX) return 64;
  if (n > UINT16_MAX) return 32;
  if (n >  UINT8_MAX) return 16;
  return 8;
}




template <unsigned S> struct Imm;
template <unsigned S> struct Rel;

#define mkimm(N,T)\
  template <> struct N<T> {\
    const uint##T##_t value;\
    constexpr N (decltype(value) a) : value {a} {}\
  };\
  using N##T = N<T>

  mkimm (Imm,8);
  mkimm (Imm,16);
  mkimm (Imm,32);
  mkimm (Imm,64);
  mkimm (Rel,8);
  mkimm (Rel,32);

#undef mkimm


template <unsigned S>
constexpr Imm<pack(S)> imm = S;


#include <queue>
#include <unistd.h>
#include <sys/stat.h>

struct Label
{
  virtual uint64_t value () = 0;
};

class KnownLabel : public Label
{
  const uint64_t _value;

public:
  inline constexpr KnownLabel (const uint64_t v)
    : _value {v}
  {}

  inline uint64_t value () final override
  {
    return _value;
  }
};

class FutureLabel : public Label
{
  const int           fd;
  std::queue<uint64_t> q;
  uint64_t             v;

  inline uint64_t loc () const
  {
    struct stat64 buf;
    fstat64 (fd, &buf);
    return buf.st_size;
  }

public:
  inline FutureLabel (const int d)
    : fd {d}
    , q  { }
    , v  {0}
  {}

  uint64_t value () final override
  {
    q.push(loc());
    return v;
  }

  void resolve ()
  {
    uint8_t diff;
    v = loc ();

    while (!q.empty()) {
      lseek64 (fd, q.front(), SEEK_SET);
      diff = (uint8_t) (v - q.front());
      write (fd, &diff, 1);
      q.pop ();
    }

    lseek64 (fd, 0, SEEK_END);
  }
};


struct M256       {};
struct FarPtr1616 {};
struct FarPtr1632 {};
struct FarPtr1664 {};
struct M16Int     {};
struct M32Int     {};
struct M64Int     {};
struct M80Bcd     {};
struct M32Fp      {};
struct M64Fp      {};
struct M80Fp      {};
struct M2Byte     {};
struct M28Byte    {};
struct M108Byte   {};
struct M512Byte   {};
struct Zero       {};
struct One        {};
struct Three      {};
struct Mm         {};
struct Ymm        {};
struct St0        {};
struct St         {};
struct Moffs8     {};
struct Moffs16    {};
struct Moffs32    {};
struct Moffs64    {};
struct Sreg       {};
struct Fs         {};
struct Gs         {};
struct Pref66     {};
struct PrefRexW   {};
struct Far        {};
struct Hint       {};


namespace Registers
{
  constexpr Ah   ah   {};
  constexpr Ch   ch   {};
  constexpr Dh   dh   {};
  constexpr Bh   bh   {};

  constexpr Al   al   {};
  constexpr Cl   cl   {};
  constexpr Dl   dl   {};
  constexpr Bl   bl   {};
  constexpr Spl  spl  {};
  constexpr Bpl  bpl  {};
  constexpr Sil  sil  {};
  constexpr Dil  dil  {};
  constexpr R8b  r8b  {};
  constexpr R9b  r9b  {};
  constexpr R10b r10b {};
  constexpr R11b r11b {};
  constexpr R12b r12b {};
  constexpr R13b r13b {};
  constexpr R14b r14b {};
  constexpr R15b r15b {};

  constexpr Ax   ax   {};
  constexpr Cx   cx   {};
  constexpr Dx   dx   {};
  constexpr Bx   bx   {};
  constexpr Sp   sp   {};
  constexpr Bp   bp   {};
  constexpr Si   si   {};
  constexpr Di   di   {};
  constexpr R8w  r8w  {};
  constexpr R9w  r9w  {};
  constexpr R10w r10w {};
  constexpr R11w r11w {};
  constexpr R12w r12w {};
  constexpr R13w r13w {};
  constexpr R14w r14w {};
  constexpr R15w r15w {};

  constexpr Eax  eax  {};
  constexpr Ecx  ecx  {};
  constexpr Edx  edx  {};
  constexpr Ebx  ebx  {};
  constexpr Esp  esp  {};
  constexpr Ebp  ebp  {};
  constexpr Esi  esi  {};
  constexpr Edi  edi  {};
  constexpr R8d  r8d  {};
  constexpr R9d  r9d  {};
  constexpr R10d r10d {};
  constexpr R11d r11d {};
  constexpr R12d r12d {};
  constexpr R13d r13d {};
  constexpr R14d r14d {};
  constexpr R15d r15d {};

  constexpr Rax  rax  {};
  constexpr Rcx  rcx  {};
  constexpr Rdx  rdx  {};
  constexpr Rbx  rbx  {};
  constexpr Rsp  rsp  {};
  constexpr Rbp  rbp  {};
  constexpr Rsi  rsi  {};
  constexpr Rdi  rdi  {};
  constexpr R8_  r8   {};
  constexpr R9   r9   {};
  constexpr R10  r10  {};
  constexpr R11  r11  {};
  constexpr R12  r12  {};
  constexpr R13  r13  {};
  constexpr R14  r14  {};
  constexpr R15  r15  {};

  constexpr Xmm0 xmm0 {};
  constexpr Xmm1 xmm1 {};
  constexpr Xmm2 xmm2 {};
  constexpr Xmm3 xmm3 {};
  constexpr Xmm4 xmm4 {};
  constexpr Xmm5 xmm5 {};
  constexpr Xmm6 xmm6 {};
  constexpr Xmm7 xmm7 {};


  // -------- utils


  constexpr auto arg1   = rdi;
  constexpr auto arg2   = rsi;
  constexpr auto arg3   = rdx;
  constexpr auto arg4   = rcx;
  constexpr auto arg5   = r8;
  constexpr auto arg6   = r9;
  constexpr auto result = rax;

  template <unsigned S = 64>
  constexpr SReg<S> res;

  template <> constexpr SReg<8>  res<8>  = al;
  template <> constexpr SReg<16> res<16> = ax;
  template <> constexpr SReg<32> res<32> = eax;
  template <> constexpr SReg<64> res<64> = rax;
}


#endif // ifndef REGISTERS_H
