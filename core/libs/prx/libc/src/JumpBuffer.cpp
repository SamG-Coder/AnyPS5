#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>

extern "C" {
__attribute__((naked, returns_twice)) int APS5_VABI setjmp_nid_postfix(void*) {
    __asm__(
        "mov (%rsp), %rax\n"
        "mov %rax, 0(%rdi)\n"
        "mov %rbx, 8(%rdi)\n"
        "mov %rsp, 16(%rdi)\n"
        "mov %rbp, 24(%rdi)\n"
        "mov %r12, 32(%rdi)\n"
        "mov %r13, 40(%rdi)\n"
        "mov %r14, 48(%rdi)\n"
        "mov %r15, 56(%rdi)\n"
        "fnstcw 64(%rdi)\n"
        "stmxcsr 68(%rdi)\n"
        "xor %eax, %eax\n"
        "ret\n");
}

__attribute__((naked, noreturn)) void APS5_VABI longjmp_nid_postfix(void*, int) {
    __asm__(
        "mov %rdi, %rdx\n"
        "mov %esi, %eax\n"
        "test %eax, %eax\n"
        "jne 1f\n"
        "inc %eax\n"
        "1:\n"
        "stmxcsr -8(%rsp)\n"
        "mov -8(%rsp), %ecx\n"
        "and $63, %ecx\n"
        "mov 68(%rdx), %edi\n"
        "and $-64, %edi\n"
        "or %edi, %ecx\n"
        "mov %ecx, -8(%rsp)\n"
        "ldmxcsr -8(%rsp)\n"
        "fldcw 64(%rdx)\n"
        "mov 8(%rdx), %rbx\n"
        "mov 24(%rdx), %rbp\n"
        "mov 32(%rdx), %r12\n"
        "mov 40(%rdx), %r13\n"
        "mov 48(%rdx), %r14\n"
        "mov 56(%rdx), %r15\n"
        "mov 16(%rdx), %rsp\n"
        "add $8, %rsp\n"
        "jmp *0(%rdx)\n");
}
}
