#ifndef ASM_COMMON_H
#define ASM_COMMON_H

#define ENTRY(name)     \
  .globl name;          \
  .type name, @function \
  name:

#endif /* ASM_COMMON_H */
