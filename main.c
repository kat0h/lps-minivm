#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define STACK_SIZE 1000

typedef float number;
typedef enum {
  o_end,
  o_push, // {.v = <number>}
  o_pop,
  o_add,     // +
  o_sub,     // -
  o_mul,     // *
  o_div,     // /
  o_mod,     // %
  o_eq,      // ==
  o_neq,     // !=
  o_lt,      // <
  o_leqt,    // <=
  o_gt,      // >
  o_geqt,    // >=
  o_setval,  // {.i = <int>}
  o_loadval, // {.i = <int>}
  o_print_f,
  o_print_i,
  o_print_cr,
  o_print_ascii,
  o_jmp,  // {.i = <int>}
  o_jmpr, // {.i = <int>}
  o_if,   // {.i = <int>}
  o_nif,  // {.i = <int>}
  o_ifr,  // {.i = <int>}
  o_nifr, // {.i = <int>}
} opc;

typedef union {
  opc o;
  number v;
  int i;
} p;

// prg: プログラム
// vc : 変数の数
void vm(p *prg, int vc) {
  static const void *ltbl[] = {
      &&l_end,     &&l_push,    &&l_pop,     &&l_add,      &&l_sub,
      &&l_mul,     &&l_div,     &&l_mod,     &&l_eq,       &&l_neq,
      &&l_lt,      &&l_leqt,    &&l_gt,      &&l_geqt,     &&l_setval,
      &&l_loadval, &&l_print_f, &&l_print_i, &&l_print_cr, &&l_print_ascii,
      &&l_jmp,     &&l_jmpr,    &&l_if,      &&l_nif,      &&l_ifr,
      &&l_nifr};
  int pc = 0;
  number stack[STACK_SIZE];
  number *sp = stack;
  number *env = (number *)calloc(vc, sizeof(number));
  goto *ltbl[prg[pc++].o];
l_push:
  *sp++ = prg[pc++].v;
  goto *ltbl[prg[pc++].o];
l_pop:
  sp--;
  goto *ltbl[prg[pc++].o];
l_add:
  *(sp - 2) = *(sp - 2) + *(sp - 1);
  sp--;
  goto *ltbl[prg[pc++].o];
l_sub:
  *(sp - 2) = *(sp - 2) - *(sp - 1);
  sp--;
  goto *ltbl[prg[pc++].o];
l_mul:
  *(sp - 2) = *(sp - 2) * *(sp - 1);
  sp--;
  goto *ltbl[prg[pc++].o];
l_div:
  *(sp - 2) = *(sp - 2) / *(sp - 1);
  sp--;
  goto *ltbl[prg[pc++].o];
l_mod:
  *(sp - 2) = (int)*(sp - 2) % (int)*(sp - 1);
  sp--;
  goto *ltbl[prg[pc++].o];
l_eq:
  *(sp - 2) = *(sp - 2) == *(sp - 1) ? 1.0 : 0.0;
  sp--;
  goto *ltbl[prg[pc++].o];
l_neq:
  *(sp - 2) = *(sp - 2) != *(sp - 1) ? 1.0 : 0.0;
  sp--;
  goto *ltbl[prg[pc++].o];
l_lt:
  *(sp - 2) = *(sp - 2) < *(sp - 1) ? 1.0 : 0.0;
  sp--;
  goto *ltbl[prg[pc++].o];
l_leqt:
  *(sp - 2) = *(sp - 2) <= *(sp - 1) ? 1.0 : 0.0;
  sp--;
  goto *ltbl[prg[pc++].o];
l_gt:
  *(sp - 2) = *(sp - 2) > *(sp - 1) ? 1.0 : 0.0;
  sp--;
  goto *ltbl[prg[pc++].o];
l_geqt:
  *(sp - 2) = *(sp - 2) >= *(sp - 1) ? 1.0 : 0.0;
  sp--;
  goto *ltbl[prg[pc++].o];
l_setval:
  env[prg[pc++].i] = *--sp;
  goto *ltbl[prg[pc++].o];
l_loadval:
  *sp++ = env[prg[pc++].i];
  goto *ltbl[prg[pc++].o];
l_print_f:
  printf("%f ", *--sp);
  goto *ltbl[prg[pc++].o];
l_print_i:
  printf("%d ", (int)*--sp);
  goto *ltbl[prg[pc++].o];
l_print_cr:
  puts("");
  goto *ltbl[prg[pc++].o];
l_print_ascii:
  printf("%c", (char)*--sp);
  goto *ltbl[prg[pc++].o];
l_jmp:
  pc = prg[pc].i;
  goto *ltbl[prg[pc++].o];
l_jmpr:
  pc += prg[pc].i - 1;
  goto *ltbl[prg[pc++].o];
l_if:
  if (*--sp > 0.0)
    goto *ltbl[prg[pc = prg[pc].i].o];
  else
    goto *ltbl[prg[(pc += 2) - 1].o];
l_nif:
  if (*--sp == 0.0)
    goto *ltbl[prg[pc = prg[pc].i].o];
  else
    goto *ltbl[prg[(pc += 2) - 1].o];
l_ifr:
  if (*--sp > 0.0) {
    pc += prg[pc].i - 1;
    goto *ltbl[prg[pc++].o];
  } else
    goto *ltbl[prg[(pc += 2) - 1].o];
l_nifr:
  if (*--sp == 0.0) {
    pc += prg[pc].i - 1;
    goto *ltbl[prg[pc++].o];
  } else
    goto *ltbl[prg[(pc += 2) - 1].o];
l_end:
  // check stack is empty
  if (sp != stack)
    fprintf(stderr, "Internal Error: Stack is not empty.\n");
  free(env);
}

