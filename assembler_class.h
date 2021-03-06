#ifndef ASSEMBLER_CLASS_H
#define ASSEMBLER_CLASS_H


#include <functional>
#include <bits/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>

#include "registers.h"



class Assembler
{
  int filedes;
  void    * mm_ptr;
  size_t    mm_len;

public:
  Assembler ()
    : filedes { memfd_create ("assembler", 0) }
    , mm_ptr  { nullptr }
    , mm_len  { 0 }
  {}

  ~Assembler ()
  {
    if (mm_len == 0) return;
    munmap (mm_ptr, mm_len);
    mm_ptr = nullptr;
    mm_len = 0;
  }

  // Size of written machine code in bytes
  size_t size () const
  {
    return lseek (filedes, 0, SEEK_CUR);
  }

  void reset ()
  {
    if (mm_len > 0) {
      munmap (mm_ptr, mm_len);
      mm_ptr = nullptr;
      mm_len = 0;
    }
    close (filedes);
    filedes = memfd_create ("assembler", 0);
  }

  // Label to current position
  KnownLabel label () const
  {
    return KnownLabel { size () };
  }

  // Label to future position
  FutureLabel future () const
  {
    return FutureLabel { filedes };
  }

private:
  void flush () noexcept
  {
    const size_t len = size ();

    if (len == mm_len)
      return;

    if (mm_len == 0)
      mm_ptr = mmap
        ( nullptr
        , len
        , PROT_EXEC | PROT_READ
        , MAP_SHARED
        , filedes
        , 0 );
    else
      mm_ptr = mremap (mm_ptr, mm_len, len, MREMAP_MAYMOVE);

    mm_len = len;
  }

  template <class> struct func;

  template <class R, class ... As>
  struct func <R(As...)> { using type = R (*) (As...); };

public:
  template <class T> inline
  typename func<T>::type fun () noexcept
  {
    flush ();

    return reinterpret_cast
      <typename func<T>::type> (mm_ptr);
  }

  std::string to_hex ()
  {
    flush ();

    if (mm_len == 0)
      return "";

    static constexpr char alph[] = "0123456789abcdef";
    const uint8_t * buf = reinterpret_cast<uint8_t *>(mm_ptr);
    char str [size() * 2 + 1];
    size_t j{};

    for (size_t i = 0; i < mm_len; i++)
    {
      str[j++] = alph[buf[i] >> 4];
      str[j++] = alph[buf[i] & 0xf];
    }

    str[j] = 0;
    return str;
  }



  void emit (unsigned char x) __attribute__ ((hot))
  {
    write (filedes, &x, 1);
  }

  void emit (unsigned short x)
  {
    emit ((unsigned char) (x & 0xFF));
    emit ((unsigned char) (x >> 8));
  }

  void emit (unsigned int x)
  {
    emit ((unsigned short) (x & 0xFFFF));
    emit ((unsigned short) (x >> 16));
  }

  void emit (unsigned long x)
  {
    emit ((unsigned int) (x & 0xFFFFFFFF));
    emit ((unsigned int) (x >> 32));
  }

  template <class T, class...Ts>
  void emit (T x, Ts...xs)
  {
    emit (x);
    emit (xs...);
  }


  inline void meta (const Al&, const Imm8&, const uint8_t);
  inline void meta (const Eax&, const Imm32& imm, const uint8_t code);
  inline void meta (const Eax&, const Imm8& imm, const uint8_t code);
  inline void meta (const Rax&, const Imm32& imm, const uint8_t code);
  inline void meta (const Ax&, const Imm8& imm, const uint8_t code);
  inline void meta (const Ax&, const Imm16& imm, const uint8_t code);

  template <unsigned Sm, unsigned Sr, class...Cs>
  inline void meta (const Mem<Sm>& arg0, const SReg<Sr>& arg1, const Cs...cs);

  template <unsigned Sm, unsigned Si, class...Cs>
  inline void meta(const Mem<Sm>& arg0, const Imm<Si>& arg1, const uint8_t ix, const Cs...cs);

  template <unsigned Sr, unsigned Si, class...Cs>
  inline void meta(const SReg<Sr>& arg0, const Imm<Si>& arg1, const uint8_t ix, const Cs...cs);

  template <unsigned Sra, unsigned Srb, class...Cs>
  inline void meta(const SReg<Sra>& arg0, const SReg<Srb>& arg1, const Cs...cs);


  /** Add with carry imm8 to AL */
  void adc(const Al& arg0, const Imm8& arg1);

  /** Add with carry imm16 to AX. */
  void adc(const Ax& arg0, const Imm16& arg1);

  /** Add with carry imm32 to EAX. */
  void adc(const Eax& arg0, const Imm32& arg1);

  /** Add with carry imm16 to r/m16. */
  void adc(const M16& arg0, const Imm16& arg1);

  /** Add with CF sign-extended imm8 to r/m16. */
  void adc(const M16& arg0, const Imm8& arg1);

  /** Add with carry r16 to r/m16. */
  void adc(const M16& arg0, const R16& arg1);

  /** Add with CF imm32 to r/m32. */
  void adc(const M32& arg0, const Imm32& arg1);

  /** Add with CF sign-extended imm8 into r/m32. */
  void adc(const M32& arg0, const Imm8& arg1);

  /** Add with CF r32 to r/m32. */
  void adc(const M32& arg0, const R32& arg1);

  /** Add with CF imm32 sign extended to 64-bits to r/m64. */
  void adc(const M64& arg0, const Imm32& arg1);

  /** Add with CF sign-extended imm8 into r/m64. */
  void adc(const M64& arg0, const Imm8& arg1);

  /** Add with CF r64 to r/m64. */
  void adc(const M64& arg0, const R64& arg1);

  /** Add with carry imm8 to r/m8. */
  void adc(const M8& arg0, const Imm8& arg1);

  /** Add with carry byte register to r/m8. */
  void adc(const M8& arg0, const R8& arg1);

  /** Add with carry byte register to r/m8. */
  void adc(const M8& arg0, const Rh& arg1);

  /** Add with carry imm16 to r/m16. */
  void adc(const R16& arg0, const Imm16& arg1);

  /** Add with CF sign-extended imm8 to r/m16. */
  void adc(const R16& arg0, const Imm8& arg1);

  /** Add with carry r/m16 to r16. */
  void adc(const R16& arg0, const M16& arg1);

  /** Add with carry r16 to r/m16. */
  void adc(const R16& arg0, const R16& arg1);

  /** Add with carry r/m16 to r16. */
  void adc_1(const R16& arg0, const R16& arg1);

  /** Add with CF imm32 to r/m32. */
  void adc(const R32& arg0, const Imm32& arg1);

  /** Add with CF sign-extended imm8 into r/m32. */
  void adc(const R32& arg0, const Imm8& arg1);

  /** Add with CF r/m32 to r32. */
  void adc(const R32& arg0, const M32& arg1);

  /** Add with CF r32 to r/m32. */
  void adc(const R32& arg0, const R32& arg1);

  /** Add with CF r/m32 to r32. */
  void adc_1(const R32& arg0, const R32& arg1);

  /** Add with CF imm32 sign extended to 64-bits to r/m64. */
  void adc(const R64& arg0, const Imm32& arg1);

  /** Add with CF sign-extended imm8 into r/m64. */
  void adc(const R64& arg0, const Imm8& arg1);

  /** Add with CF r/m64 to r64. */
  void adc(const R64& arg0, const M64& arg1);

  /** Add with CF r64 to r/m64. */
  void adc(const R64& arg0, const R64& arg1);

  /** Add with CF r/m64 to r64. */
  void adc_1(const R64& arg0, const R64& arg1);

  /** Add with carry imm8 to r/m8. */
  void adc(const R8& arg0, const Imm8& arg1);

  /** Add with carry r/m8 to byte register. */
  void adc(const R8& arg0, const M8& arg1);

  /** Add with carry byte register to r/m8. */
  void adc(const R8& arg0, const R8& arg1);

  /** Add with carry r/m8 to byte register. */
  void adc_1(const R8& arg0, const R8& arg1);

  /** Add with carry byte register to r/m8. */
  void adc(const R8& arg0, const Rh& arg1);

  /** Add with carry r/m8 to byte register. */
  void adc_1(const R8& arg0, const Rh& arg1);

  /** Add with carry imm32 sign extended to 64- bits to RAX. */
  void adc(const Rax& arg0, const Imm32& arg1);

  /** Add with carry imm8 to r/m8. */
  void adc(const Rh& arg0, const Imm8& arg1);

  /** Add with carry r/m8 to byte register. */
  void adc(const Rh& arg0, const M8& arg1);

  /** Add with carry byte register to r/m8. */
  void adc(const Rh& arg0, const R8& arg1);

  /** Add with carry r/m8 to byte register. */
  void adc_1(const Rh& arg0, const R8& arg1);

  /** Add with carry byte register to r/m8. */
  void adc(const Rh& arg0, const Rh& arg1);

  /** Add with carry r/m8 to byte register. */
  void adc_1(const Rh& arg0, const Rh& arg1);

  /** Add imm8 to AL. */
  void add(const Al& arg0, const Imm8& arg1);

  /** Add imm16 to AX. */
  void add(const Ax& arg0, const Imm16& arg1);

  /** Add imm32 to EAX. */
  void add(const Eax& arg0, const Imm32& arg1);

  /** Add imm16 to r/m16. */
  void add(const M16& arg0, const Imm16& arg1);

  /** Add sign-extended imm8 to r/m16. */
  void add(const M16& arg0, const Imm8& arg1);

  /** Add r16 to r/m16. */
  void add(const M16& arg0, const R16& arg1);

  /** Add imm32 to r/m32. */
  void add(const M32& arg0, const Imm32& arg1);

  /** Add sign-extended imm8 to r/m32. */
  void add(const M32& arg0, const Imm8& arg1);

  /** Add r32 to r/m32. */
  void add(const M32& arg0, const R32& arg1);

  /** Add imm32 sign-extended to 64-bits to r/m64. */
  void add(const M64& arg0, const Imm32& arg1);

  /** Add sign-extended imm8 to r/m64. */
  void add(const M64& arg0, const Imm8& arg1);

  /** Add r64 to r/m64. */
  void add(const M64& arg0, const R64& arg1);

  /** Add imm8 to r/m8. */
  void add(const M8& arg0, const Imm8& arg1);

  /** Add r8 to r/m8. */
  void add(const M8& arg0, const R8& arg1);

  /** Add r8 to r/m8. */
  void add(const M8& arg0, const Rh& arg1);

  /** Add imm16 to r/m16. */
  void add(const R16& arg0, const Imm16& arg1);

  /** Add sign-extended imm8 to r/m16. */
  void add(const R16& arg0, const Imm8& arg1);

  /** Add r/m16 to r16. */
  void add(const R16& arg0, const M16& arg1);

  /** Add r16 to r/m16. */
  void add(const R16& arg0, const R16& arg1);

  /** Add r/m16 to r16. */
  void add_1(const R16& arg0, const R16& arg1);

  /** Add imm32 to r/m32. */
  void add(const R32& arg0, const Imm32& arg1);

  /** Add sign-extended imm8 to r/m32. */
  void add(const R32& arg0, const Imm8& arg1);

  /** Add r/m32 to r32. */
  void add(const R32& arg0, const M32& arg1);

  /** Add r32 to r/m32. */
  void add(const R32& arg0, const R32& arg1);

  /** Add r/m32 to r32. */
  void add_1(const R32& arg0, const R32& arg1);

  /** Add imm32 sign-extended to 64-bits to r/m64. */
  void add(const R64& arg0, const Imm32& arg1);

  /** Add sign-extended imm8 to r/m64. */
  void add(const R64& arg0, const Imm8& arg1);

  /** Add r/m64 to r64. */
  void add(const R64& arg0, const M64& arg1);

  /** Add r64 to r/m64. */
  void add(const R64& arg0, const R64& arg1);

  /** Add r/m64 to r64. */
  void add_1(const R64& arg0, const R64& arg1);

  /** Add imm8 to r/m8. */
  void add(const R8& arg0, const Imm8& arg1);

  /** Add r/m8 to r8. */
  void add(const R8& arg0, const M8& arg1);

  /** Add r8 to r/m8. */
  void add(const R8& arg0, const R8& arg1);

  /** Add r/m8 to r8. */
  void add_1(const R8& arg0, const R8& arg1);

  /** Add r8 to r/m8. */
  void add(const R8& arg0, const Rh& arg1);

  /** Add r/m8 to r8. */
  void add_1(const R8& arg0, const Rh& arg1);

  /** Add imm32 sign-extended to 64-bits to RAX. */
  void add(const Rax& arg0, const Imm32& arg1);

  /** Add imm8 to r/m8. */
  void add(const Rh& arg0, const Imm8& arg1);

  /** Add r/m8 to r8. */
  void add(const Rh& arg0, const M8& arg1);

  /** Add r8 to r/m8. */
  void add(const Rh& arg0, const R8& arg1);

  /** Add r/m8 to r8. */
  void add_1(const Rh& arg0, const R8& arg1);

  /** Add r8 to r/m8. */
  void add(const Rh& arg0, const Rh& arg1);

  /** Add r/m8 to r8. */
  void add_1(const Rh& arg0, const Rh& arg1);

  /** Add packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void addpd(const Xmm& arg0, const M128& arg1);

  /** Add packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void addpd(const Xmm& arg0, const Xmm& arg1);

  /** Add packed single-precision floating-point values from xmm2/m128 to xmm1 and stores result in xmm1. */
  void addps(const Xmm& arg0, const M128& arg1);

  /** Add packed single-precision floating-point values from xmm2/m128 to xmm1 and stores result in xmm1. */
  void addps(const Xmm& arg0, const Xmm& arg1);

  /** Add the low double-precision floating-point value from xmm2/m64 to xmm1. */
  void addsd(const Xmm& arg0, const M64& arg1);

  /** Add the low double-precision floating-point value from xmm2/m64 to xmm1. */
  void addsd(const Xmm& arg0, const Xmm& arg1);

  /** Add the low single-precision floating-point value from xmm2/m32 to xmm1. */
  void addss(const Xmm& arg0, const M32& arg1);

  /** Add the low single-precision floating-point value from xmm2/m32 to xmm1. */
  void addss(const Xmm& arg0, const Xmm& arg1);

  /** Add/subtract double-precision floating-point values from xmm2/m128 to xmm1. */
  void addsubpd(const Xmm& arg0, const M128& arg1);

  /** Add/subtract double-precision floating-point values from xmm2/m128 to xmm1. */
  void addsubpd(const Xmm& arg0, const Xmm& arg1);

  /** Add/subtract single-precision floating-point values from xmm2/m128 to xmm1. */
  void addsubps(const Xmm& arg0, const M128& arg1);

  /** Add/subtract single-precision floating-point values from xmm2/m128 to xmm1. */
  void addsubps(const Xmm& arg0, const Xmm& arg1);

  /** Perform one round of an AES decryption flow, using the Equivalent Inverse Cipher, operating on a 128-bit data (state) from xmm1 with a 128-bit round key from xmm2/m128. */
  void aesdec(const Xmm& arg0, const M128& arg1);

  /** Perform one round of an AES decryption flow, using the Equivalent Inverse Cipher, operating on a 128-bit data (state) from xmm1 with a 128-bit round key from xmm2/m128. */
  void aesdec(const Xmm& arg0, const Xmm& arg1);

  /** Perform the last round of an AES decryption flow, using the Equivalent Inverse Cipher, operating on a 128-bit data (state) from xmm1 with a 128-bit round key from xmm2/m128. */
  void aesdeclast(const Xmm& arg0, const M128& arg1);

  /** Perform the last round of an AES decryption flow, using the Equivalent Inverse Cipher, operating on a 128-bit data (state) from xmm1 with a 128-bit round key from xmm2/m128. */
  void aesdeclast(const Xmm& arg0, const Xmm& arg1);

  /** Perform one round of an AES encryption flow, operating on a 128-bit data (state) from xmm1 with a 128-bit round key from xmm2/m128. */
  void aesenc(const Xmm& arg0, const M128& arg1);

  /** Perform one round of an AES encryption flow, operating on a 128-bit data (state) from xmm1 with a 128-bit round key from xmm2/m128. */
  void aesenc(const Xmm& arg0, const Xmm& arg1);

  /** Perform the last round of an AES encryption flow, operating on a 128-bit data (state) from xmm1 with a 128-bit round key from xmm2/m128. */
  void aesenclast(const Xmm& arg0, const M128& arg1);

  /** Perform the last round of an AES encryption flow, operating on a 128-bit data (state) from xmm1 with a 128-bit round key from xmm2/m128. */
  void aesenclast(const Xmm& arg0, const Xmm& arg1);

  /** Perform the InvMixColumn transformation on a 128-bit round key from xmm2/m128 and store the result in xmm1. */
  void aesimc(const Xmm& arg0, const M128& arg1);

  /** Perform the InvMixColumn transformation on a 128-bit round key from xmm2/m128 and store the result in xmm1. */
  void aesimc(const Xmm& arg0, const Xmm& arg1);

  /** Assist in AES round key generation using an 8 bits Round Constant (RCON) specified in the immediate byte, operating on 128 bits of data specified in xmm2/m128 and stores the result in xmm1. */
  void aeskeygenassist(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Assist in AES round key generation using an 8 bits Round Constant (RCON) specified in the immediate byte, operating on 128 bits of data specified in xmm2/m128 and stores the result in xmm1. */
  void aeskeygenassist(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** AL AND imm8. */
  void and_(const Al& arg0, const Imm8& arg1);

  /** AX AND imm16. */
  void and_(const Ax& arg0, const Imm16& arg1);

  /** EAX AND imm32. */
  void and_(const Eax& arg0, const Imm32& arg1);

  /** r/m16 AND imm16. */
  void and_(const M16& arg0, const Imm16& arg1);

  /** r/m16 AND imm8 (sign-extended). */
  void and_(const M16& arg0, const Imm8& arg1);

  /** r/m16 AND r16. */
  void and_(const M16& arg0, const R16& arg1);

  /** r/m32 AND imm32. */
  void and_(const M32& arg0, const Imm32& arg1);

  /** r/m32 AND imm8 (sign-extended). */
  void and_(const M32& arg0, const Imm8& arg1);

  /** r/m32 AND r32. */
  void and_(const M32& arg0, const R32& arg1);

  /** r/m64 AND imm32 sign extended to 64-bits. */
  void and_(const M64& arg0, const Imm32& arg1);

  /** r/m64 AND imm8 (sign-extended). */
  void and_(const M64& arg0, const Imm8& arg1);

  /** r/m64 AND r32. */
  void and_(const M64& arg0, const R64& arg1);

  /** r/m8 AND imm8. */
  void and_(const M8& arg0, const Imm8& arg1);

  /** r/m8 AND r8. */
  void and_(const M8& arg0, const R8& arg1);

  /** r/m8 AND r8. */
  void and_(const M8& arg0, const Rh& arg1);

  /** r/m16 AND imm16. */
  void and_(const R16& arg0, const Imm16& arg1);

  /** r/m16 AND imm8 (sign-extended). */
  void and_(const R16& arg0, const Imm8& arg1);

  /** r16 AND r/m16. */
  void and_(const R16& arg0, const M16& arg1);

  /** r/m16 AND r16. */
  void and_(const R16& arg0, const R16& arg1);

  /** r16 AND r/m16. */
  void and__1(const R16& arg0, const R16& arg1);

  /** r/m32 AND imm32. */
  void and_(const R32& arg0, const Imm32& arg1);

  /** r/m32 AND imm8 (sign-extended). */
  void and_(const R32& arg0, const Imm8& arg1);

  /** r32 AND r/m32. */
  void and_(const R32& arg0, const M32& arg1);

  /** r/m32 AND r32. */
  void and_(const R32& arg0, const R32& arg1);

  /** r32 AND r/m32. */
  void and__1(const R32& arg0, const R32& arg1);

  /** r/m64 AND imm32 sign extended to 64-bits. */
  void and_(const R64& arg0, const Imm32& arg1);

  /** r/m64 AND imm8 (sign-extended). */
  void and_(const R64& arg0, const Imm8& arg1);

  /** r64 AND r/m64. */
  void and_(const R64& arg0, const M64& arg1);

  /** r/m64 AND r32. */
  void and_(const R64& arg0, const R64& arg1);

  /** r64 AND r/m64. */
  void and__1(const R64& arg0, const R64& arg1);

  /** r/m8 AND imm8. */
  void and_(const R8& arg0, const Imm8& arg1);

  /** r8 AND r/m8. */
  void and_(const R8& arg0, const M8& arg1);

  /** r/m8 AND r8. */
  void and_(const R8& arg0, const R8& arg1);

  /** r8 AND r/m8. */
  void and__1(const R8& arg0, const R8& arg1);

  /** r/m8 AND r8. */
  void and_(const R8& arg0, const Rh& arg1);

  /** r8 AND r/m8. */
  void and__1(const R8& arg0, const Rh& arg1);

  /** RAX AND imm32 sign-extended to 64-bits. */
  void and_(const Rax& arg0, const Imm32& arg1);

  /** r/m8 AND imm8. */
  void and_(const Rh& arg0, const Imm8& arg1);

  /** r8 AND r/m8. */
  void and_(const Rh& arg0, const M8& arg1);

  /** r/m8 AND r8. */
  void and_(const Rh& arg0, const R8& arg1);

  /** r8 AND r/m8. */
  void and__1(const Rh& arg0, const R8& arg1);

  /** r/m8 AND r8. */
  void and_(const Rh& arg0, const Rh& arg1);

  /** r8 AND r/m8. */
  void and__1(const Rh& arg0, const Rh& arg1);

  /** Bitwise AND of inverted r32b with r/m32, store result in r32a */
  void andn(const R32& arg0, const R32& arg1, const M32& arg2);

  /** Bitwise AND of inverted r32b with r/m32, store result in r32a */
  void andn(const R32& arg0, const R32& arg1, const R32& arg2);

  /** Bitwise AND of inverted r64b with r/m64, store result in r64a */
  void andn(const R64& arg0, const R64& arg1, const M64& arg2);

  /** Bitwise AND of inverted r64b with r/m64, store result in r64a */
  void andn(const R64& arg0, const R64& arg1, const R64& arg2);

  /** Bitwise logical AND NOT of xmm2/m128 and xmm1. */
  void andnpd(const Xmm& arg0, const M128& arg1);

  /** Bitwise logical AND NOT of xmm2/m128 and xmm1. */
  void andnpd(const Xmm& arg0, const Xmm& arg1);

  /** Bitwise logical AND NOT of xmm2/m128 and xmm1. */
  void andnps(const Xmm& arg0, const M128& arg1);

  /** Bitwise logical AND NOT of xmm2/m128 and xmm1. */
  void andnps(const Xmm& arg0, const Xmm& arg1);

  /** Return the bitwise logical AND of packed double-precision floating-point values in xmm1 and xmm2/m128. */
  void andpd(const Xmm& arg0, const M128& arg1);

  /** Return the bitwise logical AND of packed double-precision floating-point values in xmm1 and xmm2/m128. */
  void andpd(const Xmm& arg0, const Xmm& arg1);

  /** Bitwise logical AND of xmm2/m128 and xmm1. */
  void andps(const Xmm& arg0, const M128& arg1);

  /** Bitwise logical AND of xmm2/m128 and xmm1. */
  void andps(const Xmm& arg0, const Xmm& arg1);

  /** Contiguous bitwise extract from r/m32 using r32b as control; store result in r32a. */
  void bextr(const R32& arg0, const M32& arg1, const R32& arg2);

  /** Contiguous bitwise extract from r/m32 using r32b as control; store result in r32a. */
  void bextr(const R32& arg0, const R32& arg1, const R32& arg2);

  /** Contiguous bitwise extract from r/m64 using r64b as control; store result in r64a */
  void bextr(const R64& arg0, const M64& arg1, const R64& arg2);

  /** Contiguous bitwise extract from r/m64 using r64b as control; store result in r64a */
  void bextr(const R64& arg0, const R64& arg1, const R64& arg2);

  /** Select packed DP-FP values from xmm1 and xmm2/m128 from mask specified in imm8 and store the values into xmm1. */
  void blendpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Select packed DP-FP values from xmm1 and xmm2/m128 from mask specified in imm8 and store the values into xmm1. */
  void blendpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Select packed single precision floating-point values from xmm1 and xmm2/m128 from mask specified in imm8 and store the values into xmm1. */
  void blendps(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Select packed single precision floating-point values from xmm1 and xmm2/m128 from mask specified in imm8 and store the values into xmm1. */
  void blendps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Select packed DP FP values from xmm1 and xmm2 from mask specified in XMM0 and store the values in xmm1. */
  void blendvpd(const Xmm& arg0, const M128& arg1, const Xmm0& arg2);

  /** Select packed DP FP values from xmm1 and xmm2 from mask specified in XMM0 and store the values in xmm1. */
  void blendvpd(const Xmm& arg0, const Xmm& arg1, const Xmm0& arg2);

  /** Select packed single precision floating-point values from xmm1 and xmm2/m128 from mask specified in XMM0 and store the values into xmm1. */
  void blendvps(const Xmm& arg0, const M128& arg1, const Xmm0& arg2);

  /** Select packed single precision floating-point values from xmm1 and xmm2/m128 from mask specified in XMM0 and store the values into xmm1. */
  void blendvps(const Xmm& arg0, const Xmm& arg1, const Xmm0& arg2);

  /** Extract lowest set bit from r/m32 and set that bit in r32. */
  void blsi(const R32& arg0, const M32& arg1);

  /** Extract lowest set bit from r/m32 and set that bit in r32. */
  void blsi(const R32& arg0, const R32& arg1);

  /** Extract lowest set bit from r/m64, and set that bit in r64. */
  void blsi(const R64& arg0, const M64& arg1);

  /** Extract lowest set bit from r/m64, and set that bit in r64. */
  void blsi(const R64& arg0, const R64& arg1);

  /** Set all lower bits in r32 to "1" starting from bit 0 to lowest set bit in r/m32 */
  void blsmsk(const R32& arg0, const M32& arg1);

  /** Set all lower bits in r32 to "1" starting from bit 0 to lowest set bit in r/m32 */
  void blsmsk(const R32& arg0, const R32& arg1);

  /** Set all lower bits in r64 to "1" starting from bit 0 to lowest set bit in r/m64 */
  void blsmsk(const R64& arg0, const M64& arg1);

  /** Set all lower bits in r64 to "1" starting from bit 0 to lowest set bit in r/m64 */
  void blsmsk(const R64& arg0, const R64& arg1);

  /** Reset lowest set bit of r/m32, keep all other bits of r/m32 and write result to r32. */
  void blsr(const R32& arg0, const M32& arg1);

  /** Reset lowest set bit of r/m32, keep all other bits of r/m32 and write result to r32. */
  void blsr(const R32& arg0, const R32& arg1);

  /** Reset lowest set bit of r/m64, keep all other bits of r/m64 and write result to r64. */
  void blsr(const R64& arg0, const M64& arg1);

  /** Reset lowest set bit of r/m64, keep all other bits of r/m64 and write result to r64. */
  void blsr(const R64& arg0, const R64& arg1);

  /** Bit scan forward on r/m16. */
  void bsf(const R16& arg0, const M16& arg1);

  /** Bit scan forward on r/m16. */
  void bsf(const R16& arg0, const R16& arg1);

  /** Bit scan forward on r/m32. */
  void bsf(const R32& arg0, const M32& arg1);

  /** Bit scan forward on r/m32. */
  void bsf(const R32& arg0, const R32& arg1);

  /** Bit scan forward on r/m64. */
  void bsf(const R64& arg0, const M64& arg1);

  /** Bit scan forward on r/m64. */
  void bsf(const R64& arg0, const R64& arg1);

  /** Bit scan reverse on r/m16. */
  void bsr(const R16& arg0, const M16& arg1);

  /** Bit scan reverse on r/m16. */
  void bsr(const R16& arg0, const R16& arg1);

  /** Bit scan reverse on r/m32. */
  void bsr(const R32& arg0, const M32& arg1);

  /** Bit scan reverse on r/m32. */
  void bsr(const R32& arg0, const R32& arg1);

  /** Bit scan reverse on r/m64. */
  void bsr(const R64& arg0, const M64& arg1);

  /** Bit scan reverse on r/m64. */
  void bsr(const R64& arg0, const R64& arg1);

  /** Reverses the byte order of a 32-bit register. */
  void bswap(const R32& arg0);

  /** Reverses the byte order of a 64-bit register. */
  void bswap(const R64& arg0);

  /** Store selected bit in CF flag. */
  void bt(const M16& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag. */
  void bt(const M16& arg0, const R16& arg1);

  /** Store selected bit in CF flag. */
  void bt(const M32& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag. */
  void bt(const M32& arg0, const R32& arg1);

  /** Store selected bit in CF flag. */
  void bt(const M64& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag. */
  void bt(const M64& arg0, const R64& arg1);

  /** Store selected bit in CF flag. */
  void bt(const R16& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag. */
  void bt(const R16& arg0, const R16& arg1);

  /** Store selected bit in CF flag. */
  void bt(const R32& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag. */
  void bt(const R32& arg0, const R32& arg1);

  /** Store selected bit in CF flag. */
  void bt(const R64& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag. */
  void bt(const R64& arg0, const R64& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const M16& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const M16& arg0, const R16& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const M32& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const M32& arg0, const R32& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const M64& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const M64& arg0, const R64& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const R16& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const R16& arg0, const R16& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const R32& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const R32& arg0, const R32& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const R64& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and complement. */
  void btc(const R64& arg0, const R64& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const M16& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const M16& arg0, const R16& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const M32& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const M32& arg0, const R32& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const M64& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const M64& arg0, const R64& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const R16& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const R16& arg0, const R16& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const R32& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const R32& arg0, const R32& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const R64& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and clear. */
  void btr(const R64& arg0, const R64& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const M16& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const M16& arg0, const R16& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const M32& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const M32& arg0, const R32& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const M64& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const M64& arg0, const R64& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const R16& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const R16& arg0, const R16& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const R32& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const R32& arg0, const R32& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const R64& arg0, const Imm8& arg1);

  /** Store selected bit in CF flag and set. */
  void bts(const R64& arg0, const R64& arg1);

  /** Zero bits in r/m32 starting with the position in r32b, write result to r32a. */
  void bzhi(const R32& arg0, const M32& arg1, const R32& arg2);

  /** Zero bits in r/m32 starting with the position in r32b, write result to r32a. */
  void bzhi(const R32& arg0, const R32& arg1, const R32& arg2);

  /** Zero bits in r/m64 starting with the position in r64b, write result to r64a. */
  void bzhi(const R64& arg0, const M64& arg1, const R64& arg2);

  /** Zero bits in r/m64 starting with the position in r64b, write result to r64a. */
  void bzhi(const R64& arg0, const R64& arg1, const R64& arg2);

  /** Call far, absolute indirect address given in m16:16. In 32-bit mode: if selector points to a gate, then RIP = 32-bit zero extended displacement taken from gate; else RIP = zero extended 16- bit offset from far pointer referenced in the instruction. */
  void call(const FarPtr1616& arg0);

  /** In 64-bit mode: If selector points to a gate, then RIP = 64-bit displacement taken from gate; else RIP = zero extended 32-bit offset from far pointer referenced in the instruction. */
  void call(const FarPtr1632& arg0);

  /** In 64-bit mode: If selector points to a gate, then RIP = 64-bit displacement taken from gate; else RIP = 64-bit offset from far pointer referenced in the instruction. */
  void call(const FarPtr1664& arg0);

  /** Call near, relative, displacement relative to next instruction. 32-bit displacement sign extended to 64-bits in 64-bit mode. */
  void call(Label& arg0);

  /** Call near, absolute indirect, address given in r/m64. */
  void call(const M64& arg0);

  /** Call near, absolute indirect, address given in r/m64. */
  void call(const R64& arg0);

  /** Call near, relative, displacement relative to next instruction. 32-bit displacement sign extended to 64-bits in 64-bit mode. */
  void call(const Rel32& arg0);

  /** AX = sign-extend of AL. */
  void cbw();

  /** EDX:EAX = sign-extend of EAX. */
  void cdq();

  /** RAX = sign-extend of EAX. */
  void cdqe();

  /** Clear CF flag. */
  void clc();

  /** Clear DF flag. */
  void cld();

  /** Flushes cache line containing m8. */
  void clflush(const M8& arg0);

  /** Clear interrupt flag; interrupts disabled when interrupt flag cleared. */
  void cli();

  /** Complement CF flag. */
  void cmc();

  /** Move if above (CF=0 and ZF=0). */
  void cmova(const R16& arg0, const M16& arg1);

  /** Move if above (CF=0 and ZF=0). */
  void cmova(const R16& arg0, const R16& arg1);

  /** Move if above (CF=0 and ZF=0). */
  void cmova(const R32& arg0, const M32& arg1);

  /** Move if above (CF=0 and ZF=0). */
  void cmova(const R32& arg0, const R32& arg1);

  /** Move if above (CF=0 and ZF=0). */
  void cmova(const R64& arg0, const M64& arg1);

  /** Move if above (CF=0 and ZF=0). */
  void cmova(const R64& arg0, const R64& arg1);

  /** Move if above or equal (CF=0). */
  void cmovae(const R16& arg0, const M16& arg1);

  /** Move if above or equal (CF=0). */
  void cmovae(const R16& arg0, const R16& arg1);

  /** Move if above or equal (CF=0). */
  void cmovae(const R32& arg0, const M32& arg1);

  /** Move if above or equal (CF=0). */
  void cmovae(const R32& arg0, const R32& arg1);

  /** Move if above or equal (CF=0). */
  void cmovae(const R64& arg0, const M64& arg1);

  /** Move if above or equal (CF=0). */
  void cmovae(const R64& arg0, const R64& arg1);

  /** Move if below (CF=1). */
  void cmovb(const R16& arg0, const M16& arg1);

  /** Move if below (CF=1). */
  void cmovb(const R16& arg0, const R16& arg1);

  /** Move if below (CF=1). */
  void cmovb(const R32& arg0, const M32& arg1);

  /** Move if below (CF=1). */
  void cmovb(const R32& arg0, const R32& arg1);

  /** Move if below (CF=1). */
  void cmovb(const R64& arg0, const M64& arg1);

  /** Move if below (CF=1). */
  void cmovb(const R64& arg0, const R64& arg1);

  /** Move if below or equal (CF=1 or ZF=1). */
  void cmovbe(const R16& arg0, const M16& arg1);

  /** Move if below or equal (CF=1 or ZF=1). */
  void cmovbe(const R16& arg0, const R16& arg1);

  /** Move if below or equal (CF=1 or ZF=1). */
  void cmovbe(const R32& arg0, const M32& arg1);

  /** Move if below or equal (CF=1 or ZF=1). */
  void cmovbe(const R32& arg0, const R32& arg1);

  /** Move if below or equal (CF=1 or ZF=1). */
  void cmovbe(const R64& arg0, const M64& arg1);

  /** Move if below or equal (CF=1 or ZF=1). */
  void cmovbe(const R64& arg0, const R64& arg1);

  /** Move if carry (CF=1). */
  void cmovc(const R16& arg0, const M16& arg1);

  /** Move if carry (CF=1). */
  void cmovc(const R16& arg0, const R16& arg1);

  /** Move if carry (CF=1). */
  void cmovc(const R32& arg0, const M32& arg1);

  /** Move if carry (CF=1). */
  void cmovc(const R32& arg0, const R32& arg1);

  /** Move if carry (CF=1). */
  void cmovc(const R64& arg0, const M64& arg1);

  /** Move if carry (CF=1). */
  void cmovc(const R64& arg0, const R64& arg1);

  /** Move if equal (ZF=1). */
  void cmove(const R16& arg0, const M16& arg1);

  /** Move if equal (ZF=1). */
  void cmove(const R16& arg0, const R16& arg1);

  /** Move if equal (ZF=1). */
  void cmove(const R32& arg0, const M32& arg1);

  /** Move if equal (ZF=1). */
  void cmove(const R32& arg0, const R32& arg1);

  /** Move if equal (ZF=1). */
  void cmove(const R64& arg0, const M64& arg1);

  /** Move if equal (ZF=1). */
  void cmove(const R64& arg0, const R64& arg1);

  /** Move if greater (ZF=0 and SF=OF). */
  void cmovg(const R16& arg0, const M16& arg1);

  /** Move if greater (ZF=0 and SF=OF). */
  void cmovg(const R16& arg0, const R16& arg1);

  /** Move if greater (ZF=0 and SF=OF). */
  void cmovg(const R32& arg0, const M32& arg1);

  /** Move if greater (ZF=0 and SF=OF). */
  void cmovg(const R32& arg0, const R32& arg1);

  /** Move if greater (ZF=0 and SF=OF). */
  void cmovg(const R64& arg0, const M64& arg1);

  /** Move if greater (ZF=0 and SF=OF). */
  void cmovg(const R64& arg0, const R64& arg1);

  /** Move if greater or equal (SF=OF). */
  void cmovge(const R16& arg0, const M16& arg1);

  /** Move if greater or equal (SF=OF). */
  void cmovge(const R16& arg0, const R16& arg1);

  /** Move if greater or equal (SF=OF). */
  void cmovge(const R32& arg0, const M32& arg1);

  /** Move if greater or equal (SF=OF). */
  void cmovge(const R32& arg0, const R32& arg1);

  /** Move if greater or equal (SF=OF). */
  void cmovge(const R64& arg0, const M64& arg1);

  /** Move if greater or equal (SF=OF). */
  void cmovge(const R64& arg0, const R64& arg1);

  /** Move if less (SF != OF). */
  void cmovl(const R16& arg0, const M16& arg1);

  /** Move if less (SF != OF). */
  void cmovl(const R16& arg0, const R16& arg1);

  /** Move if less (SF!= OF). */
  void cmovl(const R32& arg0, const M32& arg1);

  /** Move if less (SF!= OF). */
  void cmovl(const R32& arg0, const R32& arg1);

  /** Move if less (SF!= OF). */
  void cmovl(const R64& arg0, const M64& arg1);

  /** Move if less (SF!= OF). */
  void cmovl(const R64& arg0, const R64& arg1);

  /** Move if less or equal (ZF=1 or SF!= OF). */
  void cmovle(const R16& arg0, const M16& arg1);

  /** Move if less or equal (ZF=1 or SF!= OF). */
  void cmovle(const R16& arg0, const R16& arg1);

  /** Move if less or equal (ZF=1 or SF!= OF). */
  void cmovle(const R32& arg0, const M32& arg1);

  /** Move if less or equal (ZF=1 or SF!= OF). */
  void cmovle(const R32& arg0, const R32& arg1);

  /** Move if less or equal (ZF=1 or SF!= OF). */
  void cmovle(const R64& arg0, const M64& arg1);

  /** Move if less or equal (ZF=1 or SF!= OF). */
  void cmovle(const R64& arg0, const R64& arg1);

  /** Move if not above (CF=1 or ZF=1). */
  void cmovna(const R16& arg0, const M16& arg1);

  /** Move if not above (CF=1 or ZF=1). */
  void cmovna(const R16& arg0, const R16& arg1);

  /** Move if not above (CF=1 or ZF=1). */
  void cmovna(const R32& arg0, const M32& arg1);

  /** Move if not above (CF=1 or ZF=1). */
  void cmovna(const R32& arg0, const R32& arg1);

  /** Move if not above (CF=1 or ZF=1). */
  void cmovna(const R64& arg0, const M64& arg1);

  /** Move if not above (CF=1 or ZF=1). */
  void cmovna(const R64& arg0, const R64& arg1);

  /** Move if not above or equal (CF=1). */
  void cmovnae(const R16& arg0, const M16& arg1);

  /** Move if not above or equal (CF=1). */
  void cmovnae(const R16& arg0, const R16& arg1);

  /** Move if not above or equal (CF=1). */
  void cmovnae(const R32& arg0, const M32& arg1);

  /** Move if not above or equal (CF=1). */
  void cmovnae(const R32& arg0, const R32& arg1);

  /** Move if not above or equal (CF=1). */
  void cmovnae(const R64& arg0, const M64& arg1);

  /** Move if not above or equal (CF=1). */
  void cmovnae(const R64& arg0, const R64& arg1);

  /** Move if not below (CF=0). */
  void cmovnb(const R16& arg0, const M16& arg1);

  /** Move if not below (CF=0). */
  void cmovnb(const R16& arg0, const R16& arg1);

  /** Move if not below (CF=0). */
  void cmovnb(const R32& arg0, const M32& arg1);

  /** Move if not below (CF=0). */
  void cmovnb(const R32& arg0, const R32& arg1);

  /** Move if not below (CF=0). */
  void cmovnb(const R64& arg0, const M64& arg1);

  /** Move if not below (CF=0). */
  void cmovnb(const R64& arg0, const R64& arg1);

  /** Move if not below or equal (CF=0 and ZF=0). */
  void cmovnbe(const R16& arg0, const M16& arg1);

  /** Move if not below or equal (CF=0 and ZF=0). */
  void cmovnbe(const R16& arg0, const R16& arg1);

  /** Move if not below or equal (CF=0 and ZF=0). */
  void cmovnbe(const R32& arg0, const M32& arg1);

  /** Move if not below or equal (CF=0 and ZF=0). */
  void cmovnbe(const R32& arg0, const R32& arg1);

  /** Move if not below or equal (CF=0 and ZF=0). */
  void cmovnbe(const R64& arg0, const M64& arg1);

  /** Move if not below or equal (CF=0 and ZF=0). */
  void cmovnbe(const R64& arg0, const R64& arg1);

  /** Move if not carry (CF=0). */
  void cmovnc(const R16& arg0, const M16& arg1);

  /** Move if not carry (CF=0). */
  void cmovnc(const R16& arg0, const R16& arg1);

  /** Move if not carry (CF=0). */
  void cmovnc(const R32& arg0, const M32& arg1);

  /** Move if not carry (CF=0). */
  void cmovnc(const R32& arg0, const R32& arg1);

  /** Move if not carry (CF=0). */
  void cmovnc(const R64& arg0, const M64& arg1);

  /** Move if not carry (CF=0). */
  void cmovnc(const R64& arg0, const R64& arg1);

  /** Move if not equal (ZF=0). */
  void cmovne(const R16& arg0, const M16& arg1);

  /** Move if not equal (ZF=0). */
  void cmovne(const R16& arg0, const R16& arg1);

  /** Move if not equal (ZF=0). */
  void cmovne(const R32& arg0, const M32& arg1);

  /** Move if not equal (ZF=0). */
  void cmovne(const R32& arg0, const R32& arg1);

  /** Move if not equal (ZF=0). */
  void cmovne(const R64& arg0, const M64& arg1);

  /** Move if not equal (ZF=0). */
  void cmovne(const R64& arg0, const R64& arg1);

  /** Move if not greater (ZF=1 or SF!= OF). */
  void cmovng(const R16& arg0, const M16& arg1);

  /** Move if not greater (ZF=1 or SF!= OF). */
  void cmovng(const R16& arg0, const R16& arg1);

  /** Move if not greater (ZF=1 or SF!= OF). */
  void cmovng(const R32& arg0, const M32& arg1);

  /** Move if not greater (ZF=1 or SF!= OF). */
  void cmovng(const R32& arg0, const R32& arg1);

  /** Move if not greater (ZF=1 or SF!= OF). */
  void cmovng(const R64& arg0, const M64& arg1);

  /** Move if not greater (ZF=1 or SF!= OF). */
  void cmovng(const R64& arg0, const R64& arg1);

  /** Move if not greater or equal (SF!= OF). */
  void cmovnge(const R16& arg0, const M16& arg1);

  /** Move if not greater or equal (SF!= OF). */
  void cmovnge(const R16& arg0, const R16& arg1);

  /** Move if not greater or equal (SF!= OF). */
  void cmovnge(const R32& arg0, const M32& arg1);

  /** Move if not greater or equal (SF!= OF). */
  void cmovnge(const R32& arg0, const R32& arg1);

  /** Move if not greater or equal (SF!= OF). */
  void cmovnge(const R64& arg0, const M64& arg1);

  /** Move if not greater or equal (SF!= OF). */
  void cmovnge(const R64& arg0, const R64& arg1);

  /** Move if not less (SF=OF). */
  void cmovnl(const R16& arg0, const M16& arg1);

  /** Move if not less (SF=OF). */
  void cmovnl(const R16& arg0, const R16& arg1);

  /** Move if not less (SF=OF). */
  void cmovnl(const R32& arg0, const M32& arg1);

  /** Move if not less (SF=OF). */
  void cmovnl(const R32& arg0, const R32& arg1);

  /** Move if not less (SF=OF). */
  void cmovnl(const R64& arg0, const M64& arg1);

  /** Move if not less (SF=OF). */
  void cmovnl(const R64& arg0, const R64& arg1);

  /** Move if not less or equal (ZF=0 and SF=OF). */
  void cmovnle(const R16& arg0, const M16& arg1);

  /** Move if not less or equal (ZF=0 and SF=OF). */
  void cmovnle(const R16& arg0, const R16& arg1);

  /** Move if not less or equal (ZF=0 and SF=OF). */
  void cmovnle(const R32& arg0, const M32& arg1);

  /** Move if not less or equal (ZF=0 and SF=OF). */
  void cmovnle(const R32& arg0, const R32& arg1);

  /** Move if not less or equal (ZF=0 and SF=OF). */
  void cmovnle(const R64& arg0, const M64& arg1);

  /** Move if not less or equal (ZF=0 and SF=OF). */
  void cmovnle(const R64& arg0, const R64& arg1);

  /** Move if not overflow (OF=0). */
  void cmovno(const R16& arg0, const M16& arg1);

  /** Move if not overflow (OF=0). */
  void cmovno(const R16& arg0, const R16& arg1);

  /** Move if not overflow (OF=0). */
  void cmovno(const R32& arg0, const M32& arg1);

  /** Move if not overflow (OF=0). */
  void cmovno(const R32& arg0, const R32& arg1);

  /** Move if not overflow (OF=0). */
  void cmovno(const R64& arg0, const M64& arg1);

  /** Move if not overflow (OF=0). */
  void cmovno(const R64& arg0, const R64& arg1);

  /** Move if not parity (PF=0). */
  void cmovnp(const R16& arg0, const M16& arg1);

  /** Move if not parity (PF=0). */
  void cmovnp(const R16& arg0, const R16& arg1);

  /** Move if not parity (PF=0). */
  void cmovnp(const R32& arg0, const M32& arg1);

  /** Move if not parity (PF=0). */
  void cmovnp(const R32& arg0, const R32& arg1);

  /** Move if not parity (PF=0). */
  void cmovnp(const R64& arg0, const M64& arg1);

  /** Move if not parity (PF=0). */
  void cmovnp(const R64& arg0, const R64& arg1);

  /** Move if not sign (SF=0). */
  void cmovns(const R16& arg0, const M16& arg1);

  /** Move if not sign (SF=0). */
  void cmovns(const R16& arg0, const R16& arg1);

  /** Move if not sign (SF=0). */
  void cmovns(const R32& arg0, const M32& arg1);

  /** Move if not sign (SF=0). */
  void cmovns(const R32& arg0, const R32& arg1);

  /** Move if not sign (SF=0). */
  void cmovns(const R64& arg0, const M64& arg1);

  /** Move if not sign (SF=0). */
  void cmovns(const R64& arg0, const R64& arg1);

  /** Move if not zero (ZF=0). */
  void cmovnz(const R16& arg0, const M16& arg1);

  /** Move if not zero (ZF=0). */
  void cmovnz(const R16& arg0, const R16& arg1);

  /** Move if not zero (ZF=0). */
  void cmovnz(const R32& arg0, const M32& arg1);

  /** Move if not zero (ZF=0). */
  void cmovnz(const R32& arg0, const R32& arg1);

  /** Move if not zero (ZF=0). */
  void cmovnz(const R64& arg0, const M64& arg1);

  /** Move if not zero (ZF=0). */
  void cmovnz(const R64& arg0, const R64& arg1);

  /** Move if overflow (OF=1). */
  void cmovo(const R16& arg0, const M16& arg1);

  /** Move if overflow (OF=1). */
  void cmovo(const R16& arg0, const R16& arg1);

  /** Move if overflow (OF=1). */
  void cmovo(const R32& arg0, const M32& arg1);

  /** Move if overflow (OF=1). */
  void cmovo(const R32& arg0, const R32& arg1);

  /** Move if overflow (OF=1). */
  void cmovo(const R64& arg0, const M64& arg1);

  /** Move if overflow (OF=1). */
  void cmovo(const R64& arg0, const R64& arg1);

  /** Move if parity (PF=1). */
  void cmovp(const R16& arg0, const M16& arg1);

  /** Move if parity (PF=1). */
  void cmovp(const R16& arg0, const R16& arg1);

  /** Move if parity (PF=1). */
  void cmovp(const R32& arg0, const M32& arg1);

  /** Move if parity (PF=1). */
  void cmovp(const R32& arg0, const R32& arg1);

  /** Move if parity (PF=1). */
  void cmovp(const R64& arg0, const M64& arg1);

  /** Move if parity (PF=1). */
  void cmovp(const R64& arg0, const R64& arg1);

  /** Move if parity even (PF=1). */
  void cmovpe(const R16& arg0, const M16& arg1);

  /** Move if parity even (PF=1). */
  void cmovpe(const R16& arg0, const R16& arg1);

  /** Move if parity even (PF=1). */
  void cmovpe(const R32& arg0, const M32& arg1);

  /** Move if parity even (PF=1). */
  void cmovpe(const R32& arg0, const R32& arg1);

  /** Move if parity even (PF=1). */
  void cmovpe(const R64& arg0, const M64& arg1);

  /** Move if parity even (PF=1). */
  void cmovpe(const R64& arg0, const R64& arg1);

  /** Move if parity odd (PF=0). */
  void cmovpo(const R16& arg0, const M16& arg1);

  /** Move if parity odd (PF=0). */
  void cmovpo(const R16& arg0, const R16& arg1);

  /** Move if parity odd (PF=0). */
  void cmovpo(const R32& arg0, const M32& arg1);

  /** Move if parity odd (PF=0). */
  void cmovpo(const R32& arg0, const R32& arg1);

  /** Move if parity odd (PF=0). */
  void cmovpo(const R64& arg0, const M64& arg1);

  /** Move if parity odd (PF=0). */
  void cmovpo(const R64& arg0, const R64& arg1);

  /** Move if sign (SF=1). */
  void cmovs(const R16& arg0, const M16& arg1);

  /** Move if sign (SF=1). */
  void cmovs(const R16& arg0, const R16& arg1);

  /** Move if sign (SF=1). */
  void cmovs(const R32& arg0, const M32& arg1);

  /** Move if sign (SF=1). */
  void cmovs(const R32& arg0, const R32& arg1);

  /** Move if sign (SF=1). */
  void cmovs(const R64& arg0, const M64& arg1);

  /** Move if sign (SF=1). */
  void cmovs(const R64& arg0, const R64& arg1);

  /** Move if zero (ZF=1). */
  void cmovz(const R16& arg0, const M16& arg1);

  /** Move if zero (ZF=1). */
  void cmovz(const R16& arg0, const R16& arg1);

  /** Move if zero (ZF=1). */
  void cmovz(const R32& arg0, const M32& arg1);

  /** Move if zero (ZF=1). */
  void cmovz(const R32& arg0, const R32& arg1);

  /** Move if zero (ZF=1). */
  void cmovz(const R64& arg0, const M64& arg1);

  /** Move if zero (ZF=1). */
  void cmovz(const R64& arg0, const R64& arg1);

  /** Compare imm8 with AL. */
  void cmp(const Al& arg0, const Imm8& arg1);

  /** Compare imm16 with AX. */
  void cmp(const Ax& arg0, const Imm16& arg1);

  /** Compare imm32 with EAX. */
  void cmp(const Eax& arg0, const Imm32& arg1);

  /** Compare imm16 with r/m16. */
  void cmp(const M16& arg0, const Imm16& arg1);

  /** Compare imm8 with r/m16. */
  void cmp(const M16& arg0, const Imm8& arg1);

  /** Compare r16 with r/m16. */
  void cmp(const M16& arg0, const R16& arg1);

  /** Compare imm32 with r/m32. */
  void cmp(const M32& arg0, const Imm32& arg1);

  /** Compare imm8 with r/m32. */
  void cmp(const M32& arg0, const Imm8& arg1);

  /** Compare r32 with r/m32. */
  void cmp(const M32& arg0, const R32& arg1);

  /** Compare imm32 sign-extended to 64-bits with r/m64. */
  void cmp(const M64& arg0, const Imm32& arg1);

  /** Compare imm8 with r/m64. */
  void cmp(const M64& arg0, const Imm8& arg1);

  /** Compare r64 with r/m64. */
  void cmp(const M64& arg0, const R64& arg1);

  /** Compare imm8 with r/m8. */
  void cmp(const M8& arg0, const Imm8& arg1);

  /** Compare r8 with r/m8. */
  void cmp(const M8& arg0, const R8& arg1);

  /** Compare r8 with r/m8. */
  void cmp(const M8& arg0, const Rh& arg1);

  /** Compare imm16 with r/m16. */
  void cmp(const R16& arg0, const Imm16& arg1);

  /** Compare imm8 with r/m16. */
  void cmp(const R16& arg0, const Imm8& arg1);

  /** Compare r/m16 with r16. */
  void cmp(const R16& arg0, const M16& arg1);

  /** Compare r16 with r/m16. */
  void cmp(const R16& arg0, const R16& arg1);

  /** Compare r/m16 with r16. */
  void cmp_1(const R16& arg0, const R16& arg1);

  /** Compare imm32 with r/m32. */
  void cmp(const R32& arg0, const Imm32& arg1);

  /** Compare imm8 with r/m32. */
  void cmp(const R32& arg0, const Imm8& arg1);

  /** Compare r/m32 with r32. */
  void cmp(const R32& arg0, const M32& arg1);

  /** Compare r32 with r/m32. */
  void cmp(const R32& arg0, const R32& arg1);

  /** Compare r/m32 with r32. */
  void cmp_1(const R32& arg0, const R32& arg1);

  /** Compare imm32 sign-extended to 64-bits with r/m64. */
  void cmp(const R64& arg0, const Imm32& arg1);

  /** Compare imm8 with r/m64. */
  void cmp(const R64& arg0, const Imm8& arg1);

  /** Compare r/m64 with r64. */
  void cmp(const R64& arg0, const M64& arg1);

  /** Compare r64 with r/m64. */
  void cmp(const R64& arg0, const R64& arg1);

  /** Compare r/m64 with r64. */
  void cmp_1(const R64& arg0, const R64& arg1);

  /** Compare imm8 with r/m8. */
  void cmp(const R8& arg0, const Imm8& arg1);

  /** Compare r/m8 with r8. */
  void cmp(const R8& arg0, const M8& arg1);

  /** Compare r8 with r/m8. */
  void cmp(const R8& arg0, const R8& arg1);

  /** Compare r/m8 with r8. */
  void cmp_1(const R8& arg0, const R8& arg1);

  /** Compare r8 with r/m8. */
  void cmp(const R8& arg0, const Rh& arg1);

  /** Compare r/m8 with r8. */
  void cmp_1(const R8& arg0, const Rh& arg1);

  /** Compare imm32 sign-extended to 64-bits with RAX. */
  void cmp(const Rax& arg0, const Imm32& arg1);

  /** Compare imm8 with r/m8. */
  void cmp(const Rh& arg0, const Imm8& arg1);

  /** Compare r/m8 with r8. */
  void cmp(const Rh& arg0, const M8& arg1);

  /** Compare r8 with r/m8. */
  void cmp(const Rh& arg0, const R8& arg1);

  /** Compare r/m8 with r8. */
  void cmp_1(const Rh& arg0, const R8& arg1);

  /** Compare r8 with r/m8. */
  void cmp(const Rh& arg0, const Rh& arg1);

  /** Compare r/m8 with r8. */
  void cmp_1(const Rh& arg0, const Rh& arg1);

  /** Compare packed double-precision floating- point values in xmm2/m128 and xmm1 using imm8 as comparison predicate. */
  void cmppd(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Compare packed double-precision floating- point values in xmm2/m128 and xmm1 using imm8 as comparison predicate. */
  void cmppd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Compare packed single-precision floating- point values in xmm2/mem and xmm1 using imm8 as comparison predicate. */
  void cmpps(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Compare packed single-precision floating- point values in xmm2/mem and xmm1 using imm8 as comparison predicate. */
  void cmpps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** For legacy mode, compare word at address DS:(E)SI with word at address ES:(E)DI; For 64-bit mode compare word at address (R|E)SI with word at address (R|E)DI. The status flags are set accordingly. */
  void cmps(const M16& arg0, const M16& arg1);

  /** For legacy mode, compare dword at address DS:(E)SI at dword at address ES:(E)DI; For 64-bit mode compare dword at address (R|E)SI at dword at address (R|E)DI. The status flags are set accordingly. */
  void cmps(const M32& arg0, const M32& arg1);

  /** Compares quadword at address (R|E)SI with quadword at address (R|E)DI and sets the status flags accordingly. */
  void cmps(const M64& arg0, const M64& arg1);

  /** For legacy mode, compare byte at address DS:(E)SI with byte at address ES:(E)DI; For 64-bit mode compare byte at address (R|E)SI to byte at address (R|E)DI. The status flags are set accordingly. */
  void cmps(const M8& arg0, const M8& arg1);

  /** For legacy mode, compare byte at address DS:(E)SI with byte at address ES:(E)DI; For 64- bit mode compare byte at address (R|E)SI with byte at address (R|E)DI. The status flags are set accordingly. */
  void cmpsb();

  /** For legacy mode, compare dword at address DS:(E)SI with dword at address ES:(E)DI; For 64-bit mode compare dword at address (R|E)SI with dword at address (R|E)DI. The status flags are set accordingly. */
  void cmpsd();

  /** Compare low double-precision floating-point value in xmm2/m64 and xmm1 using imm8 as comparison predicate. */
  void cmpsd(const Xmm& arg0, const M64& arg1, const Imm8& arg2);

  /** Compare low double-precision floating-point value in xmm2/m64 and xmm1 using imm8 as comparison predicate. */
  void cmpsd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Compares quadword at address (R|E)SI with quadword at address (R|E)DI and sets the status flags accordingly. */
  void cmpsq();

  /** Compare low single-precision floating-point value in xmm2/m32 and xmm1 using imm8 as comparison predicate. */
  void cmpss(const Xmm& arg0, const M32& arg1, const Imm8& arg2);

  /** Compare low single-precision floating-point value in xmm2/m32 and xmm1 using imm8 as comparison predicate. */
  void cmpss(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** For legacy mode, compare word at address DS:(E)SI with word at address ES:(E)DI; For 64- bit mode compare word at address (R|E)SI with word at address (R|E)DI. The status flags are set accordingly. */
  void cmpsw();

  /** Compare AX with r/m16. If equal, ZF is set and r16 is loaded into r/m16. Else, clear ZF and load r/m16 into AX. */
  void cmpxchg(const M16& arg0, const R16& arg1);

  /** Compare EAX with r/m32. If equal, ZF is set and r32 is loaded into r/m32. Else, clear ZF and load r/m32 into EAX. */
  void cmpxchg(const M32& arg0, const R32& arg1);

  /** Compare RAX with r/m64. If equal, ZF is set and r64 is loaded into r/m64. Else, clear ZF and load r/m64 into RAX. */
  void cmpxchg(const M64& arg0, const R64& arg1);

  /** Compare AL with r/m8. If equal, ZF is set and r8 is loaded into r/m8. Else, clear ZF and load r/m8 into AL. */
  void cmpxchg(const M8& arg0, const R8& arg1);

  /** Compare AL with r/m8. If equal, ZF is set and r8 is loaded into r/m8. Else, clear ZF and load r/m8 into AL. */
  void cmpxchg(const M8& arg0, const Rh& arg1);

  /** Compare AX with r/m16. If equal, ZF is set and r16 is loaded into r/m16. Else, clear ZF and load r/m16 into AX. */
  void cmpxchg(const R16& arg0, const R16& arg1);

  /** Compare EAX with r/m32. If equal, ZF is set and r32 is loaded into r/m32. Else, clear ZF and load r/m32 into EAX. */
  void cmpxchg(const R32& arg0, const R32& arg1);

  /** Compare RAX with r/m64. If equal, ZF is set and r64 is loaded into r/m64. Else, clear ZF and load r/m64 into RAX. */
  void cmpxchg(const R64& arg0, const R64& arg1);

  /** Compare AL with r/m8. If equal, ZF is set and r8 is loaded into r/m8. Else, clear ZF and load r/m8 into AL. */
  void cmpxchg(const R8& arg0, const R8& arg1);

  /** Compare AL with r/m8. If equal, ZF is set and r8 is loaded into r/m8. Else, clear ZF and load r/m8 into AL. */
  void cmpxchg(const R8& arg0, const Rh& arg1);

  /** Compare AL with r/m8. If equal, ZF is set and r8 is loaded into r/m8. Else, clear ZF and load r/m8 into AL. */
  void cmpxchg(const Rh& arg0, const R8& arg1);

  /** Compare AL with r/m8. If equal, ZF is set and r8 is loaded into r/m8. Else, clear ZF and load r/m8 into AL. */
  void cmpxchg(const Rh& arg0, const Rh& arg1);

  /** Compare RDX:RAX with m128. If equal, set ZF and load RCX:RBX into m128. Else, clear ZF and load m128 into RDX:RAX. */
  void cmpxchg16b(const M128& arg0);

  /** Compare EDX:EAX with m64. If equal, set ZF and load ECX:EBX into m64. Else, clear ZF and load m64 into EDX:EAX. */
  void cmpxchg8b(const M64& arg0);

  /** Compare low double-precision floating-point values in xmm1 and xmm2/mem64 and set the EFLAGS flags accordingly. */
  void comisd(const Xmm& arg0, const M64& arg1);

  /** Compare low double-precision floating-point values in xmm1 and xmm2/mem64 and set the EFLAGS flags accordingly. */
  void comisd(const Xmm& arg0, const Xmm& arg1);

  /** Compare low single-precision floating-point values in xmm1 and xmm2/mem32 and set the EFLAGS flags accordingly. */
  void comiss(const Xmm& arg0, const M32& arg1);

  /** Compare low single-precision floating-point values in xmm1 and xmm2/mem32 and set the EFLAGS flags accordingly. */
  void comiss(const Xmm& arg0, const Xmm& arg1);

  /** Returns processor identification and feature information to the EAX, EBX, ECX, and EDX registers, as determined by input entered in EAX (in some cases, ECX as well). */
  void cpuid();

  /** RDX:RAX = sign-extend of RAX. */
  void cqo();

  /** Accumulate CRC32 on r/m16. */
  void crc32(const R32& arg0, const M16& arg1);

  /** Accumulate CRC32 on r/m32. */
  void crc32(const R32& arg0, const M32& arg1);

  /** Accumulate CRC32 on r/m8. */
  void crc32(const R32& arg0, const M8& arg1);

  /** Accumulate CRC32 on r/m16. */
  void crc32(const R32& arg0, const R16& arg1);

  /** Accumulate CRC32 on r/m32. */
  void crc32(const R32& arg0, const R32& arg1);

  /** Accumulate CRC32 on r/m8. */
  void crc32(const R32& arg0, const R8& arg1);

  /** Accumulate CRC32 on r/m8. */
  void crc32(const R32& arg0, const Rh& arg1);

  /** Accumulate CRC32 on r/m64. */
  void crc32(const R64& arg0, const M64& arg1);

  /** Accumulate CRC32 on r/m8. */
  void crc32(const R64& arg0, const M8& arg1);

  /** Accumulate CRC32 on r/m64. */
  void crc32(const R64& arg0, const R64& arg1);

  /** Accumulate CRC32 on r/m8. */
  void crc32(const R64& arg0, const R8& arg1);

  /** Convert two packed signed doubleword integers from xmm2/m128 to two packed double-precision floating-point values in xmm1. */
  void cvtdq2pd(const Xmm& arg0, const M64& arg1);

  /** Convert two packed signed doubleword integers from xmm2/m128 to two packed double-precision floating-point values in xmm1. */
  void cvtdq2pd(const Xmm& arg0, const Xmm& arg1);

  /** Convert four packed signed doubleword integers from xmm2/m128 to four packed single-precision floating-point values in xmm1. */
  void cvtdq2ps(const Xmm& arg0, const M128& arg1);

  /** Convert four packed signed doubleword integers from xmm2/m128 to four packed single-precision floating-point values in xmm1. */
  void cvtdq2ps(const Xmm& arg0, const Xmm& arg1);

  /** Convert two packed double-precision floating- point values from xmm2/m128 to two packed signed doubleword integers in xmm1. */
  void cvtpd2dq(const Xmm& arg0, const M128& arg1);

  /** Convert two packed double-precision floating- point values from xmm2/m128 to two packed signed doubleword integers in xmm1. */
  void cvtpd2dq(const Xmm& arg0, const Xmm& arg1);

  /** Convert two packed double-precision floating- point values from xmm/m128 to two packed signed doubleword integers in mm. */
  void cvtpd2pi(const Mm& arg0, const M128& arg1);

  /** Convert two packed double-precision floating- point values from xmm/m128 to two packed signed doubleword integers in mm. */
  void cvtpd2pi(const Mm& arg0, const Xmm& arg1);

  /** Convert two packed double-precision floating- point values in xmm2/m128 to two packed single-precision floating-point values in xmm1. */
  void cvtpd2ps(const Xmm& arg0, const M128& arg1);

  /** Convert two packed double-precision floating- point values in xmm2/m128 to two packed single-precision floating-point values in xmm1. */
  void cvtpd2ps(const Xmm& arg0, const Xmm& arg1);

  /** Convert two packed signed doubleword integers from mm/mem64 to two packed double-precision floating-point values in xmm. */
  void cvtpi2pd(const Xmm& arg0, const M64& arg1);

  /** Convert two packed signed doubleword integers from mm/mem64 to two packed double-precision floating-point values in xmm. */
  void cvtpi2pd(const Xmm& arg0, const Mm& arg1);

  /** Convert two signed doubleword integers from mm/m64 to two single-precision floating-point values in xmm. */
  void cvtpi2ps(const Xmm& arg0, const M64& arg1);

  /** Convert two signed doubleword integers from mm/m64 to two single-precision floating-point values in xmm. */
  void cvtpi2ps(const Xmm& arg0, const Mm& arg1);

  /** Convert four packed single-precision floating- point values from xmm2/m128 to four packed signed doubleword integers in xmm1. */
  void cvtps2dq(const Xmm& arg0, const M128& arg1);

  /** Convert four packed single-precision floating- point values from xmm2/m128 to four packed signed doubleword integers in xmm1. */
  void cvtps2dq(const Xmm& arg0, const Xmm& arg1);

  /** Convert two packed single-precision floating- point values in xmm2/m64 to two packed double-precision floating-point values in xmm1. */
  void cvtps2pd(const Xmm& arg0, const M64& arg1);

  /** Convert two packed single-precision floating- point values in xmm2/m64 to two packed double-precision floating-point values in xmm1. */
  void cvtps2pd(const Xmm& arg0, const Xmm& arg1);

  /** Convert two packed single-precision floating- point values from xmm/m64 to two packed signed doubleword integers in mm. */
  void cvtps2pi(const Mm& arg0, const M64& arg1);

  /** Convert two packed single-precision floating- point values from xmm/m64 to two packed signed doubleword integers in mm. */
  void cvtps2pi(const Mm& arg0, const Xmm& arg1);

  /** Convert one double-precision floating-point value from xmm/m64 to one signed doubleword integer r32. */
  void cvtsd2si(const R32& arg0, const M64& arg1);

  /** Convert one double-precision floating-point value from xmm/m64 to one signed doubleword integer r32. */
  void cvtsd2si(const R32& arg0, const Xmm& arg1);

  /** Convert one double-precision floating-point value from xmm/m64 to one signed quadword integer sign-extended into r64. */
  void cvtsd2si(const R64& arg0, const M64& arg1);

  /** Convert one double-precision floating-point value from xmm/m64 to one signed quadword integer sign-extended into r64. */
  void cvtsd2si(const R64& arg0, const Xmm& arg1);

  /** Convert one double-precision floating-point value in xmm2/m64 to one single-precision floating-point value in xmm1. */
  void cvtsd2ss(const Xmm& arg0, const M64& arg1);

  /** Convert one double-precision floating-point value in xmm2/m64 to one single-precision floating-point value in xmm1. */
  void cvtsd2ss(const Xmm& arg0, const Xmm& arg1);

  /** Convert one signed doubleword integer from r/m32 to one double-precision floating-point value in xmm. */
  void cvtsi2sd(const Xmm& arg0, const M32& arg1);

  /** Convert one signed quadword integer from r/m64 to one double-precision floating-point value in xmm. */
  void cvtsi2sd(const Xmm& arg0, const M64& arg1);

  /** Convert one signed doubleword integer from r/m32 to one double-precision floating-point value in xmm. */
  void cvtsi2sd(const Xmm& arg0, const R32& arg1);

  /** Convert one signed quadword integer from r/m64 to one double-precision floating-point value in xmm. */
  void cvtsi2sd(const Xmm& arg0, const R64& arg1);

  /** Convert one signed doubleword integer from r/m32 to one single-precision floating-point value in xmm. */
  void cvtsi2ss(const Xmm& arg0, const M32& arg1);

  /** Convert one signed quadword integer from r/m64 to one single-precision floating-point value in xmm. */
  void cvtsi2ss(const Xmm& arg0, const M64& arg1);

  /** Convert one signed doubleword integer from r/m32 to one single-precision floating-point value in xmm. */
  void cvtsi2ss(const Xmm& arg0, const R32& arg1);

  /** Convert one signed quadword integer from r/m64 to one single-precision floating-point value in xmm. */
  void cvtsi2ss(const Xmm& arg0, const R64& arg1);

  /** Convert one single-precision floating-point value in xmm2/m32 to one double-precision floating-point value in xmm1. */
  void cvtss2sd(const Xmm& arg0, const M32& arg1);

  /** Convert one single-precision floating-point value in xmm2/m32 to one double-precision floating-point value in xmm1. */
  void cvtss2sd(const Xmm& arg0, const Xmm& arg1);

  /** Convert one single-precision floating-point value from xmm/m32 to one signed doubleword integer in r32. */
  void cvtss2si(const R32& arg0, const M32& arg1);

  /** Convert one single-precision floating-point value from xmm/m32 to one signed doubleword integer in r32. */
  void cvtss2si(const R32& arg0, const Xmm& arg1);

  /** Convert one single-precision floating-point value from xmm/m32 to one signed quadword integer in r64. */
  void cvtss2si(const R64& arg0, const M32& arg1);

  /** Convert one single-precision floating-point value from xmm/m32 to one signed quadword integer in r64. */
  void cvtss2si(const R64& arg0, const Xmm& arg1);

  /** Convert two packed double-precision floating- point values from xmm2/m128 to two packed signed doubleword integers in xmm1 using truncation. */
  void cvttpd2dq(const Xmm& arg0, const M128& arg1);

  /** Convert two packed double-precision floating- point values from xmm2/m128 to two packed signed doubleword integers in xmm1 using truncation. */
  void cvttpd2dq(const Xmm& arg0, const Xmm& arg1);

  /** Convert two packer double-precision floating- point values from xmm/m128 to two packed signed doubleword integers in mm using truncation. */
  void cvttpd2pi(const Mm& arg0, const M128& arg1);

  /** Convert two packer double-precision floating- point values from xmm/m128 to two packed signed doubleword integers in mm using truncation. */
  void cvttpd2pi(const Mm& arg0, const Xmm& arg1);

  /** Convert four single-precision floating-point values from xmm2/m128 to four signed doubleword integers in xmm1 using truncation. */
  void cvttps2dq(const Xmm& arg0, const M128& arg1);

  /** Convert four single-precision floating-point values from xmm2/m128 to four signed doubleword integers in xmm1 using truncation. */
  void cvttps2dq(const Xmm& arg0, const Xmm& arg1);

  /** Convert two single-precision floating-point values from xmm/m64 to two signed doubleword signed integers in mm using truncation. */
  void cvttps2pi(const Mm& arg0, const M64& arg1);

  /** Convert two single-precision floating-point values from xmm/m64 to two signed doubleword signed integers in mm using truncation. */
  void cvttps2pi(const Mm& arg0, const Xmm& arg1);

  /** Convert one double-precision floating-point value from xmm/m64 to one signed doubleword integer in r32 using truncation. */
  void cvttsd2si(const R32& arg0, const M64& arg1);

  /** Convert one double-precision floating-point value from xmm/m64 to one signed doubleword integer in r32 using truncation. */
  void cvttsd2si(const R32& arg0, const Xmm& arg1);

  /** Convert one double precision floating-point value from xmm/m64 to one signedquadword integer in r64 using truncation. */
  void cvttsd2si(const R64& arg0, const M64& arg1);

  /** Convert one double precision floating-point value from xmm/m64 to one signedquadword integer in r64 using truncation. */
  void cvttsd2si(const R64& arg0, const Xmm& arg1);

  /** Convert one single-precision floating-point value from xmm/m32 to one signed doubleword integer in r32 using truncation. */
  void cvttss2si(const R32& arg0, const M32& arg1);

  /** Convert one single-precision floating-point value from xmm/m32 to one signed doubleword integer in r32 using truncation. */
  void cvttss2si(const R32& arg0, const Xmm& arg1);

  /** Convert one single-precision floating-point value from xmm/m32 to one signed quadword integer in r64 using truncation. */
  void cvttss2si(const R64& arg0, const M32& arg1);

  /** Convert one single-precision floating-point value from xmm/m32 to one signed quadword integer in r64 using truncation. */
  void cvttss2si(const R64& arg0, const Xmm& arg1);

  /** DX:AX = sign-extend of AX. */
  void cwd();

  /** EAX = sign-extend of AX. */
  void cwde();

  /** Decrement r/m16 by 1. */
  void dec(const M16& arg0);

  /** Decrement r/m32 by 1. */
  void dec(const M32& arg0);

  /** Decrement r/m64 by 1. */
  void dec(const M64& arg0);

  /** Decrement r/m8 by 1. */
  void dec(const M8& arg0);

  /** Decrement r/m16 by 1. */
  void dec(const R16& arg0);

  /** Decrement r/m32 by 1. */
  void dec(const R32& arg0);

  /** Decrement r/m64 by 1. */
  void dec(const R64& arg0);

  /** Decrement r/m8 by 1. */
  void dec(const R8& arg0);

  /** Decrement r/m8 by 1. */
  void dec(const Rh& arg0);

  /** Unsigned divide DX:AX by r/m16, with result stored in AX = Quotient, DX = Remainder. */
  void div(const M16& arg0);

  /** Unsigned divide EDX:EAX by r/m32, with result stored in EAX = Quotient, EDX = Remainder. */
  void div(const M32& arg0);

  /** Unsigned divide RDX:RAX by r/m64, with result stored in RAX = Quotient, RDX = Remainder. */
  void div(const M64& arg0);

  /** Unsigned divide AX by r/m8, with result stored in AL = Quotient, AH = Remainder. */
  void div(const M8& arg0);

  /** Unsigned divide DX:AX by r/m16, with result stored in AX = Quotient, DX = Remainder. */
  void div(const R16& arg0);

  /** Unsigned divide EDX:EAX by r/m32, with result stored in EAX = Quotient, EDX = Remainder. */
  void div(const R32& arg0);

  /** Unsigned divide RDX:RAX by r/m64, with result stored in RAX = Quotient, RDX = Remainder. */
  void div(const R64& arg0);

  /** Unsigned divide AX by r/m8, with result stored in AL = Quotient, AH = Remainder. */
  void div(const R8& arg0);

  /** Unsigned divide AX by r/m8, with result stored in AL = Quotient, AH = Remainder. */
  void div(const Rh& arg0);

  /** Divide packed double-precision floating-point values in xmm1 by packed double-precision floating-point values xmm2/m128. */
  void divpd(const Xmm& arg0, const M128& arg1);

  /** Divide packed double-precision floating-point values in xmm1 by packed double-precision floating-point values xmm2/m128. */
  void divpd(const Xmm& arg0, const Xmm& arg1);

  /** Divide packed single-precision floating-point values in xmm1 by packed single-precision floating-point values xmm2/m128. */
  void divps(const Xmm& arg0, const M128& arg1);

  /** Divide packed single-precision floating-point values in xmm1 by packed single-precision floating-point values xmm2/m128. */
  void divps(const Xmm& arg0, const Xmm& arg1);

  /** Divide low double-precision floating-point value in xmm1 by low double-precision floating-point value in xmm2/mem64. */
  void divsd(const Xmm& arg0, const M64& arg1);

  /** Divide low double-precision floating-point value in xmm1 by low double-precision floating-point value in xmm2/mem64. */
  void divsd(const Xmm& arg0, const Xmm& arg1);

  /** Divide low single-precision floating-point value in xmm1 by low single-precision floating-point value in xmm2/m32. */
  void divss(const Xmm& arg0, const M32& arg1);

  /** Divide low single-precision floating-point value in xmm1 by low single-precision floating-point value in xmm2/m32. */
  void divss(const Xmm& arg0, const Xmm& arg1);

  /** Selectively multiply packed DP floating-point values from xmm1 with packed DP floating- point values from xmm2, add and selectively store the packed DP floating-point values to xmm1. */
  void dppd(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Selectively multiply packed DP floating-point values from xmm1 with packed DP floating- point values from xmm2, add and selectively store the packed DP floating-point values to xmm1. */
  void dppd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Selectively multiply packed SP floating-point values from xmm1 with packed SP floating- point values from xmm2, add and selectively store the packed SP floating-point values or zero values to xmm1. */
  void dpps(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Selectively multiply packed SP floating-point values from xmm1 with packed SP floating- point values from xmm2, add and selectively store the packed SP floating-point values or zero values to xmm1. */
  void dpps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Set the x87 FPU tag word to empty. */
  void emms();

  /** Create a nested stack frame for a procedure. */
  void enter(const Imm8& arg0, const Imm16& arg1);

  /** Create a nested stack frame for a procedure. */
  void enter(const One& arg0, const Imm16& arg1);

  /** Create a stack frame for a procedure. */
  void enter(const Zero& arg0, const Imm16& arg1);

  /** Extract a single-precision floating-point value from xmm2 at the source offset specified by imm8 and store the result to reg or m32. The upper 32 bits of r64 is zeroed if reg is r64. */
  void extractps(const M32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a single-precision floating-point value from xmm2 at the source offset specified by imm8 and store the result to reg or m32. The upper 32 bits of r64 is zeroed if reg is r64. */
  void extractps(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a single-precision floating-point value from xmm2 at the source offset specified by imm8 and store the result to reg or m32. The upper 32 bits of r64 is zeroed if reg is r64. */
  void extractps(const R64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Replace ST(0) with (2^(ST(0)) - 1). */
  void f2xm1();

  /** Replace ST with its absolute value. */
  void fabs();

  /** Add m32fp to ST(0) and store result in ST(0). */
  void fadd(const M32Fp& arg0);

  /** Add m64fp to ST(0) and store result in ST(0). */
  void fadd(const M64Fp& arg0);

  /** Add ST(i) to ST(0) and store result in ST(i). */
  void fadd(const St& arg0, const St0& arg1);

  /** Add ST(0) to ST(i) and store result in ST(0). */
  void fadd(const St0& arg0, const St& arg1);

  /** Add ST(0) to ST(1), store result in ST(1), and pop the register stack. */
  void faddp();

  /** Add ST(0) to ST(i), store result in ST(i), and pop the register stack. */
  void faddp(const St& arg0, const St0& arg1);

  /** Convert BCD value to floating-point and push onto the FPU stack. */
  void fbld(const M80Bcd& arg0);

  /** Store ST(0) in m80bcd and pop ST(0). */
  void fbstp(const M80Bcd& arg0);

  /** Complements sign of ST(0). */
  void fchs();

  /** Clear floating-point exception flags after checking for pending unmasked floating-point exceptions. */
  void fclex();

  /** Move if below (CF=1). */
  void fcmovb(const St0& arg0, const St& arg1);

  /** Move if below or equal (CF=1 or ZF=1). */
  void fcmovbe(const St0& arg0, const St& arg1);

  /** Move if equal (ZF=1). */
  void fcmove(const St0& arg0, const St& arg1);

  /** Move if not below (CF=0). */
  void fcmovnb(const St0& arg0, const St& arg1);

  /** Move if not below or equal (CF=0 and ZF=0). */
  void fcmovnbe(const St0& arg0, const St& arg1);

  /** Move if not equal (ZF=0). */
  void fcmovne(const St0& arg0, const St& arg1);

  /** Move if not unordered (PF=0). */
  void fcmovnu(const St0& arg0, const St& arg1);

  /** Move if unordered (PF=1). */
  void fcmovu(const St0& arg0, const St& arg1);

  /** Compare ST(0) with ST(1). */
  void fcom();

  /** Compare ST(0) with m32fp. */
  void fcom(const M32Fp& arg0);

  /** Compare ST(0) with m64fp. */
  void fcom(const M64Fp& arg0);

  /** Compare ST(0) with ST(i). */
  void fcom(const St& arg0);

  /** Compare ST(0) with ST(i) and set status flags accordingly. */
  void fcomi(const St0& arg0, const St& arg1);

  /** Compare ST(0) with ST(i), set status flags accordingly, and pop register stack. */
  void fcomip(const St0& arg0, const St& arg1);

  /** Compare ST(0) with ST(1) and pop register stack. */
  void fcomp();

  /** Compare ST(0) with m32fp and pop register stack. */
  void fcomp(const M32Fp& arg0);

  /** Compare ST(0) with m64fp and pop register stack. */
  void fcomp(const M64Fp& arg0);

  /** Compare ST(0) with ST(i) and pop register stack. */
  void fcomp(const St& arg0);

  /** Compare ST(0) with ST(1) and pop register stack twice. */
  void fcompp();

  /** Replace ST(0) with its cosine. */
  void fcos();

  /** Decrement TOP field in FPU status word. */
  void fdecstp();

  /** Divide ST(0) by m32fp and store result in ST(0). */
  void fdiv(const M32Fp& arg0);

  /** Compare ST(0) with ST(i), set status flags accordingly, and pop register stack. */
  void fdiv(const M64Fp& arg0);

  /** Divide ST(i) by ST(0) and store result in ST(i). */
  void fdiv(const St& arg0, const St0& arg1);

  /** Divide ST(0) by ST(i) and store result in ST(0). */
  void fdiv(const St0& arg0, const St& arg1);

  /** Divide ST(1) by ST(0), store result in ST(1), and pop the register stack. */
  void fdivp();

  /** Divide ST(i) by ST(0), store result in ST(i), and pop the register stack. */
  void fdivp(const St& arg0, const St0& arg1);

  /** Divide m32fp by ST(0) and store result in ST(0). */
  void fdivr(const M32Fp& arg0);

  /** Divide m64fp by ST(0) and store result in ST(0). */
  void fdivr(const M64Fp& arg0);

  /** Divide ST(0) by ST(i) and store result in ST(i). */
  void fdivr(const St& arg0, const St0& arg1);

  /** Divide ST(i) by ST(0) and store result in ST(0). */
  void fdivr(const St0& arg0, const St& arg1);

  /** Divide ST(0) by ST(1), store result in ST(1), and pop the register stack. */
  void fdivrp();

  /** Divide ST(0) by ST(i), store result in ST(i), and pop the register stack. */
  void fdivrp(const St& arg0, const St0& arg1);

  /** Sets tag for ST(i) to empty. */
  void ffree(const St& arg0);

  /** Add m16int to ST(0) and store result in ST(0). */
  void fiadd(const M16Int& arg0);

  /** Add m32int to ST(0) and store result in ST(0). */
  void fiadd(const M32Int& arg0);

  /** Compare ST(0) with m16int. */
  void ficom(const M16Int& arg0);

  /** Compare ST(0) with m32int. */
  void ficom(const M32Int& arg0);

  /** Compare ST(0) with m16int and pop stack register. */
  void ficomp(const M16Int& arg0);

  /** Compare ST(0) with m32int and pop stack register. */
  void ficomp(const M32Int& arg0);

  /** Divide ST(0) by m64int and store result in ST(0). */
  void fidiv(const M16Int& arg0);

  /** Divide ST(0) by m32int and store result in ST(0). */
  void fidiv(const M32Int& arg0);

  /** Divide m16int by ST(0) and store result in ST(0). */
  void fidivr(const M16Int& arg0);

  /** Divide m32int by ST(0) and store result in ST(0). */
  void fidivr(const M32Int& arg0);

  /** Push m16int onto the FPU register stack. */
  void fild(const M16Int& arg0);

  /** Push m32int onto the FPU register stack. */
  void fild(const M32Int& arg0);

  /** Push m64int onto the FPU register stack. */
  void fild(const M64Int& arg0);

  /** Multiply ST(0) by m16int and store result in ST(0). */
  void fimul(const M16Int& arg0);

  /** Multiply ST(0) by m32int and store result in ST(0). */
  void fimul(const M32Int& arg0);

  /** Increment the TOP field in the FPU status register. */
  void fincstp();

  /** Initialize FPU after checking for pending unmasked floating-point exceptions. */
  void finit();

  /** Store ST(0) in m16int. */
  void fist(const M16Int& arg0);

  /** Store ST(0) in m32int. */
  void fist(const M32Int& arg0);

  /** Store ST(0) in m16int and pop register stack. */
  void fistp(const M16Int& arg0);

  /** Store ST(0) in m32int and pop register stack. */
  void fistp(const M32Int& arg0);

  /** Store ST(0) in m64int and pop register stack. */
  void fistp(const M64Int& arg0);

  /** Store ST(0) in m16int with truncation. */
  void fisttp(const M16Int& arg0);

  /** Store ST(0) in m32int with truncation. */
  void fisttp(const M32Int& arg0);

  /** Store ST(0) in m64int with truncation. */
  void fisttp(const M64Int& arg0);

  /** Subtract m16int from ST(0) and store result in ST(0). */
  void fisub(const M16Int& arg0);

  /** Subtract m32int from ST(0) and store result in ST(0). */
  void fisub(const M32Int& arg0);

  /** Subtract ST(0) from m16int and store result in ST(0). */
  void fisubr(const M16Int& arg0);

  /** Subtract ST(0) from m32int and store result in ST(0). */
  void fisubr(const M32Int& arg0);

  /** Push m32fp onto the FPU register stack. */
  void fld(const M32Fp& arg0);

  /** Push m64fp onto the FPU register stack. */
  void fld(const M64Fp& arg0);

  /** Push m80fp onto the FPU register stack. */
  void fld(const M80Fp& arg0);

  /** Push ST(i) onto the FPU register stack. */
  void fld(const St& arg0);

  /** Push +1.0 onto the FPU register stack. */
  void fld1();

  /** Load FPU control word from m2byte. */
  void fldcw(const M2Byte& arg0);

  /** Load FPU environment from m14byte or m28byte. */
  void fldenv(const M28Byte& arg0);

  /** Push log2e onto the FPU register stack. */
  void fldl2e();

  /** Push log210 onto the FPU register stack. */
  void fldl2t();

  /** Push log102 onto the FPU register stack. */
  void fldlg2();

  /** Push loge2 onto the FPU register stack. */
  void fldln2();

  /** Push pi onto the FPU register stack. */
  void fldpi();

  /** Push +0.0 onto the FPU register stack. */
  void fldz();

  /** Multiply ST(0) by m32fp and store result in ST(0). */
  void fmul(const M32Fp& arg0);

  /** Multiply ST(0) by m64fp and store result in ST(0). */
  void fmul(const M64Fp& arg0);

  /** Multiply ST(i) by ST(0) and store result in ST(i). */
  void fmul(const St& arg0, const St0& arg1);

  /** Multiply ST(0) by ST(i) and store result in ST(0). */
  void fmul(const St0& arg0, const St& arg1);

  /** Multiply ST(1) by ST(0), store result in ST(1), and pop the register stack. */
  void fmulp();

  /** Multiply ST(i) by ST(0), store result in ST(i), and pop the register stack. */
  void fmulp(const St& arg0, const St0& arg1);

  /** Clear floating-point exception flags without checking for pending unmasked floating-point exceptions. */
  void fnclex();

  /** Initialize FPU without checking for pending unmasked floating-point exceptions. */
  void fninit();

  /** No operation is performed. */
  void fnop();

  /** Store FPU environment to m94byte or m108byte without checking for pending unmasked floating-point exceptions. Then re-initialize the FPU. */
  void fnsave(const M108Byte& arg0);

  /** Store FPU control word to m2byte without checking for pending unmasked floating-point exceptions. */
  void fnstcw(const M2Byte& arg0);

  /** Store FPU environment to m14byte or m28byte without checking for pending unmasked floating-point exceptions. Then mask all floating-point exceptions. */
  void fnstenv(const M28Byte& arg0);

  /** Store FPU status word in AX register without checking for pending unmasked floating-point exceptions. */
  void fnstsw(const Ax& arg0);

  /** Store FPU status word at m2byte without checking for pending unmasked floating-point exceptions. */
  void fnstsw(const M2Byte& arg0);

  /** Replace ST(1) with arctan(ST(1)/ST(0)) and pop the register stack. */
  void fpatan();

  /** Replace ST(0) with the remainder obtained from dividing ST(0) by ST(1). */
  void fprem();

  /** Replace ST(0) with the IEEE remainder obtained from dividing ST(0) by ST(1). */
  void fprem1();

  /** Replace ST(0) with its tangent and push 1 onto the FPU stack. */
  void fptan();

  /** Round ST(0) to an integer. */
  void frndint();

  /** Load FPU state from m94byte or m108byte. */
  void frstor(const M108Byte& arg0);

  /** Store FPU state to m94byte or m108byte after checking for pending unmasked floating-point exceptions. Then re-initialize the FPU. */
  void fsave(const M108Byte& arg0);

  /** Scale ST(0) by ST(1). */
  void fscale();

  /** Replace ST(0) with its sine. */
  void fsin();

  /** Compute the sine and cosine of ST(0); replace ST(0) with the sine, and push the cosine onto the register stack. */
  void fsincos();

  /** Computes square root of ST(0) and stores the result in ST(0). */
  void fsqrt();

  /** Copy ST(0) to m32fp. */
  void fst(const M32Fp& arg0);

  /** Copy ST(0) to m64fp. */
  void fst(const M64Fp& arg0);

  /** Copy ST(0) to ST(i). */
  void fst(const St& arg0);

  /** Store FPU control word to m2byte after checking for pending unmasked floating-point exceptions. */
  void fstcw(const M2Byte& arg0);

  /** Store FPU environment to m14byte or m28byte after checking for pending unmasked floating-point exceptions. Then mask all floating-point exceptions. */
  void fstenv(const M28Byte& arg0);

  /** Copy ST(0) to m32fp and pop register stack. */
  void fstp(const M32Fp& arg0);

  /** Copy ST(0) to m64fp and pop register stack. */
  void fstp(const M64Fp& arg0);

  /** Copy ST(0) to m80fp and pop register stack. */
  void fstp(const M80Fp& arg0);

  /** Copy ST(0) to ST(i) and pop register stack. */
  void fstp(const St& arg0);

  /** Store FPU status word in AX register after checking for pending unmasked floating-point exceptions. */
  void fstsw(const Ax& arg0);

  /** Store FPU status word at m2byte after checking for pending unmasked floating-point exceptions. */
  void fstsw(const M2Byte& arg0);

  /** Subtract m32fp from ST(0) and store result in ST(0). */
  void fsub(const M32Fp& arg0);

  /** Subtract m64fp from ST(0) and store result in ST(0). */
  void fsub(const M64Fp& arg0);

  /** Subtract ST(0) from ST(i) and store result in ST(i). */
  void fsub(const St& arg0, const St0& arg1);

  /** Subtract ST(i) from ST(0) and store result in ST(0). */
  void fsub(const St0& arg0, const St& arg1);

  /** Subtract ST(0) from ST(1), store result in ST(1), and pop register stack. */
  void fsubp();

  /** Subtract ST(0) from ST(i), store result in ST(i), and pop register stack. */
  void fsubp(const St& arg0, const St0& arg1);

  /** Subtract ST(0) from m32fp and store result in ST(0). */
  void fsubr(const M32Fp& arg0);

  /** Subtract ST(0) from m64fp and store result in ST(0). */
  void fsubr(const M64Fp& arg0);

  /** Subtract ST(i) from ST(0) and store result in ST(i). */
  void fsubr(const St& arg0, const St0& arg1);

  /** Subtract ST(0) from ST(i) and store result in ST(0). */
  void fsubr(const St0& arg0, const St& arg1);

  /** Subtract ST(1) from ST(0), store result in ST(1), and pop register stack. */
  void fsubrp();

  /** Subtract ST(i) from ST(0), store result in ST(i), and pop register stack. */
  void fsubrp(const St& arg0, const St0& arg1);

  /** Compare ST(0) with 0.0. */
  void ftst();

  /** Compare ST(0) with ST(1). */
  void fucom();

  /** Compare ST(0) with ST(i). */
  void fucom(const St& arg0);

  /** Compare ST(0) with ST(i), check for ordered values, and set status flags accordingly. */
  void fucomi(const St0& arg0, const St& arg1);

  /** Compare ST(0) with ST(i), check for ordered values, set status flags accordingly, and pop register stack. */
  void fucomip(const St0& arg0, const St& arg1);

  /** Compare ST(0) with ST(1) and pop register stack. */
  void fucomp();

  /** Compare ST(0) with ST(i) and pop register stack. */
  void fucomp(const St& arg0);

  /** Compare ST(0) with ST(1) and pop register stack twice. */
  void fucompp();

  /** Check pending unmasked floating-point exceptions. */
  void fwait();

  /** Classify value or number in ST(0). */
  void fxam();

  /** Exchange the contents of ST(0) and ST(1). */
  void fxch();

  /** Exchange the contents of ST(0) and ST(i). */
  void fxch(const St& arg0);

  /** Restore the x87 FPU, MMX, XMM, and MXCSR register state from m512byte. */
  void fxrstor(const M512Byte& arg0);

  /** Restore the x87 FPU, MMX, XMM, and MXCSR register state from m512byte. */
  void fxrstor64(const M512Byte& arg0);

  /** Save the x87 FPU, MMX, XMM, and MXCSR register state to m512byte. */
  void fxsave(const M512Byte& arg0);

  /** Save the x87 FPU, MMX, XMM, and MXCSR register state to m512byte. */
  void fxsave64(const M512Byte& arg0);

  /** Separate value in ST(0) into exponent and significand, store exponent in ST(0), and push the significand onto the register stack. */
  void fxtract();

  /** Replace ST(1) with (ST(1) * log2ST(0)) and pop the register stack. */
  void fyl2x();

  /** Replace ST(1) with ST(1) * log2(ST(0) + 1.0) and pop the register stack. */
  void fyl2xp1();

  /** Horizontal add packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void haddpd(const Xmm& arg0, const M128& arg1);

  /** Horizontal add packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void haddpd(const Xmm& arg0, const Xmm& arg1);

  /** Horizontal add packed single-precision floating-point values from xmm2/m128 to xmm1. */
  void haddps(const Xmm& arg0, const M128& arg1);

  /** Horizontal add packed single-precision floating-point values from xmm2/m128 to xmm1. */
  void haddps(const Xmm& arg0, const Xmm& arg1);

  /** Horizontal subtract packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void hsubpd(const Xmm& arg0, const M128& arg1);

  /** Horizontal subtract packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void hsubpd(const Xmm& arg0, const Xmm& arg1);

  /** Horizontal subtract packed single-precision floating-point values from xmm2/m128 to xmm1. */
  void hsubps(const Xmm& arg0, const M128& arg1);

  /** Horizontal subtract packed single-precision floating-point values from xmm2/m128 to xmm1. */
  void hsubps(const Xmm& arg0, const Xmm& arg1);

  /** Signed divide DX:AX by r/m16, with result stored in AX = Quotient, DX = Remainder. */
  void idiv(const M16& arg0);

  /** Signed divide EDX:EAX by r/m32, with result stored in EAX = Quotient, EDX = Remainder. */
  void idiv(const M32& arg0);

  /** Signed divide RDX:RAX by r/m64, with result stored in RAX = Quotient, RDX = Remainder. */
  void idiv(const M64& arg0);

  /** Signed divide AX by r/m8, with result stored in: AL = Quotient, AH = Remainder. */
  void idiv(const M8& arg0);

  /** Signed divide DX:AX by r/m16, with result stored in AX = Quotient, DX = Remainder. */
  void idiv(const R16& arg0);

  /** Signed divide EDX:EAX by r/m32, with result stored in EAX = Quotient, EDX = Remainder. */
  void idiv(const R32& arg0);

  /** Signed divide RDX:RAX by r/m64, with result stored in RAX = Quotient, RDX = Remainder. */
  void idiv(const R64& arg0);

  /** Signed divide AX by r/m8, with result stored in: AL = Quotient, AH = Remainder. */
  void idiv(const R8& arg0);

  /** Signed divide AX by r/m8, with result stored in: AL = Quotient, AH = Remainder. */
  void idiv(const Rh& arg0);

  /** DX:AX = AX * r/m word. */
  void imul(const M16& arg0);

  /** EDX:EAX = EAX * r/m32. */
  void imul(const M32& arg0);

  /** RDX:RAX = RAX * r/m64. */
  void imul(const M64& arg0);

  /** AX= AL * r/m byte. */
  void imul(const M8& arg0);

  /** DX:AX = AX * r/m word. */
  void imul(const R16& arg0);

  /** word register = word register * r/m16. */
  void imul(const R16& arg0, const M16& arg1);

  /** word register = r/m16 * immediate word. */
  void imul(const R16& arg0, const M16& arg1, const Imm16& arg2);

  /** word register = r/m16 * sign-extended immediate byte. */
  void imul(const R16& arg0, const M16& arg1, const Imm8& arg2);

  /** word register = word register * r/m16. */
  void imul(const R16& arg0, const R16& arg1);

  /** word register = r/m16 * immediate word. */
  void imul(const R16& arg0, const R16& arg1, const Imm16& arg2);

  /** word register = r/m16 * sign-extended immediate byte. */
  void imul(const R16& arg0, const R16& arg1, const Imm8& arg2);

  /** EDX:EAX = EAX * r/m32. */
  void imul(const R32& arg0);

  /** doubleword register = doubleword register *  r/m32. */
  void imul(const R32& arg0, const M32& arg1);

  /** doubleword register = r/m32 * immediate doubleword. */
  void imul(const R32& arg0, const M32& arg1, const Imm32& arg2);

  /** doubleword register = r/m32 * sign- extended immediate byte. */
  void imul(const R32& arg0, const M32& arg1, const Imm8& arg2);

  /** doubleword register = doubleword register *  r/m32. */
  void imul(const R32& arg0, const R32& arg1);

  /** doubleword register = r/m32 * immediate doubleword. */
  void imul(const R32& arg0, const R32& arg1, const Imm32& arg2);

  /** doubleword register = r/m32 * sign- extended immediate byte. */
  void imul(const R32& arg0, const R32& arg1, const Imm8& arg2);

  /** RDX:RAX = RAX * r/m64. */
  void imul(const R64& arg0);

  /** Quadword register = Quadword register *  r/m64. */
  void imul(const R64& arg0, const M64& arg1);

  /** Quadword register = r/m64 * immediate doubleword. */
  void imul(const R64& arg0, const M64& arg1, const Imm32& arg2);

  /** Quadword register = r/m64 * sign-extended  immediate byte. */
  void imul(const R64& arg0, const M64& arg1, const Imm8& arg2);

  /** Quadword register = Quadword register *  r/m64. */
  void imul(const R64& arg0, const R64& arg1);

  /** Quadword register = r/m64 * immediate doubleword. */
  void imul(const R64& arg0, const R64& arg1, const Imm32& arg2);

  /** Quadword register = r/m64 * sign-extended  immediate byte. */
  void imul(const R64& arg0, const R64& arg1, const Imm8& arg2);

  /** AX= AL * r/m byte. */
  void imul(const R8& arg0);

  /** AX= AL * r/m byte. */
  void imul(const Rh& arg0);

  /** Input byte from I/O port in DX into AL. */
  void in(const Al& arg0, const Dx& arg1);

  /** Input byte from imm8 I/O port address into AL. */
  void in(const Al& arg0, const Imm8& arg1);

  /** Input word from I/O port in DX into AX. */
  void in(const Ax& arg0, const Dx& arg1);

  /** Input word from imm8 I/O port address into AX. */
  void in(const Ax& arg0, const Imm8& arg1);

  /** Input doubleword from I/O port in DX into EAX. */
  void in(const Eax& arg0, const Dx& arg1);

  /** Input dword from imm8 I/O port address into EAX. */
  void in(const Eax& arg0, const Imm8& arg1);

  /** Increment r/m word by 1. */
  void inc(const M16& arg0);

  /** Increment r/m doubleword by 1. */
  void inc(const M32& arg0);

  /** Increment r/m quadword by 1. */
  void inc(const M64& arg0);

  /** Increment r/m byte by 1. */
  void inc(const M8& arg0);

  /** Increment r/m word by 1. */
  void inc(const R16& arg0);

  /** Increment r/m doubleword by 1. */
  void inc(const R32& arg0);

  /** Increment r/m quadword by 1. */
  void inc(const R64& arg0);

  /** Increment r/m byte by 1. */
  void inc(const R8& arg0);

  /** Increment r/m byte by 1. */
  void inc(const Rh& arg0);

  /** Input word from I/O port specified in DX into memory location specified in ES:(E)DI or RDI. */
  void ins(const M16& arg0, const Dx& arg1);

  /** Input doubleword from I/O port specified in DX into memory location specified in ES:(E)DI or RDI. */
  void ins(const M32& arg0, const Dx& arg1);

  /** Input byte from I/O port specified in DX into memory location specified in ES:(E)DI or RDI. */
  void ins(const M8& arg0, const Dx& arg1);

  /** Input byte from I/O port specified in DX into memory location specified with ES:(E)DI or RDI. */
  void insb();

  /** Input doubleword from I/O port specified in DX into memory location specified in ES:(E)DI or RDI. */
  void insd();

  /** Insert a single precision floating-point value selected by imm8 from xmm2/m32 into xmm1 at the specified destination element specified by imm8 and zero out destination elements in xmm1 as indicated in imm8. */
  void insertps(const Xmm& arg0, const M32& arg1, const Imm8& arg2);

  /** Insert a single precision floating-point value selected by imm8 from xmm2/m32 into xmm1 at the specified destination element specified by imm8 and zero out destination elements in xmm1 as indicated in imm8. */
  void insertps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Input word from I/O port specified in DX into memory location specified in ES:(E)DI or RDI. */
  void insw();

  /** Interrupt vector number specified by immediate byte. */
  void int_(const Imm8& arg0);

  /** Interrupt 3-trap to debugger. */
  void int_(const Three& arg0);

  /** Invalidates entries in the TLBs and paging-structure caches based on invalidation type in r64 and descriptor in m128. */
  void invpcid(const R64& arg0, const M128& arg1);

  /** Interrupt return (16-bit operand size). */
  void iret();

  /** Interrupt return (32-bit operand size). */
  void iretd();

  /** Interrupt return (64-bit operand size). */
  void iretq();

  /** Jump short if above (CF=0 and ZF=0). */
  void ja(Label& arg0);

  /** Jump near if above (CF=0 and ZF=0). */
  void ja_1(Label& arg0);

  /** Jump short if above (CF=0 and ZF=0). */
  void ja(Label& arg0, const Hint& arg1);

  /** Jump near if above (CF=0 and ZF=0). */
  void ja_1(Label& arg0, const Hint& arg1);

  /** Jump near if above (CF=0 and ZF=0). */
  void ja(const Rel32& arg0);

  /** Jump near if above (CF=0 and ZF=0). */
  void ja(const Rel32& arg0, const Hint& arg1);

  /** Jump short if above (CF=0 and ZF=0). */
  void ja(const Rel8& arg0);

  /** Jump short if above (CF=0 and ZF=0). */
  void ja(const Rel8& arg0, const Hint& arg1);

  /** Jump short if above or equal (CF=0). */
  void jae(Label& arg0);

  /** Jump near if above or equal (CF=0). */
  void jae_1(Label& arg0);

  /** Jump short if above or equal (CF=0). */
  void jae(Label& arg0, const Hint& arg1);

  /** Jump near if above or equal (CF=0). */
  void jae_1(Label& arg0, const Hint& arg1);

  /** Jump near if above or equal (CF=0). */
  void jae(const Rel32& arg0);

  /** Jump near if above or equal (CF=0). */
  void jae(const Rel32& arg0, const Hint& arg1);

  /** Jump short if above or equal (CF=0). */
  void jae(const Rel8& arg0);

  /** Jump short if above or equal (CF=0). */
  void jae(const Rel8& arg0, const Hint& arg1);

  /** Jump short if below (CF=1). */
  void jb(Label& arg0);

  /** Jump near if below (CF=1). */
  void jb_1(Label& arg0);

  /** Jump short if below (CF=1). */
  void jb(Label& arg0, const Hint& arg1);

  /** Jump near if below (CF=1). */
  void jb_1(Label& arg0, const Hint& arg1);

  /** Jump near if below (CF=1). */
  void jb(const Rel32& arg0);

  /** Jump near if below (CF=1). */
  void jb(const Rel32& arg0, const Hint& arg1);

  /** Jump short if below (CF=1). */
  void jb(const Rel8& arg0);

  /** Jump short if below (CF=1). */
  void jb(const Rel8& arg0, const Hint& arg1);

  /** Jump short if below or equal (CF=1 or ZF=1). */
  void jbe(Label& arg0);

  /** Jump near if below or equal (CF=1 or ZF=1). */
  void jbe_1(Label& arg0);

  /** Jump short if below or equal (CF=1 or ZF=1). */
  void jbe(Label& arg0, const Hint& arg1);

  /** Jump near if below or equal (CF=1 or ZF=1). */
  void jbe_1(Label& arg0, const Hint& arg1);

  /** Jump near if below or equal (CF=1 or ZF=1). */
  void jbe(const Rel32& arg0);

  /** Jump near if below or equal (CF=1 or ZF=1). */
  void jbe(const Rel32& arg0, const Hint& arg1);

  /** Jump short if below or equal (CF=1 or ZF=1). */
  void jbe(const Rel8& arg0);

  /** Jump short if below or equal (CF=1 or ZF=1). */
  void jbe(const Rel8& arg0, const Hint& arg1);

  /** Jump short if carry (CF=1). */
  void jc(Label& arg0);

  /** Jump near if carry (CF=1). */
  void jc_1(Label& arg0);

  /** Jump short if carry (CF=1). */
  void jc(Label& arg0, const Hint& arg1);

  /** Jump near if carry (CF=1). */
  void jc_1(Label& arg0, const Hint& arg1);

  /** Jump near if carry (CF=1). */
  void jc(const Rel32& arg0);

  /** Jump near if carry (CF=1). */
  void jc(const Rel32& arg0, const Hint& arg1);

  /** Jump short if carry (CF=1). */
  void jc(const Rel8& arg0);

  /** Jump short if carry (CF=1). */
  void jc(const Rel8& arg0, const Hint& arg1);

  /** Jump short if equal (ZF=1). */
  void je(Label& arg0);

  /** Jump near if 0 (ZF=1). */
  void je_1(Label& arg0);

  /** Jump short if equal (ZF=1). */
  void je(Label& arg0, const Hint& arg1);

  /** Jump near if 0 (ZF=1). */
  void je_1(Label& arg0, const Hint& arg1);

  /** Jump near if 0 (ZF=1). */
  void je(const Rel32& arg0);

  /** Jump near if 0 (ZF=1). */
  void je(const Rel32& arg0, const Hint& arg1);

  /** Jump short if equal (ZF=1). */
  void je(const Rel8& arg0);

  /** Jump short if equal (ZF=1). */
  void je(const Rel8& arg0, const Hint& arg1);

  /** Jump short if ECX register is 0. */
  void jecxz(Label& arg0);

  /** Jump short if ECX register is 0. */
  void jecxz(Label& arg0, const Hint& arg1);

  /** Jump short if ECX register is 0. */
  void jecxz(const Rel8& arg0);

  /** Jump short if ECX register is 0. */
  void jecxz(const Rel8& arg0, const Hint& arg1);

  /** Jump short if greater (ZF=0 and SF=OF). */
  void jg(Label& arg0);

  /** Jump near if greater (ZF=0 and SF=OF). */
  void jg_1(Label& arg0);

  /** Jump short if greater (ZF=0 and SF=OF). */
  void jg(Label& arg0, const Hint& arg1);

  /** Jump near if greater (ZF=0 and SF=OF). */
  void jg_1(Label& arg0, const Hint& arg1);

  /** Jump near if greater (ZF=0 and SF=OF). */
  void jg(const Rel32& arg0);

  /** Jump near if greater (ZF=0 and SF=OF). */
  void jg(const Rel32& arg0, const Hint& arg1);

  /** Jump short if greater (ZF=0 and SF=OF). */
  void jg(const Rel8& arg0);

  /** Jump short if greater (ZF=0 and SF=OF). */
  void jg(const Rel8& arg0, const Hint& arg1);

  /** Jump short if greater or equal (SF=OF). */
  void jge(Label& arg0);

  /** Jump near if greater or equal (SF=OF). */
  void jge_1(Label& arg0);

  /** Jump short if greater or equal (SF=OF). */
  void jge(Label& arg0, const Hint& arg1);

  /** Jump near if greater or equal (SF=OF). */
  void jge_1(Label& arg0, const Hint& arg1);

  /** Jump near if greater or equal (SF=OF). */
  void jge(const Rel32& arg0);

  /** Jump near if greater or equal (SF=OF). */
  void jge(const Rel32& arg0, const Hint& arg1);

  /** Jump short if greater or equal (SF=OF). */
  void jge(const Rel8& arg0);

  /** Jump short if greater or equal (SF=OF). */
  void jge(const Rel8& arg0, const Hint& arg1);

  /** Jump short if less (SF!= OF). */
  void jl(Label& arg0);

  /** Jump near if less (SF!= OF). */
  void jl_1(Label& arg0);

  /** Jump short if less (SF!= OF). */
  void jl(Label& arg0, const Hint& arg1);

  /** Jump near if less (SF!= OF). */
  void jl_1(Label& arg0, const Hint& arg1);

  /** Jump near if less (SF!= OF). */
  void jl(const Rel32& arg0);

  /** Jump near if less (SF!= OF). */
  void jl(const Rel32& arg0, const Hint& arg1);

  /** Jump short if less (SF!= OF). */
  void jl(const Rel8& arg0);

  /** Jump short if less (SF!= OF). */
  void jl(const Rel8& arg0, const Hint& arg1);

  /** Jump short if less or equal (ZF=1 or SF!= OF). */
  void jle(Label& arg0);

  /** Jump near if less or equal (ZF=1 or SF!= OF). */
  void jle_1(Label& arg0);

  /** Jump short if less or equal (ZF=1 or SF!= OF). */
  void jle(Label& arg0, const Hint& arg1);

  /** Jump near if less or equal (ZF=1 or SF!= OF). */
  void jle_1(Label& arg0, const Hint& arg1);

  /** Jump near if less or equal (ZF=1 or SF!= OF). */
  void jle(const Rel32& arg0);

  /** Jump near if less or equal (ZF=1 or SF!= OF). */
  void jle(const Rel32& arg0, const Hint& arg1);

  /** Jump short if less or equal (ZF=1 or SF!= OF). */
  void jle(const Rel8& arg0);

  /** Jump short if less or equal (ZF=1 or SF!= OF). */
  void jle(const Rel8& arg0, const Hint& arg1);

  /** Jump far, absolute indirect, address given in m16:16 */
  void jmp(const FarPtr1616& arg0);

  /** Jump far, absolute indirect, address given in m16:32. */
  void jmp(const FarPtr1632& arg0);

  /** Jump far, absolute indirect, address given in m16:64. */
  void jmp(const FarPtr1664& arg0);

  /** Jump short, RIP = RIP + 8-bit displacement sign extended to 64-bits */
  void jmp(Label& arg0);

  /** Jump near, relative, RIP = RIP + 32-bit displacement sign extended to 64-bits */
  void jmp_1(Label& arg0);

  /** Jump near, absolute indirect, RIP = 64-Bit offset from register or memory */
  void jmp(const M64& arg0);

  /** Jump near, absolute indirect, RIP = 64-Bit offset from register or memory */
  void jmp(const R64& arg0);

  /** Jump near, relative, RIP = RIP + 32-bit displacement sign extended to 64-bits */
  void jmp(const Rel32& arg0);

  /** Jump short, RIP = RIP + 8-bit displacement sign extended to 64-bits */
  void jmp(const Rel8& arg0);

  /** Jump short if not above (CF=1 or ZF=1). */
  void jna(Label& arg0);

  /** Jump near if not above (CF=1 or ZF=1). */
  void jna_1(Label& arg0);

  /** Jump short if not above (CF=1 or ZF=1). */
  void jna(Label& arg0, const Hint& arg1);

  /** Jump near if not above (CF=1 or ZF=1). */
  void jna_1(Label& arg0, const Hint& arg1);

  /** Jump near if not above (CF=1 or ZF=1). */
  void jna(const Rel32& arg0);

  /** Jump near if not above (CF=1 or ZF=1). */
  void jna(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not above (CF=1 or ZF=1). */
  void jna(const Rel8& arg0);

  /** Jump short if not above (CF=1 or ZF=1). */
  void jna(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not above or equal (CF=1). */
  void jnae(Label& arg0);

  /** Jump near if not above or equal (CF=1). */
  void jnae_1(Label& arg0);

  /** Jump short if not above or equal (CF=1). */
  void jnae(Label& arg0, const Hint& arg1);

  /** Jump near if not above or equal (CF=1). */
  void jnae_1(Label& arg0, const Hint& arg1);

  /** Jump near if not above or equal (CF=1). */
  void jnae(const Rel32& arg0);

  /** Jump near if not above or equal (CF=1). */
  void jnae(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not above or equal (CF=1). */
  void jnae(const Rel8& arg0);

  /** Jump short if not above or equal (CF=1). */
  void jnae(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not below (CF=0). */
  void jnb(Label& arg0);

  /** Jump near if not below (CF=0). */
  void jnb_1(Label& arg0);

  /** Jump short if not below (CF=0). */
  void jnb(Label& arg0, const Hint& arg1);

  /** Jump near if not below (CF=0). */
  void jnb_1(Label& arg0, const Hint& arg1);

  /** Jump near if not below (CF=0). */
  void jnb(const Rel32& arg0);

  /** Jump near if not below (CF=0). */
  void jnb(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not below (CF=0). */
  void jnb(const Rel8& arg0);

  /** Jump short if not below (CF=0). */
  void jnb(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not below or equal (CF=0 and ZF=0). */
  void jnbe(Label& arg0);

  /** Jump near if not below or equal (CF=0 and ZF=0). */
  void jnbe_1(Label& arg0);

  /** Jump short if not below or equal (CF=0 and ZF=0). */
  void jnbe(Label& arg0, const Hint& arg1);

  /** Jump near if not below or equal (CF=0 and ZF=0). */
  void jnbe_1(Label& arg0, const Hint& arg1);

  /** Jump near if not below or equal (CF=0 and ZF=0). */
  void jnbe(const Rel32& arg0);

  /** Jump near if not below or equal (CF=0 and ZF=0). */
  void jnbe(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not below or equal (CF=0 and ZF=0). */
  void jnbe(const Rel8& arg0);

  /** Jump short if not below or equal (CF=0 and ZF=0). */
  void jnbe(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not carry (CF=0). */
  void jnc(Label& arg0);

  /** Jump near if not carry (CF=0). */
  void jnc_1(Label& arg0);

  /** Jump short if not carry (CF=0). */
  void jnc(Label& arg0, const Hint& arg1);

  /** Jump near if not carry (CF=0). */
  void jnc_1(Label& arg0, const Hint& arg1);

  /** Jump near if not carry (CF=0). */
  void jnc(const Rel32& arg0);

  /** Jump near if not carry (CF=0). */
  void jnc(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not carry (CF=0). */
  void jnc(const Rel8& arg0);

  /** Jump short if not carry (CF=0). */
  void jnc(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not equal (ZF=0). */
  void jne(Label& arg0);

  /** Jump near if not equal (ZF=0). */
  void jne_1(Label& arg0);

  /** Jump short if not equal (ZF=0). */
  void jne(Label& arg0, const Hint& arg1);

  /** Jump near if not equal (ZF=0). */
  void jne_1(Label& arg0, const Hint& arg1);

  /** Jump near if not equal (ZF=0). */
  void jne(const Rel32& arg0);

  /** Jump near if not equal (ZF=0). */
  void jne(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not equal (ZF=0). */
  void jne(const Rel8& arg0);

  /** Jump short if not equal (ZF=0). */
  void jne(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not greater (ZF=1 or SF!= OF). */
  void jng(Label& arg0);

  /** Jump near if not greater (ZF=1 or SF != OF). */
  void jng_1(Label& arg0);

  /** Jump short if not greater (ZF=1 or SF!= OF). */
  void jng(Label& arg0, const Hint& arg1);

  /** Jump near if not greater (ZF=1 or SF != OF). */
  void jng_1(Label& arg0, const Hint& arg1);

  /** Jump near if not greater (ZF=1 or SF != OF). */
  void jng(const Rel32& arg0);

  /** Jump near if not greater (ZF=1 or SF != OF). */
  void jng(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not greater (ZF=1 or SF!= OF). */
  void jng(const Rel8& arg0);

  /** Jump short if not greater (ZF=1 or SF!= OF). */
  void jng(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not greater or equal (SF!= OF). */
  void jnge(Label& arg0);

  /** Jump near if not greater or equal (SF != OF). */
  void jnge_1(Label& arg0);

  /** Jump short if not greater or equal (SF!= OF). */
  void jnge(Label& arg0, const Hint& arg1);

  /** Jump near if not greater or equal (SF != OF). */
  void jnge_1(Label& arg0, const Hint& arg1);

  /** Jump near if not greater or equal (SF != OF). */
  void jnge(const Rel32& arg0);

  /** Jump near if not greater or equal (SF != OF). */
  void jnge(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not greater or equal (SF!= OF). */
  void jnge(const Rel8& arg0);

  /** Jump short if not greater or equal (SF!= OF). */
  void jnge(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not less (SF=OF). */
  void jnl(Label& arg0);

  /** Jump near if not less (SF=OF). */
  void jnl_1(Label& arg0);

  /** Jump short if not less (SF=OF). */
  void jnl(Label& arg0, const Hint& arg1);

  /** Jump near if not less (SF=OF). */
  void jnl_1(Label& arg0, const Hint& arg1);

  /** Jump near if not less (SF=OF). */
  void jnl(const Rel32& arg0);

  /** Jump near if not less (SF=OF). */
  void jnl(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not less (SF=OF). */
  void jnl(const Rel8& arg0);

  /** Jump short if not less (SF=OF). */
  void jnl(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not less or equal (ZF=0 and SF=OF). */
  void jnle(Label& arg0);

  /** Jump near if not less or equal (ZF=0 and SF=OF). */
  void jnle_1(Label& arg0);

  /** Jump short if not less or equal (ZF=0 and SF=OF). */
  void jnle(Label& arg0, const Hint& arg1);

  /** Jump near if not less or equal (ZF=0 and SF=OF). */
  void jnle_1(Label& arg0, const Hint& arg1);

  /** Jump near if not less or equal (ZF=0 and SF=OF). */
  void jnle(const Rel32& arg0);

  /** Jump near if not less or equal (ZF=0 and SF=OF). */
  void jnle(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not less or equal (ZF=0 and SF=OF). */
  void jnle(const Rel8& arg0);

  /** Jump short if not less or equal (ZF=0 and SF=OF). */
  void jnle(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not overflow (OF=0). */
  void jno(Label& arg0);

  /** Jump near if not overflow (OF=0). */
  void jno_1(Label& arg0);

  /** Jump short if not overflow (OF=0). */
  void jno(Label& arg0, const Hint& arg1);

  /** Jump near if not overflow (OF=0). */
  void jno_1(Label& arg0, const Hint& arg1);

  /** Jump near if not overflow (OF=0). */
  void jno(const Rel32& arg0);

  /** Jump near if not overflow (OF=0). */
  void jno(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not overflow (OF=0). */
  void jno(const Rel8& arg0);

  /** Jump short if not overflow (OF=0). */
  void jno(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not parity (PF=0). */
  void jnp(Label& arg0);

  /** Jump near if not parity (PF=0). */
  void jnp_1(Label& arg0);

  /** Jump short if not parity (PF=0). */
  void jnp(Label& arg0, const Hint& arg1);

  /** Jump near if not parity (PF=0). */
  void jnp_1(Label& arg0, const Hint& arg1);

  /** Jump near if not parity (PF=0). */
  void jnp(const Rel32& arg0);

  /** Jump near if not parity (PF=0). */
  void jnp(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not parity (PF=0). */
  void jnp(const Rel8& arg0);

  /** Jump short if not parity (PF=0). */
  void jnp(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not sign (SF=0). */
  void jns(Label& arg0);

  /** Jump near if not sign (SF=0). */
  void jns_1(Label& arg0);

  /** Jump short if not sign (SF=0). */
  void jns(Label& arg0, const Hint& arg1);

  /** Jump near if not sign (SF=0). */
  void jns_1(Label& arg0, const Hint& arg1);

  /** Jump near if not sign (SF=0). */
  void jns(const Rel32& arg0);

  /** Jump near if not sign (SF=0). */
  void jns(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not sign (SF=0). */
  void jns(const Rel8& arg0);

  /** Jump short if not sign (SF=0). */
  void jns(const Rel8& arg0, const Hint& arg1);

  /** Jump short if not zero (ZF=0). */
  void jnz(Label& arg0);

  /** Jump near if not zero (ZF=0). */
  void jnz_1(Label& arg0);

  /** Jump short if not zero (ZF=0). */
  void jnz(Label& arg0, const Hint& arg1);

  /** Jump near if not zero (ZF=0). */
  void jnz_1(Label& arg0, const Hint& arg1);

  /** Jump near if not zero (ZF=0). */
  void jnz(const Rel32& arg0);

  /** Jump near if not zero (ZF=0). */
  void jnz(const Rel32& arg0, const Hint& arg1);

  /** Jump short if not zero (ZF=0). */
  void jnz(const Rel8& arg0);

  /** Jump short if not zero (ZF=0). */
  void jnz(const Rel8& arg0, const Hint& arg1);

  /** Jump short if overflow (OF=1). */
  void jo(Label& arg0);

  /** Jump near if overflow (OF=1). */
  void jo_1(Label& arg0);

  /** Jump short if overflow (OF=1). */
  void jo(Label& arg0, const Hint& arg1);

  /** Jump near if overflow (OF=1). */
  void jo_1(Label& arg0, const Hint& arg1);

  /** Jump near if overflow (OF=1). */
  void jo(const Rel32& arg0);

  /** Jump near if overflow (OF=1). */
  void jo(const Rel32& arg0, const Hint& arg1);

  /** Jump short if overflow (OF=1). */
  void jo(const Rel8& arg0);

  /** Jump short if overflow (OF=1). */
  void jo(const Rel8& arg0, const Hint& arg1);

  /** Jump short if parity (PF=1). */
  void jp(Label& arg0);

  /** Jump near if parity (PF=1). */
  void jp_1(Label& arg0);

  /** Jump short if parity (PF=1). */
  void jp(Label& arg0, const Hint& arg1);

  /** Jump near if parity (PF=1). */
  void jp_1(Label& arg0, const Hint& arg1);

  /** Jump near if parity (PF=1). */
  void jp(const Rel32& arg0);

  /** Jump near if parity (PF=1). */
  void jp(const Rel32& arg0, const Hint& arg1);

  /** Jump short if parity (PF=1). */
  void jp(const Rel8& arg0);

  /** Jump short if parity (PF=1). */
  void jp(const Rel8& arg0, const Hint& arg1);

  /** Jump short if parity even (PF=1). */
  void jpe(Label& arg0);

  /** Jump near if parity even (PF=1). */
  void jpe_1(Label& arg0);

  /** Jump short if parity even (PF=1). */
  void jpe(Label& arg0, const Hint& arg1);

  /** Jump near if parity even (PF=1). */
  void jpe_1(Label& arg0, const Hint& arg1);

  /** Jump near if parity even (PF=1). */
  void jpe(const Rel32& arg0);

  /** Jump near if parity even (PF=1). */
  void jpe(const Rel32& arg0, const Hint& arg1);

  /** Jump short if parity even (PF=1). */
  void jpe(const Rel8& arg0);

  /** Jump short if parity even (PF=1). */
  void jpe(const Rel8& arg0, const Hint& arg1);

  /** Jump short if parity odd (PF=0). */
  void jpo(Label& arg0);

  /** Jump near if parity odd (PF=0). */
  void jpo_1(Label& arg0);

  /** Jump short if parity odd (PF=0). */
  void jpo(Label& arg0, const Hint& arg1);

  /** Jump near if parity odd (PF=0). */
  void jpo_1(Label& arg0, const Hint& arg1);

  /** Jump near if parity odd (PF=0). */
  void jpo(const Rel32& arg0);

  /** Jump near if parity odd (PF=0). */
  void jpo(const Rel32& arg0, const Hint& arg1);

  /** Jump short if parity odd (PF=0). */
  void jpo(const Rel8& arg0);

  /** Jump short if parity odd (PF=0). */
  void jpo(const Rel8& arg0, const Hint& arg1);

  /** Jump short if RCX register is 0. */
  void jrcxz(Label& arg0);

  /** Jump short if RCX register is 0. */
  void jrcxz(Label& arg0, const Hint& arg1);

  /** Jump short if RCX register is 0. */
  void jrcxz(const Rel8& arg0);

  /** Jump short if RCX register is 0. */
  void jrcxz(const Rel8& arg0, const Hint& arg1);

  /** Jump short if sign (SF=1). */
  void js(Label& arg0);

  /** Jump near if sign (SF=1). */
  void js_1(Label& arg0);

  /** Jump short if sign (SF=1). */
  void js(Label& arg0, const Hint& arg1);

  /** Jump near if sign (SF=1). */
  void js_1(Label& arg0, const Hint& arg1);

  /** Jump near if sign (SF=1). */
  void js(const Rel32& arg0);

  /** Jump near if sign (SF=1). */
  void js(const Rel32& arg0, const Hint& arg1);

  /** Jump short if sign (SF=1). */
  void js(const Rel8& arg0);

  /** Jump short if sign (SF=1). */
  void js(const Rel8& arg0, const Hint& arg1);

  /** Jump short if zero (ZF = 1). */
  void jz(Label& arg0);

  /** Jump near if 0 (ZF=1). */
  void jz_1(Label& arg0);

  /** Jump short if zero (ZF = 1). */
  void jz(Label& arg0, const Hint& arg1);

  /** Jump near if 0 (ZF=1). */
  void jz_1(Label& arg0, const Hint& arg1);

  /** Jump near if 0 (ZF=1). */
  void jz(const Rel32& arg0);

  /** Jump near if 0 (ZF=1). */
  void jz(const Rel32& arg0, const Hint& arg1);

  /** Jump short if zero (ZF = 1). */
  void jz(const Rel8& arg0);

  /** Jump short if zero (ZF = 1). */
  void jz(const Rel8& arg0, const Hint& arg1);

  /** Load: AH = EFLAGS(SF:ZF:0:AF:0:PF:1:CF). */
  void lahf();

  /** r16 = access rights referenced by r16/m16 */
  void lar(const R16& arg0, const M16& arg1);

  /** r16 = access rights referenced by r16/m16 */
  void lar(const R16& arg0, const R16& arg1);

  /** reg = access rights referenced by r32/m16 */
  void lar(const R32& arg0, const M16& arg1);

  /** reg = access rights referenced by r32/m16 */
  void lar(const R32& arg0, const R32& arg1);

  /** reg = access rights referenced by r32/m16 */
  void lar(const R64& arg0, const M16& arg1);

  /** reg = access rights referenced by r32/m16 */
  void lar(const R64& arg0, const R32& arg1);

  /** Load unaligned data from mem and return double quadword in xmm1. */
  void lddqu(const Xmm& arg0, const M128& arg1);

  /** Load MXCSR register from m32. */
  void ldmxcsr(const M32& arg0);

  /** Store effective address for m in register r16. */
  void lea(const R16& arg0, const M16& arg1);

  /** Store effective address for m in register r16. */
  void lea(const R16& arg0, const M32& arg1);

  /** Store effective address for m in register r16. */
  void lea(const R16& arg0, const M64& arg1);

  /** Store effective address for m in register r32. */
  void lea(const R32& arg0, const M16& arg1);

  /** Store effective address for m in register r32. */
  void lea(const R32& arg0, const M32& arg1);

  /** Store effective address for m in register r32. */
  void lea(const R32& arg0, const M64& arg1);

  /** Store effective address for m in register r64. */
  void lea(const R64& arg0, const M16& arg1);

  /** Store effective address for m in register r64. */
  void lea(const R64& arg0, const M32& arg1);

  /** Store effective address for m in register r64. */
  void lea(const R64& arg0, const M64& arg1);

  /** Set RSP to RBP, then pop RBP. */
  void leave();

  /** Set SP to BP, then pop BP. */
  void leave(const Pref66& arg0);

  /** Serializes load operations. */
  void lfence();

  /** Load FS:r16 with far pointer from memory. */
  void lfs(const R16& arg0, const FarPtr1616& arg1);

  /** Load FS:r32 with far pointer from memory. */
  void lfs(const R32& arg0, const FarPtr1632& arg1);

  /** Load FS:r64 with far pointer from memory. */
  void lfs(const R64& arg0, const FarPtr1664& arg1);

  /** Load GS:r16 with far pointer from memory. */
  void lgs(const R16& arg0, const FarPtr1616& arg1);

  /** Load GS:r32 with far pointer from memory. */
  void lgs(const R32& arg0, const FarPtr1632& arg1);

  /** Load GS:r64 with far pointer from memory. */
  void lgs(const R64& arg0, const FarPtr1664& arg1);

  /** Asserts LOCK# signal for duration of the accompanying instruction. */
  void lock();

  /** For legacy mode, Load word at address DS:(E)SI into AX. For 64-bit mode load word at address (R)SI into AX. */
  void lods(const M16& arg0);

  /** For legacy mode, Load dword at address DS:(E)SI into EAX. For 64-bit mode load dword at address (R)SI into EAX. */
  void lods(const M32& arg0);

  /** Load qword at address (R)SI into RAX. */
  void lods(const M64& arg0);

  /** For legacy mode, Load byte at address DS:(E)SI into AL. For 64-bit mode load byte at address (R)SI into AL. */
  void lods(const M8& arg0);

  /** For legacy mode, Load byte at address DS:(E)SI into AL. For 64-bit mode load byte at address (R)SI into AL. */
  void lodsb();

  /** For legacy mode, Load dword at address DS:(E)SI into EAX. For 64-bit mode load dword at address (R)SI into EAX. */
  void lodsd();

  /** Load qword at address (R)SI into RAX. */
  void lodsq();

  /** For legacy mode, Load word at address DS:(E)SI into AX. For 64-bit mode load word at address (R)SI into AX. */
  void lodsw();

  /** Decrement count; jump short if count != 0. */
  void loop(Label& arg0);

  /** Decrement count; jump short if count != 0. */
  void loop(const Rel8& arg0);

  /** Decrement count; jump short if count != 0 and  ZF = 1. */
  void loope(Label& arg0);

  /** Decrement count; jump short if count != 0 and  ZF = 1. */
  void loope(const Rel8& arg0);

  /** Decrement count; jump short if count != 0 and  ZF = 0. */
  void loopne(Label& arg0);

  /** Decrement count; jump short if count != 0 and  ZF = 0. */
  void loopne(const Rel8& arg0);

  /** Load: r16 = segment limit, selector r16/m16. */
  void lsl(const R16& arg0, const M16& arg1);

  /** Load: r16 = segment limit, selector r16/m16. */
  void lsl(const R16& arg0, const R16& arg1);

  /** Load: r32 = segment limit, selector r32/m16. */
  void lsl(const R32& arg0, const M16& arg1);

  /** Load: r32 = segment limit, selector r32/m16. */
  void lsl(const R32& arg0, const R32& arg1);

  /** Load: r64 = segment limit, selector r32/m16 */
  void lsl(const R64& arg0, const M16& arg1);

  /** Load: r64 = segment limit, selector r32/m16 */
  void lsl(const R64& arg0, const R32& arg1);

  /** Load SS:r16 with far pointer from memory. */
  void lss(const R16& arg0, const FarPtr1616& arg1);

  /** Load SS:r32 with far pointer from memory. */
  void lss(const R32& arg0, const FarPtr1632& arg1);

  /** Load SS:r64 with far pointer from memory. */
  void lss(const R64& arg0, const FarPtr1664& arg1);

  /** Count the number of leading zero bits in r/m16, return result in r16 */
  void lzcnt(const R16& arg0, const M16& arg1);

  /** Count the number of leading zero bits in r/m16, return result in r16 */
  void lzcnt(const R16& arg0, const R16& arg1);

  /** Count the number of leading zero bits in r/m32, return result in r32 */
  void lzcnt(const R32& arg0, const M32& arg1);

  /** Count the number of leading zero bits in r/m32, return result in r32 */
  void lzcnt(const R32& arg0, const R32& arg1);

  /** Count the number of leading zero bits in r/m64, return result in r64 */
  void lzcnt(const R64& arg0, const M64& arg1);

  /** Count the number of leading zero bits in r/m64, return result in r64 */
  void lzcnt(const R64& arg0, const R64& arg1);

  /** Selectively write bytes from xmm1 to memory location using the byte mask in xmm2. The default memory location is specified by DS:DI/EDI/RDI. */
  void maskmovdqu(const Xmm& arg0, const Xmm& arg1);

  /** Selectively write bytes from mm1 to memory location using the byte mask in mm2. The default memory location is specified by DS:DI/EDI/RDI. */
  void maskmovq(const Mm& arg0, const Mm& arg1);

  /** Return the maximum double-precision floating-point values between xmm2/m128 and xmm1. */
  void maxpd(const Xmm& arg0, const M128& arg1);

  /** Return the maximum double-precision floating-point values between xmm2/m128 and xmm1. */
  void maxpd(const Xmm& arg0, const Xmm& arg1);

  /** Return the maximum single-precision floating-point values between xmm2/m128 and xmm1. */
  void maxps(const Xmm& arg0, const M128& arg1);

  /** Return the maximum single-precision floating-point values between xmm2/m128 and xmm1. */
  void maxps(const Xmm& arg0, const Xmm& arg1);

  /** Return the maximum scalar double-precision floating-point value between xmm2/mem64 and xmm1. */
  void maxsd(const Xmm& arg0, const M64& arg1);

  /** Return the maximum scalar double-precision floating-point value between xmm2/mem64 and xmm1. */
  void maxsd(const Xmm& arg0, const Xmm& arg1);

  /** Return the maximum scalar single-precision floating-point value between xmm2/mem32 and xmm1. */
  void maxss(const Xmm& arg0, const M32& arg1);

  /** Return the maximum scalar single-precision floating-point value between xmm2/mem32 and xmm1. */
  void maxss(const Xmm& arg0, const Xmm& arg1);

  /** Serializes load and store operations. */
  void mfence();

  /** Return the minimum double-precision floating-point values between xmm2/m128 and xmm1. */
  void minpd(const Xmm& arg0, const M128& arg1);

  /** Return the minimum double-precision floating-point values between xmm2/m128 and xmm1. */
  void minpd(const Xmm& arg0, const Xmm& arg1);

  /** Return the minimum single-precision floating-point values between xmm2/m128 and xmm1. */
  void minps(const Xmm& arg0, const M128& arg1);

  /** Return the minimum single-precision floating-point values between xmm2/m128 and xmm1. */
  void minps(const Xmm& arg0, const Xmm& arg1);

  /** Return the minimum scalar double-precision floating-point value between xmm2/mem64 and xmm1. */
  void minsd(const Xmm& arg0, const M64& arg1);

  /** Return the minimum scalar double-precision floating-point value between xmm2/mem64 and xmm1. */
  void minsd(const Xmm& arg0, const Xmm& arg1);

  /** Return the minimum scalar single-precision floating-point value between xmm2/mem32 and xmm1. */
  void minss(const Xmm& arg0, const M32& arg1);

  /** Return the minimum scalar single-precision floating-point value between xmm2/mem32 and xmm1. */
  void minss(const Xmm& arg0, const Xmm& arg1);

  /** Sets up a linear address range to be monitored by hardware and activates the monitor. The address range should be a write-back memory caching type. The address is DS:EAX (DS:RAX in 64-bit mode). */
  void monitor();

  /** Move byte at (seg:offset) to AL. */
  void mov(const Al& arg0, const Moffs8& arg1);

  /** Move byte at (offset) to AL. */
  void mov(const Al& arg0, const Moffs8& arg1, const PrefRexW& arg2);

  /** Move word at (seg:offset) to AX. */
  void mov(const Ax& arg0, const Moffs16& arg1);

  /** Move doubleword at (seg:offset) to EAX. */
  void mov(const Eax& arg0, const Moffs32& arg1);

  /** Move imm16 to r/m16. */
  void mov(const M16& arg0, const Imm16& arg1);

  /** Move r16 to r/m16. */
  void mov(const M16& arg0, const R16& arg1);

  /** Move segment register to r/m16. */
  void mov(const M16& arg0, const Sreg& arg1);

  /** Move imm32 to r/m32. */
  void mov(const M32& arg0, const Imm32& arg1);

  /** Move r32 to r/m32. */
  void mov(const M32& arg0, const R32& arg1);

  /** Move imm32 sign extended to 64-bits to r/m64. */
  void mov(const M64& arg0, const Imm32& arg1);

  /** Move r64 to r/m64. */
  void mov(const M64& arg0, const R64& arg1);

  /** Move zero extended 16-bit segment register to r/m64. */
  void mov(const M64& arg0, const Sreg& arg1);

  /** Move imm8 to r/m8. */
  void mov(const M8& arg0, const Imm8& arg1);

  /** Move r8 to r/m8. */
  void mov(const M8& arg0, const R8& arg1);

  /** Move r8 to r/m8. */
  void mov(const M8& arg0, const Rh& arg1);

  /** Move AX to (seg:offset). */
  void mov(const Moffs16& arg0, const Ax& arg1);

  /** Move EAX to (seg:offset). */
  void mov(const Moffs32& arg0, const Eax& arg1);

  /** Move RAX to (offset). */
  void mov(const Moffs64& arg0, const Rax& arg1);

  /** Move AL to (seg:offset). */
  void mov(const Moffs8& arg0, const Al& arg1);

  /** Move AL to (offset). */
  void mov(const Moffs8& arg0, const Al& arg1, const PrefRexW& arg2);

  /** Move imm16 to r16. */
  void mov(const R16& arg0, const Imm16& arg1);

  /** Move imm16 to r/m16. */
  void mov_1(const R16& arg0, const Imm16& arg1);

  /** Move r/m16 to r16. */
  void mov(const R16& arg0, const M16& arg1);

  /** Move r16 to r/m16. */
  void mov(const R16& arg0, const R16& arg1);

  /** Move r/m16 to r16. */
  void mov_1(const R16& arg0, const R16& arg1);

  /** Move segment register to r/m16. */
  void mov(const R16& arg0, const Sreg& arg1);

  /** Move imm32 to r32. */
  void mov(const R32& arg0, const Imm32& arg1);

  /** Move imm32 to r/m32. */
  void mov_1(const R32& arg0, const Imm32& arg1);

  /** Move r/m32 to r32. */
  void mov(const R32& arg0, const M32& arg1);

  /** Move r32 to r/m32. */
  void mov(const R32& arg0, const R32& arg1);

  /** Move r/m32 to r32. */
  void mov_1(const R32& arg0, const R32& arg1);

  /** Move imm32 sign extended to 64-bits to r/m64. */
  void mov(const R64& arg0, const Imm32& arg1);

  /** Move imm64 to r64. */
  void mov(const R64& arg0, const Imm64& arg1);

  /** Move r/m64 to r64. */
  void mov(const R64& arg0, const M64& arg1);

  /** Move r64 to r/m64. */
  void mov(const R64& arg0, const R64& arg1);

  /** Move r/m64 to r64. */
  void mov_1(const R64& arg0, const R64& arg1);

  /** Move zero extended 16-bit segment register to r/m64. */
  void mov(const R64& arg0, const Sreg& arg1);

  /** Move imm8 to r8. */
  void mov(const R8& arg0, const Imm8& arg1);

  /** Move imm8 to r/m8. */
  void mov_1(const R8& arg0, const Imm8& arg1);

  /** Move r/m8 to r8. */
  void mov(const R8& arg0, const M8& arg1);

  /** Move r8 to r/m8. */
  void mov(const R8& arg0, const R8& arg1);

  /** Move r/m8 to r8. */
  void mov_1(const R8& arg0, const R8& arg1);

  /** Move r8 to r/m8. */
  void mov(const R8& arg0, const Rh& arg1);

  /** Move r/m8 to r8. */
  void mov_1(const R8& arg0, const Rh& arg1);

  /** Move quadword at (offset) to RAX. */
  void mov(const Rax& arg0, const Moffs64& arg1);

  /** Move imm8 to r8. */
  void mov(const Rh& arg0, const Imm8& arg1);

  /** Move imm8 to r/m8. */
  void mov_1(const Rh& arg0, const Imm8& arg1);

  /** Move r/m8 to r8. */
  void mov(const Rh& arg0, const M8& arg1);

  /** Move r8 to r/m8. */
  void mov(const Rh& arg0, const R8& arg1);

  /** Move r/m8 to r8. */
  void mov_1(const Rh& arg0, const R8& arg1);

  /** Move r8 to r/m8. */
  void mov(const Rh& arg0, const Rh& arg1);

  /** Move r/m8 to r8. */
  void mov_1(const Rh& arg0, const Rh& arg1);

  /** Move r/m16 to segment register. */
  void mov(const Sreg& arg0, const M16& arg1);

  /** Move lower 16 bits of r/m64 to segment register. */
  void mov(const Sreg& arg0, const M64& arg1);

  /** Move r/m16 to segment register. */
  void mov(const Sreg& arg0, const R16& arg1);

  /** Move lower 16 bits of r/m64 to segment register. */
  void mov(const Sreg& arg0, const R64& arg1);

  /** Move packed double-precision floating-point values from xmm1 to xmm2/m128. */
  void movapd(const M128& arg0, const Xmm& arg1);

  /** Move packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void movapd(const Xmm& arg0, const M128& arg1);

  /** Move packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void movapd(const Xmm& arg0, const Xmm& arg1);

  /** Move packed double-precision floating-point values from xmm1 to xmm2/m128. */
  void movapd_1(const Xmm& arg0, const Xmm& arg1);

  /** Move packed single-precision floating-point values from xmm1 to xmm2/m128. */
  void movaps(const M128& arg0, const Xmm& arg1);

  /** Move packed single-precision floating-point values from xmm2/m128 to xmm1. */
  void movaps(const Xmm& arg0, const M128& arg1);

  /** Move packed single-precision floating-point values from xmm2/m128 to xmm1. */
  void movaps(const Xmm& arg0, const Xmm& arg1);

  /** Move packed single-precision floating-point values from xmm1 to xmm2/m128. */
  void movaps_1(const Xmm& arg0, const Xmm& arg1);

  /** Reverse byte order in r16 and move to m16 */
  void movbe(const M16& arg0, const R16& arg1);

  /** Reverse byte order in r32 and move to m32 */
  void movbe(const M32& arg0, const R32& arg1);

  /** Reverse byte order in r64 and move to m64. */
  void movbe(const M64& arg0, const R64& arg1);

  /** Reverse byte order in m16 and move to r16 */
  void movbe(const R16& arg0, const M16& arg1);

  /** Reverse byte order in m32 and move to r32 */
  void movbe(const R32& arg0, const M32& arg1);

  /** Reverse byte order in m64 and move to r64. */
  void movbe(const R64& arg0, const M64& arg1);

  /** Move doubleword from mm to r/m32. */
  void movd(const M32& arg0, const Mm& arg1);

  /** Move doubleword from xmm register to r/m32. */
  void movd(const M32& arg0, const Xmm& arg1);

  /** Move doubleword from r/m32 to mm. */
  void movd(const Mm& arg0, const M32& arg1);

  /** Move doubleword from r/m32 to mm. */
  void movd(const Mm& arg0, const R32& arg1);

  /** Move doubleword from mm to r/m32. */
  void movd(const R32& arg0, const Mm& arg1);

  /** Move doubleword from xmm register to r/m32. */
  void movd(const R32& arg0, const Xmm& arg1);

  /** Move doubleword from r/m32 to xmm. */
  void movd(const Xmm& arg0, const M32& arg1);

  /** Move doubleword from r/m32 to xmm. */
  void movd(const Xmm& arg0, const R32& arg1);

  /** Move one double-precision floating-point value from the lower 64-bit operand in xmm2/m64 to xmm1 and duplicate. */
  void movddup(const Xmm& arg0, const M64& arg1);

  /** Move one double-precision floating-point value from the lower 64-bit operand in xmm2/m64 to xmm1 and duplicate. */
  void movddup(const Xmm& arg0, const Xmm& arg1);

  /** Move low quadword from xmm to mmx register. */
  void movdq2q(const Mm& arg0, const Xmm& arg1);

  /** Move aligned double quadword from xmm1 to xmm2/m128. */
  void movdqa(const M128& arg0, const Xmm& arg1);

  /** Move aligned double quadword from xmm2/m128 to xmm1. */
  void movdqa(const Xmm& arg0, const M128& arg1);

  /** Move aligned double quadword from xmm2/m128 to xmm1. */
  void movdqa(const Xmm& arg0, const Xmm& arg1);

  /** Move aligned double quadword from xmm1 to xmm2/m128. */
  void movdqa_1(const Xmm& arg0, const Xmm& arg1);

  /** Move unaligned double quadword from xmm1 to xmm2/m128. */
  void movdqu(const M128& arg0, const Xmm& arg1);

  /** Move unaligned double quadword from xmm2/m128 to xmm1. */
  void movdqu(const Xmm& arg0, const M128& arg1);

  /** Move unaligned double quadword from xmm2/m128 to xmm1. */
  void movdqu(const Xmm& arg0, const Xmm& arg1);

  /** Move unaligned double quadword from xmm1 to xmm2/m128. */
  void movdqu_1(const Xmm& arg0, const Xmm& arg1);

  /** Move two packed single-precision floating-point values from high quadword of xmm2 to low quadword of xmm1. */
  void movhlps(const Xmm& arg0, const Xmm& arg1);

  /** Move double-precision floating-point value from high quadword of xmm to m64. */
  void movhpd(const M64& arg0, const Xmm& arg1);

  /** Move double-precision floating-point value from m64 to high quadword of xmm. */
  void movhpd(const Xmm& arg0, const M64& arg1);

  /** Move two packed single-precision floating-point values from high quadword of xmm to m64. */
  void movhps(const M64& arg0, const Xmm& arg1);

  /** Move two packed single-precision floating-point values from m64 to high quadword of xmm. */
  void movhps(const Xmm& arg0, const M64& arg1);

  /** Move two packed single-precision floating-point values from low quadword of xmm2 to high quadword of xmm1. */
  void movlhps(const Xmm& arg0, const Xmm& arg1);

  /** Move double-precision floating-point nvalue from low quadword of xmm register to m64. */
  void movlpd(const M64& arg0, const Xmm& arg1);

  /** Move double-precision floating-point value from m64 to low quadword of xmm register. */
  void movlpd(const Xmm& arg0, const M64& arg1);

  /** Move two packed single-precision floating-point values from low quadword of xmm to m64. */
  void movlps(const M64& arg0, const Xmm& arg1);

  /** Move two packed single-precision floating-point values from m64 to low quadword of xmm. */
  void movlps(const Xmm& arg0, const M64& arg1);

  /** Extract 2-bit sign mask from xmm and store in reg. The upper bits of r32 or r64 are filled with zeros. */
  void movmskpd(const R32& arg0, const Xmm& arg1);

  /** Extract 2-bit sign mask from xmm and store in reg. The upper bits of r32 or r64 are filled with zeros. */
  void movmskpd(const R64& arg0, const Xmm& arg1);

  /** Extract 4-bit sign mask from xmm and store in reg. The upper bits of r32 or r64 are filled with zeros. */
  void movmskps(const R32& arg0, const Xmm& arg1);

  /** Extract 4-bit sign mask from xmm and store in reg. The upper bits of r32 or r64 are filled with zeros. */
  void movmskps(const R64& arg0, const Xmm& arg1);

  /** Move double quadword from xmm to m128 using non-temporal hint. */
  void movntdq(const M128& arg0, const Xmm& arg1);

  /** Move double quadword from m128 to xmm using non-temporal hint if WC memory type. */
  void movntdqa(const Xmm& arg0, const M128& arg1);

  /** Move doubleword from r32 to m32 using non-temporal hint. */
  void movnti(const M32& arg0, const R32& arg1);

  /** Move quadword from r64 to m64 using non-temporal hint. */
  void movnti(const M64& arg0, const R64& arg1);

  /** Move packed double-precision floating-point values from xmm to m128 using non-temporal hint. */
  void movntpd(const M128& arg0, const Xmm& arg1);

  /** Move packed single-precision floating-point values from xmm to m128 using non-temporal hint. */
  void movntps(const M128& arg0, const Xmm& arg1);

  /** Move quadword from mm to m64 using non-temporal hint. */
  void movntq(const M64& arg0, const Mm& arg1);

  /** Move quadword from mm to r/m64. */
  void movq(const M64& arg0, const Mm& arg1);

  /** Move quadword from mm to mm/m64. */
  void movq_1(const M64& arg0, const Mm& arg1);

  /** Move quadword from xmm register to r/m64. */
  void movq(const M64& arg0, const Xmm& arg1);

  /** Move quadword from xmm1 to xmm2/mem64. */
  void movq_1(const M64& arg0, const Xmm& arg1);

  /** Move quadword from r/m64 to mm. */
  void movq(const Mm& arg0, const M64& arg1);

  /** Move quadword from mm/m64 to mm. */
  void movq_1(const Mm& arg0, const M64& arg1);

  /** Move quadword from mm/m64 to mm. */
  void movq(const Mm& arg0, const Mm& arg1);

  /** Move quadword from mm to mm/m64. */
  void movq_1(const Mm& arg0, const Mm& arg1);

  /** Move quadword from r/m64 to mm. */
  void movq(const Mm& arg0, const R64& arg1);

  /** Move quadword from mm to r/m64. */
  void movq(const R64& arg0, const Mm& arg1);

  /** Move quadword from xmm register to r/m64. */
  void movq(const R64& arg0, const Xmm& arg1);

  /** Move quadword from r/m64 to xmm. */
  void movq(const Xmm& arg0, const M64& arg1);

  /** Move quadword from xmm2/mem64 to xmm1. */
  void movq_1(const Xmm& arg0, const M64& arg1);

  /** Move quadword from r/m64 to xmm. */
  void movq(const Xmm& arg0, const R64& arg1);

  /** Move quadword from xmm2/mem64 to xmm1. */
  void movq(const Xmm& arg0, const Xmm& arg1);

  /** Move quadword from xmm1 to xmm2/mem64. */
  void movq_1(const Xmm& arg0, const Xmm& arg1);

  /** Move quadword from mmx to low quadword of xmm. */
  void movq2dq(const Xmm& arg0, const Mm& arg1);

  /** For legacy mode, move word from address DS:(E)SI to ES:(E)DI. For 64-bit mode move word at address (R|E)SI to (R|E)DI. */
  void movs(const M16& arg0, const M16& arg1);

  /** For legacy mode, move dword from address DS:(E)SI to ES:(E)DI. For 64-bit mode move dword from address (R|E)SI to (R|E)DI. */
  void movs(const M32& arg0, const M32& arg1);

  /** Move qword from address (R|E)SI to (R|E)DI. */
  void movs(const M64& arg0, const M64& arg1);

  /** For legacy mode, Move byte from address DS:(E)SI to ES:(E)DI. For 64-bit mode move byte from address (R|E)SI to (R|E)DI. */
  void movs(const M8& arg0, const M8& arg1);

  /** For legacy mode, Move byte from address DS:(E)SI to ES:(E)DI. For 64-bit mode move byte from address (R|E)SI to (R|E)DI. */
  void movsb();

  /** For legacy mode, move dword from address DS:(E)SI to ES:(E)DI. For 64-bit mode move dword from address (R|E)SI to (R|E)DI. */
  void movsd();

  /** Move scalar double-precision floating-point value from xmm1 register to xmm2/m64. */
  void movsd(const M64& arg0, const Xmm& arg1);

  /** Move scalar double-precision floating-point value from xmm2/m64 to xmm1 register. */
  void movsd(const Xmm& arg0, const M64& arg1);

  /** Move scalar double-precision floating-point value from xmm2/m64 to xmm1 register. */
  void movsd(const Xmm& arg0, const Xmm& arg1);

  /** Move scalar double-precision floating-point value from xmm1 register to xmm2/m64. */
  void movsd_1(const Xmm& arg0, const Xmm& arg1);

  /** Move two single-precision floating-point values from the higher 32-bit operand of each qword in xmm2/m128 to xmm1 and duplicate each 32-bit operand to the lower 32-bits of each qword. */
  void movshdup(const Xmm& arg0, const M128& arg1);

  /** Move two single-precision floating-point values from the higher 32-bit operand of each qword in xmm2/m128 to xmm1 and duplicate each 32-bit operand to the lower 32-bits of each qword. */
  void movshdup(const Xmm& arg0, const Xmm& arg1);

  /** Move two single-precision floating-point values from the lower 32-bit operand of each qword in xmm2/m128 to xmm1 and duplicate each 32-bit operand to the higher 32-bits of each qword. */
  void movsldup(const Xmm& arg0, const M128& arg1);

  /** Move two single-precision floating-point values from the lower 32-bit operand of each qword in xmm2/m128 to xmm1 and duplicate each 32-bit operand to the higher 32-bits of each qword. */
  void movsldup(const Xmm& arg0, const Xmm& arg1);

  /** Move qword from address (R|E)SI to (R|E)DI. */
  void movsq();

  /** Move scalar single-precision floating-point value from xmm1 register to xmm2/m32. */
  void movss(const M32& arg0, const Xmm& arg1);

  /** Move scalar single-precision floating-point value from xmm2/m32 to xmm1 register. */
  void movss(const Xmm& arg0, const M32& arg1);

  /** Move scalar single-precision floating-point value from xmm2/m32 to xmm1 register. */
  void movss(const Xmm& arg0, const Xmm& arg1);

  /** Move scalar single-precision floating-point value from xmm1 register to xmm2/m32. */
  void movss_1(const Xmm& arg0, const Xmm& arg1);

  /** For legacy mode, move word from address DS:(E)SI to ES:(E)DI. For 64-bit mode move word at address (R|E)SI to (R|E)DI. */
  void movsw();

  /** Move byte to word with sign-extension. */
  void movsx(const R16& arg0, const M8& arg1);

  /** Move byte to word with sign-extension. */
  void movsx(const R16& arg0, const R8& arg1);

  /** Move byte to word with sign-extension. */
  void movsx(const R16& arg0, const Rh& arg1);

  /** Move word to doubleword, with sign-extension. */
  void movsx(const R32& arg0, const M16& arg1);

  /** Move byte to doubleword with sign-extension. */
  void movsx(const R32& arg0, const M8& arg1);

  /** Move word to doubleword, with sign-extension. */
  void movsx(const R32& arg0, const R16& arg1);

  /** Move byte to doubleword with sign-extension. */
  void movsx(const R32& arg0, const R8& arg1);

  /** Move byte to doubleword with sign-extension. */
  void movsx(const R32& arg0, const Rh& arg1);

  /** Move word to quadword with sign-extension. */
  void movsx(const R64& arg0, const M16& arg1);

  /** Move byte to quadword with sign-extension. */
  void movsx(const R64& arg0, const M8& arg1);

  /** Move word to quadword with sign-extension. */
  void movsx(const R64& arg0, const R16& arg1);

  /** Move byte to quadword with sign-extension. */
  void movsx(const R64& arg0, const R8& arg1);

  /** Move doubleword to quadword with sign-extension. */
  void movsxd(const R64& arg0, const M32& arg1);

  /** Move doubleword to quadword with sign-extension. */
  void movsxd(const R64& arg0, const R32& arg1);

  /** Move packed double-precision floating-point values from xmm1 to xmm2/m128. */
  void movupd(const M128& arg0, const Xmm& arg1);

  /** Move packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void movupd(const Xmm& arg0, const M128& arg1);

  /** Move packed double-precision floating-point values from xmm2/m128 to xmm1. */
  void movupd(const Xmm& arg0, const Xmm& arg1);

  /** Move packed double-precision floating-point values from xmm1 to xmm2/m128. */
  void movupd_1(const Xmm& arg0, const Xmm& arg1);

  /** Move packed single-precision floating-point values from xmm1 to xmm2/m128. */
  void movups(const M128& arg0, const Xmm& arg1);

  /** Move packed single-precision floating-point values from xmm2/m128 to xmm1. */
  void movups(const Xmm& arg0, const M128& arg1);

  /** Move packed single-precision floating-point values from xmm2/m128 to xmm1. */
  void movups(const Xmm& arg0, const Xmm& arg1);

  /** Move packed single-precision floating-point values from xmm1 to xmm2/m128. */
  void movups_1(const Xmm& arg0, const Xmm& arg1);

  /** Move byte to word with zero-extension. */
  void movzx(const R16& arg0, const M8& arg1);

  /** Move byte to word with zero-extension. */
  void movzx(const R16& arg0, const R8& arg1);

  /** Move byte to word with zero-extension. */
  void movzx(const R16& arg0, const Rh& arg1);

  /** Move word to doubleword, zero-extension. */
  void movzx(const R32& arg0, const M16& arg1);

  /** Move byte to doubleword, zero-extension. */
  void movzx(const R32& arg0, const M8& arg1);

  /** Move word to doubleword, zero-extension. */
  void movzx(const R32& arg0, const R16& arg1);

  /** Move byte to doubleword, zero-extension. */
  void movzx(const R32& arg0, const R8& arg1);

  /** Move byte to doubleword, zero-extension. */
  void movzx(const R32& arg0, const Rh& arg1);

  /** Move word to quadword, zero-extension. */
  void movzx(const R64& arg0, const M16& arg1);

  /** Move byte to quadword, zero-extension. */
  void movzx(const R64& arg0, const M8& arg1);

  /** Move word to quadword, zero-extension. */
  void movzx(const R64& arg0, const R16& arg1);

  /** Move byte to quadword, zero-extension. */
  void movzx(const R64& arg0, const R8& arg1);

  /** Sums absolute 8-bit integer difference of adjacent groups of 4 byte integers in xmm1 and xmm2/m128 and writes the results in xmm1. Starting offsets within xmm1 and xmm2/m128 are determined by imm8. */
  void mpsadbw(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Sums absolute 8-bit integer difference of adjacent groups of 4 byte integers in xmm1 and xmm2/m128 and writes the results in xmm1. Starting offsets within xmm1 and xmm2/m128 are determined by imm8. */
  void mpsadbw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Unsigned multiply (DX:AX = AX * r/m16). */
  void mul(const M16& arg0);

  /** Unsigned multiply (EDX:EAX = EAX * r/m32). */
  void mul(const M32& arg0);

  /** Unsigned multiply (RDX:RAX = RAX * r/m64. */
  void mul(const M64& arg0);

  /** Unsigned multiply (AX = AL * r/m8). */
  void mul(const M8& arg0);

  /** Unsigned multiply (DX:AX = AX * r/m16). */
  void mul(const R16& arg0);

  /** Unsigned multiply (EDX:EAX = EAX * r/m32). */
  void mul(const R32& arg0);

  /** Unsigned multiply (RDX:RAX = RAX * r/m64. */
  void mul(const R64& arg0);

  /** Unsigned multiply (AX = AL * r/m8). */
  void mul(const R8& arg0);

  /** Unsigned multiply (AX = AL * r/m8). */
  void mul(const Rh& arg0);

  /** Multiply packed double-precision floating-point values in xmm2/m128 by xmm1. */
  void mulpd(const Xmm& arg0, const M128& arg1);

  /** Multiply packed double-precision floating-point values in xmm2/m128 by xmm1. */
  void mulpd(const Xmm& arg0, const Xmm& arg1);

  /** Multiply packed single-precision floating-point values in xmm2/mem by xmm1. */
  void mulps(const Xmm& arg0, const M128& arg1);

  /** Multiply packed single-precision floating-point values in xmm2/mem by xmm1. */
  void mulps(const Xmm& arg0, const Xmm& arg1);

  /** Multiply the low double-precision floating-point value in xmm2/mem64 by low double-precision floating-point value in xmm1. */
  void mulsd(const Xmm& arg0, const M64& arg1);

  /** Multiply the low double-precision floating-point value in xmm2/mem64 by low double-precision floating-point value in xmm1. */
  void mulsd(const Xmm& arg0, const Xmm& arg1);

  /** Multiply the low single-precision floating-point value in xmm2/mem by the low single-precision floating-point value in xmm1. */
  void mulss(const Xmm& arg0, const M32& arg1);

  /** Multiply the low single-precision floating-point value in xmm2/mem by the low single-precision floating-point value in xmm1. */
  void mulss(const Xmm& arg0, const Xmm& arg1);

  /** Unsigned multiply of r/m32 with EDX without affecting arithmetic flags */
  void mulx(const R32& arg0, const R32& arg1, const M32& arg2);

  /** Unsigned multiply of r/m32 with EDX without affecting arithmetic flags */
  void mulx(const R32& arg0, const R32& arg1, const R32& arg2);

  /** Unsigned multiply of r/m64 with RDX without affecting arithmetic flags */
  void mulx(const R64& arg0, const R64& arg1, const M64& arg2);

  /** Unsigned multiply of r/m64 with RDX without affecting arithmetic flags */
  void mulx(const R64& arg0, const R64& arg1, const R64& arg2);

  /** A hint that allow the processor to stop instruction execution and enter an implementation-dependent optimized state until occurrence of a class of events. */
  void mwait();

  /** Two's complement negate r/m16. */
  void neg(const M16& arg0);

  /** Two's complement negate r/m32. */
  void neg(const M32& arg0);

  /** Two's complement negate r/m64. */
  void neg(const M64& arg0);

  /** Two's complement negate r/m8. */
  void neg(const M8& arg0);

  /** Two's complement negate r/m16. */
  void neg(const R16& arg0);

  /** Two's complement negate r/m32. */
  void neg(const R32& arg0);

  /** Two's complement negate r/m64. */
  void neg(const R64& arg0);

  /** Two's complement negate r/m8. */
  void neg(const R8& arg0);

  /** Two's complement negate r/m8. */
  void neg(const Rh& arg0);

  /** One byte no-operation instruction. */
  void nop();

  /** Multi-byte no-operation instruction. */
  void nop(const M16& arg0);

  /** Multi-byte no-operation instruction. */
  void nop(const M32& arg0);

  /** Multi-byte no-operation instruction. */
  void nop(const R16& arg0);

  /** Multi-byte no-operation instruction. */
  void nop(const R32& arg0);

  /** Reverse each bit of r/m16. */
  void not_(const M16& arg0);

  /** Reverse each bit of r/m32. */
  void not_(const M32& arg0);

  /** Reverse each bit of r/m64. */
  void not_(const M64& arg0);

  /** Reverse each bit of r/m8. */
  void not_(const M8& arg0);

  /** Reverse each bit of r/m16. */
  void not_(const R16& arg0);

  /** Reverse each bit of r/m32. */
  void not_(const R32& arg0);

  /** Reverse each bit of r/m64. */
  void not_(const R64& arg0);

  /** Reverse each bit of r/m8. */
  void not_(const R8& arg0);

  /** Reverse each bit of r/m8. */
  void not_(const Rh& arg0);

  /** AL OR imm8. */
  void or_(const Al& arg0, const Imm8& arg1);

  /** AX OR imm16. */
  void or_(const Ax& arg0, const Imm16& arg1);

  /** EAX OR imm32. */
  void or_(const Eax& arg0, const Imm32& arg1);

  /** r/m16 OR imm16. */
  void or_(const M16& arg0, const Imm16& arg1);

  /** r/m16 OR imm8 (sign-extended). */
  void or_(const M16& arg0, const Imm8& arg1);

  /** r/m16 OR r16. */
  void or_(const M16& arg0, const R16& arg1);

  /** r/m32 OR imm32. */
  void or_(const M32& arg0, const Imm32& arg1);

  /** r/m32 OR imm8 (sign-extended). */
  void or_(const M32& arg0, const Imm8& arg1);

  /** r/m32 OR r32. */
  void or_(const M32& arg0, const R32& arg1);

  /** r/m64 OR imm32 (sign-extended). */
  void or_(const M64& arg0, const Imm32& arg1);

  /** r/m64 OR imm8 (sign-extended). */
  void or_(const M64& arg0, const Imm8& arg1);

  /** r/m64 OR r64. */
  void or_(const M64& arg0, const R64& arg1);

  /** r/m8 OR imm8. */
  void or_(const M8& arg0, const Imm8& arg1);

  /** r/m8 OR r8. */
  void or_(const M8& arg0, const R8& arg1);

  /** r/m8 OR r8. */
  void or_(const M8& arg0, const Rh& arg1);

  /** r/m16 OR imm16. */
  void or_(const R16& arg0, const Imm16& arg1);

  /** r/m16 OR imm8 (sign-extended). */
  void or_(const R16& arg0, const Imm8& arg1);

  /** r16 OR r/m16. */
  void or_(const R16& arg0, const M16& arg1);

  /** r/m16 OR r16. */
  void or_(const R16& arg0, const R16& arg1);

  /** r16 OR r/m16. */
  void or__1(const R16& arg0, const R16& arg1);

  /** r/m32 OR imm32. */
  void or_(const R32& arg0, const Imm32& arg1);

  /** r/m32 OR imm8 (sign-extended). */
  void or_(const R32& arg0, const Imm8& arg1);

  /** r32 OR r/m32. */
  void or_(const R32& arg0, const M32& arg1);

  /** r/m32 OR r32. */
  void or_(const R32& arg0, const R32& arg1);

  /** r32 OR r/m32. */
  void or__1(const R32& arg0, const R32& arg1);

  /** r/m64 OR imm32 (sign-extended). */
  void or_(const R64& arg0, const Imm32& arg1);

  /** r/m64 OR imm8 (sign-extended). */
  void or_(const R64& arg0, const Imm8& arg1);

  /** r64 OR r/m64. */
  void or_(const R64& arg0, const M64& arg1);

  /** r/m64 OR r64. */
  void or_(const R64& arg0, const R64& arg1);

  /** r64 OR r/m64. */
  void or__1(const R64& arg0, const R64& arg1);

  /** r/m8 OR imm8. */
  void or_(const R8& arg0, const Imm8& arg1);

  /** r8 OR r/m8. */
  void or_(const R8& arg0, const M8& arg1);

  /** r/m8 OR r8. */
  void or_(const R8& arg0, const R8& arg1);

  /** r8 OR r/m8. */
  void or__1(const R8& arg0, const R8& arg1);

  /** r/m8 OR r8. */
  void or_(const R8& arg0, const Rh& arg1);

  /** r8 OR r/m8. */
  void or__1(const R8& arg0, const Rh& arg1);

  /** RAX OR imm32 (sign-extended). */
  void or_(const Rax& arg0, const Imm32& arg1);

  /** r/m8 OR imm8. */
  void or_(const Rh& arg0, const Imm8& arg1);

  /** r8 OR r/m8. */
  void or_(const Rh& arg0, const M8& arg1);

  /** r/m8 OR r8. */
  void or_(const Rh& arg0, const R8& arg1);

  /** r8 OR r/m8. */
  void or__1(const Rh& arg0, const R8& arg1);

  /** r/m8 OR r8. */
  void or_(const Rh& arg0, const Rh& arg1);

  /** r8 OR r/m8. */
  void or__1(const Rh& arg0, const Rh& arg1);

  /** Bitwise OR of xmm2/m128 and xmm1. */
  void orpd(const Xmm& arg0, const M128& arg1);

  /** Bitwise OR of xmm2/m128 and xmm1. */
  void orpd(const Xmm& arg0, const Xmm& arg1);

  /** Bitwise OR of xmm1 and xmm2/m128. */
  void orps(const Xmm& arg0, const M128& arg1);

  /** Bitwise OR of xmm1 and xmm2/m128. */
  void orps(const Xmm& arg0, const Xmm& arg1);

  /** Output byte in AL to I/O port address in DX. */
  void out(const Dx& arg0, const Al& arg1);

  /** Output word in AX to I/O port address in DX. */
  void out(const Dx& arg0, const Ax& arg1);

  /** Output doubleword in EAX to I/O port address in DX. */
  void out(const Dx& arg0, const Eax& arg1);

  /** Output byte in AL to I/O port address imm8. */
  void out(const Imm8& arg0, const Al& arg1);

  /** Output word in AX to I/O port address imm8. */
  void out(const Imm8& arg0, const Ax& arg1);

  /** Output doubleword in EAX to I/O port address imm8. */
  void out(const Imm8& arg0, const Eax& arg1);

  /** Output word from memory location specified in DS:(E)SI or RSI to I/O port specified in DX. */
  void outs(const Dx& arg0, const M16& arg1);

  /** Output doubleword from memory location specified in DS:(E)SI or RSI to I/O port specified in DX. */
  void outs(const Dx& arg0, const M32& arg1);

  /** Output byte from memory location specified in DS:(E)SI or RSI to I/O port specified in DX. */
  void outs(const Dx& arg0, const M8& arg1);

  /** Output byte from memory location specified in DS:(E)SI or RSI to I/O port specified in DX. */
  void outsb();

  /** Output doubleword from memory location specified in DS:(E)SI or RSI to I/O port specified in DX. */
  void outsd();

  /** Output word from memory location specified in DS:(E)SI or RSI to I/O port specified in DX. */
  void outsw();

  /** Compute the absolute value of bytes in mm2/m64 and store UNSIGNED result in mm1. */
  void pabsb(const Mm& arg0, const M64& arg1);

  /** Compute the absolute value of bytes in mm2/m64 and store UNSIGNED result in mm1. */
  void pabsb(const Mm& arg0, const Mm& arg1);

  /** Compute the absolute value of bytes in xmm2/m128 and store UNSIGNED result in xmm1. */
  void pabsb(const Xmm& arg0, const M128& arg1);

  /** Compute the absolute value of bytes in xmm2/m128 and store UNSIGNED result in xmm1. */
  void pabsb(const Xmm& arg0, const Xmm& arg1);

  /** Compute the absolute value of 32-bit integers in mm2/m64 and store UNSIGNED result in mm1. */
  void pabsd(const Mm& arg0, const M64& arg1);

  /** Compute the absolute value of 32-bit integers in mm2/m64 and store UNSIGNED result in mm1. */
  void pabsd(const Mm& arg0, const Mm& arg1);

  /** Compute the absolute value of 32-bit integers in xmm2/m128 and store UNSIGNED result in xmm1. */
  void pabsd(const Xmm& arg0, const M128& arg1);

  /** Compute the absolute value of 32-bit integers in xmm2/m128 and store UNSIGNED result in xmm1. */
  void pabsd(const Xmm& arg0, const Xmm& arg1);

  /** Compute the absolute value of 16-bit integers in mm2/m64 and store UNSIGNED result in mm1. */
  void pabsw(const Mm& arg0, const M64& arg1);

  /** Compute the absolute value of 16-bit integers in mm2/m64 and store UNSIGNED result in mm1. */
  void pabsw(const Mm& arg0, const Mm& arg1);

  /** Compute the absolute value of 16-bit integers in xmm2/m128 and store UNSIGNED result in xmm1. */
  void pabsw(const Xmm& arg0, const M128& arg1);

  /** Compute the absolute value of 16-bit integers in xmm2/m128 and store UNSIGNED result in xmm1. */
  void pabsw(const Xmm& arg0, const Xmm& arg1);

  /** Converts 2 packed signed doubleword integers from mm1 and from mm2/m64 into 4 packed signed word integers in mm1 using signed saturation. */
  void packssdw(const Mm& arg0, const M64& arg1);

  /** Converts 2 packed signed doubleword integers from mm1 and from mm2/m64 into 4 packed signed word integers in mm1 using signed saturation. */
  void packssdw(const Mm& arg0, const Mm& arg1);

  /** Converts 4 packed signed doubleword integers from xmm1 and from xxm2/m128 into 8 packed signed word integers in xxm1 using signed saturation. */
  void packssdw(const Xmm& arg0, const M128& arg1);

  /** Converts 4 packed signed doubleword integers from xmm1 and from xxm2/m128 into 8 packed signed word integers in xxm1 using signed saturation. */
  void packssdw(const Xmm& arg0, const Xmm& arg1);

  /** Converts 4 packed signed word integers from mm1 and from mm2/m64 into 8 packed signed byte integers in mm1 using signed saturation. */
  void packsswb(const Mm& arg0, const M64& arg1);

  /** Converts 4 packed signed word integers from mm1 and from mm2/m64 into 8 packed signed byte integers in mm1 using signed saturation. */
  void packsswb(const Mm& arg0, const Mm& arg1);

  /** Converts 8 packed signed word integers from xmm1 and from xxm2/m128 into 16 packed signed byte integers in xxm1 using signed saturation. */
  void packsswb(const Xmm& arg0, const M128& arg1);

  /** Converts 8 packed signed word integers from xmm1 and from xxm2/m128 into 16 packed signed byte integers in xxm1 using signed saturation. */
  void packsswb(const Xmm& arg0, const Xmm& arg1);

  /** Convert 4 packed signed doubleword integers from xmm1 and 4 packed signed doubleword integers from xmm2/m128 into 8 packed unsigned word integers in xmm1 using unsigned saturation. */
  void packusdw(const Xmm& arg0, const M128& arg1);

  /** Convert 4 packed signed doubleword integers from xmm1 and 4 packed signed doubleword integers from xmm2/m128 into 8 packed unsigned word integers in xmm1 using unsigned saturation. */
  void packusdw(const Xmm& arg0, const Xmm& arg1);

  /** Converts 4 signed word integers from mm and 4 signed word integers from mm/m64 into 8 unsigned byte integers in mm using unsigned saturation. */
  void packuswb(const Mm& arg0, const M64& arg1);

  /** Converts 4 signed word integers from mm and 4 signed word integers from mm/m64 into 8 unsigned byte integers in mm using unsigned saturation. */
  void packuswb(const Mm& arg0, const Mm& arg1);

  /** Converts 8 signed word integers from xmm1 and 8 signed word integers from xmm2/m128 into 16 unsigned byte integers in xmm1 using unsigned saturation. */
  void packuswb(const Xmm& arg0, const M128& arg1);

  /** Converts 8 signed word integers from xmm1 and 8 signed word integers from xmm2/m128 into 16 unsigned byte integers in xmm1 using unsigned saturation. */
  void packuswb(const Xmm& arg0, const Xmm& arg1);

  /** Add packed byte integers from mm/m64 and mm. */
  void paddb(const Mm& arg0, const M64& arg1);

  /** Add packed byte integers from mm/m64 and mm. */
  void paddb(const Mm& arg0, const Mm& arg1);

  /** Add packed byte integers from xmm2/m128 and xmm1. */
  void paddb(const Xmm& arg0, const M128& arg1);

  /** Add packed byte integers from xmm2/m128 and xmm1. */
  void paddb(const Xmm& arg0, const Xmm& arg1);

  /** Add packed doubleword integers from mm/m64 and mm. */
  void paddd(const Mm& arg0, const M64& arg1);

  /** Add packed doubleword integers from mm/m64 and mm. */
  void paddd(const Mm& arg0, const Mm& arg1);

  /** Add packed doubleword integers from xmm2/m128 and xmm1. */
  void paddd(const Xmm& arg0, const M128& arg1);

  /** Add packed doubleword integers from xmm2/m128 and xmm1. */
  void paddd(const Xmm& arg0, const Xmm& arg1);

  /** Add quadword integer mm2/m64 to mm1. */
  void paddq(const Mm& arg0, const M64& arg1);

  /** Add quadword integer mm2/m64 to mm1. */
  void paddq(const Mm& arg0, const Mm& arg1);

  /** Add packed quadword integers xmm2/m128 to xmm1. */
  void paddq(const Xmm& arg0, const M128& arg1);

  /** Add packed quadword integers xmm2/m128 to xmm1. */
  void paddq(const Xmm& arg0, const Xmm& arg1);

  /** Add packed signed byte integers from mm/m64 and mm and saturate the results. */
  void paddsb(const Mm& arg0, const M64& arg1);

  /** Add packed signed byte integers from mm/m64 and mm and saturate the results. */
  void paddsb(const Mm& arg0, const Mm& arg1);

  /** Add packed signed byte integers from xmm2/m128 and xmm1 saturate the results. */
  void paddsb(const Xmm& arg0, const M128& arg1);

  /** Add packed signed byte integers from xmm2/m128 and xmm1 saturate the results. */
  void paddsb(const Xmm& arg0, const Xmm& arg1);

  /** Add packed signed word integers from mm/m64 and mm and saturate the results. */
  void paddsw(const Mm& arg0, const M64& arg1);

  /** Add packed signed word integers from mm/m64 and mm and saturate the results. */
  void paddsw(const Mm& arg0, const Mm& arg1);

  /** Add packed signed word integers from xmm2/m128 and xmm1 and saturate the results. */
  void paddsw(const Xmm& arg0, const M128& arg1);

  /** Add packed signed word integers from xmm2/m128 and xmm1 and saturate the results. */
  void paddsw(const Xmm& arg0, const Xmm& arg1);

  /** Add packed unsigned byte integers from mm/m64 and mm and saturate the results. */
  void paddusb(const Mm& arg0, const M64& arg1);

  /** Add packed unsigned byte integers from mm/m64 and mm and saturate the results. */
  void paddusb(const Mm& arg0, const Mm& arg1);

  /** Add packed unsigned byte integers from xmm2/m128 and xmm1 saturate the results. */
  void paddusb(const Xmm& arg0, const M128& arg1);

  /** Add packed unsigned byte integers from xmm2/m128 and xmm1 saturate the results. */
  void paddusb(const Xmm& arg0, const Xmm& arg1);

  /** Add packed unsigned word integers from mm/m64 and mm and saturate the results. */
  void paddusw(const Mm& arg0, const M64& arg1);

  /** Add packed unsigned word integers from mm/m64 and mm and saturate the results. */
  void paddusw(const Mm& arg0, const Mm& arg1);

  /** Add packed unsigned word integers from xmm2/m128 to xmm1 and saturate the results. */
  void paddusw(const Xmm& arg0, const M128& arg1);

  /** Add packed unsigned word integers from xmm2/m128 to xmm1 and saturate the results. */
  void paddusw(const Xmm& arg0, const Xmm& arg1);

  /** Add packed word integers from mm/m64 and mm. */
  void paddw(const Mm& arg0, const M64& arg1);

  /** Add packed word integers from mm/m64 and mm. */
  void paddw(const Mm& arg0, const Mm& arg1);

  /** Add packed word integers from xmm2/m128 and xmm1. */
  void paddw(const Xmm& arg0, const M128& arg1);

  /** Add packed word integers from xmm2/m128 and xmm1. */
  void paddw(const Xmm& arg0, const Xmm& arg1);

  /** Concatenate destination and source operands, extract byte-aligned result shifted to the right by constant value in imm8 into mm1. */
  void palignr(const Mm& arg0, const M64& arg1, const Imm8& arg2);

  /** Concatenate destination and source operands, extract byte-aligned result shifted to the right by constant value in imm8 into mm1. */
  void palignr(const Mm& arg0, const Mm& arg1, const Imm8& arg2);

  /** Concatenate destination and source operands, extract byte-aligned result shifted to the right by constant value in imm8 into xmm1 */
  void palignr(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Concatenate destination and source operands, extract byte-aligned result shifted to the right by constant value in imm8 into xmm1 */
  void palignr(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Bitwise AND mm/m64 and mm. */
  void pand(const Mm& arg0, const M64& arg1);

  /** Bitwise AND mm/m64 and mm. */
  void pand(const Mm& arg0, const Mm& arg1);

  /** Bitwise AND of xmm2/m128 and xmm1. */
  void pand(const Xmm& arg0, const M128& arg1);

  /** Bitwise AND of xmm2/m128 and xmm1. */
  void pand(const Xmm& arg0, const Xmm& arg1);

  /** Bitwise AND NOT of mm/m64 and mm. */
  void pandn(const Mm& arg0, const M64& arg1);

  /** Bitwise AND NOT of mm/m64 and mm. */
  void pandn(const Mm& arg0, const Mm& arg1);

  /** Bitwise AND NOT of xmm2/m128 and xmm1. */
  void pandn(const Xmm& arg0, const M128& arg1);

  /** Bitwise AND NOT of xmm2/m128 and xmm1. */
  void pandn(const Xmm& arg0, const Xmm& arg1);

  /** Gives hint to processor that improves performance of spin-wait loops. */
  void pause();

  /** Average packed unsigned byte integers from mm2/m64 and mm1 with rounding. */
  void pavgb(const Mm& arg0, const M64& arg1);

  /** Average packed unsigned byte integers from mm2/m64 and mm1 with rounding. */
  void pavgb(const Mm& arg0, const Mm& arg1);

  /** Average packed unsigned byte integers from xmm2/m128 and xmm1 with rounding. */
  void pavgb(const Xmm& arg0, const M128& arg1);

  /** Average packed unsigned byte integers from xmm2/m128 and xmm1 with rounding. */
  void pavgb(const Xmm& arg0, const Xmm& arg1);

  /** Average packed unsigned word integers from mm2/m64 and mm1 with rounding. */
  void pavgw(const Mm& arg0, const M64& arg1);

  /** Average packed unsigned word integers from mm2/m64 and mm1 with rounding. */
  void pavgw(const Mm& arg0, const Mm& arg1);

  /** Average packed unsigned word integers from xmm2/m128 and xmm1 with rounding. */
  void pavgw(const Xmm& arg0, const M128& arg1);

  /** Average packed unsigned word integers from xmm2/m128 and xmm1 with rounding. */
  void pavgw(const Xmm& arg0, const Xmm& arg1);

  /** Select byte values from xmm1 and xmm2/m128 from mask specified in the high bit of each byte in XMM0 and store the values into xmm1. */
  void pblendvb(const Xmm& arg0, const M128& arg1, const Xmm0& arg2);

  /** Select byte values from xmm1 and xmm2/m128 from mask specified in the high bit of each byte in XMM0 and store the values into xmm1. */
  void pblendvb(const Xmm& arg0, const Xmm& arg1, const Xmm0& arg2);

  /** Select words from xmm1 and xmm2/m128 from mask specified in imm8 and store the values into xmm1. */
  void pblendw(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Select words from xmm1 and xmm2/m128 from mask specified in imm8 and store the values into xmm1. */
  void pblendw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Carry-less multiplication of one quadword of xmm1 by one quadword of xmm2/m128, stores the 128-bit result in xmm1. The immediate is used to determine which quadwords of xmm1 and xmm2/m128 should be used. */
  void pclmulqdq(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Carry-less multiplication of one quadword of xmm1 by one quadword of xmm2/m128, stores the 128-bit result in xmm1. The immediate is used to determine which quadwords of xmm1 and xmm2/m128 should be used. */
  void pclmulqdq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Compare packed bytes in mm/m64 and mm for equality. */
  void pcmpeqb(const Mm& arg0, const M64& arg1);

  /** Compare packed bytes in mm/m64 and mm for equality. */
  void pcmpeqb(const Mm& arg0, const Mm& arg1);

  /** Compare packed bytes in xmm2/m128 and xmm1 for equality. */
  void pcmpeqb(const Xmm& arg0, const M128& arg1);

  /** Compare packed bytes in xmm2/m128 and xmm1 for equality. */
  void pcmpeqb(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed doublewords in mm/m64 and mm for equality. */
  void pcmpeqd(const Mm& arg0, const M64& arg1);

  /** Compare packed doublewords in mm/m64 and mm for equality. */
  void pcmpeqd(const Mm& arg0, const Mm& arg1);

  /** Compare packed doublewords in xmm2/m128 and xmm1 for equality. */
  void pcmpeqd(const Xmm& arg0, const M128& arg1);

  /** Compare packed doublewords in xmm2/m128 and xmm1 for equality. */
  void pcmpeqd(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed qwords in xmm2/m128 and xmm1 for equality. */
  void pcmpeqq(const Xmm& arg0, const M128& arg1);

  /** Compare packed qwords in xmm2/m128 and xmm1 for equality. */
  void pcmpeqq(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed words in mm/m64 and mm for equality. */
  void pcmpeqw(const Mm& arg0, const M64& arg1);

  /** Compare packed words in mm/m64 and mm for equality. */
  void pcmpeqw(const Mm& arg0, const Mm& arg1);

  /** Compare packed words in xmm2/m128 and xmm1 for equality. */
  void pcmpeqw(const Xmm& arg0, const M128& arg1);

  /** Compare packed words in xmm2/m128 and xmm1 for equality. */
  void pcmpeqw(const Xmm& arg0, const Xmm& arg1);

  /** Perform a packed comparison of string data with explicit lengths, generating an index, and storing the result in ECX. */
  void pcmpestri(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with explicit lengths, generating an index, and storing the result in ECX. */
  void pcmpestri(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with explicit lengths, generating a mask, and storing the result in XMM0 */
  void pcmpestrm(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with explicit lengths, generating a mask, and storing the result in XMM0 */
  void pcmpestrm(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Compare packed signed byte integers in mm and mm/m64 for greater than. */
  void pcmpgtb(const Mm& arg0, const M64& arg1);

  /** Compare packed signed byte integers in mm and mm/m64 for greater than. */
  void pcmpgtb(const Mm& arg0, const Mm& arg1);

  /** Compare packed signed byte integers in xmm1 and xmm2/m128 for greater than. */
  void pcmpgtb(const Xmm& arg0, const M128& arg1);

  /** Compare packed signed byte integers in xmm1 and xmm2/m128 for greater than. */
  void pcmpgtb(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed signed doubleword integers in mm and mm/m64 for greater than. */
  void pcmpgtd(const Mm& arg0, const M64& arg1);

  /** Compare packed signed doubleword integers in mm and mm/m64 for greater than. */
  void pcmpgtd(const Mm& arg0, const Mm& arg1);

  /** Compare packed signed doubleword integers in xmm1 and xmm2/m128 for greater than. */
  void pcmpgtd(const Xmm& arg0, const M128& arg1);

  /** Compare packed signed doubleword integers in xmm1 and xmm2/m128 for greater than. */
  void pcmpgtd(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed signed qwords in xmm2/m128 and xmm1 for greater than. */
  void pcmpgtq(const Xmm& arg0, const M128& arg1);

  /** Compare packed signed qwords in xmm2/m128 and xmm1 for greater than. */
  void pcmpgtq(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed signed word integers in mm and mm/m64 for greater than. */
  void pcmpgtw(const Mm& arg0, const M64& arg1);

  /** Compare packed signed word integers in mm and mm/m64 for greater than. */
  void pcmpgtw(const Mm& arg0, const Mm& arg1);

  /** Compare packed signed word integers in xmm1 and xmm2/m128 for greater than. */
  void pcmpgtw(const Xmm& arg0, const M128& arg1);

  /** Compare packed signed word integers in xmm1 and xmm2/m128 for greater than. */
  void pcmpgtw(const Xmm& arg0, const Xmm& arg1);

  /** Perform a packed comparison of string data with implicit lengths, generating an index, and storing the result in ECX. */
  void pcmpistri(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with implicit lengths, generating an index, and storing the result in ECX. */
  void pcmpistri(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with implicit lengths, generating a mask, and storing the result in XMM0. */
  void pcmpistrm(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with implicit lengths, generating a mask, and storing the result in XMM0. */
  void pcmpistrm(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Parallel deposit of bits from r32b using mask in r/m32, result is written to r32a. */
  void pdep(const R32& arg0, const R32& arg1, const M32& arg2);

  /** Parallel deposit of bits from r32b using mask in r/m32, result is written to r32a. */
  void pdep(const R32& arg0, const R32& arg1, const R32& arg2);

  /** Parallel deposit of bits from r64b using mask in r/m64, result is written to r64a. */
  void pdep(const R64& arg0, const R64& arg1, const M64& arg2);

  /** Parallel deposit of bits from r64b using mask in r/m64, result is written to r64a. */
  void pdep(const R64& arg0, const R64& arg1, const R64& arg2);

  /** Parallel extract of bits from r32b using mask in r/m32, result is written to r32a. */
  void pext(const R32& arg0, const R32& arg1, const M32& arg2);

  /** Parallel extract of bits from r32b using mask in r/m32, result is written to r32a. */
  void pext(const R32& arg0, const R32& arg1, const R32& arg2);

  /** Parallel extract of bits from r64b using mask in r/m64, result is written to r64a. */
  void pext(const R64& arg0, const R64& arg1, const M64& arg2);

  /** Parallel extract of bits from r64b using mask in r/m64, result is written to r64a. */
  void pext(const R64& arg0, const R64& arg1, const R64& arg2);

  /** Extract a byte integer value from xmm2 at the source byte offset specified by imm8 into rreg or m8. The upper bits of r32 or r64 are zeroed. */
  void pextrb(const M8& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a byte integer value from xmm2 at the source byte offset specified by imm8 into rreg or m8. The upper bits of r32 or r64 are zeroed. */
  void pextrb(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a byte integer value from xmm2 at the source byte offset specified by imm8 into rreg or m8. The upper bits of r32 or r64 are zeroed. */
  void pextrb(const R64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a dword integer value from xmm2 at the source dword offset specified by imm8 into r/m32. */
  void pextrd(const M32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a dword integer value from xmm2 at the source dword offset specified by imm8 into r/m32. */
  void pextrd(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a qword integer value from xmm2 at the source qword offset specified by imm8 into r/m64. */
  void pextrq(const M64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a qword integer value from xmm2 at the source qword offset specified by imm8 into r/m64. */
  void pextrq(const R64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract the word specified by imm8 from xmm and copy it to lowest 16 bits of reg or m16. Zero-extend the result in the destination, r32 or r64. */
  void pextrw(const M16& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract the word specified by imm8 from mm and move it to reg, bits 15-0. The upper bits of r32 or r64 is zeroed. */
  void pextrw(const R32& arg0, const Mm& arg1, const Imm8& arg2);

  /** Extract the word specified by imm8 from xmm and move it to reg, bits 15-0. The upper bits of r32 or r64 is zeroed. */
  void pextrw(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract the word specified by imm8 from xmm and copy it to lowest 16 bits of reg or m16. Zero-extend the result in the destination, r32 or r64. */
  void pextrw_1(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract the word specified by imm8 from mm and move it to reg, bits 15-0. The upper bits of r32 or r64 is zeroed. */
  void pextrw(const R64& arg0, const Mm& arg1, const Imm8& arg2);

  /** Extract the word specified by imm8 from xmm and move it to reg, bits 15-0. The upper bits of r32 or r64 is zeroed. */
  void pextrw(const R64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract the word specified by imm8 from xmm and copy it to lowest 16 bits of reg or m16. Zero-extend the result in the destination, r32 or r64. */
  void pextrw_1(const R64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Add 32-bit integers horizontally, pack to MM1. */
  void phaddd(const Mm& arg0, const M64& arg1);

  /** Add 32-bit integers horizontally, pack to MM1. */
  void phaddd(const Mm& arg0, const Mm& arg1);

  /** Add 32-bit integers horizontally, pack to XMM1. */
  void phaddd(const Xmm& arg0, const M128& arg1);

  /** Add 32-bit integers horizontally, pack to XMM1. */
  void phaddd(const Xmm& arg0, const Xmm& arg1);

  /** Add 16-bit signed integers horizontally, pack saturated integers to MM1. */
  void phaddsw(const Mm& arg0, const M64& arg1);

  /** Add 16-bit signed integers horizontally, pack saturated integers to MM1. */
  void phaddsw(const Mm& arg0, const Mm& arg1);

  /** Add 16-bit signed integers horizontally, pack saturated integers to XMM1. */
  void phaddsw(const Xmm& arg0, const M128& arg1);

  /** Add 16-bit signed integers horizontally, pack saturated integers to XMM1. */
  void phaddsw(const Xmm& arg0, const Xmm& arg1);

  /** Add 16-bit integers horizontally, pack to MM1. */
  void phaddw(const Mm& arg0, const M64& arg1);

  /** Add 16-bit integers horizontally, pack to MM1. */
  void phaddw(const Mm& arg0, const Mm& arg1);

  /** Add 16-bit integers horizontally, pack to XMM1. */
  void phaddw(const Xmm& arg0, const M128& arg1);

  /** Add 16-bit integers horizontally, pack to XMM1. */
  void phaddw(const Xmm& arg0, const Xmm& arg1);

  /** Find the minimum unsigned word in xmm2/m128 and place its value in the low word of xmm1 and its index in the second-lowest word of xmm1. */
  void phminposuw(const Xmm& arg0, const M128& arg1);

  /** Find the minimum unsigned word in xmm2/m128 and place its value in the low word of xmm1 and its index in the second-lowest word of xmm1. */
  void phminposuw(const Xmm& arg0, const Xmm& arg1);

  /** Subtract 32-bit signed integers horizontally, pack to MM1. */
  void phsubd(const Mm& arg0, const M64& arg1);

  /** Subtract 32-bit signed integers horizontally, pack to MM1. */
  void phsubd(const Mm& arg0, const Mm& arg1);

  /** Subtract 32-bit signed integers horizontally, pack to XMM1 */
  void phsubd(const Xmm& arg0, const M128& arg1);

  /** Subtract 32-bit signed integers horizontally, pack to XMM1 */
  void phsubd(const Xmm& arg0, const Xmm& arg1);

  /** Subtract 16-bit signed integer horizontally, pack saturated integers to MM1. */
  void phsubsw(const Mm& arg0, const M64& arg1);

  /** Subtract 16-bit signed integer horizontally, pack saturated integers to MM1. */
  void phsubsw(const Mm& arg0, const Mm& arg1);

  /** Subtract 16-bit signed integer horizontally, pack saturated integers to XMM1 */
  void phsubsw(const Xmm& arg0, const M128& arg1);

  /** Subtract 16-bit signed integer horizontally, pack saturated integers to XMM1 */
  void phsubsw(const Xmm& arg0, const Xmm& arg1);

  /** Subtract 16-bit signed integers horizontally, pack to MM1. */
  void phsubw(const Mm& arg0, const M64& arg1);

  /** Subtract 16-bit signed integers horizontally, pack to MM1. */
  void phsubw(const Mm& arg0, const Mm& arg1);

  /** Subtract 16-bit signed integers horizontally, pack to XMM1. */
  void phsubw(const Xmm& arg0, const M128& arg1);

  /** Subtract 16-bit signed integers horizontally, pack to XMM1. */
  void phsubw(const Xmm& arg0, const Xmm& arg1);

  /** Insert a byte integer value from r32/m8 into xmm1 at the destination element in xmm1 specified by imm8. */
  void pinsrb(const Xmm& arg0, const M8& arg1, const Imm8& arg2);

  /** Insert a byte integer value from r32/m8 into xmm1 at the destination element in xmm1 specified by imm8. */
  void pinsrb(const Xmm& arg0, const R32& arg1, const Imm8& arg2);

  /** Insert a dword integer value from r/m32 into the xmm1 at the destination element specified by imm8. */
  void pinsrd(const Xmm& arg0, const M32& arg1, const Imm8& arg2);

  /** Insert a dword integer value from r/m32 into the xmm1 at the destination element specified by imm8. */
  void pinsrd(const Xmm& arg0, const R32& arg1, const Imm8& arg2);

  /** Insert the low word from r32 or from m16 into mm at the word position specified by imm8 */
  void pinsrw(const Mm& arg0, const M16& arg1, const Imm8& arg2);

  /** Insert the low word from r32 or from m16 into mm at the word position specified by imm8 */
  void pinsrw(const Mm& arg0, const R32& arg1, const Imm8& arg2);

  /** Move the low word of r32 or from m16 into xmm at the word position specified by imm8. */
  void pinsrw(const Xmm& arg0, const M16& arg1, const Imm8& arg2);

  /** Move the low word of r32 or from m16 into xmm at the word position specified by imm8. */
  void pinsrw(const Xmm& arg0, const R32& arg1, const Imm8& arg2);

  /** Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to MM1. */
  void pmaddubsw(const Mm& arg0, const M64& arg1);

  /** Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to MM1. */
  void pmaddubsw(const Mm& arg0, const Mm& arg1);

  /** Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to XMM1. */
  void pmaddubsw(const Xmm& arg0, const M128& arg1);

  /** Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to XMM1. */
  void pmaddubsw(const Xmm& arg0, const Xmm& arg1);

  /** Multiply the packed words in mm by the packed words in mm/m64, add adjacent doubleword results, and store in mm. */
  void pmaddwd(const Mm& arg0, const M64& arg1);

  /** Multiply the packed words in mm by the packed words in mm/m64, add adjacent doubleword results, and store in mm. */
  void pmaddwd(const Mm& arg0, const Mm& arg1);

  /** Multiply the packed word integers in xmm1 by the packed word integers in xmm2/m128, add adjacent doubleword results, and store in xmm1. */
  void pmaddwd(const Xmm& arg0, const M128& arg1);

  /** Multiply the packed word integers in xmm1 by the packed word integers in xmm2/m128, add adjacent doubleword results, and store in xmm1. */
  void pmaddwd(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed signed byte integers in xmm1 and xmm2/m128 and store packed maximum values in xmm1. */
  void pmaxsb(const Xmm& arg0, const M128& arg1);

  /** Compare packed signed byte integers in xmm1 and xmm2/m128 and store packed maximum values in xmm1. */
  void pmaxsb(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed signed dword integers in xmm1 and xmm2/m128 and store packed maximum values in xmm1. */
  void pmaxsd(const Xmm& arg0, const M128& arg1);

  /** Compare packed signed dword integers in xmm1 and xmm2/m128 and store packed maximum values in xmm1. */
  void pmaxsd(const Xmm& arg0, const Xmm& arg1);

  /** Compare signed word integers in mm2/m64 and mm1 and return maximum values. */
  void pmaxsw(const Mm& arg0, const M64& arg1);

  /** Compare signed word integers in mm2/m64 and mm1 and return maximum values. */
  void pmaxsw(const Mm& arg0, const Mm& arg1);

  /** Compare signed word integers in xmm2/m128 and xmm1 and return maximum values. */
  void pmaxsw(const Xmm& arg0, const M128& arg1);

  /** Compare signed word integers in xmm2/m128 and xmm1 and return maximum values. */
  void pmaxsw(const Xmm& arg0, const Xmm& arg1);

  /** Compare unsigned byte integers in mm2/m64 and mm1 and returns maximum values. */
  void pmaxub(const Mm& arg0, const M64& arg1);

  /** Compare unsigned byte integers in mm2/m64 and mm1 and returns maximum values. */
  void pmaxub(const Mm& arg0, const Mm& arg1);

  /** Compare unsigned byte integers in xmm2/m128 and xmm1 and returns maximum values. */
  void pmaxub(const Xmm& arg0, const M128& arg1);

  /** Compare unsigned byte integers in xmm2/m128 and xmm1 and returns maximum values. */
  void pmaxub(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed unsigned dword integers in xmm1 and xmm2/m128 and store packed maximum values in xmm1. */
  void pmaxud(const Xmm& arg0, const M128& arg1);

  /** Compare packed unsigned dword integers in xmm1 and xmm2/m128 and store packed maximum values in xmm1. */
  void pmaxud(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed unsigned word integers in xmm1 and xmm2/m128 and store packed maximum values in xmm1. */
  void pmaxuw(const Xmm& arg0, const M128& arg1);

  /** Compare packed unsigned word integers in xmm1 and xmm2/m128 and store packed maximum values in xmm1. */
  void pmaxuw(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed signed byte integers in xmm1 and xmm2/m128 and store packed minimum values in xmm1. */
  void pminsb(const Xmm& arg0, const M128& arg1);

  /** Compare packed signed byte integers in xmm1 and xmm2/m128 and store packed minimum values in xmm1. */
  void pminsb(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed signed dword integers in xmm1 and xmm2/m128 and store packed minimum values in xmm1. */
  void pminsd(const Xmm& arg0, const M128& arg1);

  /** Compare packed signed dword integers in xmm1 and xmm2/m128 and store packed minimum values in xmm1. */
  void pminsd(const Xmm& arg0, const Xmm& arg1);

  /** Compare signed word integers in mm2/m64 and mm1 and return minimum values. */
  void pminsw(const Mm& arg0, const M64& arg1);

  /** Compare signed word integers in mm2/m64 and mm1 and return minimum values. */
  void pminsw(const Mm& arg0, const Mm& arg1);

  /** Compare signed word integers in xmm2/m128 and xmm1 and return minimum values. */
  void pminsw(const Xmm& arg0, const M128& arg1);

  /** Compare signed word integers in xmm2/m128 and xmm1 and return minimum values. */
  void pminsw(const Xmm& arg0, const Xmm& arg1);

  /** Compare unsigned byte integers in mm2/m64 and mm1 and returns minimum values. */
  void pminub(const Mm& arg0, const M64& arg1);

  /** Compare unsigned byte integers in mm2/m64 and mm1 and returns minimum values. */
  void pminub(const Mm& arg0, const Mm& arg1);

  /** Compare unsigned byte integers in xmm2/m128 and xmm1 and returns minimum values. */
  void pminub(const Xmm& arg0, const M128& arg1);

  /** Compare unsigned byte integers in xmm2/m128 and xmm1 and returns minimum values. */
  void pminub(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed unsigned dword integers in xmm1 and xmm2/m128 and store packed minimum values in xmm1. */
  void pminud(const Xmm& arg0, const M128& arg1);

  /** Compare packed unsigned dword integers in xmm1 and xmm2/m128 and store packed minimum values in xmm1. */
  void pminud(const Xmm& arg0, const Xmm& arg1);

  /** Compare packed unsigned word integers in xmm1 and xmm2/m128 and store packed minimum values in xmm1. */
  void pminuw(const Xmm& arg0, const M128& arg1);

  /** Compare packed unsigned word integers in xmm1 and xmm2/m128 and store packed minimum values in xmm1. */
  void pminuw(const Xmm& arg0, const Xmm& arg1);

  /** Move a byte mask of mm to reg. The upper bits of r32 or r64 are zeroed */
  void pmovmskb(const R32& arg0, const Mm& arg1);

  /** Move a byte mask of xmm to reg. The upper bits of r32 or r64 are zeroed */
  void pmovmskb(const R32& arg0, const Xmm& arg1);

  /** Move a byte mask of mm to reg. The upper bits of r32 or r64 are zeroed */
  void pmovmskb(const R64& arg0, const Mm& arg1);

  /** Move a byte mask of xmm to reg. The upper bits of r32 or r64 are zeroed */
  void pmovmskb(const R64& arg0, const Xmm& arg1);

  /** Sign extend 4 packed signed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed signed 32-bit integers in xmm1. */
  void pmovsxbd(const Xmm& arg0, const M32& arg1);

  /** Sign extend 4 packed signed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed signed 32-bit integers in xmm1. */
  void pmovsxbd(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 2 packed signed 8-bit integers in the low 2 bytes of xmm2/m16 to 2 packed signed 64-bit integers in xmm1. */
  void pmovsxbq(const Xmm& arg0, const M16& arg1);

  /** Sign extend 2 packed signed 8-bit integers in the low 2 bytes of xmm2/m16 to 2 packed signed 64-bit integers in xmm1. */
  void pmovsxbq(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 8 packed signed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed signed 16-bit integers in xmm1. */
  void pmovsxbw(const Xmm& arg0, const M64& arg1);

  /** Sign extend 8 packed signed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed signed 16-bit integers in xmm1. */
  void pmovsxbw(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 2 packed signed 32-bit integers in the low 8 bytes of xmm2/m64 to 2 packed signed 64-bit integers in xmm1. */
  void pmovsxdq(const Xmm& arg0, const M64& arg1);

  /** Sign extend 2 packed signed 32-bit integers in the low 8 bytes of xmm2/m64 to 2 packed signed 64-bit integers in xmm1. */
  void pmovsxdq(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 4 packed signed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed signed 32-bit integers in xmm1. */
  void pmovsxwd(const Xmm& arg0, const M64& arg1);

  /** Sign extend 4 packed signed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed signed 32-bit integers in xmm1. */
  void pmovsxwd(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 2 packed signed 16-bit integers in the low 4 bytes of xmm2/m32 to 2 packed signed 64-bit integers in xmm1. */
  void pmovsxwq(const Xmm& arg0, const M32& arg1);

  /** Sign extend 2 packed signed 16-bit integers in the low 4 bytes of xmm2/m32 to 2 packed signed 64-bit integers in xmm1. */
  void pmovsxwq(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 32-bit integers in xmm1. */
  void pmovzxbd(const Xmm& arg0, const M32& arg1);

  /** Zero extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 32-bit integers in xmm1. */
  void pmovzxbd(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 2 packed 8-bit integers in the low 2 bytes of xmm2/m16 to 2 packed 64-bit integers in xmm1. */
  void pmovzxbq(const Xmm& arg0, const M16& arg1);

  /** Zero extend 2 packed 8-bit integers in the low 2 bytes of xmm2/m16 to 2 packed 64-bit integers in xmm1. */
  void pmovzxbq(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 16-bit integers in xmm1. */
  void pmovzxbw(const Xmm& arg0, const M64& arg1);

  /** Zero extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 16-bit integers in xmm1. */
  void pmovzxbw(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 2 packed 32-bit integers in the low 8 bytes of xmm2/m64 to 2 packed 64-bit integers in xmm1. */
  void pmovzxdq(const Xmm& arg0, const M64& arg1);

  /** Zero extend 2 packed 32-bit integers in the low 8 bytes of xmm2/m64 to 2 packed 64-bit integers in xmm1. */
  void pmovzxdq(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 32-bit integers in xmm1. */
  void pmovzxwd(const Xmm& arg0, const M64& arg1);

  /** Zero extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 32-bit integers in xmm1. */
  void pmovzxwd(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 2 packed 16-bit integers in the low 4 bytes of xmm2/m32 to 2 packed 64-bit integers in xmm1. */
  void pmovzxwq(const Xmm& arg0, const M32& arg1);

  /** Zero extend 2 packed 16-bit integers in the low 4 bytes of xmm2/m32 to 2 packed 64-bit integers in xmm1. */
  void pmovzxwq(const Xmm& arg0, const Xmm& arg1);

  /** Multiply the packed signed dword integers in xmm1 and xmm2/m128 and store the quadword product in xmm1. */
  void pmuldq(const Xmm& arg0, const M128& arg1);

  /** Multiply the packed signed dword integers in xmm1 and xmm2/m128 and store the quadword product in xmm1. */
  void pmuldq(const Xmm& arg0, const Xmm& arg1);

  /** Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to MM1. */
  void pmulhrsw(const Mm& arg0, const M64& arg1);

  /** Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to MM1. */
  void pmulhrsw(const Mm& arg0, const Mm& arg1);

  /** Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to XMM1. */
  void pmulhrsw(const Xmm& arg0, const M128& arg1);

  /** Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to XMM1. */
  void pmulhrsw(const Xmm& arg0, const Xmm& arg1);

  /** Multiply the packed unsigned word integers in mm1 register and mm2/m64, and store the high 16 bits of the results in mm1. */
  void pmulhuw(const Mm& arg0, const M64& arg1);

  /** Multiply the packed unsigned word integers in mm1 register and mm2/m64, and store the high 16 bits of the results in mm1. */
  void pmulhuw(const Mm& arg0, const Mm& arg1);

  /** Multiply the packed unsigned word integers in xmm1 and xmm2/m128, and store the high 16 bits of the results in xmm1. */
  void pmulhuw(const Xmm& arg0, const M128& arg1);

  /** Multiply the packed unsigned word integers in xmm1 and xmm2/m128, and store the high 16 bits of the results in xmm1. */
  void pmulhuw(const Xmm& arg0, const Xmm& arg1);

  /** Multiply the packed signed word integers in mm1 register and mm2/m64, and store the high 16 bits of the results in mm1. */
  void pmulhw(const Mm& arg0, const M64& arg1);

  /** Multiply the packed signed word integers in mm1 register and mm2/m64, and store the high 16 bits of the results in mm1. */
  void pmulhw(const Mm& arg0, const Mm& arg1);

  /** Multiply the packed signed word integers in xmm1 and xmm2/m128, and store the high 16 bits of the results in xmm1. */
  void pmulhw(const Xmm& arg0, const M128& arg1);

  /** Multiply the packed signed word integers in xmm1 and xmm2/m128, and store the high 16 bits of the results in xmm1. */
  void pmulhw(const Xmm& arg0, const Xmm& arg1);

  /** Multiply the packed dword signed integers in xmm1 and xmm2/m128 and store the low 32 bits of each product in xmm1. */
  void pmulld(const Xmm& arg0, const M128& arg1);

  /** Multiply the packed dword signed integers in xmm1 and xmm2/m128 and store the low 32 bits of each product in xmm1. */
  void pmulld(const Xmm& arg0, const Xmm& arg1);

  /** Multiply the packed signed word integers in mm1 register and mm2/m64, and store the low 16 bits of the results in mm1. */
  void pmullw(const Mm& arg0, const M64& arg1);

  /** Multiply the packed signed word integers in mm1 register and mm2/m64, and store the low 16 bits of the results in mm1. */
  void pmullw(const Mm& arg0, const Mm& arg1);

  /** Multiply the packed signed word integers in xmm1 and xmm2/m128, and store the low 16 bits of the results in xmm1. */
  void pmullw(const Xmm& arg0, const M128& arg1);

  /** Multiply the packed signed word integers in xmm1 and xmm2/m128, and store the low 16 bits of the results in xmm1. */
  void pmullw(const Xmm& arg0, const Xmm& arg1);

  /** Multiply unsigned doubleword integer in mm1 by unsigned doubleword integer in mm2/m64, and store the quadword result in mm1. */
  void pmuludq(const Mm& arg0, const M64& arg1);

  /** Multiply unsigned doubleword integer in mm1 by unsigned doubleword integer in mm2/m64, and store the quadword result in mm1. */
  void pmuludq(const Mm& arg0, const Mm& arg1);

  /** Multiply packed unsigned doubleword integers in xmm1 by packed unsigned doubleword integers in xmm2/m128, and store the quadword results in xmm1. */
  void pmuludq(const Xmm& arg0, const M128& arg1);

  /** Multiply packed unsigned doubleword integers in xmm1 by packed unsigned doubleword integers in xmm2/m128, and store the quadword results in xmm1. */
  void pmuludq(const Xmm& arg0, const Xmm& arg1);

  /** Pop top of stack into FS; increment stack pointer by 64 bits. */
  void pop(const Fs& arg0);

  /** Pop top of stack into FS; increment stack pointer by 16 bits. */
  void pop(const Fs& arg0, const Pref66& arg1);

  /** Pop top of stack into GS; increment stack pointer by 64 bits. */
  void pop(const Gs& arg0);

  /** Pop top of stack into GS; increment stack pointer by 16 bits. */
  void pop(const Gs& arg0, const Pref66& arg1);

  /** Pop top of stack into m16; increment stack pointer. */
  void pop(const M16& arg0);

  /** Pop top of stack into m64; increment stack pointer. Cannot encode 32-bit operand size. */
  void pop(const M64& arg0);

  /** Pop top of stack into m16; increment stack pointer. */
  void pop(const R16& arg0);

  /** Pop top of stack into r16; increment stack pointer. */
  void pop_1(const R16& arg0);

  /** Pop top of stack into m64; increment stack pointer. Cannot encode 32-bit operand size. */
  void pop(const R64& arg0);

  /** Pop top of stack into r64; increment stack pointer. Cannot encode 32-bit operand size. */
  void pop_1(const R64& arg0);

  /** POPCNT on r/m16 */
  void popcnt(const R16& arg0, const M16& arg1);

  /** POPCNT on r/m16 */
  void popcnt(const R16& arg0, const R16& arg1);

  /** POPCNT on r/m32 */
  void popcnt(const R32& arg0, const M32& arg1);

  /** POPCNT on r/m32 */
  void popcnt(const R32& arg0, const R32& arg1);

  /** POPCNT on r/m64 */
  void popcnt(const R64& arg0, const M64& arg1);

  /** POPCNT on r/m64 */
  void popcnt(const R64& arg0, const R64& arg1);

  /** Pop top of stack into lower 16 bits of EFLAGS. */
  void popf();

  /** Pop top of stack and zero-extend into RFLAGS. */
  void popfq();

  /** Bitwise OR of mm/m64 and mm. */
  void por(const Mm& arg0, const M64& arg1);

  /** Bitwise OR of mm/m64 and mm. */
  void por(const Mm& arg0, const Mm& arg1);

  /** Bitwise OR of xmm2/m128 and xmm1. */
  void por(const Xmm& arg0, const M128& arg1);

  /** Bitwise OR of xmm2/m128 and xmm1. */
  void por(const Xmm& arg0, const Xmm& arg1);

  /** Move data from m8 closer to the processor using NTA hint. */
  void prefetchnta(const M8& arg0);

  /** Move data from m8 closer to the processor using T0 hint. */
  void prefetcht0(const M8& arg0);

  /** Move data from m8 closer to the processor using T1 hint. */
  void prefetcht1(const M8& arg0);

  /** Move data from m8 closer to the processor using T2 hint. */
  void prefetcht2(const M8& arg0);

  /** Computes the absolute differences of the packed unsigned byte integers from mm2 /m64 and mm1; differences are then summed to produce an unsigned word integer result. */
  void psadbw(const Mm& arg0, const M64& arg1);

  /** Computes the absolute differences of the packed unsigned byte integers from mm2 /m64 and mm1; differences are then summed to produce an unsigned word integer result. */
  void psadbw(const Mm& arg0, const Mm& arg1);

  /** Computes the absolute differences of the packed unsigned byte integers from xmm2 /m128 and xmm1; the 8 low differences and 8 high differences are then summed separately to produce two unsigned word integer results. */
  void psadbw(const Xmm& arg0, const M128& arg1);

  /** Computes the absolute differences of the packed unsigned byte integers from xmm2 /m128 and xmm1; the 8 low differences and 8 high differences are then summed separately to produce two unsigned word integer results. */
  void psadbw(const Xmm& arg0, const Xmm& arg1);

  /** Shuffle bytes in mm1 according to contents of mm2/m64. */
  void pshufb(const Mm& arg0, const M64& arg1);

  /** Shuffle bytes in mm1 according to contents of mm2/m64. */
  void pshufb(const Mm& arg0, const Mm& arg1);

  /** Shuffle bytes in xmm1 according to contents of xmm2/m128. */
  void pshufb(const Xmm& arg0, const M128& arg1);

  /** Shuffle bytes in xmm1 according to contents of xmm2/m128. */
  void pshufb(const Xmm& arg0, const Xmm& arg1);

  /** Shuffle the doublewords in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void pshufd(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Shuffle the doublewords in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void pshufd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shuffle the high words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void pshufhw(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Shuffle the high words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void pshufhw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shuffle the low words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void pshuflw(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Shuffle the low words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void pshuflw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shuffle the words in mm2/m64 based on the encoding in imm8 and store the result in mm1. */
  void pshufw(const Mm& arg0, const M64& arg1, const Imm8& arg2);

  /** Shuffle the words in mm2/m64 based on the encoding in imm8 and store the result in mm1. */
  void pshufw(const Mm& arg0, const Mm& arg1, const Imm8& arg2);

  /** Negate/zero/preserve packed byte integers in mm1 depending on the corresponding sign in mm2/m64 */
  void psignb(const Mm& arg0, const M64& arg1);

  /** Negate/zero/preserve packed byte integers in mm1 depending on the corresponding sign in mm2/m64 */
  void psignb(const Mm& arg0, const Mm& arg1);

  /** Negate/zero/preserve packed byte integers in xmm1 depending on the corresponding sign in xmm2/m128. */
  void psignb(const Xmm& arg0, const M128& arg1);

  /** Negate/zero/preserve packed byte integers in xmm1 depending on the corresponding sign in xmm2/m128. */
  void psignb(const Xmm& arg0, const Xmm& arg1);

  /** Negate/zero/preserve packed doubleword integers in mm1 depending on the corresponding sign in mm2/m128. */
  void psignd(const Mm& arg0, const M64& arg1);

  /** Negate/zero/preserve packed doubleword integers in mm1 depending on the corresponding sign in mm2/m128. */
  void psignd(const Mm& arg0, const Mm& arg1);

  /** Negate/zero/preserve packed doubleword integers in xmm1 depending on the corresponding sign in xmm2/m128. */
  void psignd(const Xmm& arg0, const M128& arg1);

  /** Negate/zero/preserve packed doubleword integers in xmm1 depending on the corresponding sign in xmm2/m128. */
  void psignd(const Xmm& arg0, const Xmm& arg1);

  /** Negate/zero/preserve packed word integers in mm1 depending on the corresponding sign in mm2/m128. */
  void psignw(const Mm& arg0, const M64& arg1);

  /** Negate/zero/preserve packed word integers in mm1 depending on the corresponding sign in mm2/m128. */
  void psignw(const Mm& arg0, const Mm& arg1);

  /** Negate/zero/preserve packed word integers in xmm1 depending on the corresponding sign in xmm2/m128. */
  void psignw(const Xmm& arg0, const M128& arg1);

  /** Negate/zero/preserve packed word integers in xmm1 depending on the corresponding sign in xmm2/m128. */
  void psignw(const Xmm& arg0, const Xmm& arg1);

  /** Shift doublewords in mm left by imm8 while shifting in 0s. */
  void pslld(const Mm& arg0, const Imm8& arg1);

  /** Shift doublewords in mm left by mm/m64 while shifting in 0s. */
  void pslld(const Mm& arg0, const M64& arg1);

  /** Shift doublewords in mm left by mm/m64 while shifting in 0s. */
  void pslld(const Mm& arg0, const Mm& arg1);

  /** Shift doublewords in xmm1 left by imm8 while shifting in 0s. */
  void pslld(const Xmm& arg0, const Imm8& arg1);

  /** Shift doublewords in xmm1 left by xmm2/m128 while shifting in 0s. */
  void pslld(const Xmm& arg0, const M128& arg1);

  /** Shift doublewords in xmm1 left by xmm2/m128 while shifting in 0s. */
  void pslld(const Xmm& arg0, const Xmm& arg1);

  /** Shift xmm1 left by imm8 bytes while shifting in 0s. */
  void pslldq(const Xmm& arg0, const Imm8& arg1);

  /** Shift quadword in mm left by imm8 while shifting in 0s. */
  void psllq(const Mm& arg0, const Imm8& arg1);

  /** Shift quadword in mm left by mm/m64 while shifting in 0s. */
  void psllq(const Mm& arg0, const M64& arg1);

  /** Shift quadword in mm left by mm/m64 while shifting in 0s. */
  void psllq(const Mm& arg0, const Mm& arg1);

  /** Shift quadwords in xmm1 left by imm8 while shifting in 0s. */
  void psllq(const Xmm& arg0, const Imm8& arg1);

  /** Shift quadwords in xmm1 left by xmm2/m128 while shifting in 0s. */
  void psllq(const Xmm& arg0, const M128& arg1);

  /** Shift quadwords in xmm1 left by xmm2/m128 while shifting in 0s. */
  void psllq(const Xmm& arg0, const Xmm& arg1);

  /** Shift words in mm left by imm8 while shifting in 0s. */
  void psllw(const Mm& arg0, const Imm8& arg1);

  /** Shift words in mm left mm/m64 while shifting in 0s. */
  void psllw(const Mm& arg0, const M64& arg1);

  /** Shift words in mm left mm/m64 while shifting in 0s. */
  void psllw(const Mm& arg0, const Mm& arg1);

  /** Shift words in xmm1 left by imm8 while shifting in 0s. */
  void psllw(const Xmm& arg0, const Imm8& arg1);

  /** Shift words in xmm1 left by xmm2/m128 while shifting in 0s. */
  void psllw(const Xmm& arg0, const M128& arg1);

  /** Shift words in xmm1 left by xmm2/m128 while shifting in 0s. */
  void psllw(const Xmm& arg0, const Xmm& arg1);

  /** Shift doublewords in mm right by imm8 while shifting in sign bits. */
  void psrad(const Mm& arg0, const Imm8& arg1);

  /** Shift doublewords in mm right by mm/m64 while shifting in sign bits. */
  void psrad(const Mm& arg0, const M64& arg1);

  /** Shift doublewords in mm right by mm/m64 while shifting in sign bits. */
  void psrad(const Mm& arg0, const Mm& arg1);

  /** Shift doublewords in xmm1 right by imm8 while shifting in sign bits. */
  void psrad(const Xmm& arg0, const Imm8& arg1);

  /** Shift doubleword in xmm1 right by xmm2 /m128 while shifting in sign bits. */
  void psrad(const Xmm& arg0, const M128& arg1);

  /** Shift doubleword in xmm1 right by xmm2 /m128 while shifting in sign bits. */
  void psrad(const Xmm& arg0, const Xmm& arg1);

  /** Shift words in mm right by imm8 while shifting in sign bits */
  void psraw(const Mm& arg0, const Imm8& arg1);

  /** Shift words in mm right by mm/m64 while shifting in sign bits. */
  void psraw(const Mm& arg0, const M64& arg1);

  /** Shift words in mm right by mm/m64 while shifting in sign bits. */
  void psraw(const Mm& arg0, const Mm& arg1);

  /** Shift words in xmm1 right by imm8 while shifting in sign bits */
  void psraw(const Xmm& arg0, const Imm8& arg1);

  /** Shift words in xmm1 right by xmm2/m128 while shifting in sign bits. */
  void psraw(const Xmm& arg0, const M128& arg1);

  /** Shift words in xmm1 right by xmm2/m128 while shifting in sign bits. */
  void psraw(const Xmm& arg0, const Xmm& arg1);

  /** Shift doublewords in mm right by imm8 while shifting in 0s. */
  void psrld(const Mm& arg0, const Imm8& arg1);

  /** Shift doublewords in mm right by amount specified in mm/m64 while shifting in 0s. */
  void psrld(const Mm& arg0, const M64& arg1);

  /** Shift doublewords in mm right by amount specified in mm/m64 while shifting in 0s. */
  void psrld(const Mm& arg0, const Mm& arg1);

  /** Shift doublewords in xmm1 right by imm8 while shifting in 0s. */
  void psrld(const Xmm& arg0, const Imm8& arg1);

  /** Shift doublewords in xmm1 right by amount specified in xmm2 /m128 while shifting in 0s. */
  void psrld(const Xmm& arg0, const M128& arg1);

  /** Shift doublewords in xmm1 right by amount specified in xmm2 /m128 while shifting in 0s. */
  void psrld(const Xmm& arg0, const Xmm& arg1);

  /** Shift xmm1 right by imm8 while shifting in 0s. */
  void psrldq(const Xmm& arg0, const Imm8& arg1);

  /** Shift mm right by imm8 while shifting in 0s. */
  void psrlq(const Mm& arg0, const Imm8& arg1);

  /** Shift mm right by amount specified in mm/m64 while shifting in 0s. */
  void psrlq(const Mm& arg0, const M64& arg1);

  /** Shift mm right by amount specified in mm/m64 while shifting in 0s. */
  void psrlq(const Mm& arg0, const Mm& arg1);

  /** Shift quadwords in xmm1 right by imm8 while shifting in 0s. */
  void psrlq(const Xmm& arg0, const Imm8& arg1);

  /** Shift quadwords in xmm1 right by amount specified in xmm2/m128 while shifting in 0s. */
  void psrlq(const Xmm& arg0, const M128& arg1);

  /** Shift quadwords in xmm1 right by amount specified in xmm2/m128 while shifting in 0s. */
  void psrlq(const Xmm& arg0, const Xmm& arg1);

  /** Shift words in mm right by imm8 while shifting in 0s. */
  void psrlw(const Mm& arg0, const Imm8& arg1);

  /** Shift words in mm right by amount specified in mm/m64 while shifting in 0s. */
  void psrlw(const Mm& arg0, const M64& arg1);

  /** Shift words in mm right by amount specified in mm/m64 while shifting in 0s. */
  void psrlw(const Mm& arg0, const Mm& arg1);

  /** Shift words in xmm1 right by imm8 while shifting in 0s. */
  void psrlw(const Xmm& arg0, const Imm8& arg1);

  /** Shift words in xmm1 right by amount specified in xmm2/m128 while shifting in 0s. */
  void psrlw(const Xmm& arg0, const M128& arg1);

  /** Shift words in xmm1 right by amount specified in xmm2/m128 while shifting in 0s. */
  void psrlw(const Xmm& arg0, const Xmm& arg1);

  /** Subtract packed byte integers in mm/m64 from packed byte integers in mm. */
  void psubb(const Mm& arg0, const M64& arg1);

  /** Subtract packed byte integers in mm/m64 from packed byte integers in mm. */
  void psubb(const Mm& arg0, const Mm& arg1);

  /** Subtract packed byte integers in xmm2/m128 from packed byte integers in xmm1. */
  void psubb(const Xmm& arg0, const M128& arg1);

  /** Subtract packed byte integers in xmm2/m128 from packed byte integers in xmm1. */
  void psubb(const Xmm& arg0, const Xmm& arg1);

  /** Subtract packed doubleword integers in mm/m64 from packed doubleword integers in mm. */
  void psubd(const Mm& arg0, const M64& arg1);

  /** Subtract packed doubleword integers in mm/m64 from packed doubleword integers in mm. */
  void psubd(const Mm& arg0, const Mm& arg1);

  /** Subtract packed doubleword integers in xmm2/mem128 from packed doubleword integers in xmm1. */
  void psubd(const Xmm& arg0, const M128& arg1);

  /** Subtract packed doubleword integers in xmm2/mem128 from packed doubleword integers in xmm1. */
  void psubd(const Xmm& arg0, const Xmm& arg1);

  /** Subtract quadword integer in mm1 from mm2 /m64. */
  void psubq(const Mm& arg0, const M64& arg1);

  /** Subtract quadword integer in mm1 from mm2 /m64. */
  void psubq(const Mm& arg0, const Mm& arg1);

  /** Subtract packed quadword integers in xmm1 from xmm2 /m128. */
  void psubq(const Xmm& arg0, const M128& arg1);

  /** Subtract packed quadword integers in xmm1 from xmm2 /m128. */
  void psubq(const Xmm& arg0, const Xmm& arg1);

  /** Subtract signed packed bytes in mm/m64 from signed packed bytes in mm and saturate results. */
  void psubsb(const Mm& arg0, const M64& arg1);

  /** Subtract signed packed bytes in mm/m64 from signed packed bytes in mm and saturate results. */
  void psubsb(const Mm& arg0, const Mm& arg1);

  /** Subtract packed signed byte integers in xmm2/m128 from packed signed byte integers in xmm1 and saturate results. */
  void psubsb(const Xmm& arg0, const M128& arg1);

  /** Subtract packed signed byte integers in xmm2/m128 from packed signed byte integers in xmm1 and saturate results. */
  void psubsb(const Xmm& arg0, const Xmm& arg1);

  /** Subtract signed packed words in mm/m64 from signed packed words in mm and saturate results. */
  void psubsw(const Mm& arg0, const M64& arg1);

  /** Subtract signed packed words in mm/m64 from signed packed words in mm and saturate results. */
  void psubsw(const Mm& arg0, const Mm& arg1);

  /** Subtract packed signed word integers in xmm2/m128 from packed signed word integers in xmm1 and saturate results. */
  void psubsw(const Xmm& arg0, const M128& arg1);

  /** Subtract packed signed word integers in xmm2/m128 from packed signed word integers in xmm1 and saturate results. */
  void psubsw(const Xmm& arg0, const Xmm& arg1);

  /** Subtract unsigned packed bytes in mm/m64 from unsigned packed bytes in mm and saturate result. */
  void psubusb(const Mm& arg0, const M64& arg1);

  /** Subtract unsigned packed bytes in mm/m64 from unsigned packed bytes in mm and saturate result. */
  void psubusb(const Mm& arg0, const Mm& arg1);

  /** Subtract packed unsigned byte integers in xmm2/m128 from packed unsigned byte integers in xmm1 and saturate result. */
  void psubusb(const Xmm& arg0, const M128& arg1);

  /** Subtract packed unsigned byte integers in xmm2/m128 from packed unsigned byte integers in xmm1 and saturate result. */
  void psubusb(const Xmm& arg0, const Xmm& arg1);

  /** Subtract unsigned packed words in mm/m64 from unsigned packed words in mm and saturate result. */
  void psubusw(const Mm& arg0, const M64& arg1);

  /** Subtract unsigned packed words in mm/m64 from unsigned packed words in mm and saturate result. */
  void psubusw(const Mm& arg0, const Mm& arg1);

  /** Subtract packed unsigned word integers in xmm2/m128 from packed unsigned word integers in xmm1 and saturate result. */
  void psubusw(const Xmm& arg0, const M128& arg1);

  /** Subtract packed unsigned word integers in xmm2/m128 from packed unsigned word integers in xmm1 and saturate result. */
  void psubusw(const Xmm& arg0, const Xmm& arg1);

  /** Subtract packed word integers in mm/m64 from packed word integers in mm. */
  void psubw(const Mm& arg0, const M64& arg1);

  /** Subtract packed word integers in mm/m64 from packed word integers in mm. */
  void psubw(const Mm& arg0, const Mm& arg1);

  /** Subtract packed word integers in xmm2/m128 from packed word integers in xmm1. */
  void psubw(const Xmm& arg0, const M128& arg1);

  /** Subtract packed word integers in xmm2/m128 from packed word integers in xmm1. */
  void psubw(const Xmm& arg0, const Xmm& arg1);

  /** Set ZF if xmm2/m128 AND xmm1 result is all 0s. Set CF if xmm2/m128 AND NOT xmm1 result is all 0s. */
  void ptest(const Xmm& arg0, const M128& arg1);

  /** Set ZF if xmm2/m128 AND xmm1 result is all 0s. Set CF if xmm2/m128 AND NOT xmm1 result is all 0s. */
  void ptest(const Xmm& arg0, const Xmm& arg1);

  /** Unpack and interleave high-order bytes from mm and mm/m64 into mm. */
  void punpckhbw(const Mm& arg0, const M64& arg1);

  /** Unpack and interleave high-order bytes from mm and mm/m64 into mm. */
  void punpckhbw(const Mm& arg0, const Mm& arg1);

  /** Unpack and interleave high-order bytes from xmm1 and xmm2/m128 into xmm1. */
  void punpckhbw(const Xmm& arg0, const M128& arg1);

  /** Unpack and interleave high-order bytes from xmm1 and xmm2/m128 into xmm1. */
  void punpckhbw(const Xmm& arg0, const Xmm& arg1);

  /** Unpack and interleave high-order doublewords from mm and mm/m64 into mm. */
  void punpckhdq(const Mm& arg0, const M64& arg1);

  /** Unpack and interleave high-order doublewords from mm and mm/m64 into mm. */
  void punpckhdq(const Mm& arg0, const Mm& arg1);

  /** Unpack and interleave high-order doublewords from xmm1 and xmm2/m128 into xmm1. */
  void punpckhdq(const Xmm& arg0, const M128& arg1);

  /** Unpack and interleave high-order doublewords from xmm1 and xmm2/m128 into xmm1. */
  void punpckhdq(const Xmm& arg0, const Xmm& arg1);

  /** Unpack and interleave high-order quadwords from xmm1 and xmm2/m128 into xmm1. */
  void punpckhqdq(const Xmm& arg0, const M128& arg1);

  /** Unpack and interleave high-order quadwords from xmm1 and xmm2/m128 into xmm1. */
  void punpckhqdq(const Xmm& arg0, const Xmm& arg1);

  /** Unpack and interleave high-order words from mm and mm/m64 into mm. */
  void punpckhwd(const Mm& arg0, const M64& arg1);

  /** Unpack and interleave high-order words from mm and mm/m64 into mm. */
  void punpckhwd(const Mm& arg0, const Mm& arg1);

  /** Unpack and interleave high-order words from xmm1 and xmm2/m128 into xmm1. */
  void punpckhwd(const Xmm& arg0, const M128& arg1);

  /** Unpack and interleave high-order words from xmm1 and xmm2/m128 into xmm1. */
  void punpckhwd(const Xmm& arg0, const Xmm& arg1);

  /** Interleave low-order bytes from mm and mm/m32 into mm. */
  void punpcklbw(const Mm& arg0, const M32& arg1);

  /** Interleave low-order bytes from mm and mm/m32 into mm. */
  void punpcklbw(const Mm& arg0, const Mm& arg1);

  /** Interleave low-order bytes from xmm1 and xmm2/m128 into xmm1. */
  void punpcklbw(const Xmm& arg0, const M128& arg1);

  /** Interleave low-order bytes from xmm1 and xmm2/m128 into xmm1. */
  void punpcklbw(const Xmm& arg0, const Xmm& arg1);

  /** Interleave low-order doublewords from mm and mm/m32 into mm. */
  void punpckldq(const Mm& arg0, const M32& arg1);

  /** Interleave low-order doublewords from mm and mm/m32 into mm. */
  void punpckldq(const Mm& arg0, const Mm& arg1);

  /** Interleave low-order doublewords from xmm1 and xmm2/m128 into xmm1. */
  void punpckldq(const Xmm& arg0, const M128& arg1);

  /** Interleave low-order doublewords from xmm1 and xmm2/m128 into xmm1. */
  void punpckldq(const Xmm& arg0, const Xmm& arg1);

  /** Interleave low-order quadword from xmm1 and xmm2/m128 into xmm1 register. */
  void punpcklqdq(const Xmm& arg0, const M128& arg1);

  /** Interleave low-order quadword from xmm1 and xmm2/m128 into xmm1 register. */
  void punpcklqdq(const Xmm& arg0, const Xmm& arg1);

  /** Interleave low-order words from mm and mm/m32 into mm. */
  void punpcklwd(const Mm& arg0, const M32& arg1);

  /** Interleave low-order words from mm and mm/m32 into mm. */
  void punpcklwd(const Mm& arg0, const Mm& arg1);

  /** Interleave low-order words from xmm1 and xmm2/m128 into xmm1. */
  void punpcklwd(const Xmm& arg0, const M128& arg1);

  /** Interleave low-order words from xmm1 and xmm2/m128 into xmm1. */
  void punpcklwd(const Xmm& arg0, const Xmm& arg1);

  /** Push FS. */
  void push(const Fs& arg0);

  /** Push GS. */
  void push(const Gs& arg0);

  /** Push r/m16. */
  void push(const M16& arg0);

  /** Push r/m64. */
  void push(const M64& arg0);

  /** Push r/m16. */
  void push(const R16& arg0);

  /** Push r16. */
  void push_1(const R16& arg0);

  /** Push r/m64. */
  void push(const R64& arg0);

  /** Push r64. */
  void push_1(const R64& arg0);

  /** Push lower 16 bits of EFLAGS. */
  void pushf();

  /** Push RFLAGS. */
  void pushfq();

  /** Push imm16 (sign-extended to 64-bits). */
  void pushq(const Imm16& arg0);

  /** Push imm32 (sign-extended to 64-bits). */
  void pushq(const Imm32& arg0);

  /** Push imm8 (sign-extended to 64-bits). */
  void pushq(const Imm8& arg0);

  /** Push imm16 (sign-extended to 16-bits). */
  void pushw(const Imm16& arg0);

  /** Push imm8 (sign-extended to 16-bits). */
  void pushw(const Imm8& arg0);

  /** Bitwise XOR of mm/m64 and mm. */
  void pxor(const Mm& arg0, const M64& arg1);

  /** Bitwise XOR of mm/m64 and mm. */
  void pxor(const Mm& arg0, const Mm& arg1);

  /** Bitwise XOR of xmm2/m128 and xmm1. */
  void pxor(const Xmm& arg0, const M128& arg1);

  /** Bitwise XOR of xmm2/m128 and xmm1. */
  void pxor(const Xmm& arg0, const Xmm& arg1);

  /** Rotate 17 bits (CF, r/m16) left CL times. */
  void rcl(const M16& arg0, const Cl& arg1);

  /** Rotate 17 bits (CF, r/m16) left imm8 times. */
  void rcl(const M16& arg0, const Imm8& arg1);

  /** Rotate 17 bits (CF, r/m16) left once. */
  void rcl(const M16& arg0, const One& arg1);

  /** Rotate 33 bits (CF, r/m32) left CL times. */
  void rcl(const M32& arg0, const Cl& arg1);

  /** Rotate 33 bits (CF, r/m32) left imm8 times. */
  void rcl(const M32& arg0, const Imm8& arg1);

  /** Rotate 33 bits (CF, r/m32) left once. */
  void rcl(const M32& arg0, const One& arg1);

  /** Rotate 65 bits (CF, r/m64) left CL times. Uses a 6 bit count. */
  void rcl(const M64& arg0, const Cl& arg1);

  /** Rotate 65 bits (CF, r/m64) left imm8 times. Uses a 6 bit count. */
  void rcl(const M64& arg0, const Imm8& arg1);

  /** Rotate 65 bits (CF, r/m64) left once. Uses a 6 bit count. */
  void rcl(const M64& arg0, const One& arg1);

  /** Rotate 9 bits (CF, r/m8) left CL times. */
  void rcl(const M8& arg0, const Cl& arg1);

  /** Rotate 9 bits (CF, r/m8) left imm8 times. */
  void rcl(const M8& arg0, const Imm8& arg1);

  /** Rotate 9 bits (CF, r/m8) left once. */
  void rcl(const M8& arg0, const One& arg1);

  /** Rotate 17 bits (CF, r/m16) left CL times. */
  void rcl(const R16& arg0, const Cl& arg1);

  /** Rotate 17 bits (CF, r/m16) left imm8 times. */
  void rcl(const R16& arg0, const Imm8& arg1);

  /** Rotate 17 bits (CF, r/m16) left once. */
  void rcl(const R16& arg0, const One& arg1);

  /** Rotate 33 bits (CF, r/m32) left CL times. */
  void rcl(const R32& arg0, const Cl& arg1);

  /** Rotate 33 bits (CF, r/m32) left imm8 times. */
  void rcl(const R32& arg0, const Imm8& arg1);

  /** Rotate 33 bits (CF, r/m32) left once. */
  void rcl(const R32& arg0, const One& arg1);

  /** Rotate 65 bits (CF, r/m64) left CL times. Uses a 6 bit count. */
  void rcl(const R64& arg0, const Cl& arg1);

  /** Rotate 65 bits (CF, r/m64) left imm8 times. Uses a 6 bit count. */
  void rcl(const R64& arg0, const Imm8& arg1);

  /** Rotate 65 bits (CF, r/m64) left once. Uses a 6 bit count. */
  void rcl(const R64& arg0, const One& arg1);

  /** Rotate 9 bits (CF, r/m8) left CL times. */
  void rcl(const R8& arg0, const Cl& arg1);

  /** Rotate 9 bits (CF, r/m8) left imm8 times. */
  void rcl(const R8& arg0, const Imm8& arg1);

  /** Rotate 9 bits (CF, r/m8) left once. */
  void rcl(const R8& arg0, const One& arg1);

  /** Rotate 9 bits (CF, r/m8) left CL times. */
  void rcl(const Rh& arg0, const Cl& arg1);

  /** Rotate 9 bits (CF, r/m8) left imm8 times. */
  void rcl(const Rh& arg0, const Imm8& arg1);

  /** Rotate 9 bits (CF, r/m8) left once. */
  void rcl(const Rh& arg0, const One& arg1);

  /** Computes the approximate reciprocals of the packed single-precision floating-point values in xmm2/m128 and stores the results in xmm1. */
  void rcpps(const Xmm& arg0, const M128& arg1);

  /** Computes the approximate reciprocals of the packed single-precision floating-point values in xmm2/m128 and stores the results in xmm1. */
  void rcpps(const Xmm& arg0, const Xmm& arg1);

  /** Computes the approximate reciprocal of the scalar single-precision floating-point value in xmm2/m32 and stores the result in xmm1. */
  void rcpss(const Xmm& arg0, const M32& arg1);

  /** Computes the approximate reciprocal of the scalar single-precision floating-point value in xmm2/m32 and stores the result in xmm1. */
  void rcpss(const Xmm& arg0, const Xmm& arg1);

  /** Rotate 17 bits (CF, r/m16) right CL times. */
  void rcr(const M16& arg0, const Cl& arg1);

  /** Rotate 17 bits (CF, r/m16) right imm8 times. */
  void rcr(const M16& arg0, const Imm8& arg1);

  /** Rotate 17 bits (CF, r/m16) right once. */
  void rcr(const M16& arg0, const One& arg1);

  /** Rotate 33 bits (CF, r/m32) right CL times. */
  void rcr(const M32& arg0, const Cl& arg1);

  /** Rotate 33 bits (CF, r/m32) right imm8 times. */
  void rcr(const M32& arg0, const Imm8& arg1);

  /** Rotate 33 bits (CF, r/m32) right once. Uses a 6 bit count. */
  void rcr(const M32& arg0, const One& arg1);

  /** Rotate 65 bits (CF, r/m64) right CL times. Uses a 6 bit count. */
  void rcr(const M64& arg0, const Cl& arg1);

  /** Rotate 65 bits (CF, r/m64) right imm8 times. Uses a 6 bit count. */
  void rcr(const M64& arg0, const Imm8& arg1);

  /** Rotate 65 bits (CF, r/m64) right once. Uses a 6 bit count. */
  void rcr(const M64& arg0, const One& arg1);

  /** Rotate 9 bits (CF, r/m8) right CL times. */
  void rcr(const M8& arg0, const Cl& arg1);

  /** Rotate 9 bits (CF, r/m8) right imm8 times. */
  void rcr(const M8& arg0, const Imm8& arg1);

  /** Rotate 9 bits (CF, r/m8) right once. */
  void rcr(const M8& arg0, const One& arg1);

  /** Rotate 17 bits (CF, r/m16) right CL times. */
  void rcr(const R16& arg0, const Cl& arg1);

  /** Rotate 17 bits (CF, r/m16) right imm8 times. */
  void rcr(const R16& arg0, const Imm8& arg1);

  /** Rotate 17 bits (CF, r/m16) right once. */
  void rcr(const R16& arg0, const One& arg1);

  /** Rotate 33 bits (CF, r/m32) right CL times. */
  void rcr(const R32& arg0, const Cl& arg1);

  /** Rotate 33 bits (CF, r/m32) right imm8 times. */
  void rcr(const R32& arg0, const Imm8& arg1);

  /** Rotate 33 bits (CF, r/m32) right once. Uses a 6 bit count. */
  void rcr(const R32& arg0, const One& arg1);

  /** Rotate 65 bits (CF, r/m64) right CL times. Uses a 6 bit count. */
  void rcr(const R64& arg0, const Cl& arg1);

  /** Rotate 65 bits (CF, r/m64) right imm8 times. Uses a 6 bit count. */
  void rcr(const R64& arg0, const Imm8& arg1);

  /** Rotate 65 bits (CF, r/m64) right once. Uses a 6 bit count. */
  void rcr(const R64& arg0, const One& arg1);

  /** Rotate 9 bits (CF, r/m8) right CL times. */
  void rcr(const R8& arg0, const Cl& arg1);

  /** Rotate 9 bits (CF, r/m8) right imm8 times. */
  void rcr(const R8& arg0, const Imm8& arg1);

  /** Rotate 9 bits (CF, r/m8) right once. */
  void rcr(const R8& arg0, const One& arg1);

  /** Rotate 9 bits (CF, r/m8) right CL times. */
  void rcr(const Rh& arg0, const Cl& arg1);

  /** Rotate 9 bits (CF, r/m8) right imm8 times. */
  void rcr(const Rh& arg0, const Imm8& arg1);

  /** Rotate 9 bits (CF, r/m8) right once. */
  void rcr(const Rh& arg0, const One& arg1);

  /** Load the 32-bit destination register with the FS base address. */
  void rdfsbase(const R32& arg0);

  /** Load the 64-bit destination register with the FS base address. */
  void rdfsbase(const R64& arg0);

  /** Load the 32-bit destination register with the GS base address. */
  void rdgsbase(const R32& arg0);

  /** Load the 64-bit destination register with the GS base address. */
  void rdgsbase(const R64& arg0);

  /** Read a 16-bit random number and store in the destination register. */
  void rdrand(const R16& arg0);

  /** Read a 32-bit random number and store in the destination register. */
  void rdrand(const R32& arg0);

  /** Read a 64-bit random number and store in the destination register. */
  void rdrand(const R64& arg0);

  /** Input (E)CX words from port DX into ES:[(E)DI.] */
  void rep_ins(const M16& arg0, const Dx& arg1);

  /** Input (E)CX doublewords from port DX into ES:[(E)DI]. */
  void rep_ins(const M32& arg0, const Dx& arg1);

  /** Input RCX default size from port DX into [RDI]. */
  void rep_ins(const M64& arg0, const Dx& arg1);

  /** Input (E)CX bytes from port DX into ES:[(E)DI]. */
  void rep_ins(const M8& arg0, const Dx& arg1);

  /** Input RCX bytes from port DX into [RDI]. */
  void rep_ins_1(const M8& arg0, const Dx& arg1);

  /** Load (E)CX bytes from DS:[(E)SI] to AL. */
  void rep_lods(const Al& arg0);

  /** Load RCX bytes from [RSI] to AL. */
  void rep_lods_1(const Al& arg0);

  /** Load (E)CX words from DS:[(E)SI] to AX. */
  void rep_lods(const Ax& arg0);

  /** Load (E)CX doublewords from DS:[(E)SI] to EAX. */
  void rep_lods(const Eax& arg0);

  /** Load RCX quadwords from [RSI] to RAX. */
  void rep_lods(const Rax& arg0);

  /** Move (E)CX words from DS:[(E)SI] to ES:[(E)DI]. */
  void rep_movs(const M16& arg0, const M16& arg1);

  /** Move (E)CX doublewords from DS:[(E)SI] to ES:[(E)DI]. */
  void rep_movs(const M32& arg0, const M32& arg1);

  /** Move RCX quadwords from [RSI] to [RDI]. */
  void rep_movs(const M64& arg0, const M64& arg1);

  /** Move (E)CX bytes from DS:[(E)SI] to ES:[(E)DI]. */
  void rep_movs(const M8& arg0, const M8& arg1);

  /** Move RCX bytes from [RSI] to [RDI]. */
  void rep_movs_1(const M8& arg0, const M8& arg1);

  /** Output (E)CX words from DS:[(E)SI] to port DX. */
  void rep_outs(const Dx& arg0, const M16& arg1);

  /** Output (E)CX doublewords from DS:[(E)SI] to port DX. */
  void rep_outs(const Dx& arg0, const M32& arg1);

  /** Output RCX default size from [RSI] to port DX. */
  void rep_outs(const Dx& arg0, const M64& arg1);

  /** Output (E)CX bytes from DS:[(E)SI] to port DX. */
  void rep_outs(const Dx& arg0, const M8& arg1);

  /** Output RCX bytes from [RSI] to port DX. */
  void rep_outs_1(const Dx& arg0, const M8& arg1);

  /** Fill (E)CX words at ES:[(E)DI] with AX. */
  void rep_stos(const M16& arg0);

  /** Fill (E)CX doublewords at ES:[(E)DI] with EAX. */
  void rep_stos(const M32& arg0);

  /** Fill RCX quadwords at [RDI] with RAX. */
  void rep_stos(const M64& arg0);

  /** Fill (E)CX bytes at ES:[(E)DI] with AL. */
  void rep_stos(const M8& arg0);

  /** Fill RCX bytes at [RDI] with AL. */
  void rep_stos_1(const M8& arg0);

  /** Find nonmatching words in ES:[(E)DI] and DS:[(E)SI]. */
  void repe_cmps(const M16& arg0, const M16& arg1);

  /** Find nonmatching doublewords in ES:[(E)DI] and DS:[(E)SI]. */
  void repe_cmps(const M32& arg0, const M32& arg1);

  /** Find non-matching quadwords in [RDI] and [RSI]. */
  void repe_cmps(const M64& arg0, const M64& arg1);

  /** Find nonmatching bytes in ES:[(E)DI] and DS:[(E)SI]. */
  void repe_cmps(const M8& arg0, const M8& arg1);

  /** Find non-matching bytes in [RDI] and [RSI]. */
  void repe_cmps_1(const M8& arg0, const M8& arg1);

  /** Find non-AX word starting at ES:[(E)DI]. */
  void repe_scas(const M16& arg0);

  /** Find non-EAX doubleword starting at ES:[(E)DI]. */
  void repe_scas(const M32& arg0);

  /** Find non-RAX quadword starting at [RDI]. */
  void repe_scas(const M64& arg0);

  /** Find non-AL byte starting at ES:[(E)DI]. */
  void repe_scas(const M8& arg0);

  /** Find non-AL byte starting at [RDI]. */
  void repe_scas_1(const M8& arg0);

  /** Find matching words in ES:[(E)DI] and DS:[(E)SI]. */
  void repne_cmps(const M16& arg0, const M16& arg1);

  /** Find matching doublewords in ES:[(E)DI] and DS:[(E)SI]. */
  void repne_cmps(const M32& arg0, const M32& arg1);

  /** Find matching doublewords in [RDI] and [RSI]. */
  void repne_cmps(const M64& arg0, const M64& arg1);

  /** Find matching bytes in ES:[(E)DI] and DS:[(E)SI]. */
  void repne_cmps(const M8& arg0, const M8& arg1);

  /** Find matching bytes in [RDI] and [RSI]. */
  void repne_cmps_1(const M8& arg0, const M8& arg1);

  /** Find AX, starting at ES:[(E)DI]. */
  void repne_scas(const M16& arg0);

  /** Find EAX, starting at ES:[(E)DI]. */
  void repne_scas(const M32& arg0);

  /** Find RAX, starting at [RDI]. */
  void repne_scas(const M64& arg0);

  /** Find AL, starting at ES:[(E)DI]. */
  void repne_scas(const M8& arg0);

  /** Find AL, starting at [RDI]. */
  void repne_scas_1(const M8& arg0);

  /** Near return to calling procedure. */
  void ret();

  /** Far return to calling procedure. */
  void ret(const Far& arg0);

  /** Near return to calling procedure and pop imm16 bytes from stack. */
  void ret(const Imm16& arg0);

  /** Far return to calling procedure and pop imm16 bytes from stack. */
  void ret(const Imm16& arg0, const Far& arg1);

  /** Rotate 16 bits r/m16 left CL times. */
  void rol(const M16& arg0, const Cl& arg1);

  /** Rotate 16 bits r/m16 left imm8 times. */
  void rol(const M16& arg0, const Imm8& arg1);

  /** Rotate 16 bits r/m16 left once. */
  void rol(const M16& arg0, const One& arg1);

  /** Rotate 32 bits r/m32 left CL times. */
  void rol(const M32& arg0, const Cl& arg1);

  /** Rotate 32 bits r/m32 left imm8 times. */
  void rol(const M32& arg0, const Imm8& arg1);

  /** Rotate 32 bits r/m32 left once. */
  void rol(const M32& arg0, const One& arg1);

  /** Rotate 64 bits r/m64 left CL times. Uses a 6 bit count. */
  void rol(const M64& arg0, const Cl& arg1);

  /** Rotate 64 bits r/m64 left imm8 times. Uses a 6 bit count. */
  void rol(const M64& arg0, const Imm8& arg1);

  /** Rotate 64 bits r/m64 left once. Uses a 6 bit count. */
  void rol(const M64& arg0, const One& arg1);

  /** Rotate 8 bits r/m8 left CL times. */
  void rol(const M8& arg0, const Cl& arg1);

  /** Rotate 8 bits r/m8 left imm8 times. */
  void rol(const M8& arg0, const Imm8& arg1);

  /** Rotate 8 bits r/m8 left once. */
  void rol(const M8& arg0, const One& arg1);

  /** Rotate 16 bits r/m16 left CL times. */
  void rol(const R16& arg0, const Cl& arg1);

  /** Rotate 16 bits r/m16 left imm8 times. */
  void rol(const R16& arg0, const Imm8& arg1);

  /** Rotate 16 bits r/m16 left once. */
  void rol(const R16& arg0, const One& arg1);

  /** Rotate 32 bits r/m32 left CL times. */
  void rol(const R32& arg0, const Cl& arg1);

  /** Rotate 32 bits r/m32 left imm8 times. */
  void rol(const R32& arg0, const Imm8& arg1);

  /** Rotate 32 bits r/m32 left once. */
  void rol(const R32& arg0, const One& arg1);

  /** Rotate 64 bits r/m64 left CL times. Uses a 6 bit count. */
  void rol(const R64& arg0, const Cl& arg1);

  /** Rotate 64 bits r/m64 left imm8 times. Uses a 6 bit count. */
  void rol(const R64& arg0, const Imm8& arg1);

  /** Rotate 64 bits r/m64 left once. Uses a 6 bit count. */
  void rol(const R64& arg0, const One& arg1);

  /** Rotate 8 bits r/m8 left CL times. */
  void rol(const R8& arg0, const Cl& arg1);

  /** Rotate 8 bits r/m8 left imm8 times. */
  void rol(const R8& arg0, const Imm8& arg1);

  /** Rotate 8 bits r/m8 left once. */
  void rol(const R8& arg0, const One& arg1);

  /** Rotate 8 bits r/m8 left CL times. */
  void rol(const Rh& arg0, const Cl& arg1);

  /** Rotate 8 bits r/m8 left imm8 times. */
  void rol(const Rh& arg0, const Imm8& arg1);

  /** Rotate 8 bits r/m8 left once. */
  void rol(const Rh& arg0, const One& arg1);

  /** Rotate 16 bits r/m16 right CL times. */
  void ror(const M16& arg0, const Cl& arg1);

  /** Rotate 16 bits r/m16 right imm8 times. */
  void ror(const M16& arg0, const Imm8& arg1);

  /** Rotate 16 bits r/m16 right once. */
  void ror(const M16& arg0, const One& arg1);

  /** Rotate 32 bits r/m32 right CL times. */
  void ror(const M32& arg0, const Cl& arg1);

  /** Rotate 32 bits r/m32 right imm8 times. */
  void ror(const M32& arg0, const Imm8& arg1);

  /** Rotate 32 bits r/m32 right once. */
  void ror(const M32& arg0, const One& arg1);

  /** Rotate 64 bits r/m64 right CL times. Uses a 6 bit count. */
  void ror(const M64& arg0, const Cl& arg1);

  /** Rotate 64 bits r/m64 right imm8 times. Uses a 6 bit count. */
  void ror(const M64& arg0, const Imm8& arg1);

  /** Rotate 64 bits r/m64 right once. Uses a 6 bit count. */
  void ror(const M64& arg0, const One& arg1);

  /** Rotate 8 bits r/m8 right CL times. */
  void ror(const M8& arg0, const Cl& arg1);

  /** Rotate 8 bits r/m16 right imm8 times. */
  void ror(const M8& arg0, const Imm8& arg1);

  /** Rotate 8 bits r/m8 right once. */
  void ror(const M8& arg0, const One& arg1);

  /** Rotate 16 bits r/m16 right CL times. */
  void ror(const R16& arg0, const Cl& arg1);

  /** Rotate 16 bits r/m16 right imm8 times. */
  void ror(const R16& arg0, const Imm8& arg1);

  /** Rotate 16 bits r/m16 right once. */
  void ror(const R16& arg0, const One& arg1);

  /** Rotate 32 bits r/m32 right CL times. */
  void ror(const R32& arg0, const Cl& arg1);

  /** Rotate 32 bits r/m32 right imm8 times. */
  void ror(const R32& arg0, const Imm8& arg1);

  /** Rotate 32 bits r/m32 right once. */
  void ror(const R32& arg0, const One& arg1);

  /** Rotate 64 bits r/m64 right CL times. Uses a 6 bit count. */
  void ror(const R64& arg0, const Cl& arg1);

  /** Rotate 64 bits r/m64 right imm8 times. Uses a 6 bit count. */
  void ror(const R64& arg0, const Imm8& arg1);

  /** Rotate 64 bits r/m64 right once. Uses a 6 bit count. */
  void ror(const R64& arg0, const One& arg1);

  /** Rotate 8 bits r/m8 right CL times. */
  void ror(const R8& arg0, const Cl& arg1);

  /** Rotate 8 bits r/m16 right imm8 times. */
  void ror(const R8& arg0, const Imm8& arg1);

  /** Rotate 8 bits r/m8 right once. */
  void ror(const R8& arg0, const One& arg1);

  /** Rotate 8 bits r/m8 right CL times. */
  void ror(const Rh& arg0, const Cl& arg1);

  /** Rotate 8 bits r/m16 right imm8 times. */
  void ror(const Rh& arg0, const Imm8& arg1);

  /** Rotate 8 bits r/m8 right once. */
  void ror(const Rh& arg0, const One& arg1);

  /** Rotate 32-bit r/m32 right imm8 times without affecting arithmetic flags. */
  void rorx(const R32& arg0, const M32& arg1, const Imm8& arg2);

  /** Rotate 32-bit r/m32 right imm8 times without affecting arithmetic flags. */
  void rorx(const R32& arg0, const R32& arg1, const Imm8& arg2);

  /** Rotate 64-bit r/m64 right imm8 times without affecting arithmetic flags. */
  void rorx(const R64& arg0, const M64& arg1, const Imm8& arg2);

  /** Rotate 64-bit r/m64 right imm8 times without affecting arithmetic flags. */
  void rorx(const R64& arg0, const R64& arg1, const Imm8& arg2);

  /** Round packed double precision floating-point values in xmm2/m128 and place the result in xmm1. The rounding mode is determined by imm8. */
  void roundpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Round packed double precision floating-point values in xmm2/m128 and place the result in xmm1. The rounding mode is determined by imm8. */
  void roundpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Round packed single precision floating-point values in xmm2/m128 and place the result in xmm1. The rounding mode is determined by imm8. */
  void roundps(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Round packed single precision floating-point values in xmm2/m128 and place the result in xmm1. The rounding mode is determined by imm8. */
  void roundps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Round the low packed double precision floating-point value in xmm2/m64 and place the result in xmm1. The rounding mode is determined by imm8. */
  void roundsd(const Xmm& arg0, const M64& arg1, const Imm8& arg2);

  /** Round the low packed double precision floating-point value in xmm2/m64 and place the result in xmm1. The rounding mode is determined by imm8. */
  void roundsd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Round the low packed single precision floating-point value in xmm2/m32 and place the result in xmm1. The rounding mode is determined by imm8. */
  void roundss(const Xmm& arg0, const M32& arg1, const Imm8& arg2);

  /** Round the low packed single precision floating-point value in xmm2/m32 and place the result in xmm1. The rounding mode is determined by imm8. */
  void roundss(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Computes the approximate reciprocals of the square roots of the packed single-precision floating-point values in xmm2/m128 and stores the results in xmm1. */
  void rsqrtps(const Xmm& arg0, const M128& arg1);

  /** Computes the approximate reciprocals of the square roots of the packed single-precision floating-point values in xmm2/m128 and stores the results in xmm1. */
  void rsqrtps(const Xmm& arg0, const Xmm& arg1);

  /** Computes the approximate reciprocal of the square root of the low single-precision floating-point value in xmm2/m32 and stores the results in xmm1. */
  void rsqrtss(const Xmm& arg0, const M32& arg1);

  /** Computes the approximate reciprocal of the square root of the low single-precision floating-point value in xmm2/m32 and stores the results in xmm1. */
  void rsqrtss(const Xmm& arg0, const Xmm& arg1);

  /** Loads SF, ZF, AF, PF, and CF from AH into EFLAGS register. */
  void sahf();

  /** Multiply r/m16 by 2, CL times. */
  void sal(const M16& arg0, const Cl& arg1);

  /** Multiply r/m16 by 2, imm8 times. */
  void sal(const M16& arg0, const Imm8& arg1);

  /** Multiply r/m16 by 2, once. */
  void sal(const M16& arg0, const One& arg1);

  /** Multiply r/m32 by 2, CL times. */
  void sal(const M32& arg0, const Cl& arg1);

  /** Multiply r/m32 by 2, imm8 times. */
  void sal(const M32& arg0, const Imm8& arg1);

  /** Multiply r/m32 by 2, once. */
  void sal(const M32& arg0, const One& arg1);

  /** Multiply r/m64 by 2, CL times. */
  void sal(const M64& arg0, const Cl& arg1);

  /** Multiply r/m64 by 2, imm8 times. */
  void sal(const M64& arg0, const Imm8& arg1);

  /** Multiply r/m64 by 2, once. */
  void sal(const M64& arg0, const One& arg1);

  /** Multiply r/m8 by 2, CL times. */
  void sal(const M8& arg0, const Cl& arg1);

  /** Multiply r/m8 by 2, imm8 times. */
  void sal(const M8& arg0, const Imm8& arg1);

  /** Multiply r/m8 by 2, once. */
  void sal(const M8& arg0, const One& arg1);

  /** Multiply r/m16 by 2, CL times. */
  void sal(const R16& arg0, const Cl& arg1);

  /** Multiply r/m16 by 2, imm8 times. */
  void sal(const R16& arg0, const Imm8& arg1);

  /** Multiply r/m16 by 2, once. */
  void sal(const R16& arg0, const One& arg1);

  /** Multiply r/m32 by 2, CL times. */
  void sal(const R32& arg0, const Cl& arg1);

  /** Multiply r/m32 by 2, imm8 times. */
  void sal(const R32& arg0, const Imm8& arg1);

  /** Multiply r/m32 by 2, once. */
  void sal(const R32& arg0, const One& arg1);

  /** Multiply r/m64 by 2, CL times. */
  void sal(const R64& arg0, const Cl& arg1);

  /** Multiply r/m64 by 2, imm8 times. */
  void sal(const R64& arg0, const Imm8& arg1);

  /** Multiply r/m64 by 2, once. */
  void sal(const R64& arg0, const One& arg1);

  /** Multiply r/m8 by 2, CL times. */
  void sal(const R8& arg0, const Cl& arg1);

  /** Multiply r/m8 by 2, imm8 times. */
  void sal(const R8& arg0, const Imm8& arg1);

  /** Multiply r/m8 by 2, once. */
  void sal(const R8& arg0, const One& arg1);

  /** Multiply r/m8 by 2, CL times. */
  void sal(const Rh& arg0, const Cl& arg1);

  /** Multiply r/m8 by 2, imm8 times. */
  void sal(const Rh& arg0, const Imm8& arg1);

  /** Multiply r/m8 by 2, once. */
  void sal(const Rh& arg0, const One& arg1);

  /** Signed divide r/m16 by 2, CL times. */
  void sar(const M16& arg0, const Cl& arg1);

  /** Signed divide r/m16 by 2, imm8 times. */
  void sar(const M16& arg0, const Imm8& arg1);

  /** Signed divide r/m16 by 2, once. */
  void sar(const M16& arg0, const One& arg1);

  /** Signed divide r/m32 by 2, CL times. */
  void sar(const M32& arg0, const Cl& arg1);

  /** Signed divide r/m32 by 2, imm8 times. */
  void sar(const M32& arg0, const Imm8& arg1);

  /** Signed divide r/m32 by 2, once. */
  void sar(const M32& arg0, const One& arg1);

  /** Signed divide r/m32 by 2, CL times. */
  void sar(const M64& arg0, const Cl& arg1);

  /** Signed divide r/m32 by 2, imm8 times. */
  void sar(const M64& arg0, const Imm8& arg1);

  /** Signed divide r/m32 by 2, once. */
  void sar(const M64& arg0, const One& arg1);

  /** Signed divide r/m8 by 2, CL times. */
  void sar(const M8& arg0, const Cl& arg1);

  /** Signed divide r/m8 by 2, imm8 time. */
  void sar(const M8& arg0, const Imm8& arg1);

  /** Signed divide r/m8 by 2, once. */
  void sar(const M8& arg0, const One& arg1);

  /** Signed divide r/m16 by 2, CL times. */
  void sar(const R16& arg0, const Cl& arg1);

  /** Signed divide r/m16 by 2, imm8 times. */
  void sar(const R16& arg0, const Imm8& arg1);

  /** Signed divide r/m16 by 2, once. */
  void sar(const R16& arg0, const One& arg1);

  /** Signed divide r/m32 by 2, CL times. */
  void sar(const R32& arg0, const Cl& arg1);

  /** Signed divide r/m32 by 2, imm8 times. */
  void sar(const R32& arg0, const Imm8& arg1);

  /** Signed divide r/m32 by 2, once. */
  void sar(const R32& arg0, const One& arg1);

  /** Signed divide r/m32 by 2, CL times. */
  void sar(const R64& arg0, const Cl& arg1);

  /** Signed divide r/m32 by 2, imm8 times. */
  void sar(const R64& arg0, const Imm8& arg1);

  /** Signed divide r/m32 by 2, once. */
  void sar(const R64& arg0, const One& arg1);

  /** Signed divide r/m8 by 2, CL times. */
  void sar(const R8& arg0, const Cl& arg1);

  /** Signed divide r/m8 by 2, imm8 time. */
  void sar(const R8& arg0, const Imm8& arg1);

  /** Signed divide r/m8 by 2, once. */
  void sar(const R8& arg0, const One& arg1);

  /** Signed divide r/m8 by 2, CL times. */
  void sar(const Rh& arg0, const Cl& arg1);

  /** Signed divide r/m8 by 2, imm8 time. */
  void sar(const Rh& arg0, const Imm8& arg1);

  /** Signed divide r/m8 by 2, once. */
  void sar(const Rh& arg0, const One& arg1);

  /** Shift r/m32 arithmetically right with count specified in r32b */
  void sarx(const R32& arg0, const M32& arg1, const R32& arg2);

  /** Shift r/m32 arithmetically right with count specified in r32b */
  void sarx(const R32& arg0, const R32& arg1, const R32& arg2);

  /** Shift r/m64 arithmetically right with count specified in r64b */
  void sarx(const R64& arg0, const M64& arg1, const R64& arg2);

  /** Shift r/m64 arithmetically right with count specified in r64b */
  void sarx(const R64& arg0, const R64& arg1, const R64& arg2);

  /** Subtract with borrow imm8 from AL. */
  void sbb(const Al& arg0, const Imm8& arg1);

  /** Subtract with borrow imm16 from AX. */
  void sbb(const Ax& arg0, const Imm16& arg1);

  /** Subtract with borrow imm32 from EAX. */
  void sbb(const Eax& arg0, const Imm32& arg1);

  /** Subtract with borrow imm16 from r/m16. */
  void sbb(const M16& arg0, const Imm16& arg1);

  /** Subtract with borrow sign-extended imm8 from r/m16. */
  void sbb(const M16& arg0, const Imm8& arg1);

  /** Subtract with borrow r16 from r/m16. */
  void sbb(const M16& arg0, const R16& arg1);

  /** Subtract with borrow imm32 from r/m32. */
  void sbb(const M32& arg0, const Imm32& arg1);

  /** Subtract with borrow sign-extended imm8 from r/m32. */
  void sbb(const M32& arg0, const Imm8& arg1);

  /** Subtract with borrow r32 from r/m32. */
  void sbb(const M32& arg0, const R32& arg1);

  /** Subtract with borrow sign-extended imm32 to 64-bits from r/m64. */
  void sbb(const M64& arg0, const Imm32& arg1);

  /** Subtract with borrow sign-extended imm8 from r/m64. */
  void sbb(const M64& arg0, const Imm8& arg1);

  /** Subtract with borrow r64 from r/m64. */
  void sbb(const M64& arg0, const R64& arg1);

  /** Subtract with borrow imm8 from r/m8. */
  void sbb(const M8& arg0, const Imm8& arg1);

  /** Subtract with borrow r8 from r/m8. */
  void sbb(const M8& arg0, const R8& arg1);

  /** Subtract with borrow r8 from r/m8. */
  void sbb(const M8& arg0, const Rh& arg1);

  /** Subtract with borrow imm16 from r/m16. */
  void sbb(const R16& arg0, const Imm16& arg1);

  /** Subtract with borrow sign-extended imm8 from r/m16. */
  void sbb(const R16& arg0, const Imm8& arg1);

  /** Subtract with borrow r/m16 from r16. */
  void sbb(const R16& arg0, const M16& arg1);

  /** Subtract with borrow r16 from r/m16. */
  void sbb(const R16& arg0, const R16& arg1);

  /** Subtract with borrow r/m16 from r16. */
  void sbb_1(const R16& arg0, const R16& arg1);

  /** Subtract with borrow imm32 from r/m32. */
  void sbb(const R32& arg0, const Imm32& arg1);

  /** Subtract with borrow sign-extended imm8 from r/m32. */
  void sbb(const R32& arg0, const Imm8& arg1);

  /** Subtract with borrow r/m32 from r32. */
  void sbb(const R32& arg0, const M32& arg1);

  /** Subtract with borrow r32 from r/m32. */
  void sbb(const R32& arg0, const R32& arg1);

  /** Subtract with borrow r/m32 from r32. */
  void sbb_1(const R32& arg0, const R32& arg1);

  /** Subtract with borrow sign-extended imm32 to 64-bits from r/m64. */
  void sbb(const R64& arg0, const Imm32& arg1);

  /** Subtract with borrow sign-extended imm8 from r/m64. */
  void sbb(const R64& arg0, const Imm8& arg1);

  /** Subtract with borrow r/m64 from r64. */
  void sbb(const R64& arg0, const M64& arg1);

  /** Subtract with borrow r64 from r/m64. */
  void sbb(const R64& arg0, const R64& arg1);

  /** Subtract with borrow r/m64 from r64. */
  void sbb_1(const R64& arg0, const R64& arg1);

  /** Subtract with borrow imm8 from r/m8. */
  void sbb(const R8& arg0, const Imm8& arg1);

  /** Subtract with borrow r/m8 from r8. */
  void sbb(const R8& arg0, const M8& arg1);

  /** Subtract with borrow r8 from r/m8. */
  void sbb(const R8& arg0, const R8& arg1);

  /** Subtract with borrow r/m8 from r8. */
  void sbb_1(const R8& arg0, const R8& arg1);

  /** Subtract with borrow r8 from r/m8. */
  void sbb(const R8& arg0, const Rh& arg1);

  /** Subtract with borrow r/m8 from r8. */
  void sbb_1(const R8& arg0, const Rh& arg1);

  /** Subtract with borrow sign-extended imm.32 to 64-bits from RAX. */
  void sbb(const Rax& arg0, const Imm32& arg1);

  /** Subtract with borrow imm8 from r/m8. */
  void sbb(const Rh& arg0, const Imm8& arg1);

  /** Subtract with borrow r/m8 from r8. */
  void sbb(const Rh& arg0, const M8& arg1);

  /** Subtract with borrow r8 from r/m8. */
  void sbb(const Rh& arg0, const R8& arg1);

  /** Subtract with borrow r/m8 from r8. */
  void sbb_1(const Rh& arg0, const R8& arg1);

  /** Subtract with borrow r8 from r/m8. */
  void sbb(const Rh& arg0, const Rh& arg1);

  /** Subtract with borrow r/m8 from r8. */
  void sbb_1(const Rh& arg0, const Rh& arg1);

  /** Compare AX with word at ES:(E)DI or RDI, then set status flags. */
  void scas(const M16& arg0);

  /** Compare EAX with doubleword at ES(E)DI or RDI then set status flags. */
  void scas(const M32& arg0);

  /** Compare RAX with quadword at RDI or EDI then set status flags. */
  void scas(const M64& arg0);

  /** Compare AL with byte at ES:(E)DI or RDI, then set status flags. */
  void scas(const M8& arg0);

  /** Compare AL with byte at ES:(E)DI or RDI then set status flags. */
  void scasb();

  /** Compare EAX with doubleword at ES:(E)DI or RDI then set status flags. */
  void scasd();

  /** Compare RAX with quadword at RDI or EDI then set status flags. */
  void scasq();

  /** Compare AX with word at ES:(E)DI or RDI then set status flags. */
  void scasw();

  /** Set byte if above (CF=0 and ZF=0). */
  void seta(const M8& arg0);

  /** Set byte if above (CF=0 and ZF=0). */
  void seta(const R8& arg0);

  /** Set byte if above (CF=0 and ZF=0). */
  void seta(const Rh& arg0);

  /** Set byte if above or equal (CF=0). */
  void setae(const M8& arg0);

  /** Set byte if above or equal (CF=0). */
  void setae(const R8& arg0);

  /** Set byte if above or equal (CF=0). */
  void setae(const Rh& arg0);

  /** Set byte if below (CF=1). */
  void setb(const M8& arg0);

  /** Set byte if below (CF=1). */
  void setb(const R8& arg0);

  /** Set byte if below (CF=1). */
  void setb(const Rh& arg0);

  /** Set byte if below or equal (CF=1 or ZF=1). */
  void setbe(const M8& arg0);

  /** Set byte if below or equal (CF=1 or ZF=1). */
  void setbe(const R8& arg0);

  /** Set byte if below or equal (CF=1 or ZF=1). */
  void setbe(const Rh& arg0);

  /** Set byte if carry (CF=1). */
  void setc(const M8& arg0);

  /** Set byte if carry (CF=1). */
  void setc(const R8& arg0);

  /** Set byte if carry (CF=1). */
  void setc(const Rh& arg0);

  /** Set byte if equal (ZF=1). */
  void sete(const M8& arg0);

  /** Set byte if equal (ZF=1). */
  void sete(const R8& arg0);

  /** Set byte if equal (ZF=1). */
  void sete(const Rh& arg0);

  /** Set byte if greater (ZF=0 and SF=OF). */
  void setg(const M8& arg0);

  /** Set byte if greater (ZF=0 and SF=OF). */
  void setg(const R8& arg0);

  /** Set byte if greater (ZF=0 and SF=OF). */
  void setg(const Rh& arg0);

  /** Set byte if greater or equal (SF=OF). */
  void setge(const M8& arg0);

  /** Set byte if greater or equal (SF=OF). */
  void setge(const R8& arg0);

  /** Set byte if greater or equal (SF=OF). */
  void setge(const Rh& arg0);

  /** Set byte if less (SF!= OF). */
  void setl(const M8& arg0);

  /** Set byte if less (SF!= OF). */
  void setl(const R8& arg0);

  /** Set byte if less (SF!= OF). */
  void setl(const Rh& arg0);

  /** Set byte if less or equal (ZF=1 or SF!= OF). */
  void setle(const M8& arg0);

  /** Set byte if less or equal (ZF=1 or SF!= OF). */
  void setle(const R8& arg0);

  /** Set byte if less or equal (ZF=1 or SF!= OF). */
  void setle(const Rh& arg0);

  /** Set byte if not above (CF=1 or ZF=1). */
  void setna(const M8& arg0);

  /** Set byte if not above (CF=1 or ZF=1). */
  void setna(const R8& arg0);

  /** Set byte if not above (CF=1 or ZF=1). */
  void setna(const Rh& arg0);

  /** Set byte if not above or equal (CF=1). */
  void setnae(const M8& arg0);

  /** Set byte if not above or equal (CF=1). */
  void setnae(const R8& arg0);

  /** Set byte if not above or equal (CF=1). */
  void setnae(const Rh& arg0);

  /** Set byte if not below (CF=0). */
  void setnb(const M8& arg0);

  /** Set byte if not below (CF=0). */
  void setnb(const R8& arg0);

  /** Set byte if not below (CF=0). */
  void setnb(const Rh& arg0);

  /** Set byte if not below or equal (CF=0 and ZF=0). */
  void setnbe(const M8& arg0);

  /** Set byte if not below or equal (CF=0 and ZF=0). */
  void setnbe(const R8& arg0);

  /** Set byte if not below or equal (CF=0 and ZF=0). */
  void setnbe(const Rh& arg0);

  /** Set byte if not carry (CF=0). */
  void setnc(const M8& arg0);

  /** Set byte if not carry (CF=0). */
  void setnc(const R8& arg0);

  /** Set byte if not carry (CF=0). */
  void setnc(const Rh& arg0);

  /** Set byte if not equal (ZF=0). */
  void setne(const M8& arg0);

  /** Set byte if not equal (ZF=0). */
  void setne(const R8& arg0);

  /** Set byte if not equal (ZF=0). */
  void setne(const Rh& arg0);

  /** Set byte if not greater (ZF=1 or SF!= OF) */
  void setng(const M8& arg0);

  /** Set byte if not greater (ZF=1 or SF!= OF) */
  void setng(const R8& arg0);

  /** Set byte if not greater (ZF=1 or SF!= OF) */
  void setng(const Rh& arg0);

  /** Set byte if not greater or equal (SF!= OF). */
  void setnge(const M8& arg0);

  /** Set byte if not greater or equal (SF!= OF). */
  void setnge(const R8& arg0);

  /** Set byte if not greater or equal (SF!= OF). */
  void setnge(const Rh& arg0);

  /** Set byte if not less (SF=OF). */
  void setnl(const M8& arg0);

  /** Set byte if not less (SF=OF). */
  void setnl(const R8& arg0);

  /** Set byte if not less (SF=OF). */
  void setnl(const Rh& arg0);

  /** Set byte if not less or equal (ZF=0 and SF=OF). */
  void setnle(const M8& arg0);

  /** Set byte if not less or equal (ZF=0 and SF=OF). */
  void setnle(const R8& arg0);

  /** Set byte if not less or equal (ZF=0 and SF=OF). */
  void setnle(const Rh& arg0);

  /** Set byte if not overflow (OF=0). */
  void setno(const M8& arg0);

  /** Set byte if not overflow (OF=0). */
  void setno(const R8& arg0);

  /** Set byte if not overflow (OF=0). */
  void setno(const Rh& arg0);

  /** Set byte if not parity (PF=0). */
  void setnp(const M8& arg0);

  /** Set byte if not parity (PF=0). */
  void setnp(const R8& arg0);

  /** Set byte if not parity (PF=0). */
  void setnp(const Rh& arg0);

  /** Set byte if not sign (SF=0). */
  void setns(const M8& arg0);

  /** Set byte if not sign (SF=0). */
  void setns(const R8& arg0);

  /** Set byte if not sign (SF=0). */
  void setns(const Rh& arg0);

  /** Set byte if not zero (ZF=0). */
  void setnz(const M8& arg0);

  /** Set byte if not zero (ZF=0). */
  void setnz(const R8& arg0);

  /** Set byte if not zero (ZF=0). */
  void setnz(const Rh& arg0);

  /** Set byte if overflow (OF=1) */
  void seto(const M8& arg0);

  /** Set byte if overflow (OF=1) */
  void seto(const R8& arg0);

  /** Set byte if overflow (OF=1) */
  void seto(const Rh& arg0);

  /** Set byte if parity (PF=1). */
  void setp(const M8& arg0);

  /** Set byte if parity (PF=1). */
  void setp(const R8& arg0);

  /** Set byte if parity (PF=1). */
  void setp(const Rh& arg0);

  /** Set byte if parity even (PF=1). */
  void setpe(const M8& arg0);

  /** Set byte if parity even (PF=1). */
  void setpe(const R8& arg0);

  /** Set byte if parity even (PF=1). */
  void setpe(const Rh& arg0);

  /** Set byte if parity odd (PF=0). */
  void setpo(const M8& arg0);

  /** Set byte if parity odd (PF=0). */
  void setpo(const R8& arg0);

  /** Set byte if parity odd (PF=0). */
  void setpo(const Rh& arg0);

  /** Set byte if sign (SF=1). */
  void sets(const M8& arg0);

  /** Set byte if sign (SF=1). */
  void sets(const R8& arg0);

  /** Set byte if sign (SF=1). */
  void sets(const Rh& arg0);

  /** Set byte if zero (ZF=1). */
  void setz(const M8& arg0);

  /** Set byte if zero (ZF=1). */
  void setz(const R8& arg0);

  /** Set byte if zero (ZF=1). */
  void setz(const Rh& arg0);

  /** Serializes store operations. */
  void sfence();

  /** Multiply r/m16 by 2, CL times. */
  void shl(const M16& arg0, const Cl& arg1);

  /** Multiply r/m16 by 2, imm8 times. */
  void shl(const M16& arg0, const Imm8& arg1);

  /** Multiply r/m16 by 2, once. */
  void shl(const M16& arg0, const One& arg1);

  /** Multiply r/m32 by 2, CL times. */
  void shl(const M32& arg0, const Cl& arg1);

  /** Multiply r/m32 by 2, imm8 times. */
  void shl(const M32& arg0, const Imm8& arg1);

  /** Multiply r/m32 by 2, once. */
  void shl(const M32& arg0, const One& arg1);

  /** Multiply r/m32 by 2, CL times. */
  void shl(const M64& arg0, const Cl& arg1);

  /** Multiply r/m32 by 2, imm8 times. */
  void shl(const M64& arg0, const Imm8& arg1);

  /** Multiply r/m64 by 2, once. */
  void shl(const M64& arg0, const One& arg1);

  /** Multiply r/m8 by 2, CL times. */
  void shl(const M8& arg0, const Cl& arg1);

  /** Multiply r/m8 by 2, imm8 times. */
  void shl(const M8& arg0, const Imm8& arg1);

  /** Multiply r/m8 by 2, once. */
  void shl(const M8& arg0, const One& arg1);

  /** Multiply r/m16 by 2, CL times. */
  void shl(const R16& arg0, const Cl& arg1);

  /** Multiply r/m16 by 2, imm8 times. */
  void shl(const R16& arg0, const Imm8& arg1);

  /** Multiply r/m16 by 2, once. */
  void shl(const R16& arg0, const One& arg1);

  /** Multiply r/m32 by 2, CL times. */
  void shl(const R32& arg0, const Cl& arg1);

  /** Multiply r/m32 by 2, imm8 times. */
  void shl(const R32& arg0, const Imm8& arg1);

  /** Multiply r/m32 by 2, once. */
  void shl(const R32& arg0, const One& arg1);

  /** Multiply r/m32 by 2, CL times. */
  void shl(const R64& arg0, const Cl& arg1);

  /** Multiply r/m32 by 2, imm8 times. */
  void shl(const R64& arg0, const Imm8& arg1);

  /** Multiply r/m64 by 2, once. */
  void shl(const R64& arg0, const One& arg1);

  /** Multiply r/m8 by 2, CL times. */
  void shl(const R8& arg0, const Cl& arg1);

  /** Multiply r/m8 by 2, imm8 times. */
  void shl(const R8& arg0, const Imm8& arg1);

  /** Multiply r/m8 by 2, once. */
  void shl(const R8& arg0, const One& arg1);

  /** Multiply r/m8 by 2, CL times. */
  void shl(const Rh& arg0, const Cl& arg1);

  /** Multiply r/m8 by 2, imm8 times. */
  void shl(const Rh& arg0, const Imm8& arg1);

  /** Multiply r/m8 by 2, once. */
  void shl(const Rh& arg0, const One& arg1);

  /** Shift r/m16 to left CL places while shifting bits from r16 in from the right. */
  void shld(const M16& arg0, const R16& arg1, const Cl& arg2);

  /** Shift r/m16 to left imm8 places while shifting bits from r16 in from the right. */
  void shld(const M16& arg0, const R16& arg1, const Imm8& arg2);

  /** Shift r/m32 to left CL places while shifting bits from r32 in from the right. */
  void shld(const M32& arg0, const R32& arg1, const Cl& arg2);

  /** Shift r/m32 to left imm8 places while shifting bits from r32 in from the right. */
  void shld(const M32& arg0, const R32& arg1, const Imm8& arg2);

  /** Shift r/m64 to left CL places while shifting bits from r64 in from the right. */
  void shld(const M64& arg0, const R64& arg1, const Cl& arg2);

  /** Shift r/m64 to left imm8 places while shifting bits from r64 in from the right. */
  void shld(const M64& arg0, const R64& arg1, const Imm8& arg2);

  /** Shift r/m16 to left CL places while shifting bits from r16 in from the right. */
  void shld(const R16& arg0, const R16& arg1, const Cl& arg2);

  /** Shift r/m16 to left imm8 places while shifting bits from r16 in from the right. */
  void shld(const R16& arg0, const R16& arg1, const Imm8& arg2);

  /** Shift r/m32 to left CL places while shifting bits from r32 in from the right. */
  void shld(const R32& arg0, const R32& arg1, const Cl& arg2);

  /** Shift r/m32 to left imm8 places while shifting bits from r32 in from the right. */
  void shld(const R32& arg0, const R32& arg1, const Imm8& arg2);

  /** Shift r/m64 to left CL places while shifting bits from r64 in from the right. */
  void shld(const R64& arg0, const R64& arg1, const Cl& arg2);

  /** Shift r/m64 to left imm8 places while shifting bits from r64 in from the right. */
  void shld(const R64& arg0, const R64& arg1, const Imm8& arg2);

  /** Shift r/m32 logically left with count specified in r32b */
  void shlx(const R32& arg0, const M32& arg1, const R32& arg2);

  /** Shift r/m32 logically left with count specified in r32b */
  void shlx(const R32& arg0, const R32& arg1, const R32& arg2);

  /** Shift r/m64 logically left with count specified in r64b. */
  void shlx(const R64& arg0, const M64& arg1, const R64& arg2);

  /** Shift r/m64 logically left with count specified in r64b. */
  void shlx(const R64& arg0, const R64& arg1, const R64& arg2);

  /** Unsigned divide r/m16 by 2, CL times */
  void shr(const M16& arg0, const Cl& arg1);

  /** Unsigned divide r/m16 by 2, imm8 times. */
  void shr(const M16& arg0, const Imm8& arg1);

  /** Unsigned divide r/m16 by 2, once. */
  void shr(const M16& arg0, const One& arg1);

  /** Unsigned divide r/m32 by 2, CL times. */
  void shr(const M32& arg0, const Cl& arg1);

  /** Unsigned divide r/m32 by 2, imm8 times. */
  void shr(const M32& arg0, const Imm8& arg1);

  /** Unsigned divide r/m32 by 2, once. */
  void shr(const M32& arg0, const One& arg1);

  /** Unsigned divide r/m32 by 2, CL times. */
  void shr(const M64& arg0, const Cl& arg1);

  /** Unsigned divide r/m32 by 2, imm8 times. */
  void shr(const M64& arg0, const Imm8& arg1);

  /** Unsigned divide r/m32 by 2, once. */
  void shr(const M64& arg0, const One& arg1);

  /** Unsigned divide r/m8 by 2, CL times. */
  void shr(const M8& arg0, const Cl& arg1);

  /** Unsigned divide r/m8 by 2, imm8 times. */
  void shr(const M8& arg0, const Imm8& arg1);

  /** Unsigned divide r/m8 by 2, once. */
  void shr(const M8& arg0, const One& arg1);

  /** Unsigned divide r/m16 by 2, CL times */
  void shr(const R16& arg0, const Cl& arg1);

  /** Unsigned divide r/m16 by 2, imm8 times. */
  void shr(const R16& arg0, const Imm8& arg1);

  /** Unsigned divide r/m16 by 2, once. */
  void shr(const R16& arg0, const One& arg1);

  /** Unsigned divide r/m32 by 2, CL times. */
  void shr(const R32& arg0, const Cl& arg1);

  /** Unsigned divide r/m32 by 2, imm8 times. */
  void shr(const R32& arg0, const Imm8& arg1);

  /** Unsigned divide r/m32 by 2, once. */
  void shr(const R32& arg0, const One& arg1);

  /** Unsigned divide r/m32 by 2, CL times. */
  void shr(const R64& arg0, const Cl& arg1);

  /** Unsigned divide r/m32 by 2, imm8 times. */
  void shr(const R64& arg0, const Imm8& arg1);

  /** Unsigned divide r/m32 by 2, once. */
  void shr(const R64& arg0, const One& arg1);

  /** Unsigned divide r/m8 by 2, CL times. */
  void shr(const R8& arg0, const Cl& arg1);

  /** Unsigned divide r/m8 by 2, imm8 times. */
  void shr(const R8& arg0, const Imm8& arg1);

  /** Unsigned divide r/m8 by 2, once. */
  void shr(const R8& arg0, const One& arg1);

  /** Unsigned divide r/m8 by 2, CL times. */
  void shr(const Rh& arg0, const Cl& arg1);

  /** Unsigned divide r/m8 by 2, imm8 times. */
  void shr(const Rh& arg0, const Imm8& arg1);

  /** Unsigned divide r/m8 by 2, once. */
  void shr(const Rh& arg0, const One& arg1);

  /** Shift r/m16 to right CL places while shifting bits from r16 in from the left. */
  void shrd(const M16& arg0, const R16& arg1, const Cl& arg2);

  /** Shift r/m16 to right imm8 places while shifting bits from r16 in from the left. */
  void shrd(const M16& arg0, const R16& arg1, const Imm8& arg2);

  /** Shift r/m32 to right CL places while shifting bits from r32 in from the left. */
  void shrd(const M32& arg0, const R32& arg1, const Cl& arg2);

  /** Shift r/m32 to right imm8 places while shifting bits from r32 in from the left. */
  void shrd(const M32& arg0, const R32& arg1, const Imm8& arg2);

  /** Shift r/m64 to right CL places while shifting bits from r64 in from the left. */
  void shrd(const M64& arg0, const R64& arg1, const Cl& arg2);

  /** Shift r/m64 to right imm8 places while shifting bits from r64 in from the left. */
  void shrd(const M64& arg0, const R64& arg1, const Imm8& arg2);

  /** Shift r/m16 to right CL places while shifting bits from r16 in from the left. */
  void shrd(const R16& arg0, const R16& arg1, const Cl& arg2);

  /** Shift r/m16 to right imm8 places while shifting bits from r16 in from the left. */
  void shrd(const R16& arg0, const R16& arg1, const Imm8& arg2);

  /** Shift r/m32 to right CL places while shifting bits from r32 in from the left. */
  void shrd(const R32& arg0, const R32& arg1, const Cl& arg2);

  /** Shift r/m32 to right imm8 places while shifting bits from r32 in from the left. */
  void shrd(const R32& arg0, const R32& arg1, const Imm8& arg2);

  /** Shift r/m64 to right CL places while shifting bits from r64 in from the left. */
  void shrd(const R64& arg0, const R64& arg1, const Cl& arg2);

  /** Shift r/m64 to right imm8 places while shifting bits from r64 in from the left. */
  void shrd(const R64& arg0, const R64& arg1, const Imm8& arg2);

  /** Shift r/m32 logically right with count specified in r32b */
  void shrx(const R32& arg0, const M32& arg1, const R32& arg2);

  /** Shift r/m32 logically right with count specified in r32b */
  void shrx(const R32& arg0, const R32& arg1, const R32& arg2);

  /** Shift r/m64 logically right with count specified in r64b */
  void shrx(const R64& arg0, const M64& arg1, const R64& arg2);

  /** Shift r/m64 logically right with count specified in r64b */
  void shrx(const R64& arg0, const R64& arg1, const R64& arg2);

  /** Shuffle packed double-precision floating- point values selected by imm8 from xmm1 and xmm2/m128 to xmm1. */
  void shufpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Shuffle packed double-precision floating- point values selected by imm8 from xmm1 and xmm2/m128 to xmm1. */
  void shufpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shuffle packed single-precision floating-point values selected by imm8 from xmm1 and xmm1/m128 to xmm1. */
  void shufps(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Shuffle packed single-precision floating-point values selected by imm8 from xmm1 and xmm1/m128 to xmm1. */
  void shufps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Computes square roots of the packed double- precision floating-point values in xmm2/m128 and stores the results in xmm1. */
  void sqrtpd(const Xmm& arg0, const M128& arg1);

  /** Computes square roots of the packed double- precision floating-point values in xmm2/m128 and stores the results in xmm1. */
  void sqrtpd(const Xmm& arg0, const Xmm& arg1);

  /** Computes square roots of the packed single- precision floating-point values in xmm2/m128 and stores the results in xmm1. */
  void sqrtps(const Xmm& arg0, const M128& arg1);

  /** Computes square roots of the packed single- precision floating-point values in xmm2/m128 and stores the results in xmm1. */
  void sqrtps(const Xmm& arg0, const Xmm& arg1);

  /** Computes square root of the low double- precision floating-point value in xmm2/m64 and stores the results in xmm1. */
  void sqrtsd(const Xmm& arg0, const M64& arg1);

  /** Computes square root of the low double- precision floating-point value in xmm2/m64 and stores the results in xmm1. */
  void sqrtsd(const Xmm& arg0, const Xmm& arg1);

  /** Computes square root of the low single- precision floating-point value in xmm2/m32 and stores the results in xmm1. */
  void sqrtss(const Xmm& arg0, const M32& arg1);

  /** Computes square root of the low single- precision floating-point value in xmm2/m32 and stores the results in xmm1. */
  void sqrtss(const Xmm& arg0, const Xmm& arg1);

  /** Set CF flag. */
  void stc();

  /** Set DF flag. */
  void std_();

  /** Set interrupt flag; external, maskable interrupts enabled at the end of the next instruction. */
  void sti();

  /** Store contents of MXCSR register to m32. */
  void stmxcsr(const M32& arg0);

  /** For legacy mode, store AX at address ES:(E)DI; For 64-bit mode store AX at address RDI or EDI. */
  void stos(const M16& arg0);

  /** For legacy mode, store EAX at address ES:(E)DI; For 64-bit mode store EAX at address RDI or EDI. */
  void stos(const M32& arg0);

  /** Store RAX at address RDI or EDI. */
  void stos(const M64& arg0);

  /** For legacy mode, store AL at address ES:(E)DI; For 64-bit mode store AL at address RDI or EDI. */
  void stos(const M8& arg0);

  /** For legacy mode, store AL at address ES:(E)DI; For 64-bit mode store AL at address RDI or EDI. */
  void stosb();

  /** For legacy mode, store EAX at address ES:(E)DI; For 64-bit mode store EAX at address RDI or EDI. */
  void stosd();

  /** Store RAX at address RDI or EDI. */
  void stosq();

  /** For legacy mode, store AX at address ES:(E)DI; For 64-bit mode store AX at address RDI or EDI. */
  void stosw();

  /** Subtract imm8 from AL. */
  void sub(const Al& arg0, const Imm8& arg1);

  /** Subtract imm16 from AX. */
  void sub(const Ax& arg0, const Imm16& arg1);

  /** Subtract imm32 from EAX. */
  void sub(const Eax& arg0, const Imm32& arg1);

  /** Subtract imm16 from r/m16. */
  void sub(const M16& arg0, const Imm16& arg1);

  /** Subtract sign-extended imm8 from r/m16. */
  void sub(const M16& arg0, const Imm8& arg1);

  /** Subtract r16 from r/m16. */
  void sub(const M16& arg0, const R16& arg1);

  /** Subtract imm32 from r/m32. */
  void sub(const M32& arg0, const Imm32& arg1);

  /** Subtract sign-extended imm8 from r/m32. */
  void sub(const M32& arg0, const Imm8& arg1);

  /** Subtract r32 from r/m32. */
  void sub(const M32& arg0, const R32& arg1);

  /** Subtract imm32 sign-extended to 64-bits from r/m64. */
  void sub(const M64& arg0, const Imm32& arg1);

  /** Subtract sign-extended imm8 from r/m64. */
  void sub(const M64& arg0, const Imm8& arg1);

  /** Subtract r64 from r/m64. */
  void sub(const M64& arg0, const R64& arg1);

  /** Subtract imm8 from r/m8. */
  void sub(const M8& arg0, const Imm8& arg1);

  /** Subtract r8 from r/m8. */
  void sub(const M8& arg0, const R8& arg1);

  /** Subtract r8 from r/m8. */
  void sub(const M8& arg0, const Rh& arg1);

  /** Subtract imm16 from r/m16. */
  void sub(const R16& arg0, const Imm16& arg1);

  /** Subtract sign-extended imm8 from r/m16. */
  void sub(const R16& arg0, const Imm8& arg1);

  /** Subtract r/m16 from r16. */
  void sub(const R16& arg0, const M16& arg1);

  /** Subtract r16 from r/m16. */
  void sub(const R16& arg0, const R16& arg1);

  /** Subtract r/m16 from r16. */
  void sub_1(const R16& arg0, const R16& arg1);

  /** Subtract imm32 from r/m32. */
  void sub(const R32& arg0, const Imm32& arg1);

  /** Subtract sign-extended imm8 from r/m32. */
  void sub(const R32& arg0, const Imm8& arg1);

  /** Subtract r/m32 from r32. */
  void sub(const R32& arg0, const M32& arg1);

  /** Subtract r32 from r/m32. */
  void sub(const R32& arg0, const R32& arg1);

  /** Subtract r/m32 from r32. */
  void sub_1(const R32& arg0, const R32& arg1);

  /** Subtract imm32 sign-extended to 64-bits from r/m64. */
  void sub(const R64& arg0, const Imm32& arg1);

  /** Subtract sign-extended imm8 from r/m64. */
  void sub(const R64& arg0, const Imm8& arg1);

  /** Subtract r/m64 from r64. */
  void sub(const R64& arg0, const M64& arg1);

  /** Subtract r64 from r/m64. */
  void sub(const R64& arg0, const R64& arg1);

  /** Subtract r/m64 from r64. */
  void sub_1(const R64& arg0, const R64& arg1);

  /** Subtract imm8 from r/m8. */
  void sub(const R8& arg0, const Imm8& arg1);

  /** Subtract r/m8 from r8. */
  void sub(const R8& arg0, const M8& arg1);

  /** Subtract r8 from r/m8. */
  void sub(const R8& arg0, const R8& arg1);

  /** Subtract r/m8 from r8. */
  void sub_1(const R8& arg0, const R8& arg1);

  /** Subtract r8 from r/m8. */
  void sub(const R8& arg0, const Rh& arg1);

  /** Subtract r/m8 from r8. */
  void sub_1(const R8& arg0, const Rh& arg1);

  /** Subtract imm32 sign-extended to 64-bits from RAX. */
  void sub(const Rax& arg0, const Imm32& arg1);

  /** Subtract imm8 from r/m8. */
  void sub(const Rh& arg0, const Imm8& arg1);

  /** Subtract r/m8 from r8. */
  void sub(const Rh& arg0, const M8& arg1);

  /** Subtract r8 from r/m8. */
  void sub(const Rh& arg0, const R8& arg1);

  /** Subtract r/m8 from r8. */
  void sub_1(const Rh& arg0, const R8& arg1);

  /** Subtract r8 from r/m8. */
  void sub(const Rh& arg0, const Rh& arg1);

  /** Subtract r/m8 from r8. */
  void sub_1(const Rh& arg0, const Rh& arg1);

  /** Subtract packed double-precision floating- point values in xmm2/m128 from xmm1. */
  void subpd(const Xmm& arg0, const M128& arg1);

  /** Subtract packed double-precision floating- point values in xmm2/m128 from xmm1. */
  void subpd(const Xmm& arg0, const Xmm& arg1);

  /** Subtract packed single-precision floating-point values in xmm2/mem from xmm1. */
  void subps(const Xmm& arg0, const M128& arg1);

  /** Subtract packed single-precision floating-point values in xmm2/mem from xmm1. */
  void subps(const Xmm& arg0, const Xmm& arg1);

  /** Subtracts the low double-precision floating- point values in xmm2/mem64 from xmm1. */
  void subsd(const Xmm& arg0, const M64& arg1);

  /** Subtracts the low double-precision floating- point values in xmm2/mem64 from xmm1. */
  void subsd(const Xmm& arg0, const Xmm& arg1);

  /** Subtract the lower single-precision floating- point values in xmm2/m32 from xmm1. */
  void subss(const Xmm& arg0, const M32& arg1);

  /** Subtract the lower single-precision floating- point values in xmm2/m32 from xmm1. */
  void subss(const Xmm& arg0, const Xmm& arg1);

  /** Exchanges the current GS base register value with the value contained in MSR address C0000102H. */
  void swapgs();

  /** Fast call to privilege level 0 system procedures. */
  void syscall();

  /** Fast call to privilege level 0 system procedures. */
  void sysenter();

  /** Fast return to privilege level 3 user code. */
  void sysexit();

  /** Fast return to 64-bit mode privilege level 3 user code. */
  void sysexit(const PrefRexW& arg0);

  /** Return to compatibility mode from fast system call */
  void sysret();

  /** Return to 64-bit mode from fast system call */
  void sysret(const PrefRexW& arg0);

  /** AND imm8 with AL; set SF, ZF, PF according to result. */
  void test(const Al& arg0, const Imm8& arg1);

  /** AND imm16 with AX; set SF, ZF, PF according to result. */
  void test(const Ax& arg0, const Imm16& arg1);

  /** AND imm32 with EAX; set SF, ZF, PF according to result. */
  void test(const Eax& arg0, const Imm32& arg1);

  /** AND imm16 with r/m16; set SF, ZF, PF according to result. */
  void test(const M16& arg0, const Imm16& arg1);

  /** AND r16 with r/m16; set SF, ZF, PF according to result. */
  void test(const M16& arg0, const R16& arg1);

  /** AND imm32 with r/m32; set SF, ZF, PF according to result. */
  void test(const M32& arg0, const Imm32& arg1);

  /** AND r32 with r/m32; set SF, ZF, PF according to result. */
  void test(const M32& arg0, const R32& arg1);

  /** AND imm32 sign-extended to 64-bits with r/m64; set SF, ZF, PF according to result. */
  void test(const M64& arg0, const Imm32& arg1);

  /** AND r64 with r/m64; set SF, ZF, PF according to result. */
  void test(const M64& arg0, const R64& arg1);

  /** AND imm8 with r/m8; set SF, ZF, PF according to result. */
  void test(const M8& arg0, const Imm8& arg1);

  /** AND r8 with r/m8; set SF, ZF, PF according to result. */
  void test(const M8& arg0, const R8& arg1);

  /** AND r8 with r/m8; set SF, ZF, PF according to result. */
  void test(const M8& arg0, const Rh& arg1);

  /** AND imm16 with r/m16; set SF, ZF, PF according to result. */
  void test(const R16& arg0, const Imm16& arg1);

  /** AND r16 with r/m16; set SF, ZF, PF according to result. */
  void test(const R16& arg0, const R16& arg1);

  /** AND imm32 with r/m32; set SF, ZF, PF according to result. */
  void test(const R32& arg0, const Imm32& arg1);

  /** AND r32 with r/m32; set SF, ZF, PF according to result. */
  void test(const R32& arg0, const R32& arg1);

  /** AND imm32 sign-extended to 64-bits with r/m64; set SF, ZF, PF according to result. */
  void test(const R64& arg0, const Imm32& arg1);

  /** AND r64 with r/m64; set SF, ZF, PF according to result. */
  void test(const R64& arg0, const R64& arg1);

  /** AND imm8 with r/m8; set SF, ZF, PF according to result. */
  void test(const R8& arg0, const Imm8& arg1);

  /** AND r8 with r/m8; set SF, ZF, PF according to result. */
  void test(const R8& arg0, const R8& arg1);

  /** AND r8 with r/m8; set SF, ZF, PF according to result. */
  void test(const R8& arg0, const Rh& arg1);

  /** AND imm32 sign-extended to 64-bits with RAX; set SF, ZF, PF according to result. */
  void test(const Rax& arg0, const Imm32& arg1);

  /** AND imm8 with r/m8; set SF, ZF, PF according to result. */
  void test(const Rh& arg0, const Imm8& arg1);

  /** AND r8 with r/m8; set SF, ZF, PF according to result. */
  void test(const Rh& arg0, const R8& arg1);

  /** AND r8 with r/m8; set SF, ZF, PF according to result. */
  void test(const Rh& arg0, const Rh& arg1);

  /** Count the number of trailing zero bits in r/m16, return result in r16. */
  void tzcnt(const R16& arg0, const M16& arg1);

  /** Count the number of trailing zero bits in r/m16, return result in r16. */
  void tzcnt(const R16& arg0, const R16& arg1);

  /** Count the number of trailing zero bits in r/m32, return result in r32. */
  void tzcnt(const R32& arg0, const M32& arg1);

  /** Count the number of trailing zero bits in r/m32, return result in r32. */
  void tzcnt(const R32& arg0, const R32& arg1);

  /** Count the number of trailing zero bits in r/m64, return result in r64. */
  void tzcnt(const R64& arg0, const M64& arg1);

  /** Count the number of trailing zero bits in r/m64, return result in r64. */
  void tzcnt(const R64& arg0, const R64& arg1);

  /** Compares (unordered) the low double- precision floating-point values in xmm1 and xmm2/m64 and set the EFLAGS accordingly. */
  void ucomisd(const Xmm& arg0, const M64& arg1);

  /** Compares (unordered) the low double- precision floating-point values in xmm1 and xmm2/m64 and set the EFLAGS accordingly. */
  void ucomisd(const Xmm& arg0, const Xmm& arg1);

  /** Compare lower single-precision floating-point value in xmm1 register with lower single- precision floating-point value in xmm2/mem and set the status flags accordingly. */
  void ucomiss(const Xmm& arg0, const M32& arg1);

  /** Compare lower single-precision floating-point value in xmm1 register with lower single- precision floating-point value in xmm2/mem and set the status flags accordingly. */
  void ucomiss(const Xmm& arg0, const Xmm& arg1);

  /** Raise invalid opcode exception. */
  void ud2();

  /** Unpacks and Interleaves double-precision floating-point values from high quadwords of xmm1 and xmm2/m128. */
  void unpckhpd(const Xmm& arg0, const M128& arg1);

  /** Unpacks and Interleaves double-precision floating-point values from high quadwords of xmm1 and xmm2/m128. */
  void unpckhpd(const Xmm& arg0, const Xmm& arg1);

  /** Unpacks and Interleaves single-precision floating-point values from high quadwords of xmm1 and xmm2/mem into xmm1. */
  void unpckhps(const Xmm& arg0, const M128& arg1);

  /** Unpacks and Interleaves single-precision floating-point values from high quadwords of xmm1 and xmm2/mem into xmm1. */
  void unpckhps(const Xmm& arg0, const Xmm& arg1);

  /** Unpacks and Interleaves double-precision floating-point values from low quadwords of xmm1 and xmm2/m128. */
  void unpcklpd(const Xmm& arg0, const M128& arg1);

  /** Unpacks and Interleaves double-precision floating-point values from low quadwords of xmm1 and xmm2/m128. */
  void unpcklpd(const Xmm& arg0, const Xmm& arg1);

  /** Unpacks and Interleaves single-precision floating-point values from low quadwords of xmm1 and xmm2/mem into xmm1. */
  void unpcklps(const Xmm& arg0, const M128& arg1);

  /** Unpacks and Interleaves single-precision floating-point values from low quadwords of xmm1 and xmm2/mem into xmm1. */
  void unpcklps(const Xmm& arg0, const Xmm& arg1);

  /** Add packed double-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vaddpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed double-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vaddpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed double-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vaddpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed double-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vaddpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add packed single-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vaddps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed single-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vaddps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed single-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vaddps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed single-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vaddps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add the low double-precision floating-point value from xmm3/mem to xmm2 and store the result in xmm1. */
  void vaddsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Add the low double-precision floating-point value from xmm3/mem to xmm2 and store the result in xmm1. */
  void vaddsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add the low single-precision floating-point value from xmm3/mem to xmm2 and store the result in xmm1. */
  void vaddss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Add the low single-precision floating-point value from xmm3/mem to xmm2 and store the result in xmm1. */
  void vaddss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add/subtract packed double-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vaddsubpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add/subtract packed double-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vaddsubpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add / subtract packed double-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vaddsubpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add / subtract packed double-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vaddsubpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add/subtract single-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vaddsubps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add/subtract single-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vaddsubps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add / subtract single-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vaddsubps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add / subtract single-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vaddsubps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Perform one round of an AES decryption flow, using the Equivalent Inverse Cipher, operating on a 128-bit data (state) from xmm2 with a 128-bit round key from xmm3/m128; store the result in xmm1. */
  void vaesdec(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Perform one round of an AES decryption flow, using the Equivalent Inverse Cipher, operating on a 128-bit data (state) from xmm2 with a 128-bit round key from xmm3/m128; store the result in xmm1. */
  void vaesdec(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Perform the last round of an AES decryption flow, using the Equivalent Inverse Cipher, operating on a 128-bit data (state) from xmm2 with a 128-bit round key from xmm3/m128; store the result in xmm1. */
  void vaesdeclast(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Perform the last round of an AES decryption flow, using the Equivalent Inverse Cipher, operating on a 128-bit data (state) from xmm2 with a 128-bit round key from xmm3/m128; store the result in xmm1. */
  void vaesdeclast(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Perform one round of an AES encryption flow, operating on a 128-bit data (state) from xmm2 with a 128-bit round key from the xmm3/m128; store the result in xmm1. */
  void vaesenc(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Perform one round of an AES encryption flow, operating on a 128-bit data (state) from xmm2 with a 128-bit round key from the xmm3/m128; store the result in xmm1. */
  void vaesenc(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Perform the last round of an AES encryption flow, operating on a 128-bit data (state) from xmm2 with a 128 bit round key from xmm3/m128; store the result in xmm1. */
  void vaesenclast(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Perform the last round of an AES encryption flow, operating on a 128-bit data (state) from xmm2 with a 128 bit round key from xmm3/m128; store the result in xmm1. */
  void vaesenclast(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Perform the InvMixColumn transformation on a 128-bit round key from xmm2/m128 and store the result in xmm1. */
  void vaesimc(const Xmm& arg0, const M128& arg1);

  /** Perform the InvMixColumn transformation on a 128-bit round key from xmm2/m128 and store the result in xmm1. */
  void vaesimc(const Xmm& arg0, const Xmm& arg1);

  /** Assist in AES round key generation using 8 bits Round Constant (RCON) specified in the immediate byte, operating on 128 bits of data specified in xmm2/m128 and stores the result in xmm1. */
  void vaeskeygenassist(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Assist in AES round key generation using 8 bits Round Constant (RCON) specified in the immediate byte, operating on 128 bits of data specified in xmm2/m128 and stores the result in xmm1. */
  void vaeskeygenassist(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Return the bitwise logical AND NOT of packed double-precision floating-point values in xmm2 and xmm3/mem. */
  void vandnpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the bitwise logical AND NOT of packed double-precision floating-point values in xmm2 and xmm3/mem. */
  void vandnpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the bitwise logical AND NOT of packed double-precision floating-point values in ymm2 and ymm3/mem. */
  void vandnpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the bitwise logical AND NOT of packed double-precision floating-point values in ymm2 and ymm3/mem. */
  void vandnpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the bitwise logical AND NOT of packed single-precision floating-point values in xmm2 and xmm3/mem. */
  void vandnps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the bitwise logical AND NOT of packed single-precision floating-point values in xmm2 and xmm3/mem. */
  void vandnps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the bitwise logical AND NOT of packed single-precision floating-point values in ymm2 and ymm3/mem. */
  void vandnps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the bitwise logical AND NOT of packed single-precision floating-point values in ymm2 and ymm3/mem. */
  void vandnps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the bitwise logical AND of packed double-precision floating-point values in xmm2 and xmm3/mem. */
  void vandpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the bitwise logical AND of packed double-precision floating-point values in xmm2 and xmm3/mem. */
  void vandpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the bitwise logical AND of packed double-precision floating-point values in ymm2 and ymm3/mem. */
  void vandpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the bitwise logical AND of packed double-precision floating-point values in ymm2 and ymm3/mem. */
  void vandpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the bitwise logical AND of packed single-precision floating-point values in xmm2 and xmm3/mem. */
  void vandps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the bitwise logical AND of packed single-precision floating-point values in xmm2 and xmm3/mem. */
  void vandps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the bitwise logical AND of packed single-precision floating-point values in ymm2 and ymm3/mem. */
  void vandps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the bitwise logical AND of packed single-precision floating-point values in ymm2 and ymm3/mem. */
  void vandps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Select packed double-precision floating-point values from xmm2 and xmm3/m128 from mask in imm8 and store the values in xmm1. */
  void vblendpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Select packed double-precision floating-point values from xmm2 and xmm3/m128 from mask in imm8 and store the values in xmm1. */
  void vblendpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Select packed double-precision floating-point values from ymm2 and ymm3/m256 from mask in imm8 and store the values in ymm1. */
  void vblendpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Select packed double-precision floating-point values from ymm2 and ymm3/m256 from mask in imm8 and store the values in ymm1. */
  void vblendpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Select packed single-precision floating-point values from xmm2 and xmm3/m128 from mask in imm8 and store the values in xmm1. */
  void vblendps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Select packed single-precision floating-point values from xmm2 and xmm3/m128 from mask in imm8 and store the values in xmm1. */
  void vblendps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Select packed single-precision floating-point values from ymm2 and ymm3/m256 from mask in imm8 and store the values in ymm1. */
  void vblendps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Select packed single-precision floating-point values from ymm2 and ymm3/m256 from mask in imm8 and store the values in ymm1. */
  void vblendps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Conditionally copy double-precision floating- point values from xmm2 or xmm3/m128 to xmm1, based on mask bits in the mask operand, xmm4. */
  void vblendvpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Xmm& arg3);

  /** Conditionally copy double-precision floating- point values from xmm2 or xmm3/m128 to xmm1, based on mask bits in the mask operand, xmm4. */
  void vblendvpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Xmm& arg3);

  /** Conditionally copy double-precision floating- point values from ymm2 or ymm3/m256 to ymm1, based on mask bits in the mask operand, ymm4. */
  void vblendvpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Ymm& arg3);

  /** Conditionally copy double-precision floating- point values from ymm2 or ymm3/m256 to ymm1, based on mask bits in the mask operand, ymm4. */
  void vblendvpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Ymm& arg3);

  /** Conditionally copy single-precision floating- point values from xmm2 or xmm3/m128 to xmm1, based on mask bits in the specified mask operand, xmm4. */
  void vblendvps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Xmm& arg3);

  /** Conditionally copy single-precision floating- point values from xmm2 or xmm3/m128 to xmm1, based on mask bits in the specified mask operand, xmm4. */
  void vblendvps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Xmm& arg3);

  /** Conditionally copy single-precision floating- point values from ymm2 or ymm3/m256 to ymm1, based on mask bits in the specified mask register, ymm4. */
  void vblendvps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Ymm& arg3);

  /** Conditionally copy single-precision floating- point values from ymm2 or ymm3/m256 to ymm1, based on mask bits in the specified mask register, ymm4. */
  void vblendvps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Ymm& arg3);

  /** Broadcast 128 bits of floating-point data in mem to low and high 128-bits in ymm1. */
  void vbroadcastf128(const Ymm& arg0, const M128& arg1);

  /** Broadcast 128 bits of integer data in mem to low and high 128-bits in ymm1. */
  void vbroadcasti128(const Ymm& arg0, const M128& arg1);

  /** Broadcast double-precision floating-point element in mem to four locations in ymm1. */
  void vbroadcastsd(const Ymm& arg0, const M64& arg1);

  /** Broadcast low double-precision floating-point element in the source operand to four locations in ymm1. */
  void vbroadcastsd(const Ymm& arg0, const Xmm& arg1);

  /** Broadcast single-precision floating-point element in mem to four locations in xmm1. */
  void vbroadcastss(const Xmm& arg0, const M32& arg1);

  /** Broadcast the low single-precision floating- point element in the source operand to four locations in xmm1. */
  void vbroadcastss(const Xmm& arg0, const Xmm& arg1);

  /** Broadcast single-precision floating-point element in mem to eight locations in ymm1. */
  void vbroadcastss(const Ymm& arg0, const M32& arg1);

  /** Broadcast low single-precision floating-point element in the source operand to eight locations in ymm1. */
  void vbroadcastss(const Ymm& arg0, const Xmm& arg1);

  /** Compare packed double-precision floating- point values in xmm3/m128 and xmm2 using bits 4:0 of imm8 as a comparison predicate. */
  void vcmppd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Compare packed double-precision floating- point values in xmm3/m128 and xmm2 using bits 4:0 of imm8 as a comparison predicate. */
  void vcmppd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Compare packed double-precision floating- point values in ymm3/m256 and ymm2 using bits 4:0 of imm8 as a comparison predicate. */
  void vcmppd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Compare packed double-precision floating- point values in ymm3/m256 and ymm2 using bits 4:0 of imm8 as a comparison predicate. */
  void vcmppd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Compare packed single-precision floating- point values in xmm3/m128 and xmm2 using bits 4:0 of imm8 as a comparison predicate. */
  void vcmpps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Compare packed single-precision floating- point values in xmm3/m128 and xmm2 using bits 4:0 of imm8 as a comparison predicate. */
  void vcmpps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Compare packed single-precision floating- point values in ymm3/m256 and ymm2 using bits 4:0 of imm8 as a comparison predicate. */
  void vcmpps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Compare packed single-precision floating- point values in ymm3/m256 and ymm2 using bits 4:0 of imm8 as a comparison predicate. */
  void vcmpps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Compare low double precision floating-point value in xmm3/m64 and xmm2 using bits 4:0 of imm8 as comparison predicate. */
  void vcmpsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2, const Imm8& arg3);

  /** Compare low double precision floating-point value in xmm3/m64 and xmm2 using bits 4:0 of imm8 as comparison predicate. */
  void vcmpsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Compare low single precision floating-point value in xmm3/m32 and xmm2 using bits 4:0 of imm8 as comparison predicate. */
  void vcmpss(const Xmm& arg0, const Xmm& arg1, const M32& arg2, const Imm8& arg3);

  /** Compare low single precision floating-point value in xmm3/m32 and xmm2 using bits 4:0 of imm8 as comparison predicate. */
  void vcmpss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Compare low double precision floating-point values in xmm1 and xmm2/mem64 and set the EFLAGS flags accordingly. */
  void vcomisd(const Xmm& arg0, const M64& arg1);

  /** Compare low double precision floating-point values in xmm1 and xmm2/mem64 and set the EFLAGS flags accordingly. */
  void vcomisd(const Xmm& arg0, const Xmm& arg1);

  /** Compare low single precision floating-point values in xmm1 and xmm2/mem32 and set the EFLAGS flags accordingly. */
  void vcomiss(const Xmm& arg0, const M32& arg1);

  /** Compare low single precision floating-point values in xmm1 and xmm2/mem32 and set the EFLAGS flags accordingly. */
  void vcomiss(const Xmm& arg0, const Xmm& arg1);

  /** Convert two packed signed doubleword integers from xmm2/mem to two packed double-precision floating-point values in xmm1. */
  void vcvtdq2pd(const Xmm& arg0, const M64& arg1);

  /** Convert two packed signed doubleword integers from xmm2/mem to two packed double-precision floating-point values in xmm1. */
  void vcvtdq2pd(const Xmm& arg0, const Xmm& arg1);

  /** Convert four packed signed doubleword integers from ymm2/mem to four packed double-precision floating-point values in ymm1. */
  void vcvtdq2pd(const Ymm& arg0, const M128& arg1);

  /** Convert four packed signed doubleword integers from ymm2/mem to four packed double-precision floating-point values in ymm1. */
  void vcvtdq2pd(const Ymm& arg0, const Xmm& arg1);

  /** Convert four packed signed doubleword integers from xmm2/mem to four packed single-precision floating-point values in xmm1. */
  void vcvtdq2ps(const Xmm& arg0, const M128& arg1);

  /** Convert four packed signed doubleword integers from xmm2/mem to four packed single-precision floating-point values in xmm1. */
  void vcvtdq2ps(const Xmm& arg0, const Xmm& arg1);

  /** Convert eight packed signed doubleword integers from ymm2/mem to eight packed single-precision floating-point values in ymm1. */
  void vcvtdq2ps(const Ymm& arg0, const M256& arg1);

  /** Convert eight packed signed doubleword integers from ymm2/mem to eight packed single-precision floating-point values in ymm1. */
  void vcvtdq2ps(const Ymm& arg0, const Ymm& arg1);

  /** Convert two packed double-precision floating- point values in xmm2/mem to two signed doubleword integers in xmm1. */
  void vcvtpd2dq(const Xmm& arg0, const M128& arg1);

  /** Convert four packed double-precision floating- point values in ymm2/mem to four signed doubleword integers in xmm1. */
  void vcvtpd2dq(const Xmm& arg0, const M256& arg1);

  /** Convert two packed double-precision floating- point values in xmm2/mem to two signed doubleword integers in xmm1. */
  void vcvtpd2dq(const Xmm& arg0, const Xmm& arg1);

  /** Convert four packed double-precision floating- point values in ymm2/mem to four signed doubleword integers in xmm1. */
  void vcvtpd2dq(const Xmm& arg0, const Ymm& arg1);

  /** Convert two packed double-precision floating- point values in xmm2/mem to two single- precision floating-point values in xmm1. */
  void vcvtpd2ps(const Xmm& arg0, const M128& arg1);

  /** Convert four packed double-precision floating- point values in ymm2/mem to four single- precision floating-point values in xmm1. */
  void vcvtpd2ps(const Xmm& arg0, const M256& arg1);

  /** Convert two packed double-precision floating- point values in xmm2/mem to two single- precision floating-point values in xmm1. */
  void vcvtpd2ps(const Xmm& arg0, const Xmm& arg1);

  /** Convert four packed double-precision floating- point values in ymm2/mem to four single- precision floating-point values in xmm1. */
  void vcvtpd2ps(const Xmm& arg0, const Ymm& arg1);

  /** Convert four packed half precision (16-bit) floating-point values in xmm2/m64 to packed single-precision floating-point value in xmm1. */
  void vcvtph2ps(const Xmm& arg0, const M64& arg1);

  /** Convert four packed half precision (16-bit) floating-point values in xmm2/m64 to packed single-precision floating-point value in xmm1. */
  void vcvtph2ps(const Xmm& arg0, const Xmm& arg1);

  /** Convert eight packed half precision (16-bit) floating-point values in xmm2/m128 to packed single-precision floating-point value in ymm1. */
  void vcvtph2ps(const Ymm& arg0, const M128& arg1);

  /** Convert eight packed half precision (16-bit) floating-point values in xmm2/m128 to packed single-precision floating-point value in ymm1. */
  void vcvtph2ps(const Ymm& arg0, const Xmm& arg1);

  /** Convert four packed single precision floating- point values from xmm2/mem to four packed signed doubleword values in xmm1. */
  void vcvtps2dq(const Xmm& arg0, const M128& arg1);

  /** Convert four packed single precision floating- point values from xmm2/mem to four packed signed doubleword values in xmm1. */
  void vcvtps2dq(const Xmm& arg0, const Xmm& arg1);

  /** Convert eight packed single precision floating- point values from ymm2/mem to eight packed signed doubleword values in ymm1. */
  void vcvtps2dq(const Ymm& arg0, const M256& arg1);

  /** Convert eight packed single precision floating- point values from ymm2/mem to eight packed signed doubleword values in ymm1. */
  void vcvtps2dq(const Ymm& arg0, const Ymm& arg1);

  /** Convert two packed single-precision floating- point values in xmm2/mem to two packed double-precision floating-point values in xmm1. */
  void vcvtps2pd(const Xmm& arg0, const M64& arg1);

  /** Convert two packed single-precision floating- point values in xmm2/mem to two packed double-precision floating-point values in xmm1. */
  void vcvtps2pd(const Xmm& arg0, const Xmm& arg1);

  /** Convert four packed single-precision floating- point values in xmm2/mem to four packed double-precision floating-point values in ymm1. */
  void vcvtps2pd(const Ymm& arg0, const M128& arg1);

  /** Convert four packed single-precision floating- point values in xmm2/mem to four packed double-precision floating-point values in ymm1. */
  void vcvtps2pd(const Ymm& arg0, const Xmm& arg1);

  /** Convert eight packed single-precision floating-point value in ymm2 to packed half-precision (16-bit) floating-point value in xmm1/mem. Imm8 provides rounding controls. */
  void vcvtps2ph(const M128& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Convert four packed single-precision float- ing-point value in xmm2 to packed half- precision (16-bit) floating-point value in xmm1/mem. Imm8 provides rounding con- trols. */
  void vcvtps2ph(const M64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Convert four packed single-precision float- ing-point value in xmm2 to packed half- precision (16-bit) floating-point value in xmm1/mem. Imm8 provides rounding con- trols. */
  void vcvtps2ph(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Convert eight packed single-precision floating-point value in ymm2 to packed half-precision (16-bit) floating-point value in xmm1/mem. Imm8 provides rounding controls. */
  void vcvtps2ph(const Xmm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Convert one double precision floating-point value from xmm1/m64 to one signed doubleword integer r32. */
  void vcvtsd2si(const R32& arg0, const M64& arg1);

  /** Convert one double precision floating-point value from xmm1/m64 to one signed doubleword integer r32. */
  void vcvtsd2si(const R32& arg0, const Xmm& arg1);

  /** Convert one double precision floating-point value from xmm1/m64 to one signed quadword integer sign-extended into r64. */
  void vcvtsd2si(const R64& arg0, const M64& arg1);

  /** Convert one double precision floating-point value from xmm1/m64 to one signed quadword integer sign-extended into r64. */
  void vcvtsd2si(const R64& arg0, const Xmm& arg1);

  /** Convert one double-precision floating-point value in xmm3/m64 to one single-precision floating-point value and merge with high bits in xmm2. */
  void vcvtsd2ss(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Convert one double-precision floating-point value in xmm3/m64 to one single-precision floating-point value and merge with high bits in xmm2. */
  void vcvtsd2ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Convert one signed doubleword integer from r/m32 to one double-precision floating-point value in xmm1. */
  void vcvtsi2sd(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Convert one signed quadword integer from r/m64 to one double-precision floating-point value in xmm1. */
  void vcvtsi2sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Convert one signed doubleword integer from r/m32 to one double-precision floating-point value in xmm1. */
  void vcvtsi2sd(const Xmm& arg0, const Xmm& arg1, const R32& arg2);

  /** Convert one signed quadword integer from r/m64 to one double-precision floating-point value in xmm1. */
  void vcvtsi2sd(const Xmm& arg0, const Xmm& arg1, const R64& arg2);

  /** Convert one signed doubleword integer from r/m32 to one single-precision floating-point value in xmm1. */
  void vcvtsi2ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Convert one signed quadword integer from r/m64 to one single-precision floating-point value in xmm1. */
  void vcvtsi2ss(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Convert one signed doubleword integer from r/m32 to one single-precision floating-point value in xmm1. */
  void vcvtsi2ss(const Xmm& arg0, const Xmm& arg1, const R32& arg2);

  /** Convert one signed quadword integer from r/m64 to one single-precision floating-point value in xmm1. */
  void vcvtsi2ss(const Xmm& arg0, const Xmm& arg1, const R64& arg2);

  /** Convert one single-precision floating-point value in xmm3/m32 to one double-precision floating-point value and merge with high bits of xmm2. */
  void vcvtss2sd(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Convert one single-precision floating-point value in xmm3/m32 to one double-precision floating-point value and merge with high bits of xmm2. */
  void vcvtss2sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Convert one single-precision floating-point value from xmm1/m32 to one signed doubleword integer in r32. */
  void vcvtss2si(const R32& arg0, const M32& arg1);

  /** Convert one single-precision floating-point value from xmm1/m32 to one signed doubleword integer in r32. */
  void vcvtss2si(const R32& arg0, const Xmm& arg1);

  /** Convert one single-precision floating-point value from xmm1/m32 to one signed quadword integer in r64. */
  void vcvtss2si(const R64& arg0, const M32& arg1);

  /** Convert one single-precision floating-point value from xmm1/m32 to one signed quadword integer in r64. */
  void vcvtss2si(const R64& arg0, const Xmm& arg1);

  /** Convert two packed double-precision floating- point values in xmm2/mem to two signed doubleword integers in xmm1 using truncation. */
  void vcvttpd2dq(const Xmm& arg0, const M128& arg1);

  /** Convert four packed double-precision floating- point values in ymm2/mem to four signed doubleword integers in xmm1 using truncation. */
  void vcvttpd2dq(const Xmm& arg0, const M256& arg1);

  /** Convert two packed double-precision floating- point values in xmm2/mem to two signed doubleword integers in xmm1 using truncation. */
  void vcvttpd2dq(const Xmm& arg0, const Xmm& arg1);

  /** Convert four packed double-precision floating- point values in ymm2/mem to four signed doubleword integers in xmm1 using truncation. */
  void vcvttpd2dq(const Xmm& arg0, const Ymm& arg1);

  /** Convert four packed single precision floating- point values from xmm2/mem to four packed signed doubleword values in xmm1 using truncation. */
  void vcvttps2dq(const Xmm& arg0, const M128& arg1);

  /** Convert four packed single precision floating- point values from xmm2/mem to four packed signed doubleword values in xmm1 using truncation. */
  void vcvttps2dq(const Xmm& arg0, const Xmm& arg1);

  /** Convert eight packed single precision floating- point values from ymm2/mem to eight packed signed doubleword values in ymm1 using truncation. */
  void vcvttps2dq(const Ymm& arg0, const M256& arg1);

  /** Convert eight packed single precision floating- point values from ymm2/mem to eight packed signed doubleword values in ymm1 using truncation. */
  void vcvttps2dq(const Ymm& arg0, const Ymm& arg1);

  /** Convert one double-precision floating-point value from xmm1/m64 to one signed doubleword integer in r32 using truncation. */
  void vcvttsd2si(const R32& arg0, const M64& arg1);

  /** Convert one double-precision floating-point value from xmm1/m64 to one signed doubleword integer in r32 using truncation. */
  void vcvttsd2si(const R32& arg0, const Xmm& arg1);

  /** Convert one double precision floating-point value from xmm1/m64 to one signed quadword integer in r64 using truncation. */
  void vcvttsd2si(const R64& arg0, const M64& arg1);

  /** Convert one double precision floating-point value from xmm1/m64 to one signed quadword integer in r64 using truncation. */
  void vcvttsd2si(const R64& arg0, const Xmm& arg1);

  /** Convert one single-precision floating-point value from xmm1/m32 to one signed doubleword integer in r32 using truncation. */
  void vcvttss2si(const R32& arg0, const M32& arg1);

  /** Convert one single-precision floating-point value from xmm1/m32 to one signed doubleword integer in r32 using truncation. */
  void vcvttss2si(const R32& arg0, const Xmm& arg1);

  /** Convert one single-precision floating-point value from xmm1/m32 to one signed quadword integer in r64 using truncation. */
  void vcvttss2si(const R64& arg0, const M32& arg1);

  /** Convert one single-precision floating-point value from xmm1/m32 to one signed quadword integer in r64 using truncation. */
  void vcvttss2si(const R64& arg0, const Xmm& arg1);

  /** Divide packed double-precision floating-point values in xmm2 by packed double-precision floating-point values in xmm3/mem. */
  void vdivpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Divide packed double-precision floating-point values in xmm2 by packed double-precision floating-point values in xmm3/mem. */
  void vdivpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Divide packed double-precision floating-point values in ymm2 by packed double-precision floating-point values in ymm3/mem. */
  void vdivpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Divide packed double-precision floating-point values in ymm2 by packed double-precision floating-point values in ymm3/mem. */
  void vdivpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Divide packed single-precision floating-point values in xmm2 by packed double-precision floating-point values in xmm3/mem. */
  void vdivps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Divide packed single-precision floating-point values in xmm2 by packed double-precision floating-point values in xmm3/mem. */
  void vdivps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Divide packed single-precision floating-point values in ymm2 by packed double-precision floating-point values in ymm3/mem. */
  void vdivps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Divide packed single-precision floating-point values in ymm2 by packed double-precision floating-point values in ymm3/mem. */
  void vdivps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Divide low double-precision floating point values in xmm2 by low double precision floating-point value in xmm3/mem64. */
  void vdivsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Divide low double-precision floating point values in xmm2 by low double precision floating-point value in xmm3/mem64. */
  void vdivsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Divide low single-precision floating point value in xmm2 by low single precision floating-point value in xmm3/m32. */
  void vdivss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Divide low single-precision floating point value in xmm2 by low single precision floating-point value in xmm3/m32. */
  void vdivss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Selectively multiply packed DP floating-point values from xmm2 with packed DP floating- point values from xmm3, add and selectively store the packed DP floating-point values to xmm1. */
  void vdppd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Selectively multiply packed DP floating-point values from xmm2 with packed DP floating- point values from xmm3, add and selectively store the packed DP floating-point values to xmm1. */
  void vdppd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Multiply packed SP floating point values from xmm1 with packed SP floating point values from xmm2/mem selectively add and store to xmm1. */
  void vdpps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Multiply packed SP floating point values from xmm1 with packed SP floating point values from xmm2/mem selectively add and store to xmm1. */
  void vdpps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Multiply packed single-precision floating-point values from ymm2 with packed SP floating point values from ymm3/mem, selectively add pairs of elements and store to ymm1. */
  void vdpps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Multiply packed single-precision floating-point values from ymm2 with packed SP floating point values from ymm3/mem, selectively add pairs of elements and store to ymm1. */
  void vdpps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Set ZF=1 if segment specified with r/m16 can be read. */
  void verr(const M16& arg0);

  /** Set ZF=1 if segment specified with r/m16 can be read. */
  void verr(const R16& arg0);

  /** Set ZF=1 if segment specified with r/m16 can be written. */
  void verw(const M16& arg0);

  /** Set ZF=1 if segment specified with r/m16 can be written. */
  void verw(const R16& arg0);

  /** Extract 128 bits of packed floating-point values from ymm2 and store results in xmm1/mem. */
  void vextractf128(const M128& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Extract 128 bits of packed floating-point values from ymm2 and store results in xmm1/mem. */
  void vextractf128(const Xmm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Extract 128 bits of integer data from ymm2 and store results in xmm1/mem. */
  void vextracti128(const M128& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Extract 128 bits of integer data from ymm2 and store results in xmm1/mem. */
  void vextracti128(const Xmm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Extract one single-precision floating-point value from xmm1 at the offset specified by imm8 and store the result in reg or m32. Zero extend the results in 64-bit register if applicable. */
  void vextractps(const M32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract one single-precision floating-point value from xmm1 at the offset specified by imm8 and store the result in reg or m32. Zero extend the results in 64-bit register if applicable. */
  void vextractps(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, add to xmm1 and put result in xmm0. */
  void vfmadd132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, add to xmm1 and put result in xmm0. */
  void vfmadd132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, add to ymm1 and put result in ymm0. */
  void vfmadd132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, add to ymm1 and put result in ymm0. */
  void vfmadd132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, add to xmm1 and put result in xmm0 */
  void vfmadd132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, add to xmm1 and put result in xmm0 */
  void vfmadd132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, add to ymm1 and put result in ymm0. */
  void vfmadd132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, add to ymm1 and put result in ymm0. */
  void vfmadd132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm2/mem, add to xmm1 and put result in xmm0 */
  void vfmadd132sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm2/mem, add to xmm1 and put result in xmm0 */
  void vfmadd132sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm2/mem, add to xmm1 and put result in xmm0 */
  void vfmadd132ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm2/mem, add to xmm1 and put result in xmm0 */
  void vfmadd132ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, add to xmm2/mem and put result in xmm0. */
  void vfmadd213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, add to xmm2/mem and put result in xmm0. */
  void vfmadd213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, add to ymm2/mem and put result in ymm0. */
  void vfmadd213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, add to ymm2/mem and put result in ymm0. */
  void vfmadd213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, add to xmm2/mem and put result in xmm0. */
  void vfmadd213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, add to xmm2/mem and put result in xmm0. */
  void vfmadd213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, add to ymm2/mem and put result in ymm0. */
  void vfmadd213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, add to ymm2/mem and put result in ymm0. */
  void vfmadd213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm1, add to xmm2/mem and put result in xmm0. */
  void vfmadd213sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm1, add to xmm2/mem and put result in xmm0. */
  void vfmadd213sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm1, add to xmm2/mem and put result in xmm0. */
  void vfmadd213ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm1, add to xmm2/mem and put result in xmm0. */
  void vfmadd213ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, add to xmm0 and put result in xmm0. */
  void vfmadd231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, add to xmm0 and put result in xmm0. */
  void vfmadd231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, add to ymm0 and put result in ymm0. */
  void vfmadd231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, add to ymm0 and put result in ymm0. */
  void vfmadd231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, add to xmm0 and put result in xmm0. */
  void vfmadd231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, add to xmm0 and put result in xmm0. */
  void vfmadd231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, add to ymm0 and put result in ymm0. */
  void vfmadd231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, add to ymm0 and put result in ymm0. */
  void vfmadd231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm1 and xmm2/mem, add to xmm0 and put result in xmm0. */
  void vfmadd231sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm1 and xmm2/mem, add to xmm0 and put result in xmm0. */
  void vfmadd231sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm1 and xmm2/mem, add to xmm0 and put result in xmm0. */
  void vfmadd231ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm1 and xmm2/mem, add to xmm0 and put result in xmm0. */
  void vfmadd231ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, add/subtract elements in xmm1 and put result in xmm0. */
  void vfmaddsub132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, add/subtract elements in xmm1 and put result in xmm0. */
  void vfmaddsub132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, add/subtract elements in ymm1 and put result in ymm0. */
  void vfmaddsub132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, add/subtract elements in ymm1 and put result in ymm0. */
  void vfmaddsub132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, add/subtract elements in xmm1 and put result in xmm0. */
  void vfmaddsub132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, add/subtract elements in xmm1 and put result in xmm0. */
  void vfmaddsub132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, add/subtract elements in ymm1 and put result in ymm0. */
  void vfmaddsub132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, add/subtract elements in ymm1 and put result in ymm0. */
  void vfmaddsub132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, add/subtract elements in xmm2/mem and put result in xmm0. */
  void vfmaddsub213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, add/subtract elements in xmm2/mem and put result in xmm0. */
  void vfmaddsub213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, add/subtract elements in ymm2/mem and put result in ymm0. */
  void vfmaddsub213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, add/subtract elements in ymm2/mem and put result in ymm0. */
  void vfmaddsub213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, add/subtract elements in xmm2/mem and put result in xmm0. */
  void vfmaddsub213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, add/subtract elements in xmm2/mem and put result in xmm0. */
  void vfmaddsub213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, add/subtract elements in ymm2/mem and put result in ymm0. */
  void vfmaddsub213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, add/subtract elements in ymm2/mem and put result in ymm0. */
  void vfmaddsub213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, add/subtract elements in xmm0 and put result in xmm0. */
  void vfmaddsub231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, add/subtract elements in xmm0 and put result in xmm0. */
  void vfmaddsub231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, add/subtract elements in ymm0 and put result in ymm0. */
  void vfmaddsub231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, add/subtract elements in ymm0 and put result in ymm0. */
  void vfmaddsub231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, add/subtract elements in xmm0 and put result in xmm0. */
  void vfmaddsub231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, add/subtract elements in xmm0 and put result in xmm0. */
  void vfmaddsub231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, add/subtract elements in ymm0 and put result in ymm0. */
  void vfmaddsub231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, add/subtract elements in ymm0 and put result in ymm0. */
  void vfmaddsub231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, subtract xmm1 and put result in xmm0. */
  void vfmsub132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, subtract xmm1 and put result in xmm0. */
  void vfmsub132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, subtract ymm1 and put result in ymm0. */
  void vfmsub132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, subtract ymm1 and put result in ymm0. */
  void vfmsub132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, subtract xmm1 and put result in xmm0. */
  void vfmsub132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, subtract xmm1 and put result in xmm0. */
  void vfmsub132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, subtract ymm1 and put result in ymm0. */
  void vfmsub132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, subtract ymm1 and put result in ymm0. */
  void vfmsub132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm2/mem, subtract xmm1 and put result in xmm0. */
  void vfmsub132sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm2/mem, subtract xmm1 and put result in xmm0. */
  void vfmsub132sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm2/mem, subtract xmm1 and put result in xmm0. */
  void vfmsub132ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm2/mem, subtract xmm1 and put result in xmm0. */
  void vfmsub132ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, subtract xmm2/mem and put result in xmm0. */
  void vfmsub213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, subtract xmm2/mem and put result in xmm0. */
  void vfmsub213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, subtract ymm2/mem and put result in ymm0 */
  void vfmsub213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, subtract ymm2/mem and put result in ymm0 */
  void vfmsub213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, subtract xmm2/mem and put result in xmm0. */
  void vfmsub213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, subtract xmm2/mem and put result in xmm0. */
  void vfmsub213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, subtract ymm2/mem and put result in ymm0 */
  void vfmsub213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, subtract ymm2/mem and put result in ymm0 */
  void vfmsub213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm1, subtract xmm2/mem and put result in xmm0. */
  void vfmsub213sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm1, subtract xmm2/mem and put result in xmm0. */
  void vfmsub213sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm1, subtract xmm2/mem and put result in xmm0. */
  void vfmsub213ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm1, subtract xmm2/mem and put result in xmm0. */
  void vfmsub213ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, subtract xmm0 and put result in xmm0. */
  void vfmsub231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, subtract xmm0 and put result in xmm0. */
  void vfmsub231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, subtract ymm0 and put result in ymm0. */
  void vfmsub231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, subtract ymm0 and put result in ymm0. */
  void vfmsub231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, subtract xmm0 and put result in xmm0. */
  void vfmsub231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, subtract xmm0 and put result in xmm0. */
  void vfmsub231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, subtract ymm0 and put result in ymm0. */
  void vfmsub231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, subtract ymm0 and put result in ymm0. */
  void vfmsub231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm1 and xmm2/mem, subtract xmm0 and put result in xmm0. */
  void vfmsub231sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm1 and xmm2/mem, subtract xmm0 and put result in xmm0. */
  void vfmsub231sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm1 and xmm2/mem, subtract xmm0 and put result in xmm0. */
  void vfmsub231ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm1 and xmm2/mem, subtract xmm0 and put result in xmm0. */
  void vfmsub231ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, subtract/add elements in xmm1 and put result in xmm0. */
  void vfmsubadd132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, subtract/add elements in xmm1 and put result in xmm0. */
  void vfmsubadd132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, subtract/add elements in ymm1 and put result in ymm0. */
  void vfmsubadd132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, subtract/add elements in ymm1 and put result in ymm0. */
  void vfmsubadd132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, subtract/add elements in xmm1 and put result in xmm0. */
  void vfmsubadd132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, subtract/add elements in xmm1 and put result in xmm0. */
  void vfmsubadd132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, subtract/add elements in ymm1 and put result in ymm0. */
  void vfmsubadd132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, subtract/add elements in ymm1 and put result in ymm0. */
  void vfmsubadd132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, subtract/add elements in xmm2/mem and put result in xmm0. */
  void vfmsubadd213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, subtract/add elements in xmm2/mem and put result in xmm0. */
  void vfmsubadd213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, subtract/add elements in ymm2/mem and put result in ymm0. */
  void vfmsubadd213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, subtract/add elements in ymm2/mem and put result in ymm0. */
  void vfmsubadd213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, subtract/add elements in xmm2/mem and put result in xmm0. */
  void vfmsubadd213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, subtract/add elements in xmm2/mem and put result in xmm0. */
  void vfmsubadd213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, subtract/add elements in ymm2/mem and put result in ymm0. */
  void vfmsubadd213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, subtract/add elements in ymm2/mem and put result in ymm0. */
  void vfmsubadd213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, subtract/add elements in xmm0 and put result in xmm0. */
  void vfmsubadd231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, subtract/add elements in xmm0 and put result in xmm0. */
  void vfmsubadd231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, subtract/add elements in ymm0 and put result in ymm0. */
  void vfmsubadd231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, subtract/add elements in ymm0 and put result in ymm0. */
  void vfmsubadd231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, subtract/add elements in xmm0 and put result in xmm0. */
  void vfmsubadd231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, subtract/add elements in xmm0 and put result in xmm0. */
  void vfmsubadd231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, subtract/add elements in ymm0 and put result in ymm0. */
  void vfmsubadd231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, subtract/add elements in ymm0 and put result in ymm0. */
  void vfmsubadd231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, negate the multiplication result and add to xmm1 and put result in xmm0. */
  void vfnmadd132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, negate the multiplication result and add to xmm1 and put result in xmm0. */
  void vfnmadd132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, negate the multiplication result and add to ymm1 and put result in ymm0. */
  void vfnmadd132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, negate the multiplication result and add to ymm1 and put result in ymm0. */
  void vfnmadd132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, negate the multiplication result and add to xmm1 and put result in xmm0. */
  void vfnmadd132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, negate the multiplication result and add to xmm1 and put result in xmm0. */
  void vfnmadd132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, negate the multiplication result and add to ymm1 and put result in ymm0. */
  void vfnmadd132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, negate the multiplication result and add to ymm1 and put result in ymm0. */
  void vfnmadd132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm2/mem, negate the multiplication result and add to xmm1 and put result in xmm0 */
  void vfnmadd132sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm2/mem, negate the multiplication result and add to xmm1 and put result in xmm0 */
  void vfnmadd132sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm2/mem, negate the multiplication result and add to xmm1 and put result in xmm0 */
  void vfnmadd132ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm2/mem, negate the multiplication result and add to xmm1 and put result in xmm0 */
  void vfnmadd132ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, negate the multiplication result and add to xmm2/mem and put result in xmm0. */
  void vfnmadd213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, negate the multiplication result and add to xmm2/mem and put result in xmm0. */
  void vfnmadd213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, negate the multiplication result and add to ymm2/mem and put result in ymm0 */
  void vfnmadd213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, negate the multiplication result and add to ymm2/mem and put result in ymm0 */
  void vfnmadd213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, negate the multiplication result and add to xmm2/mem and put result in xmm0. */
  void vfnmadd213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, negate the multiplication result and add to xmm2/mem and put result in xmm0. */
  void vfnmadd213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, negate the multiplication result and add to ymm2/mem and put result in ymm0. */
  void vfnmadd213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, negate the multiplication result and add to ymm2/mem and put result in ymm0. */
  void vfnmadd213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm1, negate the multiplication result and add to xmm2/mem and put result in xmm0. */
  void vfnmadd213sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm1, negate the multiplication result and add to xmm2/mem and put result in xmm0. */
  void vfnmadd213sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm1, negate the multiplication result and add to xmm2/mem and put result in xmm0. */
  void vfnmadd213ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm1, negate the multiplication result and add to xmm2/mem and put result in xmm0. */
  void vfnmadd213ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, negate the multiplication result and add to xmm0 and put result in xmm0 */
  void vfnmadd231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, negate the multiplication result and add to xmm0 and put result in xmm0 */
  void vfnmadd231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, negate the multiplication result and add to ymm0 and put result in ymm0. */
  void vfnmadd231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, negate the multiplication result and add to ymm0 and put result in ymm0. */
  void vfnmadd231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, negate the multiplication result and add to xmm0 and put result in xmm0. */
  void vfnmadd231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, negate the multiplication result and add to xmm0 and put result in xmm0. */
  void vfnmadd231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, negate the multiplication result and add to ymm0 and put result in ymm0. */
  void vfnmadd231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, negate the multiplication result and add to ymm0 and put result in ymm0. */
  void vfnmadd231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm1 and xmm2/mem, negate the multiplication result and add to xmm0 and put result in xmm0. */
  void vfnmadd231sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm1 and xmm2/mem, negate the multiplication result and add to xmm0 and put result in xmm0. */
  void vfnmadd231sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm1 and xmm2/mem, negate the multiplication result and add to xmm0 and put result in xmm0. */
  void vfnmadd231ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm1 and xmm2/mem, negate the multiplication result and add to xmm0 and put result in xmm0. */
  void vfnmadd231ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, negate the multiplication result and subtract xmm1 and put result in xmm0. */
  void vfnmsub132pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm2/mem, negate the multiplication result and subtract xmm1 and put result in xmm0. */
  void vfnmsub132pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, negate the multiplication result and subtract ymm1 and put result in ymm0. */
  void vfnmsub132pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm2/mem, negate the multiplication result and subtract ymm1 and put result in ymm0. */
  void vfnmsub132pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, negate the multiplication result and subtract xmm1 and put result in xmm0. */
  void vfnmsub132ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm2/mem, negate the multiplication result and subtract xmm1 and put result in xmm0. */
  void vfnmsub132ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, negate the multiplication result and subtract ymm1 and put result in ymm0. */
  void vfnmsub132ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm2/mem, negate the multiplication result and subtract ymm1 and put result in ymm0. */
  void vfnmsub132ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm2/mem, negate the multiplication result and subtract xmm1 and put result in xmm0. */
  void vfnmsub132sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm2/mem, negate the multiplication result and subtract xmm1 and put result in xmm0. */
  void vfnmsub132sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm2/mem, negate the multiplication result and subtract xmm1 and put result in xmm0. */
  void vfnmsub132ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm2/mem, negate the multiplication result and subtract xmm1 and put result in xmm0. */
  void vfnmsub132ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, negate the multiplication result and subtract xmm2/mem and put result in xmm0 */
  void vfnmsub213pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm0 and xmm1, negate the multiplication result and subtract xmm2/mem and put result in xmm0 */
  void vfnmsub213pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, negate the multiplication result and subtract ymm2/mem and put result in ymm0. */
  void vfnmsub213pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm0 and ymm1, negate the multiplication result and subtract ymm2/mem and put result in ymm0. */
  void vfnmsub213pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, negate the multiplication result and subtract xmm2/mem and put result in xmm0. */
  void vfnmsub213ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm0 and xmm1, negate the multiplication result and subtract xmm2/mem and put result in xmm0. */
  void vfnmsub213ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, negate the multiplication result and subtract ymm2/mem and put result in ymm0. */
  void vfnmsub213ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm0 and ymm1, negate the multiplication result and subtract ymm2/mem and put result in ymm0. */
  void vfnmsub213ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm1, negate the multiplication result and subtract xmm2/mem and put result in xmm0. */
  void vfnmsub213sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm0 and xmm1, negate the multiplication result and subtract xmm2/mem and put result in xmm0. */
  void vfnmsub213sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm1, negate the multiplication result and subtract xmm2/mem and put result in xmm0. */
  void vfnmsub213ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm0 and xmm1, negate the multiplication result and subtract xmm2/mem and put result in xmm0. */
  void vfnmsub213ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, negate the multiplication result and subtract xmm0 and put result in xmm0. */
  void vfnmsub231pd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm1 and xmm2/mem, negate the multiplication result and subtract xmm0 and put result in xmm0. */
  void vfnmsub231pd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, negate the multiplication result and subtract ymm0 and put result in ymm0. */
  void vfnmsub231pd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm1 and ymm2/mem, negate the multiplication result and subtract ymm0 and put result in ymm0. */
  void vfnmsub231pd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, negate the multiplication result and subtract xmm0 and put result in xmm0. */
  void vfnmsub231ps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm1 and xmm2/mem, negate the multiplication result and subtract xmm0 and put result in xmm0. */
  void vfnmsub231ps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, negate the multiplication result and subtract ymm0 and put result in ymm0. */
  void vfnmsub231ps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm1 and ymm2/mem, negate the multiplication result and subtract ymm0 and put result in ymm0. */
  void vfnmsub231ps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply scalar double-precision floating-point value from xmm1 and xmm2/mem, negate the multiplication result and subtract xmm0 and put result in xmm0. */
  void vfnmsub231sd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply scalar double-precision floating-point value from xmm1 and xmm2/mem, negate the multiplication result and subtract xmm0 and put result in xmm0. */
  void vfnmsub231sd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply scalar single-precision floating-point value from xmm1 and xmm2/mem, negate the multiplication result and subtract xmm0 and put result in xmm0. */
  void vfnmsub231ss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply scalar single-precision floating-point value from xmm1 and xmm2/mem, negate the multiplication result and subtract xmm0 and put result in xmm0. */
  void vfnmsub231ss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Using dword indices specified in vm32x, gather double-precision FP values from memory conditioned on mask specified by xmm2. Conditionally gathered elements are merged into xmm1. */
  void vgatherdpd(const Xmm& arg0, const M32& arg1, const Xmm& arg2);

  /** Using dword indices specified in vm32x, gather double-precision FP values from memory conditioned on mask specified by ymm2. Conditionally gathered elements are merged into ymm1. */
  void vgatherdpd(const Ymm& arg0, const M32& arg1, const Ymm& arg2);

  /** Using dword indices specified in vm32x, gather single-precision FP values from memory conditioned on mask specified by xmm2. Conditionally gathered elements are merged into xmm1. */
  void vgatherdps(const Xmm& arg0, const M32& arg1, const Xmm& arg2);

  /** Using dword indices specified in vm32x, gather single-precision FP values from memory conditioned on mask specified by ymm2. Conditionally gathered elements are merged into ymm1. */
  void vgatherdps(const Ymm& arg0, const M32& arg1, const Ymm& arg2);

  /** Using qword indices specified in vm64x, gather double-precision FP values from memory conditioned on mask specified by xmm2. Conditionally gathered elements are merged into xmm1. */
  void vgatherqpd(const Xmm& arg0, const M64& arg1, const Xmm& arg2);

  /** Using qword indices specified in vm64y, gather double-precision FP values from memory conditioned on mask specified by ymm2. Conditionally gathered elements are merged into ymm1. */
  void vgatherqpd(const Ymm& arg0, const M64& arg1, const Ymm& arg2);

  /** Using qword indices specified in vm64x, gather single-precision FP values from memory conditioned on mask specified by xmm2. Conditionally gathered elements are merged into xmm1. */
  void vgatherqps(const Xmm& arg0, const M64& arg1, const Xmm& arg2);

  /** Using qword indices specified in vm64y, gather single-precision FP values from memory conditioned on mask specified by ymm2. Conditionally gathered elements are merged into ymm1. */
  void vgatherqps_1(const Xmm& arg0, const M64& arg1, const Xmm& arg2);

  /** Horizontal add packed double-precision floating-point values from xmm2 and xmm3/mem. */
  void vhaddpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Horizontal add packed double-precision floating-point values from xmm2 and xmm3/mem. */
  void vhaddpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Horizontal add packed double-precision floating-point values from ymm2 and ymm3/mem. */
  void vhaddpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Horizontal add packed double-precision floating-point values from ymm2 and ymm3/mem. */
  void vhaddpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Horizontal add packed single-precision floating-point values from xmm2 and xmm3/mem. */
  void vhaddps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Horizontal add packed single-precision floating-point values from xmm2 and xmm3/mem. */
  void vhaddps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Horizontal add packed single-precision floating-point values from ymm2 and ymm3/mem. */
  void vhaddps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Horizontal add packed single-precision floating-point values from ymm2 and ymm3/mem. */
  void vhaddps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Horizontal subtract packed double-precision floating-point values from xmm2 and xmm3/mem. */
  void vhsubpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Horizontal subtract packed double-precision floating-point values from xmm2 and xmm3/mem. */
  void vhsubpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Horizontal subtract packed double-precision floating-point values from ymm2 and ymm3/mem. */
  void vhsubpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Horizontal subtract packed double-precision floating-point values from ymm2 and ymm3/mem. */
  void vhsubpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Horizontal subtract packed single-precision floating-point values from xmm2 and xmm3/mem. */
  void vhsubps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Horizontal subtract packed single-precision floating-point values from xmm2 and xmm3/mem. */
  void vhsubps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Horizontal subtract packed single-precision floating-point values from ymm2 and ymm3/mem. */
  void vhsubps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Horizontal subtract packed single-precision floating-point values from ymm2 and ymm3/mem. */
  void vhsubps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Insert a single precision floating-point value selected by imm8 from xmm3/m128 into ymm2 at the specified destination element specified by imm8 and zero out destination elements in ymm1 as indicated in imm8. */
  void vinsertf128(const Ymm& arg0, const Ymm& arg1, const M128& arg2, const Imm8& arg3);

  /** Insert a single precision floating-point value selected by imm8 from xmm3/m128 into ymm2 at the specified destination element specified by imm8 and zero out destination elements in ymm1 as indicated in imm8. */
  void vinsertf128(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Insert 128-bits of integer data from xmm3/mem and the remaining values from ymm2 into ymm1. */
  void vinserti128(const Ymm& arg0, const Ymm& arg1, const M128& arg2, const Imm8& arg3);

  /** Insert 128-bits of integer data from xmm3/mem and the remaining values from ymm2 into ymm1. */
  void vinserti128(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Insert a single precision floating point value selected by imm8 from xmm3/m32 and merge into xmm2 at the specified destination element specified by imm8 and zero out destination elements in xmm1 as indicated in imm8. */
  void vinsertps(const Xmm& arg0, const Xmm& arg1, const M32& arg2, const Imm8& arg3);

  /** Insert a single precision floating point value selected by imm8 from xmm3/m32 and merge into xmm2 at the specified destination element specified by imm8 and zero out destination elements in xmm1 as indicated in imm8. */
  void vinsertps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Load unaligned packed integer values from mem to xmm1. */
  void vlddqu(const Xmm& arg0, const M128& arg1);

  /** Load unaligned packed integer values from mem to ymm1. */
  void vlddqu(const Ymm& arg0, const M256& arg1);

  /** Load MXCSR register from m32. */
  void vldmxcsr(const M32& arg0);

  /** Selectively write bytes from xmm1 to memory location using the byte mask in xmm2. The default memory location is specified by DS:DI/EDI/RDI. */
  void vmaskmovdqu(const Xmm& arg0, const Xmm& arg1);

  /** Conditionally store packed double-precision values from xmm2 using mask in xmm1. */
  void vmaskmovpd(const M128& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Conditionally store packed double-precision values from ymm2 using mask in ymm1. */
  void vmaskmovpd(const M256& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Conditionally load packed double-precision values from m128 using mask in xmm2 and store in xmm1. */
  void vmaskmovpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Conditionally load packed double-precision values from m256 using mask in ymm2 and store in ymm1. */
  void vmaskmovpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Conditionally store packed single-precision values from xmm2 using mask in xmm1. */
  void vmaskmovps(const M128& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Conditionally store packed single-precision values from ymm2 using mask in ymm1. */
  void vmaskmovps(const M256& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Conditionally load packed single-precision values from m128 using mask in xmm2 and store in xmm1. */
  void vmaskmovps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Conditionally load packed single-precision values from m256 using mask in ymm2 and store in ymm1. */
  void vmaskmovps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the maximum double-precision floating-point values between xmm2 and xmm3/mem. */
  void vmaxpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the maximum double-precision floating-point values between xmm2 and xmm3/mem. */
  void vmaxpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the maximum packed double-precision floating-point values between ymm2 and ymm3/mem. */
  void vmaxpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the maximum packed double-precision floating-point values between ymm2 and ymm3/mem. */
  void vmaxpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the maximum single-precision floating-point values between xmm2 and xmm3/mem. */
  void vmaxps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the maximum single-precision floating-point values between xmm2 and xmm3/mem. */
  void vmaxps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the maximum single double-precision floating-point values between ymm2 and ymm3/mem. */
  void vmaxps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the maximum single double-precision floating-point values between ymm2 and ymm3/mem. */
  void vmaxps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the maximum scalar double-precision floating-point value between xmm3/mem64 and xmm2. */
  void vmaxsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Return the maximum scalar double-precision floating-point value between xmm3/mem64 and xmm2. */
  void vmaxsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the maximum scalar single-precision floating-point value between xmm3/mem32 and xmm2. */
  void vmaxss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Return the maximum scalar single-precision floating-point value between xmm3/mem32 and xmm2. */
  void vmaxss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the minimum double-precision floating-point values between xmm2 and xmm3/mem. */
  void vminpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the minimum double-precision floating-point values between xmm2 and xmm3/mem. */
  void vminpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the minimum packed double-precision floating-point values between ymm2 and ymm3/mem. */
  void vminpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the minimum packed double-precision floating-point values between ymm2 and ymm3/mem. */
  void vminpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the minimum single-precision floating-point values between xmm2 and xmm3/mem. */
  void vminps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the minimum single-precision floating-point values between xmm2 and xmm3/mem. */
  void vminps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the minimum single double-precision floating-point values between ymm2 and ymm3/mem. */
  void vminps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the minimum single double-precision floating-point values between ymm2 and ymm3/mem. */
  void vminps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the minimum scalar double precision floating-point value between xmm3/mem64 and xmm2. */
  void vminsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Return the minimum scalar double precision floating-point value between xmm3/mem64 and xmm2. */
  void vminsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the minimum scalar single precision floating-point value between xmm3/mem32 and xmm2. */
  void vminss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Return the minimum scalar single precision floating-point value between xmm3/mem32 and xmm2. */
  void vminss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Move aligned packed double-precision floating-point values from xmm1 to xmm2/mem. */
  void vmovapd(const M128& arg0, const Xmm& arg1);

  /** Move aligned packed double-precision floating-point values from ymm1 to ymm2/mem. */
  void vmovapd(const M256& arg0, const Ymm& arg1);

  /** Move aligned packed double-precision floating-point values from xmm2/mem to xmm1. */
  void vmovapd(const Xmm& arg0, const M128& arg1);

  /** Move aligned packed double-precision floating-point values from xmm2/mem to xmm1. */
  void vmovapd(const Xmm& arg0, const Xmm& arg1);

  /** Move aligned packed double-precision floating-point values from xmm1 to xmm2/mem. */
  void vmovapd_1(const Xmm& arg0, const Xmm& arg1);

  /** Move aligned packed double-precision floating-point values from ymm2/mem to ymm1. */
  void vmovapd(const Ymm& arg0, const M256& arg1);

  /** Move aligned packed double-precision floating-point values from ymm2/mem to ymm1. */
  void vmovapd(const Ymm& arg0, const Ymm& arg1);

  /** Move aligned packed double-precision floating-point values from ymm1 to ymm2/mem. */
  void vmovapd_1(const Ymm& arg0, const Ymm& arg1);

  /** Move aligned packed single-precision floating-point values from xmm1 to xmm2/mem. */
  void vmovaps(const M128& arg0, const Xmm& arg1);

  /** Move aligned packed single-precision floating-point values from ymm1 to ymm2/mem. */
  void vmovaps(const M256& arg0, const Ymm& arg1);

  /** Move aligned packed single-precision floating-point values from xmm2/mem to xmm1. */
  void vmovaps(const Xmm& arg0, const M128& arg1);

  /** Move aligned packed single-precision floating-point values from xmm2/mem to xmm1. */
  void vmovaps(const Xmm& arg0, const Xmm& arg1);

  /** Move aligned packed single-precision floating-point values from xmm1 to xmm2/mem. */
  void vmovaps_1(const Xmm& arg0, const Xmm& arg1);

  /** Move aligned packed single-precision floating-point values from ymm2/mem to ymm1. */
  void vmovaps(const Ymm& arg0, const M256& arg1);

  /** Move aligned packed single-precision floating-point values from ymm2/mem to ymm1. */
  void vmovaps(const Ymm& arg0, const Ymm& arg1);

  /** Move aligned packed single-precision floating-point values from ymm1 to ymm2/mem. */
  void vmovaps_1(const Ymm& arg0, const Ymm& arg1);

  /** Move doubleword from xmm1 register to r/m32. */
  void vmovd(const M32& arg0, const Xmm& arg1);

  /** Move doubleword from xmm1 register to r/m32. */
  void vmovd(const R32& arg0, const Xmm& arg1);

  /** Move doubleword from r/m32 to xmm1. */
  void vmovd(const Xmm& arg0, const M32& arg1);

  /** Move doubleword from r/m32 to xmm1. */
  void vmovd(const Xmm& arg0, const R32& arg1);

  /** Move double-precision floating-point values from xmm2/mem and duplicate into xmm1. */
  void vmovddup(const Xmm& arg0, const M64& arg1);

  /** Move double-precision floating-point values from xmm2/mem and duplicate into xmm1. */
  void vmovddup(const Xmm& arg0, const Xmm& arg1);

  /** Move even index double-precision floating-point values from ymm2/mem and duplicate each element into ymm1. */
  void vmovddup(const Ymm& arg0, const M256& arg1);

  /** Move even index double-precision floating-point values from ymm2/mem and duplicate each element into ymm1. */
  void vmovddup(const Ymm& arg0, const Ymm& arg1);

  /** Move aligned packed integer values from xmm1 to xmm2/mem. */
  void vmovdqa(const M128& arg0, const Xmm& arg1);

  /** Move aligned packed integer values from ymm1 to ymm2/mem. */
  void vmovdqa(const M256& arg0, const Ymm& arg1);

  /** Move aligned packed integer values from xmm2/mem to xmm1. */
  void vmovdqa(const Xmm& arg0, const M128& arg1);

  /** Move aligned packed integer values from xmm2/mem to xmm1. */
  void vmovdqa(const Xmm& arg0, const Xmm& arg1);

  /** Move aligned packed integer values from xmm1 to xmm2/mem. */
  void vmovdqa_1(const Xmm& arg0, const Xmm& arg1);

  /** Move aligned packed integer values from ymm2/mem to ymm1. */
  void vmovdqa(const Ymm& arg0, const M256& arg1);

  /** Move aligned packed integer values from ymm2/mem to ymm1. */
  void vmovdqa(const Ymm& arg0, const Ymm& arg1);

  /** Move aligned packed integer values from ymm1 to ymm2/mem. */
  void vmovdqa_1(const Ymm& arg0, const Ymm& arg1);

  /** Move unaligned packed integer values from xmm1 to xmm2/mem. */
  void vmovdqu(const M128& arg0, const Xmm& arg1);

  /** Move unaligned packed integer values from ymm1 to ymm2/mem. */
  void vmovdqu(const M256& arg0, const Ymm& arg1);

  /** Move unaligned packed integer values from xmm2/mem to xmm1. */
  void vmovdqu(const Xmm& arg0, const M128& arg1);

  /** Move unaligned packed integer values from xmm2/mem to xmm1. */
  void vmovdqu(const Xmm& arg0, const Xmm& arg1);

  /** Move unaligned packed integer values from xmm1 to xmm2/mem. */
  void vmovdqu_1(const Xmm& arg0, const Xmm& arg1);

  /** Move unaligned packed integer values from ymm2/mem to ymm1. */
  void vmovdqu(const Ymm& arg0, const M256& arg1);

  /** Move unaligned packed integer values from ymm2/mem to ymm1. */
  void vmovdqu(const Ymm& arg0, const Ymm& arg1);

  /** Move unaligned packed integer values from ymm1 to ymm2/mem. */
  void vmovdqu_1(const Ymm& arg0, const Ymm& arg1);

  /** Merge two packed single-precision floating-point values from high quadword of xmm3 and low quadword of xmm2. */
  void vmovhlps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Move double-precision floating-point values from high quadword of xmm1 to m64. */
  void vmovhpd(const M64& arg0, const Xmm& arg1);

  /** Merge double-precision floating-point value from m64 and the low quadword of xmm1. */
  void vmovhpd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Move two packed single-precision floating-point values from high quadword of xmm1to m64. */
  void vmovhps(const M64& arg0, const Xmm& arg1);

  /** Merge two packed single-precision floating-point values from m64 and the low quadword of xmm1. */
  void vmovhps(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Merge two packed single-precision floating-point values from low quadword of xmm3 and low quadword of xmm2. */
  void vmovlhps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Move double-precision floating-point values from low quadword of xmm1 to m64. */
  void vmovlpd(const M64& arg0, const Xmm& arg1);

  /** Merge double-precision floating-point value from m64 and the high quadword of xmm1. */
  void vmovlpd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Move two packed single-precision floating-point values from low quadword of xmm1 to m64. */
  void vmovlps(const M64& arg0, const Xmm& arg1);

  /** Merge two packed single-precision floating-point values from m64 and the high quadword of xmm1. */
  void vmovlps(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Extract 2-bit sign mask from xmm2 and store in reg. The upper bits of r32 or r64 are zeroed. */
  void vmovmskpd(const R32& arg0, const Xmm& arg1);

  /** Extract 4-bit sign mask from ymm2 and store in reg. The upper bits of r32 or r64 are zeroed. */
  void vmovmskpd(const R32& arg0, const Ymm& arg1);

  /** Extract 2-bit sign mask from xmm2 and store in reg. The upper bits of r32 or r64 are zeroed. */
  void vmovmskpd(const R64& arg0, const Xmm& arg1);

  /** Extract 4-bit sign mask from ymm2 and store in reg. The upper bits of r32 or r64 are zeroed. */
  void vmovmskpd(const R64& arg0, const Ymm& arg1);

  /** Extract 4-bit sign mask from xmm2 and store in reg. The upper bits of r32 or r64 are zeroed. */
  void vmovmskps(const R32& arg0, const Xmm& arg1);

  /** Extract 8-bit sign mask from ymm2 and store in reg. The upper bits of r32 or r64 are zeroed. */
  void vmovmskps(const R32& arg0, const Ymm& arg1);

  /** Extract 4-bit sign mask from xmm2 and store in reg. The upper bits of r32 or r64 are zeroed. */
  void vmovmskps(const R64& arg0, const Xmm& arg1);

  /** Extract 8-bit sign mask from ymm2 and store in reg. The upper bits of r32 or r64 are zeroed. */
  void vmovmskps(const R64& arg0, const Ymm& arg1);

  /** Move packed integer values in xmm1 to m128 using non-temporal hint. */
  void vmovntdq(const M128& arg0, const Xmm& arg1);

  /** Move packed integer values in ymm1 to m256 using non-temporal hint. */
  void vmovntdq(const M256& arg0, const Ymm& arg1);

  /** Move double quadword from m128 to xmm using non-temporal hint if WC memory type. */
  void vmovntdqa(const Xmm& arg0, const M128& arg1);

  /** Move 256-bit data from m256 to ymm using non-temporal hint if WC memory type. */
  void vmovntdqa(const Ymm& arg0, const M256& arg1);

  /** Move packed double-precision values in xmm1 to m128 using non-temporal hint. */
  void vmovntpd(const M128& arg0, const Xmm& arg1);

  /** Move packed double-precision values in ymm1 to m256 using non-temporal hint. */
  void vmovntpd(const M256& arg0, const Ymm& arg1);

  /** Move packed single-precision values xmm1 to mem using non-temporal hint. */
  void vmovntps(const M128& arg0, const Xmm& arg1);

  /** Move packed single-precision values ymm1 to mem using non-temporal hint. */
  void vmovntps(const M256& arg0, const Ymm& arg1);

  /** Move quadword from xmm1 register to r/m64. */
  void vmovq(const M64& arg0, const Xmm& arg1);

  /** Move quadword from xmm2 register to xmm1/m64. */
  void vmovq_1(const M64& arg0, const Xmm& arg1);

  /** Move quadword from xmm1 register to r/m64. */
  void vmovq(const R64& arg0, const Xmm& arg1);

  /** Move quadword from r/m64 to xmm1. */
  void vmovq(const Xmm& arg0, const M64& arg1);

  /** Load quadword from m64 to xmm1. */
  void vmovq_1(const Xmm& arg0, const M64& arg1);

  /** Move quadword from r/m64 to xmm1. */
  void vmovq(const Xmm& arg0, const R64& arg1);

  /** Move quadword from xmm2 to xmm1. */
  void vmovq(const Xmm& arg0, const Xmm& arg1);

  /** Move quadword from xmm2 register to xmm1/m64. */
  void vmovq_1(const Xmm& arg0, const Xmm& arg1);

  /** Move scalar double-precision floating-point value from xmm1 register to m64. */
  void vmovsd(const M64& arg0, const Xmm& arg1);

  /** Load scalar double-precision floating-point value from m64 to xmm1 register. */
  void vmovsd(const Xmm& arg0, const M64& arg1);

  /** Merge scalar double-precision floating-point value from xmm2 and xmm3 to xmm1 register. */
  void vmovsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Merge scalar double-precision floating-point value from xmm2 and xmm3 registers to xmm1. */
  void vmovsd_1(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Move odd index single-precision floating-point values from xmm2/mem and duplicate each element into xmm1. */
  void vmovshdup(const Xmm& arg0, const M128& arg1);

  /** Move odd index single-precision floating-point values from xmm2/mem and duplicate each element into xmm1. */
  void vmovshdup(const Xmm& arg0, const Xmm& arg1);

  /** Move odd index single-precision floating-point values from ymm2/mem and duplicate each element into ymm1. */
  void vmovshdup(const Ymm& arg0, const M256& arg1);

  /** Move odd index single-precision floating-point values from ymm2/mem and duplicate each element into ymm1. */
  void vmovshdup(const Ymm& arg0, const Ymm& arg1);

  /** Move even index single-precision floating-point values from xmm2/mem and duplicate each element into xmm1. */
  void vmovsldup(const Xmm& arg0, const M128& arg1);

  /** Move even index single-precision floating-point values from xmm2/mem and duplicate each element into xmm1. */
  void vmovsldup(const Xmm& arg0, const Xmm& arg1);

  /** Move even index single-precision floating-point values from ymm2/mem and duplicate each element into ymm1. */
  void vmovsldup(const Ymm& arg0, const M256& arg1);

  /** Move even index single-precision floating-point values from ymm2/mem and duplicate each element into ymm1. */
  void vmovsldup(const Ymm& arg0, const Ymm& arg1);

  /** Move scalar single-precision floating-point value from xmm1 register to m32. */
  void vmovss(const M32& arg0, const Xmm& arg1);

  /** Load scalar single-precision floating-point value from m32 to xmm1 register. */
  void vmovss(const Xmm& arg0, const M32& arg1);

  /** Merge scalar single-precision floating-point value from xmm2 and xmm3 to xmm1 register. */
  void vmovss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Move scalar single-precision floating-point value from xmm2 and xmm3 to xmm1 register. */
  void vmovss_1(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Move unaligned packed double-precision floating-point from xmm1 to xmm2/mem. */
  void vmovupd(const M128& arg0, const Xmm& arg1);

  /** Move unaligned packed double-precision floating-point from ymm1 to ymm2/mem. */
  void vmovupd(const M256& arg0, const Ymm& arg1);

  /** Move unaligned packed double-precision floating-point from xmm2/mem to xmm1. */
  void vmovupd(const Xmm& arg0, const M128& arg1);

  /** Move unaligned packed double-precision floating-point from xmm2/mem to xmm1. */
  void vmovupd(const Xmm& arg0, const Xmm& arg1);

  /** Move unaligned packed double-precision floating-point from xmm1 to xmm2/mem. */
  void vmovupd_1(const Xmm& arg0, const Xmm& arg1);

  /** Move unaligned packed double-precision floating-point from ymm2/mem to ymm1. */
  void vmovupd(const Ymm& arg0, const M256& arg1);

  /** Move unaligned packed double-precision floating-point from ymm2/mem to ymm1. */
  void vmovupd(const Ymm& arg0, const Ymm& arg1);

  /** Move unaligned packed double-precision floating-point from ymm1 to ymm2/mem. */
  void vmovupd_1(const Ymm& arg0, const Ymm& arg1);

  /** Move unaligned packed single-precision floating-point from xmm1 to xmm2/mem. */
  void vmovups(const M128& arg0, const Xmm& arg1);

  /** Move unaligned packed single-precision floating-point from ymm1 to ymm2/mem. */
  void vmovups(const M256& arg0, const Ymm& arg1);

  /** Move unaligned packed single-precision floating-point from xmm2/mem to xmm1. */
  void vmovups(const Xmm& arg0, const M128& arg1);

  /** Move unaligned packed single-precision floating-point from xmm2/mem to xmm1. */
  void vmovups(const Xmm& arg0, const Xmm& arg1);

  /** Move unaligned packed single-precision floating-point from xmm1 to xmm2/mem. */
  void vmovups_1(const Xmm& arg0, const Xmm& arg1);

  /** Move unaligned packed single-precision floating-point from ymm2/mem to ymm1. */
  void vmovups(const Ymm& arg0, const M256& arg1);

  /** Move unaligned packed single-precision floating-point from ymm2/mem to ymm1. */
  void vmovups(const Ymm& arg0, const Ymm& arg1);

  /** Move unaligned packed single-precision floating-point from ymm1 to ymm2/mem. */
  void vmovups_1(const Ymm& arg0, const Ymm& arg1);

  /** Sums absolute 8-bit integer difference of adjacent groups of 4 byte integers in xmm2 and xmm3/m128 and writes the results in xmm1. Starting offsets within xmm2 and xmm3/m128 are determined by imm8. */
  void vmpsadbw(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Sums absolute 8-bit integer difference of adjacent groups of 4 byte integers in xmm2 and xmm3/m128 and writes the results in xmm1. Starting offsets within xmm2 and xmm3/m128 are determined by imm8. */
  void vmpsadbw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Sums absolute 8-bit integer difference of adjacent groups of 4 byte integers in xmm2 and ymm3/m128 and writes the results in ymm1. Starting offsets within ymm2 and xmm3/m128 are determined by imm8. */
  void vmpsadbw(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Sums absolute 8-bit integer difference of adjacent groups of 4 byte integers in xmm2 and ymm3/m128 and writes the results in ymm1. Starting offsets within ymm2 and xmm3/m128 are determined by imm8. */
  void vmpsadbw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Multiply packed double-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vmulpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed double-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vmulpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed double-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vmulpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed double-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vmulpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed single-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vmulps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed single-precision floating-point values from xmm3/mem to xmm2 and stores result in xmm1. */
  void vmulps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed single-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vmulps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed single-precision floating-point values from ymm3/mem to ymm2 and stores result in ymm1. */
  void vmulps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply the low double-precision floating-point value in xmm3/mem64 by low double precision floating-point value in xmm2. */
  void vmulsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Multiply the low double-precision floating-point value in xmm3/mem64 by low double precision floating-point value in xmm2. */
  void vmulsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply the low single-precision floating-point value in xmm3/mem by the low single-precision floating-point value in xmm2. */
  void vmulss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Multiply the low single-precision floating-point value in xmm3/mem by the low single-precision floating-point value in xmm2. */
  void vmulss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the bitwise logical OR of packed double-precision floating-point values in xmm2 and xmm3/mem. */
  void vorpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the bitwise logical OR of packed double-precision floating-point values in xmm2 and xmm3/mem. */
  void vorpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the bitwise logical OR of packed double-precision floating-point values in ymm2 and ymm3/mem. */
  void vorpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the bitwise logical OR of packed double-precision floating-point values in ymm2 and ymm3/mem. */
  void vorpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the bitwise logical OR of packed single-precision floating-point values in xmm2 and xmm3/mem. */
  void vorps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the bitwise logical OR of packed single-precision floating-point values in xmm2 and xmm3/mem. */
  void vorps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the bitwise logical OR of packed single-precision floating-point values in ymm2 and ymm3/mem. */
  void vorps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the bitwise logical OR of packed single-precision floating-point values in ymm2 and ymm3/mem. */
  void vorps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compute the absolute value of bytes in xmm2/m128 and store UNSIGNED result in xmm1. */
  void vpabsb(const Xmm& arg0, const M128& arg1);

  /** Compute the absolute value of bytes in xmm2/m128 and store UNSIGNED result in xmm1. */
  void vpabsb(const Xmm& arg0, const Xmm& arg1);

  /** Compute the absolute value of bytes in ymm2/m256 and store UNSIGNED result in ymm1. */
  void vpabsb(const Ymm& arg0, const M256& arg1);

  /** Compute the absolute value of bytes in ymm2/m256 and store UNSIGNED result in ymm1. */
  void vpabsb(const Ymm& arg0, const Ymm& arg1);

  /** Compute the absolute value of 32- bit integers in xmm2/m128 and store UNSIGNED result in xmm1. */
  void vpabsd(const Xmm& arg0, const M128& arg1);

  /** Compute the absolute value of 32- bit integers in xmm2/m128 and store UNSIGNED result in xmm1. */
  void vpabsd(const Xmm& arg0, const Xmm& arg1);

  /** Compute the absolute value of 32-bit integers in ymm2/m256 and store UNSIGNED result in ymm1. */
  void vpabsd(const Ymm& arg0, const M256& arg1);

  /** Compute the absolute value of 32-bit integers in ymm2/m256 and store UNSIGNED result in ymm1. */
  void vpabsd(const Ymm& arg0, const Ymm& arg1);

  /** Compute the absolute value of 16- bit integers in xmm2/m128 and store UNSIGNED result in xmm1. */
  void vpabsw(const Xmm& arg0, const M128& arg1);

  /** Compute the absolute value of 16- bit integers in xmm2/m128 and store UNSIGNED result in xmm1. */
  void vpabsw(const Xmm& arg0, const Xmm& arg1);

  /** Compute the absolute value of 16-bit integers in ymm2/m256 and store UNSIGNED result in ymm1 */
  void vpabsw(const Ymm& arg0, const M256& arg1);

  /** Compute the absolute value of 16-bit integers in ymm2/m256 and store UNSIGNED result in ymm1 */
  void vpabsw(const Ymm& arg0, const Ymm& arg1);

  /** Converts 4 packed signed doubleword integers from xmm2 and from xmm3/m128 into 8 packed signed word integers in xmm1 using signed saturation. */
  void vpackssdw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Converts 4 packed signed doubleword integers from xmm2 and from xmm3/m128 into 8 packed signed word integers in xmm1 using signed saturation. */
  void vpackssdw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Converts 8 packed signed doubleword integers from ymm2 and from ymm3/m256 into 16 packed signed word integers in ymm1using signed saturation. */
  void vpackssdw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Converts 8 packed signed doubleword integers from ymm2 and from ymm3/m256 into 16 packed signed word integers in ymm1using signed saturation. */
  void vpackssdw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Converts 8 packed signed word integers from xmm2 and from xmm3/m128 into 16 packed signed byte integers in xmm1 using signed saturation. */
  void vpacksswb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Converts 8 packed signed word integers from xmm2 and from xmm3/m128 into 16 packed signed byte integers in xmm1 using signed saturation. */
  void vpacksswb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Converts 16 packed signed word integers from ymm2 and from ymm3/m256 into 32 packed signed byte integers in ymm1 using signed saturation. */
  void vpacksswb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Converts 16 packed signed word integers from ymm2 and from ymm3/m256 into 32 packed signed byte integers in ymm1 using signed saturation. */
  void vpacksswb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Convert 4 packed signed doubleword integers from xmm2 and 4 packed signed doubleword integers from xmm3/m128 into 8 packed unsigned word integers in xmm1 using unsigned saturation. */
  void vpackusdw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Convert 4 packed signed doubleword integers from xmm2 and 4 packed signed doubleword integers from xmm3/m128 into 8 packed unsigned word integers in xmm1 using unsigned saturation. */
  void vpackusdw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Convert 8 packed signed doubleword integers from ymm2 and 8 packed signed doubleword integers from ymm3/m128 into 16 packed unsigned word integers in ymm1 using unsigned saturation. */
  void vpackusdw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Convert 8 packed signed doubleword integers from ymm2 and 8 packed signed doubleword integers from ymm3/m128 into 16 packed unsigned word integers in ymm1 using unsigned saturation. */
  void vpackusdw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Converts 8 signed word integers from xmm2 and 8 signed word integers from xmm3/m128 into 16 unsigned byte integers in xmm1 using unsigned saturation. */
  void vpackuswb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Converts 8 signed word integers from xmm2 and 8 signed word integers from xmm3/m128 into 16 unsigned byte integers in xmm1 using unsigned saturation. */
  void vpackuswb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Converts 16 signed word integers from ymm2 And 16 signed word integers from ymm3/m256 into 32 unsigned byte integers in ymm1 using unsigned saturation. */
  void vpackuswb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Converts 16 signed word integers from ymm2 And 16 signed word integers from ymm3/m256 into 32 unsigned byte integers in ymm1 using unsigned saturation. */
  void vpackuswb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add packed byte integers from xmm3/m128 and xmm2. */
  void vpaddb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed byte integers from xmm3/m128 and xmm2. */
  void vpaddb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed byte integers from ymm2, and ymm3/m256 and store in ymm1. */
  void vpaddb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed byte integers from ymm2, and ymm3/m256 and store in ymm1. */
  void vpaddb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add packed doubleword integers from xmm3/m128 and xmm2. */
  void vpaddd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed doubleword integers from xmm3/m128 and xmm2. */
  void vpaddd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed doubleword integers from ymm2, ymm3/m256 and store in ymm1. */
  void vpaddd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed doubleword integers from ymm2, ymm3/m256 and store in ymm1. */
  void vpaddd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add packed quadword integers xmm3/m128 and xmm2. */
  void vpaddq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed quadword integers xmm3/m128 and xmm2. */
  void vpaddq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed quadword integers from ymm2, ymm3/m256 and store in ymm1. */
  void vpaddq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed quadword integers from ymm2, ymm3/m256 and store in ymm1. */
  void vpaddq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add packed signed byte integers from xmm3/m128 and xmm2 saturate the results. */
  void vpaddsb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed signed byte integers from xmm3/m128 and xmm2 saturate the results. */
  void vpaddsb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed signed byte integers from ymm2, and ymm3/m256 and store the saturated results in ymm1. */
  void vpaddsb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed signed byte integers from ymm2, and ymm3/m256 and store the saturated results in ymm1. */
  void vpaddsb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add packed signed word integers from xmm3/m128 and xmm2 and saturate the results. */
  void vpaddsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed signed word integers from xmm3/m128 and xmm2 and saturate the results. */
  void vpaddsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed signed word integers from ymm2, and ymm3/m256 and store the saturated results in ymm1. */
  void vpaddsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed signed word integers from ymm2, and ymm3/m256 and store the saturated results in ymm1. */
  void vpaddsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add packed unsigned byte integers from xmm3/m128 to xmm2 and saturate the results. */
  void vpaddusb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed unsigned byte integers from xmm3/m128 to xmm2 and saturate the results. */
  void vpaddusb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed unsigned byte integers from ymm2, and ymm3/m256 and store the saturated results in ymm1. */
  void vpaddusb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed unsigned byte integers from ymm2, and ymm3/m256 and store the saturated results in ymm1. */
  void vpaddusb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add packed unsigned word integers from xmm3/m128 to xmm2 and saturate the results. */
  void vpaddusw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed unsigned word integers from xmm3/m128 to xmm2 and saturate the results. */
  void vpaddusw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed unsigned word integers from ymm2, and ymm3/m256 and store the saturated results in ymm1. */
  void vpaddusw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed unsigned word integers from ymm2, and ymm3/m256 and store the saturated results in ymm1. */
  void vpaddusw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add packed word integers from xmm3/m128 and xmm2. */
  void vpaddw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add packed word integers from xmm3/m128 and xmm2. */
  void vpaddw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add packed word integers from ymm2, ymm3/m256 and store in ymm1. */
  void vpaddw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add packed word integers from ymm2, ymm3/m256 and store in ymm1. */
  void vpaddw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Concatenate xmm2 and xmm3/m128, extract byte aligned result shifted to the right by constant value in imm8 and result is stored in xmm1. */
  void vpalignr(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Concatenate xmm2 and xmm3/m128, extract byte aligned result shifted to the right by constant value in imm8 and result is stored in xmm1. */
  void vpalignr(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Concatenate pairs of 16 bytes in ymm2 and ymm3/m256 into 32-byte intermediate result, extract byte-aligned, 16-byte result shifted to the right by constant values in imm8 from each intermediate result, and two 16-byte results are stored in ymm1. */
  void vpalignr(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Concatenate pairs of 16 bytes in ymm2 and ymm3/m256 into 32-byte intermediate result, extract byte-aligned, 16-byte result shifted to the right by constant values in imm8 from each intermediate result, and two 16-byte results are stored in ymm1. */
  void vpalignr(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Bitwise AND of xmm3/m128 and xmm. */
  void vpand(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Bitwise AND of xmm3/m128 and xmm. */
  void vpand(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Bitwise AND of ymm2, and ymm3/m256 and store result in ymm1. */
  void vpand(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Bitwise AND of ymm2, and ymm3/m256 and store result in ymm1. */
  void vpand(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Bitwise AND NOT of xmm3/m128 and xmm2. */
  void vpandn(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Bitwise AND NOT of xmm3/m128 and xmm2. */
  void vpandn(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Bitwise AND NOT of ymm2, and ymm3/m256 and store result in ymm1. */
  void vpandn(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Bitwise AND NOT of ymm2, and ymm3/m256 and store result in ymm1. */
  void vpandn(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Average packed unsigned byte integers from xmm3/m128 and xmm2 with rounding. */
  void vpavgb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Average packed unsigned byte integers from xmm3/m128 and xmm2 with rounding. */
  void vpavgb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Average packed unsigned byte integers from ymm2, and ymm3/m256 with rounding and store to ymm1. */
  void vpavgb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Average packed unsigned byte integers from ymm2, and ymm3/m256 with rounding and store to ymm1. */
  void vpavgb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Average packed unsigned word integers from xmm3/m128 and xmm2 with rounding. */
  void vpavgw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Average packed unsigned word integers from xmm3/m128 and xmm2 with rounding. */
  void vpavgw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Average packed unsigned word integers from ymm2, ymm3/m256 with rounding to ymm1. */
  void vpavgw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Average packed unsigned word integers from ymm2, ymm3/m256 with rounding to ymm1. */
  void vpavgw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Select dwords from xmm2 and xmm3/m128 from mask specified in imm8 and store the values into xmm1. */
  void vpblendd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Select dwords from xmm2 and xmm3/m128 from mask specified in imm8 and store the values into xmm1. */
  void vpblendd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Select dwords from ymm2 and ymm3/m256 from mask specified in imm8 and store the values into ymm1. */
  void vpblendd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Select dwords from ymm2 and ymm3/m256 from mask specified in imm8 and store the values into ymm1. */
  void vpblendd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Select byte values from xmm2 and xmm3/m128 using mask bits in the specified mask register, xmm4, and store the values into xmm1. */
  void vpblendvb(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Xmm& arg3);

  /** Select byte values from xmm2 and xmm3/m128 using mask bits in the specified mask register, xmm4, and store the values into xmm1. */
  void vpblendvb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Xmm& arg3);

  /** Select byte values from ymm2 and ymm3/m256 from mask specified in the high bit of each byte in ymm4 and store the values into ymm1. */
  void vpblendvb(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Ymm& arg3);

  /** Select byte values from ymm2 and ymm3/m256 from mask specified in the high bit of each byte in ymm4 and store the values into ymm1. */
  void vpblendvb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Ymm& arg3);

  /** Select words from xmm2 and xmm3/m128 from mask specified in imm8 and store the values into xmm1. */
  void vpblendw(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Select words from xmm2 and xmm3/m128 from mask specified in imm8 and store the values into xmm1. */
  void vpblendw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Select words from ymm2 and ymm3/m256 from mask specified in imm8 and store the values into ymm1. */
  void vpblendw(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Select words from ymm2 and ymm3/m256 from mask specified in imm8 and store the values into ymm1. */
  void vpblendw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Broadcast a byte integer in the source operand to sixteen locations in xmm1. */
  void vpbroadcastb(const Xmm& arg0, const M8& arg1);

  /** Broadcast a byte integer in the source operand to sixteen locations in xmm1. */
  void vpbroadcastb(const Xmm& arg0, const Xmm& arg1);

  /** Broadcast a byte integer in the source operand to thirty two locations in ymm1. */
  void vpbroadcastb(const Ymm& arg0, const M8& arg1);

  /** Broadcast a byte integer in the source operand to thirty two locations in ymm1. */
  void vpbroadcastb(const Ymm& arg0, const Xmm& arg1);

  /** Broadcast a dword integer in the source operand to four locations in xmm1. */
  void vpbroadcastd(const Xmm& arg0, const M32& arg1);

  /** Broadcast a dword integer in the source operand to four locations in xmm1. */
  void vpbroadcastd(const Xmm& arg0, const Xmm& arg1);

  /** Broadcast a dword integer in the source operand to eight locations in ymm1. */
  void vpbroadcastd(const Ymm& arg0, const M32& arg1);

  /** Broadcast a dword integer in the source operand to eight locations in ymm1. */
  void vpbroadcastd(const Ymm& arg0, const Xmm& arg1);

  /** Broadcast a qword element in mem to two locations in xmm1 */
  void vpbroadcastq(const Xmm& arg0, const M64& arg1);

  /** Broadcast a qword element in mem to two locations in xmm1 */
  void vpbroadcastq(const Xmm& arg0, const Xmm& arg1);

  /** Broadcast a qword element in mem to four locations in ymm1 */
  void vpbroadcastq(const Ymm& arg0, const M64& arg1);

  /** Broadcast a qword element in mem to four locations in ymm1 */
  void vpbroadcastq(const Ymm& arg0, const Xmm& arg1);

  /** Broadcast a word integer in the source operand to eight locations in xmm1. */
  void vpbroadcastw(const Xmm& arg0, const M16& arg1);

  /** Broadcast a word integer in the source operand to eight locations in xmm1. */
  void vpbroadcastw(const Xmm& arg0, const Xmm& arg1);

  /** Broadcast a word integer in the source operand to sixteen locations in ymm1. */
  void vpbroadcastw(const Ymm& arg0, const M16& arg1);

  /** Broadcast a word integer in the source operand to sixteen locations in ymm1. */
  void vpbroadcastw(const Ymm& arg0, const Xmm& arg1);

  /** Carry-less multiplication of one quadword of xmm2 by one quadword of xmm3/m128, stores the 128-bit result in xmm1. The immediate is used to determine which quadwords of xmm2 and xmm3/m128 should be used. */
  void vpclmulqdq(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Carry-less multiplication of one quadword of xmm2 by one quadword of xmm3/m128, stores the 128-bit result in xmm1. The immediate is used to determine which quadwords of xmm2 and xmm3/m128 should be used. */
  void vpclmulqdq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Compare packed bytes in xmm3/m128 and xmm2 for equality. */
  void vpcmpeqb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed bytes in xmm3/m128 and xmm2 for equality. */
  void vpcmpeqb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed bytes in ymm3/m256 and ymm2 for equality. */
  void vpcmpeqb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed bytes in ymm3/m256 and ymm2 for equality. */
  void vpcmpeqb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed doublewords in xmm3/m128 and xmm2 for equality. */
  void vpcmpeqd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed doublewords in xmm3/m128 and xmm2 for equality. */
  void vpcmpeqd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed doublewords in ymm3/m256 and ymm2 for equality. */
  void vpcmpeqd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed doublewords in ymm3/m256 and ymm2 for equality. */
  void vpcmpeqd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed quadwords in xmm3/m128 and xmm2 for equality. */
  void vpcmpeqq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed quadwords in xmm3/m128 and xmm2 for equality. */
  void vpcmpeqq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed quadwords in ymm3/m256 and ymm2 for equality. */
  void vpcmpeqq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed quadwords in ymm3/m256 and ymm2 for equality. */
  void vpcmpeqq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed words in xmm3/m128 and xmm2 for equality. */
  void vpcmpeqw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed words in xmm3/m128 and xmm2 for equality. */
  void vpcmpeqw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed words in ymm3/m256 and ymm2 for equality. */
  void vpcmpeqw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed words in ymm3/m256 and ymm2 for equality. */
  void vpcmpeqw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Perform a packed comparison of string data with explicit lengths, generating an index, and storing the result in ECX. */
  void vpcmpestri(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with explicit lengths, generating an index, and storing the result in ECX. */
  void vpcmpestri(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with explicit lengths, generating a mask, and storing the result in XMM0. */
  void vpcmpestrm(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with explicit lengths, generating a mask, and storing the result in XMM0. */
  void vpcmpestrm(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Compare packed signed byte integers in xmm2 and xmm3/m128 for greater than. */
  void vpcmpgtb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed byte integers in xmm2 and xmm3/m128 for greater than. */
  void vpcmpgtb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed signed byte integers in ymm2 and ymm3/m256 for greater than. */
  void vpcmpgtb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed byte integers in ymm2 and ymm3/m256 for greater than. */
  void vpcmpgtb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed signed doubleword integers in xmm2 and xmm3/m128 for greater than. */
  void vpcmpgtd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed doubleword integers in xmm2 and xmm3/m128 for greater than. */
  void vpcmpgtd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed signed doubleword integers in ymm2 and ymm3/m256 for greater than. */
  void vpcmpgtd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed doubleword integers in ymm2 and ymm3/m256 for greater than. */
  void vpcmpgtd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed signed qwords in xmm2 and xmm3/m128 for greater than. */
  void vpcmpgtq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed qwords in xmm2 and xmm3/m128 for greater than. */
  void vpcmpgtq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed signed qwords in ymm2 and ymm3/m256 for greater than. */
  void vpcmpgtq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed qwords in ymm2 and ymm3/m256 for greater than. */
  void vpcmpgtq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed signed word integers in xmm2 and xmm3/m128 for greater than. */
  void vpcmpgtw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed word integers in xmm2 and xmm3/m128 for greater than. */
  void vpcmpgtw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed signed word integers in ymm2 and ymm3/m256 for greater than. */
  void vpcmpgtw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed word integers in ymm2 and ymm3/m256 for greater than. */
  void vpcmpgtw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Perform a packed comparison of string data with implicit lengths, generating an index, and storing the result in ECX. */
  void vpcmpistri(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with implicit lengths, generating an index, and storing the result in ECX. */
  void vpcmpistri(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with implicit lengths, generating a Mask, and storing the result in XMM0. */
  void vpcmpistrm(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Perform a packed comparison of string data with implicit lengths, generating a Mask, and storing the result in XMM0. */
  void vpcmpistrm(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Permute 128-bit floating-point fields in ymm2 and ymm3/mem using controls from imm8 and store result in ymm1. */
  void vperm2f128(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Permute 128-bit floating-point fields in ymm2 and ymm3/mem using controls from imm8 and store result in ymm1. */
  void vperm2f128(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Permute 128-bit integer data in ymm2 and ymm3/mem using controls from imm8 and store result in ymm1. */
  void vperm2i128(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Permute 128-bit integer data in ymm2 and ymm3/mem using controls from imm8 and store result in ymm1. */
  void vperm2i128(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Permute doublewords in ymm3/m256 using indexes in ymm2 and store the result in ymm1. */
  void vpermd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Permute doublewords in ymm3/m256 using indexes in ymm2 and store the result in ymm1. */
  void vpermd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Permute double-precision floating-point values in xmm2/mem using controls from imm8. */
  void vpermilpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Permute double-precision floating-point values in xmm2/mem using controls from imm8. */
  void vpermilpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Permute double-precision floating-point values in xmm2 using controls from xmm3/mem and store result in xmm1. */
  void vpermilpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Permute double-precision floating-point values in xmm2 using controls from xmm3/mem and store result in xmm1. */
  void vpermilpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Permute double-precision floating-point values in ymm2/mem using controls from imm8. */
  void vpermilpd(const Ymm& arg0, const M256& arg1, const Imm8& arg2);

  /** Permute double-precision floating-point values in ymm2/mem using controls from imm8. */
  void vpermilpd(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Permute double-precision floating-point values in ymm2 using controls from ymm3/mem and store result in ymm1. */
  void vpermilpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Permute double-precision floating-point values in ymm2 using controls from ymm3/mem and store result in ymm1. */
  void vpermilpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Permute single-precision floating-point values in xmm2/mem using controls from imm8 and store result in xmm1. */
  void vpermilps(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Permute single-precision floating-point values in xmm2/mem using controls from imm8 and store result in xmm1. */
  void vpermilps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Permute single-precision floating-point values in xmm2 using controls from xmm3/mem and store result in xmm1. */
  void vpermilps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Permute single-precision floating-point values in xmm2 using controls from xmm3/mem and store result in xmm1. */
  void vpermilps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Permute single-precision floating-point values in ymm2/mem using controls from imm8 and store result in ymm1. */
  void vpermilps(const Ymm& arg0, const M256& arg1, const Imm8& arg2);

  /** Permute single-precision floating-point values in ymm2/mem using controls from imm8 and store result in ymm1. */
  void vpermilps(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Permute single-precision floating-point values in ymm2 using controls from ymm3/mem and store result in ymm1. */
  void vpermilps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Permute single-precision floating-point values in ymm2 using controls from ymm3/mem and store result in ymm1. */
  void vpermilps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Permute double-precision floating-point elements in ymm2/m256 using indexes in imm8 and store the result in ymm1. */
  void vpermpd(const Ymm& arg0, const M256& arg1, const Imm8& arg2);

  /** Permute double-precision floating-point elements in ymm2/m256 using indexes in imm8 and store the result in ymm1. */
  void vpermpd(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Permute single-precision floating-point elements in ymm3/m256 using indexes in ymm2 and store the result in ymm1. */
  void vpermps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Permute single-precision floating-point elements in ymm3/m256 using indexes in ymm2 and store the result in ymm1. */
  void vpermps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Permute qwords in ymm2/m256 using indexes in imm8 and store the result in ymm1. */
  void vpermq(const Ymm& arg0, const M256& arg1, const Imm8& arg2);

  /** Permute qwords in ymm2/m256 using indexes in imm8 and store the result in ymm1. */
  void vpermq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Extract a byte integer value from xmm2 at the source byte offset specified by imm8 into reg or m8. The upper bits of r64/r32 is filled with */
  void vpextrb(const M8& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a byte integer value from xmm2 at the source byte offset specified by imm8 into reg or m8. The upper bits of r64/r32 is filled with */
  void vpextrb(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a byte integer value from xmm2 at the source byte offset specified by imm8 into reg or m8. The upper bits of r64/r32 is filled with */
  void vpextrb(const R64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a dword integer value from xmm2 at the source dword offset specified by imm8 into r32/m32. */
  void vpextrd(const M32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a dword integer value from xmm2 at the source dword offset specified by imm8 into r32/m32. */
  void vpextrd(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a qword integer value from xmm2 at the source dword offset specified by imm8 into r64/m64. */
  void vpextrq(const M64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a qword integer value from xmm2 at the source dword offset specified by imm8 into r64/m64. */
  void vpextrq(const R64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a word integer value from xmm2 at the source word offset specified by imm8 into reg or m16. The upper bits of r64/r32 is filled with zeros. */
  void vpextrw(const M16& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract the word specified by imm8 from xmm1 and move it to reg, bits 15:0. Zero-extend the result. The upper bits of r64/r32 is filled with zeros. */
  void vpextrw(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a word integer value from xmm2 at the source word offset specified by imm8 into reg or m16. The upper bits of r64/r32 is filled with zeros. */
  void vpextrw_1(const R32& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract the word specified by imm8 from xmm1 and move it to reg, bits 15:0. Zero-extend the result. The upper bits of r64/r32 is filled with zeros. */
  void vpextrw(const R64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Extract a word integer value from xmm2 at the source word offset specified by imm8 into reg or m16. The upper bits of r64/r32 is filled with zeros. */
  void vpextrw_1(const R64& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Using dword indices specified in vm32x, gather dword values from memory conditioned on mask specified by xmm2. Conditionally gathered elements are merged into xmm1. */
  void vpgatherdd(const Xmm& arg0, const M32& arg1, const Xmm& arg2);

  /** Using dword indices specified in vm32y, gather dword from memory conditioned on mask specified by ymm2. Conditionally gathered elements are merged into ymm1. */
  void vpgatherdd(const Ymm& arg0, const M32& arg1, const Ymm& arg2);

  /** Using dword indices specified in vm32x, gather qword values from memory conditioned on mask specified by xmm2. Conditionally gathered elements are merged into xmm1. */
  void vpgatherdq(const Xmm& arg0, const M32& arg1, const Xmm& arg2);

  /** Using dword indices specified in vm32x, gather qword values from memory conditioned on mask specified by ymm2. Conditionally gathered elements are merged into ymm1. */
  void vpgatherdq(const Ymm& arg0, const M32& arg1, const Ymm& arg2);

  /** Using qword indices specified in vm64x, gather dword values from memory conditioned on mask specified by xmm2. Conditionally gathered elements are merged into xmm1. */
  void vpgatherqd(const Xmm& arg0, const M64& arg1, const Xmm& arg2);

  /** Using qword indices specified in vm64y, gather dword values from memory conditioned on mask specified by xmm2. Conditionally gathered elements are merged into xmm1. */
  void vpgatherqd_1(const Xmm& arg0, const M64& arg1, const Xmm& arg2);

  /** Using qword indices specified in vm64x, gather qword values from memory conditioned on mask specified by xmm2. Conditionally gathered elements are merged into xmm1. */
  void vpgatherqq(const Xmm& arg0, const M64& arg1, const Xmm& arg2);

  /** Using qword indices specified in vm64y, gather qword values from memory conditioned on mask specified by ymm2. Conditionally gathered elements are merged into ymm1. */
  void vpgatherqq(const Ymm& arg0, const M64& arg1, const Ymm& arg2);

  /** Add 32-bit integers horizontally, pack to xmm1. */
  void vphaddd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add 32-bit integers horizontally, pack to xmm1. */
  void vphaddd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add 32-bit signed integers horizontally, pack to ymm1. */
  void vphaddd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add 32-bit signed integers horizontally, pack to ymm1. */
  void vphaddd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add 16-bit signed integers horizontally, pack saturated integers to xmm1. */
  void vphaddsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add 16-bit signed integers horizontally, pack saturated integers to xmm1. */
  void vphaddsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add 16-bit signed integers horizontally, pack saturated integers to ymm1. */
  void vphaddsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add 16-bit signed integers horizontally, pack saturated integers to ymm1. */
  void vphaddsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Add 16-bit integers horizontally, pack to xmm1. */
  void vphaddw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Add 16-bit integers horizontally, pack to xmm1. */
  void vphaddw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Add 16-bit signed integers horizontally, pack to ymm1. */
  void vphaddw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Add 16-bit signed integers horizontally, pack to ymm1. */
  void vphaddw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Find the minimum unsigned word in xmm2/m128 and place its value in the low word of xmm1 and its index in the second-lowest word of xmm1. */
  void vphminposuw(const Xmm& arg0, const M128& arg1);

  /** Find the minimum unsigned word in xmm2/m128 and place its value in the low word of xmm1 and its index in the second-lowest word of xmm1. */
  void vphminposuw(const Xmm& arg0, const Xmm& arg1);

  /** Subtract 32-bit signed integers horizontally, pack to xmm1. */
  void vphsubd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract 32-bit signed integers horizontally, pack to xmm1. */
  void vphsubd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract 32-bit signed integers horizontally, pack to ymm1. */
  void vphsubd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract 32-bit signed integers horizontally, pack to ymm1. */
  void vphsubd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract 16-bit signed integer horizontally, pack saturated integers to xmm1. */
  void vphsubsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract 16-bit signed integer horizontally, pack saturated integers to xmm1. */
  void vphsubsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract 16-bit signed integer horizontally, pack saturated integers to ymm1. */
  void vphsubsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract 16-bit signed integer horizontally, pack saturated integers to ymm1. */
  void vphsubsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract 16-bit signed integers horizontally, pack to xmm1. */
  void vphsubw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract 16-bit signed integers horizontally, pack to xmm1. */
  void vphsubw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract 16-bit signed integers horizontally, pack to ymm1. */
  void vphsubw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract 16-bit signed integers horizontally, pack to ymm1. */
  void vphsubw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Merge a byte integer value from r32/m8 and rest from xmm2 into xmm1 at the byte offset in imm8. */
  void vpinsrb(const Xmm& arg0, const Xmm& arg1, const M8& arg2, const Imm8& arg3);

  /** Merge a byte integer value from r32/m8 and rest from xmm2 into xmm1 at the byte offset in imm8. */
  void vpinsrb(const Xmm& arg0, const Xmm& arg1, const R32& arg2, const Imm8& arg3);

  /** Insert a dword integer value from r32/m32 and rest from xmm2 into xmm1 at the dword offset in imm8. */
  void vpinsrd(const Xmm& arg0, const Xmm& arg1, const M32& arg2, const Imm8& arg3);

  /** Insert a dword integer value from r32/m32 and rest from xmm2 into xmm1 at the dword offset in imm8. */
  void vpinsrd(const Xmm& arg0, const Xmm& arg1, const R32& arg2, const Imm8& arg3);

  /** Insert a qword integer value from r64/m64 and rest from xmm2 into xmm1 at the qword offset in imm8. */
  void vpinsrq(const Xmm& arg0, const Xmm& arg1, const M64& arg2, const Imm8& arg3);

  /** Insert a qword integer value from r64/m64 and rest from xmm2 into xmm1 at the qword offset in imm8. */
  void vpinsrq(const Xmm& arg0, const Xmm& arg1, const R64& arg2, const Imm8& arg3);

  /** Insert a word integer value from r32/m16 and rest from xmm2 into xmm1 at the word offset in imm8. */
  void vpinsrw(const Xmm& arg0, const Xmm& arg1, const M16& arg2, const Imm8& arg3);

  /** Insert a word integer value from r32/m16 and rest from xmm2 into xmm1 at the word offset in imm8. */
  void vpinsrw(const Xmm& arg0, const Xmm& arg1, const R32& arg2, const Imm8& arg3);

  /** Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to xmm1. */
  void vpmaddubsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to xmm1. */
  void vpmaddubsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to ymm1. */
  void vpmaddubsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply signed and unsigned bytes, add horizontal pair of signed words, pack saturated signed-words to ymm1. */
  void vpmaddubsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply the packed word integers in xmm2 by the packed word integers in xmm3/m128, add adjacent doubleword results, and store in xmm1. */
  void vpmaddwd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply the packed word integers in xmm2 by the packed word integers in xmm3/m128, add adjacent doubleword results, and store in xmm1. */
  void vpmaddwd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply the packed word integers in ymm2 by the packed word integers in ymm3/m256, add adjacent doubleword results, and store in ymm1. */
  void vpmaddwd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply the packed word integers in ymm2 by the packed word integers in ymm3/m256, add adjacent doubleword results, and store in ymm1. */
  void vpmaddwd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Conditionally store dword values from xmm2 using mask in xmm1. */
  void vpmaskmovd(const M128& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Conditionally store dword values from ymm2 using mask in ymm1. */
  void vpmaskmovd(const M256& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Conditionally load dword values from m128 using mask in xmm2 and store in xmm1. */
  void vpmaskmovd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Conditionally load dword values from m256 using mask in ymm2 and store in ymm1. */
  void vpmaskmovd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Conditionally store qword values from xmm2 using mask in xmm1. */
  void vpmaskmovq(const M128& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Conditionally store qword values from ymm2 using mask in ymm1. */
  void vpmaskmovq(const M256& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Conditionally load qword values from m128 using mask in xmm2 and store in xmm1. */
  void vpmaskmovq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Conditionally load qword values from m256 using mask in ymm2 and store in ymm1. */
  void vpmaskmovq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed byte integers in xmm2 and xmm3/m128 and store packed maximum values in xmm1. */
  void vpmaxsb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed byte integers in xmm2 and xmm3/m128 and store packed maximum values in xmm1. */
  void vpmaxsb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed signed byte integers in ymm2 and ymm3/m128 and store packed maximum values in ymm1. */
  void vpmaxsb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed byte integers in ymm2 and ymm3/m128 and store packed maximum values in ymm1. */
  void vpmaxsb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed signed dword integers in xmm2 and xmm3/m128 and store packed maximum values in xmm1. */
  void vpmaxsd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed dword integers in xmm2 and xmm3/m128 and store packed maximum values in xmm1. */
  void vpmaxsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed signed dword integers in ymm2 and ymm3/m128 and store packed maximum values in ymm1. */
  void vpmaxsd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed dword integers in ymm2 and ymm3/m128 and store packed maximum values in ymm1. */
  void vpmaxsd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed signed word integers in xmm3/m128 and xmm2 and store packed maximum values in xmm1. */
  void vpmaxsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed word integers in xmm3/m128 and xmm2 and store packed maximum values in xmm1. */
  void vpmaxsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed signed word integers in ymm3/m128 and ymm2 and store packed maximum values in ymm1. */
  void vpmaxsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed word integers in ymm3/m128 and ymm2 and store packed maximum values in ymm1. */
  void vpmaxsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed unsigned byte integers in xmm2 and xmm3/m128 and store packed maximum values in xmm1. */
  void vpmaxub(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed unsigned byte integers in xmm2 and xmm3/m128 and store packed maximum values in xmm1. */
  void vpmaxub(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed unsigned byte integers in ymm2 and ymm3/m256 and store packed maximum values in ymm1. */
  void vpmaxub(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed unsigned byte integers in ymm2 and ymm3/m256 and store packed maximum values in ymm1. */
  void vpmaxub(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed unsigned dword integers in xmm2 and xmm3/m128 and store packed maximum values in xmm1. */
  void vpmaxud(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed unsigned dword integers in xmm2 and xmm3/m128 and store packed maximum values in xmm1. */
  void vpmaxud(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed unsigned dword integers in ymm2 and ymm3/m256 and store packed maximum values in ymm1. */
  void vpmaxud(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed unsigned dword integers in ymm2 and ymm3/m256 and store packed maximum values in ymm1. */
  void vpmaxud(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed unsigned word integers in xmm3/m128 and xmm2 and store maximum packed values in xmm1. */
  void vpmaxuw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed unsigned word integers in xmm3/m128 and xmm2 and store maximum packed values in xmm1. */
  void vpmaxuw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed unsigned word integers in ymm3/m256 and ymm2 and store maximum packed values in ymm1. */
  void vpmaxuw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed unsigned word integers in ymm3/m256 and ymm2 and store maximum packed values in ymm1. */
  void vpmaxuw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed signed byte integers in xmm2 and xmm3/m128 and store packed minimum values in xmm1. */
  void vpminsb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed byte integers in xmm2 and xmm3/m128 and store packed minimum values in xmm1. */
  void vpminsb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed signed byte integers in ymm2 and ymm3/m256 and store packed minimum values in ymm1. */
  void vpminsb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed byte integers in ymm2 and ymm3/m256 and store packed minimum values in ymm1. */
  void vpminsb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed signed dword integers in xmm2 and xmm3/m128 and store packed minimum values in xmm1. */
  void vpminsd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed dword integers in xmm2 and xmm3/m128 and store packed minimum values in xmm1. */
  void vpminsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed signed dword integers in ymm2 and ymm3/m128 and store packed minimum values in ymm1. */
  void vpminsd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed signed dword integers in ymm2 and ymm3/m128 and store packed minimum values in ymm1. */
  void vpminsd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed signed word integers in xmm3/m128 and xmm2 and return packed minimum values in xmm1. */
  void vpminsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed signed word integers in xmm3/m128 and xmm2 and return packed minimum values in xmm1. */
  void vpminsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed unsigned byte integers in xmm2 and xmm3/m128 and store packed minimum values in xmm1. */
  void vpminub(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed unsigned byte integers in xmm2 and xmm3/m128 and store packed minimum values in xmm1. */
  void vpminub(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed unsigned byte integers in ymm2 and ymm3/m256 and store packed minimum values in ymm1. */
  void vpminub(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed unsigned byte integers in ymm2 and ymm3/m256 and store packed minimum values in ymm1. */
  void vpminub(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed unsigned dword integers in xmm2 and xmm3/m128 and store packed minimum values in xmm1. */
  void vpminud(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed unsigned dword integers in xmm2 and xmm3/m128 and store packed minimum values in xmm1. */
  void vpminud(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed unsigned dword integers in ymm2 and ymm3/m256 and store packed minimum values in ymm1. */
  void vpminud(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed unsigned dword integers in ymm2 and ymm3/m256 and store packed minimum values in ymm1. */
  void vpminud(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Compare packed unsigned word integers in xmm3/m128 and xmm2 and return packed minimum values in xmm1. */
  void vpminuw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Compare packed unsigned word integers in xmm3/m128 and xmm2 and return packed minimum values in xmm1. */
  void vpminuw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Compare packed unsigned word integers in ymm3/m256 and ymm2 and return packed minimum values in ymm1. */
  void vpminuw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Compare packed unsigned word integers in ymm3/m256 and ymm2 and return packed minimum values in ymm1. */
  void vpminuw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Move a byte mask of xmm1 to reg. The upper bits of r32 or r64 are filled with zeros. */
  void vpmovmskb(const R32& arg0, const Xmm& arg1);

  /** Move a 32-bit mask of ymm1 to reg. The upper bits of r64 are filled with zeros. */
  void vpmovmskb(const R32& arg0, const Ymm& arg1);

  /** Move a byte mask of xmm1 to reg. The upper bits of r32 or r64 are filled with zeros. */
  void vpmovmskb(const R64& arg0, const Xmm& arg1);

  /** Move a 32-bit mask of ymm1 to reg. The upper bits of r64 are filled with zeros. */
  void vpmovmskb(const R64& arg0, const Ymm& arg1);

  /** Sign extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 32-bit integers in xmm1. */
  void vpmovsxbd(const Xmm& arg0, const M32& arg1);

  /** Sign extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 32-bit integers in xmm1. */
  void vpmovsxbd(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 32-bit integers in ymm1. */
  void vpmovsxbd(const Ymm& arg0, const M64& arg1);

  /** Sign extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 32-bit integers in ymm1. */
  void vpmovsxbd(const Ymm& arg0, const Xmm& arg1);

  /** Sign extend 2 packed 8-bit integers in the low 2 bytes of xmm2/m16 to 2 packed 64-bit integers in xmm1. */
  void vpmovsxbq(const Xmm& arg0, const M16& arg1);

  /** Sign extend 2 packed 8-bit integers in the low 2 bytes of xmm2/m16 to 2 packed 64-bit integers in xmm1. */
  void vpmovsxbq(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 64-bit integers in ymm1. */
  void vpmovsxbq(const Ymm& arg0, const M32& arg1);

  /** Sign extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 64-bit integers in ymm1. */
  void vpmovsxbq(const Ymm& arg0, const Xmm& arg1);

  /** Sign extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 16-bit integers in xmm1. */
  void vpmovsxbw(const Xmm& arg0, const M64& arg1);

  /** Sign extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 16-bit integers in xmm1. */
  void vpmovsxbw(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 16 packed 8-bit integers in xmm2/m128 to 16 packed 16-bit integers in ymm1. */
  void vpmovsxbw(const Ymm& arg0, const M128& arg1);

  /** Sign extend 16 packed 8-bit integers in xmm2/m128 to 16 packed 16-bit integers in ymm1. */
  void vpmovsxbw(const Ymm& arg0, const Xmm& arg1);

  /** Sign extend 2 packed 32-bit integers in the low 8 bytes of xmm2/m64 to 2 packed 64-bit integers in xmm1 */
  void vpmovsxdq(const Xmm& arg0, const M64& arg1);

  /** Sign extend 2 packed 32-bit integers in the low 8 bytes of xmm2/m64 to 2 packed 64-bit integers in xmm1 */
  void vpmovsxdq(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 4 packed 32-bit integers in the low 16 bytes of xmm2/m128 to 4 packed 64-bit integers in ymm1. */
  void vpmovsxdq(const Ymm& arg0, const M128& arg1);

  /** Sign extend 4 packed 32-bit integers in the low 16 bytes of xmm2/m128 to 4 packed 64-bit integers in ymm1. */
  void vpmovsxdq(const Ymm& arg0, const Xmm& arg1);

  /** Sign extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 32-bit integers in xmm1. */
  void vpmovsxwd(const Xmm& arg0, const M64& arg1);

  /** Sign extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 32-bit integers in xmm1. */
  void vpmovsxwd(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 8 packed 16-bit integers in the low 16 bytes of xmm2/m128 to 8 packed 32-bit integers in ymm1. */
  void vpmovsxwd(const Ymm& arg0, const M128& arg1);

  /** Sign extend 8 packed 16-bit integers in the low 16 bytes of xmm2/m128 to 8 packed 32-bit integers in ymm1. */
  void vpmovsxwd(const Ymm& arg0, const Xmm& arg1);

  /** Sign extend 2 packed 16-bit integers in the low 4 bytes of xmm2/m32 to 2 packed 64-bit integers in xmm1. */
  void vpmovsxwq(const Xmm& arg0, const M32& arg1);

  /** Sign extend 2 packed 16-bit integers in the low 4 bytes of xmm2/m32 to 2 packed 64-bit integers in xmm1. */
  void vpmovsxwq(const Xmm& arg0, const Xmm& arg1);

  /** Sign extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 64-bit integers in ymm1. */
  void vpmovsxwq(const Ymm& arg0, const M64& arg1);

  /** Sign extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 64-bit integers in ymm1. */
  void vpmovsxwq(const Ymm& arg0, const Xmm& arg1);

  /** Zero extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 32-bit integers in xmm1. */
  void vpmovzxbd(const Xmm& arg0, const M32& arg1);

  /** Zero extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 32-bit integers in xmm1. */
  void vpmovzxbd(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 32-bit integers in ymm1. */
  void vpmovzxbd(const Ymm& arg0, const M64& arg1);

  /** Zero extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 32-bit integers in ymm1. */
  void vpmovzxbd(const Ymm& arg0, const Xmm& arg1);

  /** Zero extend 2 packed 8-bit integers in the low 2 bytes of xmm2/m16 to 2 packed 64-bit integers in xmm1. */
  void vpmovzxbq(const Xmm& arg0, const M16& arg1);

  /** Zero extend 2 packed 8-bit integers in the low 2 bytes of xmm2/m16 to 2 packed 64-bit integers in xmm1. */
  void vpmovzxbq(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 64-bit integers in ymm1. */
  void vpmovzxbq(const Ymm& arg0, const M32& arg1);

  /** Zero extend 4 packed 8-bit integers in the low 4 bytes of xmm2/m32 to 4 packed 64-bit integers in ymm1. */
  void vpmovzxbq(const Ymm& arg0, const Xmm& arg1);

  /** Zero extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 16-bit integers in xmm1. */
  void vpmovzxbw(const Xmm& arg0, const M64& arg1);

  /** Zero extend 8 packed 8-bit integers in the low 8 bytes of xmm2/m64 to 8 packed 16-bit integers in xmm1. */
  void vpmovzxbw(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 16 packed 8-bit integers in the low 16 bytes of xmm2/m128 to 16 packed 16-bit integers in ymm1. */
  void vpmovzxbw(const Ymm& arg0, const M128& arg1);

  /** Zero extend 16 packed 8-bit integers in the low 16 bytes of xmm2/m128 to 16 packed 16-bit integers in ymm1. */
  void vpmovzxbw(const Ymm& arg0, const Xmm& arg1);

  /** Zero extend 2 packed 32-bit integers in the low 8 bytes of xmm2/m64 to 2 packed 64-bit integers in xmm1. */
  void vpmovzxdq(const Xmm& arg0, const M64& arg1);

  /** Zero extend 2 packed 32-bit integers in the low 8 bytes of xmm2/m64 to 2 packed 64-bit integers in xmm1. */
  void vpmovzxdq(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 4 packed 32-bit integers in the low 16 bytes of xmm2/m128 to 4 packed 64-bit integers in ymm1. */
  void vpmovzxdq(const Ymm& arg0, const M128& arg1);

  /** Zero extend 4 packed 32-bit integers in the low 16 bytes of xmm2/m128 to 4 packed 64-bit integers in ymm1. */
  void vpmovzxdq(const Ymm& arg0, const Xmm& arg1);

  /** Zero extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 32-bit integers in xmm1. */
  void vpmovzxwd(const Xmm& arg0, const M64& arg1);

  /** Zero extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 32-bit integers in xmm1. */
  void vpmovzxwd(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 8 packed 16-bit integers in the low 16 bytes of xmm2/m128 to 8 packed 32-bit integers in ymm1 */
  void vpmovzxwd(const Ymm& arg0, const M128& arg1);

  /** Zero extend 8 packed 16-bit integers in the low 16 bytes of xmm2/m128 to 8 packed 32-bit integers in ymm1 */
  void vpmovzxwd(const Ymm& arg0, const Xmm& arg1);

  /** Zero extend 2 packed 16-bit integers in the low 4 bytes of xmm2/m32 to 2 packed 64-bit integers in xmm1. */
  void vpmovzxwq(const Xmm& arg0, const M32& arg1);

  /** Zero extend 2 packed 16-bit integers in the low 4 bytes of xmm2/m32 to 2 packed 64-bit integers in xmm1. */
  void vpmovzxwq(const Xmm& arg0, const Xmm& arg1);

  /** Zero extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 64-bit integers in xmm1. */
  void vpmovzxwq(const Ymm& arg0, const M64& arg1);

  /** Zero extend 4 packed 16-bit integers in the low 8 bytes of xmm2/m64 to 4 packed 64-bit integers in xmm1. */
  void vpmovzxwq(const Ymm& arg0, const Xmm& arg1);

  /** Multiply packed signed doubleword integers in xmm2 by packed signed doubleword integers in xmm3/m128, and store the quadword results in xmm1. */
  void vpmuldq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed signed doubleword integers in xmm2 by packed signed doubleword integers in xmm3/m128, and store the quadword results in xmm1. */
  void vpmuldq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed signed doubleword integers in ymm2 by packed signed doubleword integers in ymm3/m256, and store the quadword results in ymm1. */
  void vpmuldq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed signed doubleword integers in ymm2 by packed signed doubleword integers in ymm3/m256, and store the quadword results in ymm1. */
  void vpmuldq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to xmm1. */
  void vpmulhrsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to xmm1. */
  void vpmulhrsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to ymm1. */
  void vpmulhrsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply 16-bit signed words, scale and round signed doublewords, pack high 16 bits to ymm1. */
  void vpmulhrsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply the packed unsigned word integers in xmm2 and xmm3/m128, and store the high 16 bits of the results in xmm1. */
  void vpmulhuw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply the packed unsigned word integers in xmm2 and xmm3/m128, and store the high 16 bits of the results in xmm1. */
  void vpmulhuw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply the packed unsigned word integers in ymm2 and ymm3/m256, and store the high 16 bits of the results in ymm1. */
  void vpmulhuw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply the packed unsigned word integers in ymm2 and ymm3/m256, and store the high 16 bits of the results in ymm1. */
  void vpmulhuw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply the packed signed word integers in xmm2 and xmm3/m128, and store the high 16 bits of the results in xmm1. */
  void vpmulhw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply the packed signed word integers in xmm2 and xmm3/m128, and store the high 16 bits of the results in xmm1. */
  void vpmulhw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply the packed signed word integers in ymm2 and ymm3/m256, and store the high 16 bits of the results in ymm1. */
  void vpmulhw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply the packed signed word integers in ymm2 and ymm3/m256, and store the high 16 bits of the results in ymm1. */
  void vpmulhw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply the packed dword signed integers in xmm2 and xmm3/m128 and store the low 32 bits of each product in xmm1. */
  void vpmulld(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply the packed dword signed integers in xmm2 and xmm3/m128 and store the low 32 bits of each product in xmm1. */
  void vpmulld(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply the packed dword signed integers in ymm2 and ymm3/m256 and store the low 32 bits of each product in ymm1. */
  void vpmulld(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply the packed dword signed integers in ymm2 and ymm3/m256 and store the low 32 bits of each product in ymm1. */
  void vpmulld(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply the packed dword signed integers in xmm2 and xmm3/m128 and store the low 32 bits of each product in xmm1. */
  void vpmullw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply the packed dword signed integers in xmm2 and xmm3/m128 and store the low 32 bits of each product in xmm1. */
  void vpmullw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply the packed signed word integers in ymm2 and ymm3/m256, and store the low 16 bits of the results in ymm1. */
  void vpmullw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply the packed signed word integers in ymm2 and ymm3/m256, and store the low 16 bits of the results in ymm1. */
  void vpmullw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Multiply packed unsigned doubleword integers in xmm2 by packed unsigned doubleword integers in xmm3/m128, and store the quadword results in xmm1. */
  void vpmuludq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Multiply packed unsigned doubleword integers in xmm2 by packed unsigned doubleword integers in xmm3/m128, and store the quadword results in xmm1. */
  void vpmuludq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Multiply packed unsigned doubleword integers in ymm2 by packed unsigned doubleword integers in ymm3/m256, and store the quadword results in ymm1. */
  void vpmuludq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Multiply packed unsigned doubleword integers in ymm2 by packed unsigned doubleword integers in ymm3/m256, and store the quadword results in ymm1. */
  void vpmuludq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Bitwise OR of xmm2/m128 and xmm3. */
  void vpor(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Bitwise OR of xmm2/m128 and xmm3. */
  void vpor(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Bitwise OR of ymm2/m256 and ymm3 */
  void vpor(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Bitwise OR of ymm2/m256 and ymm3 */
  void vpor(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Computes the absolute differences of the packed unsigned byte integers from xmm3 /m128 and xmm2; the 8 low differences and 8 high differences are then summed separately to produce two unsigned word integer results. */
  void vpsadbw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Computes the absolute differences of the packed unsigned byte integers from xmm3 /m128 and xmm2; the 8 low differences and 8 high differences are then summed separately to produce two unsigned word integer results. */
  void vpsadbw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Computes the absolute differences of the packed unsigned byte integers from ymm3/m256 and ymm2; then each consecutive 8 differences are summed separately to produce four unsigned word integer results. */
  void vpsadbw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Computes the absolute differences of the packed unsigned byte integers from ymm3/m256 and ymm2; then each consecutive 8 differences are summed separately to produce four unsigned word integer results. */
  void vpsadbw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Shuffle bytes in xmm2 according to contents of xmm3/m128. */
  void vpshufb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shuffle bytes in xmm2 according to contents of xmm3/m128. */
  void vpshufb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shuffle bytes in ymm2 according to contents of ymm3/m256. */
  void vpshufb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Shuffle bytes in ymm2 according to contents of ymm3/m256. */
  void vpshufb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Shuffle the doublewords in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void vpshufd(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Shuffle the doublewords in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void vpshufd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shuffle the doublewords in ymm2/m256 based on the encoding in imm8 and store the result in ymm1. */
  void vpshufd(const Ymm& arg0, const M256& arg1, const Imm8& arg2);

  /** Shuffle the doublewords in ymm2/m256 based on the encoding in imm8 and store the result in ymm1. */
  void vpshufd(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shuffle the high words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void vpshufhw(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Shuffle the high words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void vpshufhw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shuffle the high words in ymm2/m256 based on the encoding in imm8 and store the result in ymm1. */
  void vpshufhw(const Ymm& arg0, const M256& arg1, const Imm8& arg2);

  /** Shuffle the high words in ymm2/m256 based on the encoding in imm8 and store the result in ymm1. */
  void vpshufhw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shuffle the low words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void vpshuflw(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Shuffle the low words in xmm2/m128 based on the encoding in imm8 and store the result in xmm1. */
  void vpshuflw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shuffle the low words in ymm2/m256 based on the encoding in imm8 and store the result in ymm1. */
  void vpshuflw(const Ymm& arg0, const M256& arg1, const Imm8& arg2);

  /** Shuffle the low words in ymm2/m256 based on the encoding in imm8 and store the result in ymm1. */
  void vpshuflw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Negate/zero/preserve packed byte integers in xmm2 depending on the corresponding sign in xmm3/m128. */
  void vpsignb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Negate/zero/preserve packed byte integers in xmm2 depending on the corresponding sign in xmm3/m128. */
  void vpsignb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Negate/zero/preserve packed doubleword integers in xmm2 depending on the corresponding sign in xmm3/m128. */
  void vpsignd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Negate/zero/preserve packed doubleword integers in xmm2 depending on the corresponding sign in xmm3/m128. */
  void vpsignd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Negate/zero/preserve packed word integers in xmm2 depending on the corresponding sign in xmm3/m128. */
  void vpsignw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Negate/zero/preserve packed word integers in xmm2 depending on the corresponding sign in xmm3/m128. */
  void vpsignw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift doublewords in xmm2 left by imm8 while shifting in 0s. */
  void vpslld(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift doublewords in xmm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpslld(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift doublewords in xmm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpslld(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift doublewords in ymm2 left by imm8 while shifting in 0s. */
  void vpslld(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift doublewords in ymm2 left by amount specified in xmm3/m128 while shifting in 0s */
  void vpslld(const Ymm& arg0, const Ymm& arg1, const M128& arg2);

  /** Shift doublewords in ymm2 left by amount specified in xmm3/m128 while shifting in 0s */
  void vpslld(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2);

  /** Shift xmm2 left by imm8 bytes while shifting in 0s and store result in xmm1. */
  void vpslldq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift ymm2 left by imm8 bytes while shifting in 0s and store result in ymm1. */
  void vpslldq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift quadwords in xmm2 left by imm8 while shifting in 0s. */
  void vpsllq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift quadwords in xmm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsllq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift quadwords in xmm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsllq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift quadwords in ymm2 left by imm8 while shifting in 0s. */
  void vpsllq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift quadwords in ymm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsllq(const Ymm& arg0, const Ymm& arg1, const M128& arg2);

  /** Shift quadwords in ymm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsllq(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2);

  /** Shift bits in doublewords in xmm2 left by amount specified in the corresponding element of xmm3/m128 while shifting in 0s. */
  void vpsllvd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift bits in doublewords in xmm2 left by amount specified in the corresponding element of xmm3/m128 while shifting in 0s. */
  void vpsllvd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift bits in doublewords in ymm2 left by amount specified in the corresponding element of ymm3/m256 while shifting in 0s. */
  void vpsllvd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Shift bits in doublewords in ymm2 left by amount specified in the corresponding element of ymm3/m256 while shifting in 0s. */
  void vpsllvd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Shift bits in quadwords in xmm2 left by amount specified in the corresponding element of xmm3/m128 while shifting in 0s. */
  void vpsllvq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift bits in quadwords in xmm2 left by amount specified in the corresponding element of xmm3/m128 while shifting in 0s. */
  void vpsllvq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift bits in quadwords in ymm2 left by amount specified in the corresponding element of ymm3/m256 while shifting in 0s. */
  void vpsllvq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Shift bits in quadwords in ymm2 left by amount specified in the corresponding element of ymm3/m256 while shifting in 0s. */
  void vpsllvq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Shift words in xmm2 left by imm8 while shifting in 0s. */
  void vpsllw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift words in xmm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsllw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift words in xmm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsllw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift words in ymm2 left by imm8 while shifting in 0s. */
  void vpsllw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift words in ymm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsllw(const Ymm& arg0, const Ymm& arg1, const M128& arg2);

  /** Shift words in ymm2 left by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsllw(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2);

  /** Shift doublewords in xmm2 right by imm8 while shifting in sign bits. */
  void vpsrad(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift doublewords in xmm2 right by amount specified in xmm3/m128 while shifting in sign bits. */
  void vpsrad(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift doublewords in xmm2 right by amount specified in xmm3/m128 while shifting in sign bits. */
  void vpsrad(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift doublewords in ymm2 right by imm8 while shifting in sign bits. */
  void vpsrad(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift doublewords in ymm2 right by amount specified in xmm3/m128 while shifting in sign bits. */
  void vpsrad(const Ymm& arg0, const Ymm& arg1, const M128& arg2);

  /** Shift doublewords in ymm2 right by amount specified in xmm3/m128 while shifting in sign bits. */
  void vpsrad(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2);

  /** Shift bits in doublewords in xmm2 right by amount specified in the corresponding element of xmm3/m128 while shifting in the sign bits. */
  void vpsravd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift bits in doublewords in xmm2 right by amount specified in the corresponding element of xmm3/m128 while shifting in the sign bits. */
  void vpsravd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift bits in doublewords in ymm2 right by amount specified in the corresponding element of ymm3/m256 while shifting in the sign bits. */
  void vpsravd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Shift bits in doublewords in ymm2 right by amount specified in the corresponding element of ymm3/m256 while shifting in the sign bits. */
  void vpsravd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Shift words in xmm2 right by imm8 while shifting in sign bits. */
  void vpsraw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift words in xmm2 right by amount specified in xmm3/m128 while shifting in sign bits. */
  void vpsraw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift words in xmm2 right by amount specified in xmm3/m128 while shifting in sign bits. */
  void vpsraw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift words in ymm2 right by imm8 while shifting in sign bits. */
  void vpsraw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift words in ymm2 right by amount specified in xmm3/m128 while shifting in sign bits. */
  void vpsraw(const Ymm& arg0, const Ymm& arg1, const M128& arg2);

  /** Shift words in ymm2 right by amount specified in xmm3/m128 while shifting in sign bits. */
  void vpsraw(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2);

  /** Shift doublewords in xmm2 right by imm8 while shifting in 0s. */
  void vpsrld(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift doublewords in xmm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrld(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift doublewords in xmm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrld(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift doublewords in ymm2 right by imm8 while shifting in 0s. */
  void vpsrld(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift doublewords in ymm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrld(const Ymm& arg0, const Ymm& arg1, const M128& arg2);

  /** Shift doublewords in ymm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrld(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2);

  /** Shift xmm2 right by imm8 bytes while shifting in 0s. */
  void vpsrldq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift ymm1 right by imm8 bytes while shifting in 0s. */
  void vpsrldq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift quadwords in xmm2 right by imm8 while shifting in 0s. */
  void vpsrlq(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift quadwords in xmm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrlq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift quadwords in xmm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrlq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift quadwords in ymm2 right by imm8 while shifting in 0s. */
  void vpsrlq(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift quadwords in ymm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrlq(const Ymm& arg0, const Ymm& arg1, const M128& arg2);

  /** Shift quadwords in ymm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrlq(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2);

  /** Shift bits in doublewords in xmm2 right by amount specified in the corresponding element of xmm3/m128 while shifting in 0s. */
  void vpsrlvd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift bits in doublewords in xmm2 right by amount specified in the corresponding element of xmm3/m128 while shifting in 0s. */
  void vpsrlvd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift bits in doublewords in ymm2 right by amount specified in the corresponding element of ymm3/m256 while shifting in 0s. */
  void vpsrlvd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Shift bits in doublewords in ymm2 right by amount specified in the corresponding element of ymm3/m256 while shifting in 0s. */
  void vpsrlvd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Shift bits in quadwords in xmm2 right by amount specified in the corresponding element of xmm3/m128 while shifting in 0s. */
  void vpsrlvq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift bits in quadwords in xmm2 right by amount specified in the corresponding element of xmm3/m128 while shifting in 0s. */
  void vpsrlvq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift bits in quadwords in ymm2 right by amount specified in the corresponding element of ymm3/m256 while shifting in 0s. */
  void vpsrlvq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Shift bits in quadwords in ymm2 right by amount specified in the corresponding element of ymm3/m256 while shifting in 0s. */
  void vpsrlvq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Shift words in xmm2 right by imm8 while shifting in 0s. */
  void vpsrlw(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Shift words in xmm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrlw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Shift words in xmm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrlw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shift words in ymm2 right by imm8 while shifting in 0s. */
  void vpsrlw(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Shift words in ymm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrlw(const Ymm& arg0, const Ymm& arg1, const M128& arg2);

  /** Shift words in ymm2 right by amount specified in xmm3/m128 while shifting in 0s. */
  void vpsrlw(const Ymm& arg0, const Ymm& arg1, const Xmm& arg2);

  /** Subtract packed byte integers in xmm3/m128 from xmm2. */
  void vpsubb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed byte integers in xmm3/m128 from xmm2. */
  void vpsubb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed byte integers in ymm3/m256 from ymm2. */
  void vpsubb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed byte integers in ymm3/m256 from ymm2. */
  void vpsubb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract packed doubleword integers in xmm3/m128 from xmm2. */
  void vpsubd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed doubleword integers in xmm3/m128 from xmm2. */
  void vpsubd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed doubleword integers in ymm3/m256 from ymm2. */
  void vpsubd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed doubleword integers in ymm3/m256 from ymm2. */
  void vpsubd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract packed quadword integers in xmm3/m128 from xmm2. */
  void vpsubq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed quadword integers in xmm3/m128 from xmm2. */
  void vpsubq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed quadword integers in ymm3/m256 from ymm2. */
  void vpsubq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed quadword integers in ymm3/m256 from ymm2. */
  void vpsubq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract packed signed byte integers in xmm3/m128 from packed signed byte integers in xmm2 and saturate results. */
  void vpsubsb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed signed byte integers in xmm3/m128 from packed signed byte integers in xmm2 and saturate results. */
  void vpsubsb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed signed byte integers in ymm3/m256 from packed signed byte integers in ymm2 and saturate results. */
  void vpsubsb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed signed byte integers in ymm3/m256 from packed signed byte integers in ymm2 and saturate results. */
  void vpsubsb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract packed signed word integers in xmm3/m128 from packed signed word integers in xmm2 and saturate results. */
  void vpsubsw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed signed word integers in xmm3/m128 from packed signed word integers in xmm2 and saturate results. */
  void vpsubsw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed signed word integers in ymm3/m256 from packed signed word integers in ymm2 and saturate results. */
  void vpsubsw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed signed word integers in ymm3/m256 from packed signed word integers in ymm2 and saturate results. */
  void vpsubsw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract packed unsigned byte integers in xmm3/m128 from packed unsigned byte integers in xmm2 and saturate result. */
  void vpsubusb(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed unsigned byte integers in xmm3/m128 from packed unsigned byte integers in xmm2 and saturate result. */
  void vpsubusb(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed unsigned byte integers in ymm3/m256 from packed unsigned byte integers in ymm2 and saturate result. */
  void vpsubusb(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed unsigned byte integers in ymm3/m256 from packed unsigned byte integers in ymm2 and saturate result. */
  void vpsubusb(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract packed unsigned word integers in xmm3/m128 from packed unsigned word integers in xmm2 and saturate result. */
  void vpsubusw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed unsigned word integers in xmm3/m128 from packed unsigned word integers in xmm2 and saturate result. */
  void vpsubusw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed unsigned word integers in ymm3/m256 from packed unsigned word integers in ymm2 and saturate result. */
  void vpsubusw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed unsigned word integers in ymm3/m256 from packed unsigned word integers in ymm2 and saturate result. */
  void vpsubusw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract packed word integers in xmm3/m128 from xmm2. */
  void vpsubw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed word integers in xmm3/m128 from xmm2. */
  void vpsubw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed word integers in ymm3/m256 from ymm2. */
  void vpsubw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed word integers in ymm3/m256 from ymm2. */
  void vpsubw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Set ZF and CF depending on bitwise AND and ANDN of sources. */
  void vptest(const Xmm& arg0, const M128& arg1);

  /** Set ZF and CF depending on bitwise AND and ANDN of sources. */
  void vptest(const Xmm& arg0, const Xmm& arg1);

  /** Set ZF and CF depending on bitwise AND and ANDN of sources. */
  void vptest(const Ymm& arg0, const M256& arg1);

  /** Set ZF and CF depending on bitwise AND and ANDN of sources. */
  void vptest(const Ymm& arg0, const Ymm& arg1);

  /** Interleave high-order bytes from xmm2 and xmm3/m128 into xmm1. */
  void vpunpckhbw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Interleave high-order bytes from xmm2 and xmm3/m128 into xmm1. */
  void vpunpckhbw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Interleave high-order bytes from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckhbw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Interleave high-order bytes from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckhbw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Interleave high-order doublewords from xmm2 and xmm3/m128 into xmm1. */
  void vpunpckhdq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Interleave high-order doublewords from xmm2 and xmm3/m128 into xmm1. */
  void vpunpckhdq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Interleave high-order doublewords from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckhdq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Interleave high-order doublewords from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckhdq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Interleave high-order quadword from xmm2 and xmm3/m128 into xmm1 register. */
  void vpunpckhqdq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Interleave high-order quadword from xmm2 and xmm3/m128 into xmm1 register. */
  void vpunpckhqdq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Interleave high-order quadword from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckhqdq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Interleave high-order quadword from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckhqdq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Interleave high-order words from xmm2 and xmm3/m128 into xmm1. */
  void vpunpckhwd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Interleave high-order words from xmm2 and xmm3/m128 into xmm1. */
  void vpunpckhwd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Interleave high-order words from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckhwd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Interleave high-order words from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckhwd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Interleave low-order bytes from xmm2 and xmm3/m128 into xmm1. */
  void vpunpcklbw(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Interleave low-order bytes from xmm2 and xmm3/m128 into xmm1. */
  void vpunpcklbw(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Interleave low-order bytes from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpcklbw(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Interleave low-order bytes from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpcklbw(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Interleave low-order doublewords from xmm2 and xmm3/m128 into xmm1. */
  void vpunpckldq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Interleave low-order doublewords from xmm2 and xmm3/m128 into xmm1. */
  void vpunpckldq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Interleave low-order doublewords from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckldq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Interleave low-order doublewords from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpckldq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Interleave low-order quadword from xmm2 and xmm3/m128 into xmm1 register. */
  void vpunpcklqdq(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Interleave low-order quadword from xmm2 and xmm3/m128 into xmm1 register. */
  void vpunpcklqdq(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Interleave low-order quadword from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpcklqdq(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Interleave low-order quadword from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpcklqdq(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Interleave low-order words from xmm2 and xmm3/m128 into xmm1. */
  void vpunpcklwd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Interleave low-order words from xmm2 and xmm3/m128 into xmm1. */
  void vpunpcklwd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Interleave low-order words from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpcklwd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Interleave low-order words from ymm2 and ymm3/m256 into ymm1 register. */
  void vpunpcklwd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Bitwise XOR of xmm3/m128 and xmm2. */
  void vpxor(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Bitwise XOR of xmm3/m128 and xmm2. */
  void vpxor(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Bitwise XOR of ymm3/m256 and ymm2. */
  void vpxor(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Bitwise XOR of ymm3/m256 and ymm2. */
  void vpxor(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Computes the approximate reciprocals of packed single-precision values in xmm2/mem and stores the results in xmm1. */
  void vrcpps(const Xmm& arg0, const M128& arg1);

  /** Computes the approximate reciprocals of packed single-precision values in xmm2/mem and stores the results in xmm1. */
  void vrcpps(const Xmm& arg0, const Xmm& arg1);

  /** Computes the approximate reciprocals of packed single-precision values in ymm2/mem and stores the results in ymm1. */
  void vrcpps(const Ymm& arg0, const M256& arg1);

  /** Computes the approximate reciprocals of packed single-precision values in ymm2/mem and stores the results in ymm1. */
  void vrcpps(const Ymm& arg0, const Ymm& arg1);

  /** Computes the approximate reciprocal of the scalar single-precision floating-point value in xmm3/m32 and stores the result in xmm1. Also, upper single precision floating-point values (bits[127:32]) from xmm2 are copied to xmm1[127:32]. */
  void vrcpss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Computes the approximate reciprocal of the scalar single-precision floating-point value in xmm3/m32 and stores the result in xmm1. Also, upper single precision floating-point values (bits[127:32]) from xmm2 are copied to xmm1[127:32]. */
  void vrcpss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Round packed double-precision floating-point values in xmm2/m128 and place the result in xmm1. The rounding mode is determined by imm8. */
  void vroundpd(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Round packed double-precision floating-point values in xmm2/m128 and place the result in xmm1. The rounding mode is determined by imm8. */
  void vroundpd(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Round packed double-precision floating-point values in ymm2/m256 and place the result in ymm1. The rounding mode is determined by imm8. */
  void vroundpd(const Ymm& arg0, const M256& arg1, const Imm8& arg2);

  /** Round packed double-precision floating-point values in ymm2/m256 and place the result in ymm1. The rounding mode is determined by imm8. */
  void vroundpd(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Round packed single-precision floating-point values in xmm2/m128 and place the result in xmm1. The rounding mode is determined by imm8. */
  void vroundps(const Xmm& arg0, const M128& arg1, const Imm8& arg2);

  /** Round packed single-precision floating-point values in xmm2/m128 and place the result in xmm1. The rounding mode is determined by imm8. */
  void vroundps(const Xmm& arg0, const Xmm& arg1, const Imm8& arg2);

  /** Round packed single-precision floating-point values in ymm2/m256 and place the result in ymm1. The rounding mode is determined by imm8. */
  void vroundps(const Ymm& arg0, const M256& arg1, const Imm8& arg2);

  /** Round packed single-precision floating-point values in ymm2/m256 and place the result in ymm1. The rounding mode is determined by imm8. */
  void vroundps(const Ymm& arg0, const Ymm& arg1, const Imm8& arg2);

  /** Round the low packed double precision floating-point value in xmm3/m64 and place the result in xmm1. The rounding mode is determined by imm8. Upper packed double precision floating-point value (bits[127:64]) from xmm2 is copied to xmm1[127:64]. */
  void vroundsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2, const Imm8& arg3);

  /** Round the low packed double precision floating-point value in xmm3/m64 and place the result in xmm1. The rounding mode is determined by imm8. Upper packed double precision floating-point value (bits[127:64]) from xmm2 is copied to xmm1[127:64]. */
  void vroundsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Round the low packed single precision floating-point value in xmm3/m32 and place the result in xmm1. The rounding mode is determined by imm8. Also, upper packed single precision floating-point values (bits[127:32]) from xmm2 are copied to xmm1[127:32]. */
  void vroundss(const Xmm& arg0, const Xmm& arg1, const M32& arg2, const Imm8& arg3);

  /** Round the low packed single precision floating-point value in xmm3/m32 and place the result in xmm1. The rounding mode is determined by imm8. Also, upper packed single precision floating-point values (bits[127:32]) from xmm2 are copied to xmm1[127:32]. */
  void vroundss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Computes the approximate reciprocals of the square roots of packed single-precision values in xmm2/mem and stores the results in xmm1. */
  void vrsqrtps(const Xmm& arg0, const M128& arg1);

  /** Computes the approximate reciprocals of the square roots of packed single-precision values in xmm2/mem and stores the results in xmm1. */
  void vrsqrtps(const Xmm& arg0, const Xmm& arg1);

  /** Computes the approximate reciprocals of the square roots of packed single-precision values in ymm2/mem and stores the results in ymm1. */
  void vrsqrtps(const Ymm& arg0, const M256& arg1);

  /** Computes the approximate reciprocals of the square roots of packed single-precision values in ymm2/mem and stores the results in ymm1. */
  void vrsqrtps(const Ymm& arg0, const Ymm& arg1);

  /** Computes the approximate reciprocal of the square root of the low single precision floating-point value in xmm3/m32 and stores the results in xmm1. Also, upper single precision floating-point values (bits[127:32]) from xmm2 are copied to xmm1[127:32]. */
  void vrsqrtss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Computes the approximate reciprocal of the square root of the low single precision floating-point value in xmm3/m32 and stores the results in xmm1. Also, upper single precision floating-point values (bits[127:32]) from xmm2 are copied to xmm1[127:32]. */
  void vrsqrtss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Shuffle Packed double-precision floating- point values selected by imm8 from xmm2 and xmm3/mem. */
  void vshufpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Shuffle Packed double-precision floating- point values selected by imm8 from xmm2 and xmm3/mem. */
  void vshufpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Shuffle Packed double-precision floating- point values selected by imm8 from ymm2 and ymm3/mem. */
  void vshufpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Shuffle Packed double-precision floating- point values selected by imm8 from ymm2 and ymm3/mem. */
  void vshufpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Shuffle Packed single-precision floating-point values selected by imm8 from xmm2 and xmm3/mem. */
  void vshufps(const Xmm& arg0, const Xmm& arg1, const M128& arg2, const Imm8& arg3);

  /** Shuffle Packed single-precision floating-point values selected by imm8 from xmm2 and xmm3/mem. */
  void vshufps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2, const Imm8& arg3);

  /** Shuffle Packed single-precision floating-point values selected by imm8 from ymm2 and ymm3/mem. */
  void vshufps(const Ymm& arg0, const Ymm& arg1, const M256& arg2, const Imm8& arg3);

  /** Shuffle Packed single-precision floating-point values selected by imm8 from ymm2 and ymm3/mem. */
  void vshufps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2, const Imm8& arg3);

  /** Computes Square Roots of the packed double- precision floating-point values in xmm2/m128 and stores the result in xmm1. */
  void vsqrtpd(const Xmm& arg0, const M128& arg1);

  /** Computes Square Roots of the packed double- precision floating-point values in xmm2/m128 and stores the result in xmm1. */
  void vsqrtpd(const Xmm& arg0, const Xmm& arg1);

  /** Computes Square Roots of the packed double- precision floating-point values in ymm2/m256 and stores the result in ymm1 */
  void vsqrtpd(const Ymm& arg0, const M256& arg1);

  /** Computes Square Roots of the packed double- precision floating-point values in ymm2/m256 and stores the result in ymm1 */
  void vsqrtpd(const Ymm& arg0, const Ymm& arg1);

  /** Computes Square Roots of the packed single- precision floating-point values in xmm2/m128 and stores the result in xmm1. */
  void vsqrtps(const Xmm& arg0, const M128& arg1);

  /** Computes Square Roots of the packed single- precision floating-point values in xmm2/m128 and stores the result in xmm1. */
  void vsqrtps(const Xmm& arg0, const Xmm& arg1);

  /** Computes Square Roots of the packed single- precision floating-point values in ymm2/m256 and stores the result in ymm1. */
  void vsqrtps(const Ymm& arg0, const M256& arg1);

  /** Computes Square Roots of the packed single- precision floating-point values in ymm2/m256 and stores the result in ymm1. */
  void vsqrtps(const Ymm& arg0, const Ymm& arg1);

  /** Computes square root of the low double- precision floating point value in xmm3/m64 and stores the results in xmm2. Also, upper double precision floating-point value (bits[127:64]) from xmm2 is copied to xmm1[127:64]. */
  void vsqrtsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Computes square root of the low double- precision floating point value in xmm3/m64 and stores the results in xmm2. Also, upper double precision floating-point value (bits[127:64]) from xmm2 is copied to xmm1[127:64]. */
  void vsqrtsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Computes square root of the low single- precision floating-point value in xmm3/m32 and stores the results in xmm1. Also, upper single precision floating-point values (bits[127:32]) from xmm2 are copied to xmm1[127:32]. */
  void vsqrtss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Computes square root of the low single- precision floating-point value in xmm3/m32 and stores the results in xmm1. Also, upper single precision floating-point values (bits[127:32]) from xmm2 are copied to xmm1[127:32]. */
  void vsqrtss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Store contents of MXCSR register to m32. */
  void vstmxcsr(const M32& arg0);

  /** Subtract packed double-precision floating- point values in xmm3/mem from xmm2 and stores result in xmm1. */
  void vsubpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed double-precision floating- point values in xmm3/mem from xmm2 and stores result in xmm1. */
  void vsubpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed double-precision floating- point values in ymm3/mem from ymm2 and stores result in ymm1. */
  void vsubpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed double-precision floating- point values in ymm3/mem from ymm2 and stores result in ymm1. */
  void vsubpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract packed single-precision floating-point values in xmm3/mem from xmm2 and stores result in xmm1. */
  void vsubps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Subtract packed single-precision floating-point values in xmm3/mem from xmm2 and stores result in xmm1. */
  void vsubps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract packed single-precision floating-point values in ymm3/mem from ymm2 and stores result in ymm1. */
  void vsubps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Subtract packed single-precision floating-point values in ymm3/mem from ymm2 and stores result in ymm1. */
  void vsubps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Subtract the low double-precision floating- point value in xmm3/mem from xmm2 and store the result in xmm1. */
  void vsubsd(const Xmm& arg0, const Xmm& arg1, const M64& arg2);

  /** Subtract the low double-precision floating- point value in xmm3/mem from xmm2 and store the result in xmm1. */
  void vsubsd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Subtract the low single-precision floating- point value in xmm3/mem from xmm2 and store the result in xmm1. */
  void vsubss(const Xmm& arg0, const Xmm& arg1, const M32& arg2);

  /** Subtract the low single-precision floating- point value in xmm3/mem from xmm2 and store the result in xmm1. */
  void vsubss(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Set ZF and CF depending on sign bit AND and ANDN of packed double-precision floating- point sources. */
  void vtestpd(const Xmm& arg0, const M128& arg1);

  /** Set ZF and CF depending on sign bit AND and ANDN of packed double-precision floating- point sources. */
  void vtestpd(const Xmm& arg0, const Xmm& arg1);

  /** Set ZF and CF depending on sign bit AND and ANDN of packed double-precision floating- point sources. */
  void vtestpd(const Ymm& arg0, const M256& arg1);

  /** Set ZF and CF depending on sign bit AND and ANDN of packed double-precision floating- point sources. */
  void vtestpd(const Ymm& arg0, const Ymm& arg1);

  /** Set ZF and CF depending on sign bit AND and ANDN of packed single-precision floating- point sources. */
  void vtestps(const Xmm& arg0, const M128& arg1);

  /** Set ZF and CF depending on sign bit AND and ANDN of packed single-precision floating- point sources. */
  void vtestps(const Xmm& arg0, const Xmm& arg1);

  /** Set ZF and CF depending on sign bit AND and ANDN of packed single-precision floating- point sources. */
  void vtestps(const Ymm& arg0, const M256& arg1);

  /** Set ZF and CF depending on sign bit AND and ANDN of packed single-precision floating- point sources. */
  void vtestps(const Ymm& arg0, const Ymm& arg1);

  /** Compare low double precision floating-point values in xmm1 and xmm2/mem64 and set the EFLAGS flags accordingly. */
  void vucomisd(const Xmm& arg0, const M64& arg1);

  /** Compare low double precision floating-point values in xmm1 and xmm2/mem64 and set the EFLAGS flags accordingly. */
  void vucomisd(const Xmm& arg0, const Xmm& arg1);

  /** Compare low single precision floating-point values in xmm1 and xmm2/mem32 and set the EFLAGS flags accordingly. */
  void vucomiss(const Xmm& arg0, const M32& arg1);

  /** Compare low single precision floating-point values in xmm1 and xmm2/mem32 and set the EFLAGS flags accordingly. */
  void vucomiss(const Xmm& arg0, const Xmm& arg1);

  /** Unpacks and Interleaves double precision floating-point values from high quadwords of xmm2 and xmm3/m128. */
  void vunpckhpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Unpacks and Interleaves double precision floating-point values from high quadwords of xmm2 and xmm3/m128. */
  void vunpckhpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Unpacks and Interleaves double precision floating-point values from high quadwords of ymm2 and ymm3/m256. */
  void vunpckhpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Unpacks and Interleaves double precision floating-point values from high quadwords of ymm2 and ymm3/m256. */
  void vunpckhpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Unpacks and Interleaves single-precision floating-point values from high quadwords of xmm2 and xmm3/m128. */
  void vunpckhps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Unpacks and Interleaves single-precision floating-point values from high quadwords of xmm2 and xmm3/m128. */
  void vunpckhps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Unpacks and Interleaves single-precision floating-point values from high quadwords of ymm2 and ymm3/m256. */
  void vunpckhps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Unpacks and Interleaves single-precision floating-point values from high quadwords of ymm2 and ymm3/m256. */
  void vunpckhps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Unpacks and Interleaves double precision floating-point values low high quadwords of xmm2 and xmm3/m128. */
  void vunpcklpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Unpacks and Interleaves double precision floating-point values low high quadwords of xmm2 and xmm3/m128. */
  void vunpcklpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Unpacks and Interleaves double precision floating-point values low high quadwords of ymm2 and ymm3/m256. */
  void vunpcklpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Unpacks and Interleaves double precision floating-point values low high quadwords of ymm2 and ymm3/m256. */
  void vunpcklpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Unpacks and Interleaves single-precision floating-point values from low quadwords of xmm2 and xmm3/m128. */
  void vunpcklps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Unpacks and Interleaves single-precision floating-point values from low quadwords of xmm2 and xmm3/m128. */
  void vunpcklps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Unpacks and Interleaves single-precision floating-point values from low quadwords of ymm2 and ymm3/m256. */
  void vunpcklps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Unpacks and Interleaves single-precision floating-point values from low quadwords of ymm2 and ymm3/m256. */
  void vunpcklps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the bitwise logical XOR of packed double-precision floating-point values in xmm2 and xmm3/mem. */
  void vxorpd(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the bitwise logical XOR of packed double-precision floating-point values in xmm2 and xmm3/mem. */
  void vxorpd(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the bitwise logical XOR of packed double-precision floating-point values in ymm2 and ymm3/mem. */
  void vxorpd(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the bitwise logical XOR of packed double-precision floating-point values in ymm2 and ymm3/mem. */
  void vxorpd(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Return the bitwise logical XOR of packed single-precision floating-point values in xmm2 and xmm3/mem. */
  void vxorps(const Xmm& arg0, const Xmm& arg1, const M128& arg2);

  /** Return the bitwise logical XOR of packed single-precision floating-point values in xmm2 and xmm3/mem. */
  void vxorps(const Xmm& arg0, const Xmm& arg1, const Xmm& arg2);

  /** Return the bitwise logical XOR of packed single-precision floating-point values in ymm2 and ymm3/mem. */
  void vxorps(const Ymm& arg0, const Ymm& arg1, const M256& arg2);

  /** Return the bitwise logical XOR of packed single-precision floating-point values in ymm2 and ymm3/mem. */
  void vxorps(const Ymm& arg0, const Ymm& arg1, const Ymm& arg2);

  /** Zero all YMM registers. */
  void vzeroall();

  /** Zero upper 128 bits of all YMM registers. */
  void vzeroupper();

  /** Check pending unmasked floating-point exceptions. */
  void wait();

  /** Load the FS base address with the 32-bit value in the source register. */
  void wrfsbase(const R32& arg0);

  /** Load the FS base address with the 64-bit value in the source register. */
  void wrfsbase(const R64& arg0);

  /** Load the GS base address with the 32-bit value in the source register. */
  void wrgsbase(const R32& arg0);

  /** Load the GS base address with the 64-bit value in the source register. */
  void wrgsbase(const R64& arg0);

  /** Causes an RTM abort if in RTM execution */
  void xabort(const Imm8& arg0);

  /** A hint used with an "XACQUIRE-enabled" instruction to start lock elision on the instruction memory operand address. */
  void xacquire();

  /** Exchange r16 and r/m16; load sum into r/m16. */
  void xadd(const M16& arg0, const R16& arg1);

  /** Exchange r32 and r/m32; load sum into r/m32. */
  void xadd(const M32& arg0, const R32& arg1);

  /** Exchange r64 and r/m64; load sum into r/m64. */
  void xadd(const M64& arg0, const R64& arg1);

  /** Exchange r8 and r/m8; load sum into r/m8. */
  void xadd(const M8& arg0, const R8& arg1);

  /** Exchange r8 and r/m8; load sum into r/m8. */
  void xadd(const M8& arg0, const Rh& arg1);

  /** Exchange r16 and r/m16; load sum into r/m16. */
  void xadd(const R16& arg0, const R16& arg1);

  /** Exchange r32 and r/m32; load sum into r/m32. */
  void xadd(const R32& arg0, const R32& arg1);

  /** Exchange r64 and r/m64; load sum into r/m64. */
  void xadd(const R64& arg0, const R64& arg1);

  /** Exchange r8 and r/m8; load sum into r/m8. */
  void xadd(const R8& arg0, const R8& arg1);

  /** Exchange r8 and r/m8; load sum into r/m8. */
  void xadd(const R8& arg0, const Rh& arg1);

  /** Exchange r8 and r/m8; load sum into r/m8. */
  void xadd(const Rh& arg0, const R8& arg1);

  /** Exchange r8 and r/m8; load sum into r/m8. */
  void xadd(const Rh& arg0, const Rh& arg1);

  /** Specifies the start of an RTM region. Provides a 32-bit relative offset to compute the address of the fallback instruction address at which execution resumes following an RTM abort. */
  void xbegin(Label& arg0);

  /** Specifies the start of an RTM region. Provides a 32-bit relative offset to compute the address of the fallback instruction address at which execution resumes following an RTM abort. */
  void xbegin(const Rel32& arg0);

  /** Exchange r16 with AX. */
  void xchg(const Ax& arg0, const R16& arg1);

  /** Exchange r32 with EAX. */
  void xchg(const Eax& arg0, const R32& arg1);

  /** Exchange r16 with word from r/m16. */
  void xchg(const M16& arg0, const R16& arg1);

  /** Exchange r32 with doubleword from r/m32. */
  void xchg(const M32& arg0, const R32& arg1);

  /** Exchange r64 with quadword from r/m64. */
  void xchg(const M64& arg0, const R64& arg1);

  /** Exchange r8 (byte register) with byte from r/m8. */
  void xchg(const M8& arg0, const R8& arg1);

  /** Exchange r8 (byte register) with byte from r/m8. */
  void xchg(const M8& arg0, const Rh& arg1);

  /** Exchange AX with r16. */
  void xchg(const R16& arg0, const Ax& arg1);

  /** Exchange word from r/m16 with r16. */
  void xchg(const R16& arg0, const M16& arg1);

  /** Exchange r16 with word from r/m16. */
  void xchg(const R16& arg0, const R16& arg1);

  /** Exchange word from r/m16 with r16. */
  void xchg_1(const R16& arg0, const R16& arg1);

  /** Exchange EAX with r32. */
  void xchg(const R32& arg0, const Eax& arg1);

  /** Exchange doubleword from r/m32 with r32. */
  void xchg(const R32& arg0, const M32& arg1);

  /** Exchange r32 with doubleword from r/m32. */
  void xchg(const R32& arg0, const R32& arg1);

  /** Exchange doubleword from r/m32 with r32. */
  void xchg_1(const R32& arg0, const R32& arg1);

  /** Exchange quadword from r/m64 with r64. */
  void xchg(const R64& arg0, const M64& arg1);

  /** Exchange r64 with quadword from r/m64. */
  void xchg(const R64& arg0, const R64& arg1);

  /** Exchange quadword from r/m64 with r64. */
  void xchg_1(const R64& arg0, const R64& arg1);

  /** Exchange RAX with r64. */
  void xchg(const R64& arg0, const Rax& arg1);

  /** Exchange byte from r/m8 with r8 (byte register). */
  void xchg(const R8& arg0, const M8& arg1);

  /** Exchange r8 (byte register) with byte from r/m8. */
  void xchg(const R8& arg0, const R8& arg1);

  /** Exchange byte from r/m8 with r8 (byte register). */
  void xchg_1(const R8& arg0, const R8& arg1);

  /** Exchange r8 (byte register) with byte from r/m8. */
  void xchg(const R8& arg0, const Rh& arg1);

  /** Exchange byte from r/m8 with r8 (byte register). */
  void xchg_1(const R8& arg0, const Rh& arg1);

  /** Exchange r64 with RAX. */
  void xchg(const Rax& arg0, const R64& arg1);

  /** Exchange byte from r/m8 with r8 (byte register). */
  void xchg(const Rh& arg0, const M8& arg1);

  /** Exchange r8 (byte register) with byte from r/m8. */
  void xchg(const Rh& arg0, const R8& arg1);

  /** Exchange byte from r/m8 with r8 (byte register). */
  void xchg_1(const Rh& arg0, const R8& arg1);

  /** Exchange r8 (byte register) with byte from r/m8. */
  void xchg(const Rh& arg0, const Rh& arg1);

  /** Exchange byte from r/m8 with r8 (byte register). */
  void xchg_1(const Rh& arg0, const Rh& arg1);

  /** Specifies the end of an RTM code region. */
  void xend();

  /** Reads an XCR specified by ECX into EDX:EAX. */
  void xgetbv();

  /** Set AL to memory byte DS:[(E)BX + unsigned AL]. */
  void xlat(const M8& arg0);

  /** Set AL to memory byte DS:[(E)BX + unsigned AL]. */
  void xlatb();

  /** Set AL to memory byte [RBX + unsigned AL]. */
  void xlatb_1();

  /** AL XOR imm8. */
  void xor_(const Al& arg0, const Imm8& arg1);

  /** AX XOR imm16. */
  void xor_(const Ax& arg0, const Imm16& arg1);

  /** EAX XOR imm32. */
  void xor_(const Eax& arg0, const Imm32& arg1);

  /** r/m16 XOR imm16. */
  void xor_(const M16& arg0, const Imm16& arg1);

  /** r/m16 XOR imm8 (sign-extended). */
  void xor_(const M16& arg0, const Imm8& arg1);

  /** r/m16 XOR r16. */
  void xor_(const M16& arg0, const R16& arg1);

  /** r/m32 XOR imm32. */
  void xor_(const M32& arg0, const Imm32& arg1);

  /** r/m32 XOR imm8 (sign-extended). */
  void xor_(const M32& arg0, const Imm8& arg1);

  /** r/m32 XOR r32. */
  void xor_(const M32& arg0, const R32& arg1);

  /** r/m64 XOR imm32 (sign-extended). */
  void xor_(const M64& arg0, const Imm32& arg1);

  /** r/m64 XOR imm8 (sign-extended). */
  void xor_(const M64& arg0, const Imm8& arg1);

  /** r/m64 XOR r64. */
  void xor_(const M64& arg0, const R64& arg1);

  /** r/m8 XOR imm8. */
  void xor_(const M8& arg0, const Imm8& arg1);

  /** r/m8 XOR r8. */
  void xor_(const M8& arg0, const R8& arg1);

  /** r/m8 XOR r8. */
  void xor_(const M8& arg0, const Rh& arg1);

  /** r/m16 XOR imm16. */
  void xor_(const R16& arg0, const Imm16& arg1);

  /** r/m16 XOR imm8 (sign-extended). */
  void xor_(const R16& arg0, const Imm8& arg1);

  /** r16 XOR r/m16. */
  void xor_(const R16& arg0, const M16& arg1);

  /** r/m16 XOR r16. */
  void xor_(const R16& arg0, const R16& arg1);

  /** r16 XOR r/m16. */
  void xor__1(const R16& arg0, const R16& arg1);

  /** r/m32 XOR imm32. */
  void xor_(const R32& arg0, const Imm32& arg1);

  /** r/m32 XOR imm8 (sign-extended). */
  void xor_(const R32& arg0, const Imm8& arg1);

  /** r32 XOR r/m32. */
  void xor_(const R32& arg0, const M32& arg1);

  /** r/m32 XOR r32. */
  void xor_(const R32& arg0, const R32& arg1);

  /** r32 XOR r/m32. */
  void xor__1(const R32& arg0, const R32& arg1);

  /** r/m64 XOR imm32 (sign-extended). */
  void xor_(const R64& arg0, const Imm32& arg1);

  /** r/m64 XOR imm8 (sign-extended). */
  void xor_(const R64& arg0, const Imm8& arg1);

  /** r64 XOR r/m64. */
  void xor_(const R64& arg0, const M64& arg1);

  /** r/m64 XOR r64. */
  void xor_(const R64& arg0, const R64& arg1);

  /** r64 XOR r/m64. */
  void xor__1(const R64& arg0, const R64& arg1);

  /** r/m8 XOR imm8. */
  void xor_(const R8& arg0, const Imm8& arg1);

  /** r8 XOR r/m8. */
  void xor_(const R8& arg0, const M8& arg1);

  /** r/m8 XOR r8. */
  void xor_(const R8& arg0, const R8& arg1);

  /** r8 XOR r/m8. */
  void xor__1(const R8& arg0, const R8& arg1);

  /** r/m8 XOR r8. */
  void xor_(const R8& arg0, const Rh& arg1);

  /** r8 XOR r/m8. */
  void xor__1(const R8& arg0, const Rh& arg1);

  /** RAX XOR imm32 (sign-extended). */
  void xor_(const Rax& arg0, const Imm32& arg1);

  /** r/m8 XOR imm8. */
  void xor_(const Rh& arg0, const Imm8& arg1);

  /** r8 XOR r/m8. */
  void xor_(const Rh& arg0, const M8& arg1);

  /** r/m8 XOR r8. */
  void xor_(const Rh& arg0, const R8& arg1);

  /** r8 XOR r/m8. */
  void xor__1(const Rh& arg0, const R8& arg1);

  /** r/m8 XOR r8. */
  void xor_(const Rh& arg0, const Rh& arg1);

  /** r8 XOR r/m8. */
  void xor__1(const Rh& arg0, const Rh& arg1);

  /** Bitwise exclusive-OR of xmm2/m128 and xmm1. */
  void xorpd(const Xmm& arg0, const M128& arg1);

  /** Bitwise exclusive-OR of xmm2/m128 and xmm1. */
  void xorpd(const Xmm& arg0, const Xmm& arg1);

  /** Bitwise exclusive-OR of xmm2/m128 and xmm1. */
  void xorps(const Xmm& arg0, const M128& arg1);

  /** Bitwise exclusive-OR of xmm2/m128 and xmm1. */
  void xorps(const Xmm& arg0, const Xmm& arg1);

  /** A hint used with an "XRELEASE-enabled" instruction to end lock elision on the instruction memory operand address. */
  void xrelease();

  /** Restore processor extended states from memory. The states are specified by EDX:EAX */
  void xrstor(const M16& arg0);

  /** Restore processor extended states from memory. The states are specified by EDX:EAX */
  void xrstor(const M32& arg0);

  /** Restore processor extended states from memory. The states are specified by EDX:EAX */
  void xrstor(const M64& arg0);

  /** Restore processor extended states from memory. The states are specified by EDX:EAX */
  void xrstor64(const M16& arg0);

  /** Restore processor extended states from memory. The states are specified by EDX:EAX */
  void xrstor64(const M32& arg0);

  /** Restore processor extended states from memory. The states are specified by EDX:EAX */
  void xrstor64(const M64& arg0);

  /** Save processor extended states to memory. The states are specified by EDX:EAX */
  void xsave(const M16& arg0);

  /** Save processor extended states to memory. The states are specified by EDX:EAX */
  void xsave(const M32& arg0);

  /** Save processor extended states to memory. The states are specified by EDX:EAX */
  void xsave(const M64& arg0);

  /** Save processor extended states to memory. The states are specified by EDX:EAX */
  void xsave64(const M16& arg0);

  /** Save processor extended states to memory. The states are specified by EDX:EAX */
  void xsave64(const M32& arg0);

  /** Save processor extended states to memory. The states are specified by EDX:EAX */
  void xsave64(const M64& arg0);

  /** Save processor extended states specified in EDX:EAX to memory, optimizing the state save operation if possible. */
  void xsaveopt(const M16& arg0);

  /** Save processor extended states specified in EDX:EAX to memory, optimizing the state save operation if possible. */
  void xsaveopt(const M32& arg0);

  /** Save processor extended states specified in EDX:EAX to memory, optimizing the state save operation if possible. */
  void xsaveopt(const M64& arg0);

  /** Save processor extended states specified in EDX:EAX to memory, optimizing the state save operation if possible. */
  void xsaveopt64(const M16& arg0);

  /** Save processor extended states specified in EDX:EAX to memory, optimizing the state save operation if possible. */
  void xsaveopt64(const M32& arg0);

  /** Save processor extended states specified in EDX:EAX to memory, optimizing the state save operation if possible. */
  void xsaveopt64(const M64& arg0);

  /** Test if executing in a transactional region */
  void xtest();


#include "assembler_logic.h"

};


#endif // ifndef ASSEMBLER_CLASS_H