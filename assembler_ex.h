#include "assembler.h"
#include <functional>


enum Condition : uint8_t
{ O   = 0x0 // Overflow
, NO  = 0x1 // Not overflow
, B   = 0x2 // Below
, C   = 0x2 // Carry
, NAE = 0x2 // Not Above Eq
, AE  = 0x3 // Above Eq
, NB  = 0x3 // Not Below
, NC  = 0x3 // Not Carry
, E   = 0x4 // Eq
, Z   = 0x4 // Zero
, NE  = 0x5 // Not Eq
, NZ  = 0x5 // Not Zero
, BE  = 0x6 // Below Eq
, NA  = 0x6 // Not Above
, A   = 0x7 // Above
, NBE = 0x7 // Not Below Eq
, S   = 0x8 // Sign
, NS  = 0x9 // Not sign
, P   = 0xa // Parity
, PE  = 0xa // Parity Even
, NP  = 0xb // Not parity
, PO  = 0xb // Parity Odd
, L   = 0xc // Less
, NGE = 0xc // Not Greater Eq
, GE  = 0xd // Greater Eq
, NL  = 0xd // Not Less
, LE  = 0xe // Less Eq
, NG  = 0xe // Not Greater
, G   = 0xf // Greater
, NLE = 0xf // Not Less Eq
};

inline constexpr Condition operator ! (const Condition c)
{
  return (Condition) (c ^ 0x1);
}


struct AssemblerEx : public Assembler
{
private:
  inline void _j (const uint8_t code, const int v)
  {
    if (abs(v) <= 0xff) {
      opcode   (code);
      disp_imm ((Imm8) ((uint8_t) v - 1 - 1));
    } else {
      opcode   (0x0f, 0x10 + code);
      disp_imm ((Imm32) ((uint8_t) v - 2 - 4));
    }
  }
  inline void _j (const uint8_t code, Label & l)
  {
    _j (code, l.value() - size ());
  }

public:
  void j (Label & l) { _j (0xeb, l); }
  void j (const Condition c, Label & l) { _j (0x70 + c, l); }

  void rdtsc () { opcode (0x0f, 0x31); }
  void rdpmc () { opcode (0x0f, 0x33); }

  void align (unsigned multiple)
  {
    while (size() % multiple != 0) nop ();
  }

  template <unsigned S>
  void repeat (UInt<S> times, SReg<S> reg, std::function<void()> body)
  {
    mov  (reg, Imm<S> { times });
    align(16);
    printf("as.size() = %ld\n", size());
    auto lb = label ();
    auto a = size ();
    body ();
    sub (reg, Imm<S> { 1 });
    jnz  (lb);
    auto b = size ();
    printf("loop size = %ld\n", b-a);
  }




};



// ---------------------- Benchmarking

#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>


static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
  return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

struct rdpmc_ctx {
	int fd;
	struct perf_event_mmap_page *buf;
};

int rdpmc_open_attr(struct perf_event_attr *attr, struct rdpmc_ctx *ctx,
		    struct rdpmc_ctx *leader_ctx)
{
	ctx->fd = perf_event_open(attr, 0, -1,
			  leader_ctx ? leader_ctx->fd : -1, 0);
	if (ctx->fd < 0) {
		perror("perf_event_open");
		return -1;
	}
	ctx->buf = (perf_event_mmap_page*) mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ, MAP_SHARED, ctx->fd, 0);
	if (ctx->buf == MAP_FAILED) {
		close(ctx->fd);
		perror("mmap on perf fd");
		return -1;
	}
	return 0;
}

int rdpmc_open(unsigned counter, struct rdpmc_ctx *ctx)
{
	struct perf_event_attr attr = {
		.type = counter > 10 ? PERF_TYPE_RAW : PERF_TYPE_HARDWARE,
		.size = PERF_ATTR_SIZE_VER0,
		.config = counter,
		.sample_type = PERF_SAMPLE_READ,
    .inherit = 0,
    .pinned = 1,
		.exclude_kernel = 1,
		.exclude_hv = 1,
		.exclude_callchain_kernel = 1,
    .exclude_callchain_user = 1,
	};
	return rdpmc_open_attr(&attr, ctx, NULL);
}

void rdpmc_close(struct rdpmc_ctx *ctx)
{
	close(ctx->fd);
	munmap(ctx->buf, sysconf(_SC_PAGESIZE));
}


#define bench(WHAT,REPS,INIT,CODE)\
  ([](const long m){\
    using namespace Registers;\
    struct THIS : AssemblerEx {\
      size_t lb;\
      void PRE (const unsigned ix) {\
        mov   (ecx, Imm32 {ix});\
        rdpmc ();\
        push  (rax);\
        xor_  (eax, eax);\
        cpuid ();\
        INIT\
        if constexpr (REPS > 1)\
          mov (ecx, Imm32 { REPS });\
        align(16);\
        lb = size();\
      }\
      void POST (const unsigned ix) {\
        if constexpr (REPS > 1) {\
          sub (ecx, Imm32 {1});\
          KnownLabel _lb {lb};\
          jnz (_lb);\
        }\
        mov   (ecx, Imm32 {ix});\
        rdpmc ();\
        pop   (rbx);\
        sub   (rax, rbx);\
        ret   ();\
      }\
      void USER () { ([this] () { CODE })(); }\
    };\
    struct rdpmc_ctx ctx;\
    rdpmc_open(m, &ctx);\
    unsigned ix; uint64_t overhead, res;\
    THIS asO{}, asU{};\
    ix = ctx.buf->index - 1;\
    asO.PRE(ix);\
    asO.POST(ix);\
    overhead = asO.fun<uint64_t()>()();\
    ix = ctx.buf->index - 1;\
    asU.PRE(ix);\
    asU.USER();\
    asU.POST(ix);\
    res = asU.fun<uint64_t()>()();\
    rdpmc_close(&ctx);\
    return (res > overhead ? res - overhead : 0);\
  })(WHAT)