int main() {
  // マンデルブロ集合を描画するサンプル
  p prg[] = {
      0x1,        0x42a00000, 0x13,       0x1,        0x424c0000, 0x13,
      0x1,        0x42000000, 0x13,       0x1,        0x41200000, 0x13,
      0x1,        0x40800000, 0xe,        0,          0x1,        0x447a0000,
      0xe,        0x1,        0xf,        0x1,        0x11,       0xf,
      0x1,        0x11,       0x12,       0x1,        0x437f0000, 0x11,
      0x1,        0,          0xe,        0x2,        0xf,        0x1,
      0xf,        0x2,        0xc,        0x19,       0xc5,       0xf,
      0x2,        0xf,        0,          0x5,        0xf,        0x1,
      0x6,        0xf,        0,          0x1,        0x40000000, 0x6,
      0x4,        0xe,        0x3,        0x1,        0,          0xe,
      0x4,        0xf,        0x1,        0xf,        0x4,        0xc,
      0x19,       0xa0,       0xf,        0x4,        0xf,        0,
      0x5,        0xf,        0x1,        0x6,        0xf,        0,
      0x1,        0x40000000, 0x6,        0x4,        0xe,        0x5,
      0x1,        0,          0xe,        0x6,        0x1,        0,
      0xe,        0x7,        0x1,        0,          0xe,        0x8,
      0x1,        0,          0xe,        0x9,        0x1,        0x42480000,
      0xf,        0x9,        0xc,        0x19,       0x46,       0xf,
      0x6,        0xf,        0x6,        0x5,        0xf,        0x7,
      0xf,        0x7,        0x5,        0x4,        0xf,        0x3,
      0x3,        0xe,        0xa,        0x1,        0x40000000, 0xf,
      0x6,        0x5,        0xf,        0x7,        0x5,        0xf,
      0x5,        0x3,        0xe,        0xb,        0xf,        0xa,
      0xe,        0x6,        0xf,        0xb,        0xe,        0x7,
      0xf,        0x6,        0xf,        0x6,        0x5,        0xf,
      0x7,        0xf,        0x7,        0x5,        0x3,        0x1,
      0x40800000, 0xc,        0x19,       0x8,        0x1,        0x3f800000,
      0xe,        0x8,        0x15,       0xb,        0xf,        0x9,
      0x1,        0x3f800000, 0x3,        0xe,        0x9,        0x15,
      0xffffffb7, 0xf,        0x8,        0x19,       0x1f,       0xf,
      0x9,        0x1,        0x41400000, 0x5,        0x1,        0x437f0000,
      0xc,        0x19,       0x7,        0x1,        0x437f0000, 0x11,
      0x15,       0x8,        0xf,        0x9,        0x1,        0x41400000,
      0x5,        0x11,       0x1,        0,          0x11,       0x1,
      0,          0x11,       0x15,       0xb,        0x1,        0x437f0000,
      0x11,       0x1,        0x437f0000, 0x11,       0x1,        0x437f0000,
      0x11,       0xf,        0x4,        0x1,        0x3f800000, 0x3,
      0xe,        0x4,        0x15,       0xffffff5d, 0x12,       0xf,
      0x2,        0x1,        0x3f800000, 0x3,        0xe,        0x2,
      0x15,       0xffffff38, 0,
  };
  vm(prg, 12);

  return 0;
}
