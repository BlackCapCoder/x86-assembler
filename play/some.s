; GLOBAL _start
; _start:
bits 64

#define x rax
#define y rbx
#define t rdx

; add rsi, rdi
; mov rcx, rdi
; xor eax, eax
.loop
jl .loop
; xor rax, [rcx]
; add rcx, 8
; cmp rcx, rsi
; je .loop
; ret
; xor rax, [rcx]

; pxor xmm0, xmm0
; aesdec xmm0, [rcx]
; movq rbx, xmm0
; psrad xmm0, 8
; xor rax, rdi
