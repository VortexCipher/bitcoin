dnl escape "$0x" below using the m4 quadrigaph @S|@, and escape it again with a \ for the shell.
AC_DEFUN([SECP_X86_64_ASM_CHECK],[
AC_MSG_CHECKING(for x86_64 assembly availability)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
  #include <stdint.h>]],[[
  uint64_t a = 11, tmp;
  __asm__ __volatile__("movq \@S|@0x100000000,%1; mulq %%rsi" : "+a"(a) : "S"(tmp) : "cc", "%rdx");
  ]])], [has_x86_64_asm=yes], [has_x86_64_asm=no])
AC_MSG_RESULT([$has_x86_64_asm])
