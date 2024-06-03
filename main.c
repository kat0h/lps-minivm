#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define STACK_SIZE 1000

typedef float number;
typedef enum {
  o_end,
  o_push,
  o_pop,
  o_add,
  o_sub,
  o_mul,
  o_div,
  o_setval,
  o_loadval,
  o_print,
  o_jmp,
} opc;

typedef union {
  opc o;
  number v;
  size_t i;
} p;

void vm(p *prg, size_t vc);

int main() {
  // 計算のサンプル
  // p prg[] = { o_push, {.v = 114.0}, o_setval, {.i = 0}, o_push, {.v = 514.0},
  // o_setval, {.i = 1}, o_loadval, {.i = 0}, o_loadval, {.i = 1}, o_add,
  // o_print, o_end }; vm(prg, 2); ループのサンプル
  p prg[] = {o_push,   {.v = 0},  o_setval, {.i = 0}, o_loadval, {.i = 0},
             o_print,  o_loadval, {.i = 0}, o_push,   {.v = 1},  o_add,
             o_setval, {.i = 0},  o_jmp,    {.i = 4}};
  vm(prg, 1);
  return 0;
}

// prg: プログラム
// vc : 変数の数
void vm(p *prg, size_t vc) {
  static const void *ltbl[] = {&&l_end,     &&l_push,  &&l_pop, &&l_add,
                               &&l_sub,     &&l_mul,   &&l_div, &&l_setval,
                               &&l_loadval, &&l_print, &&l_jmp};
  size_t pc = 0;
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
l_setval:
  env[prg[pc++].i] = *--sp;
  goto *ltbl[prg[pc++].o];
l_loadval:
  *sp++ = env[prg[pc++].i];
  goto *ltbl[prg[pc++].o];
l_print:
  printf("%f\n", *--sp);
  sleep(1);
  goto *ltbl[prg[pc++].o];
l_jmp:
  pc = prg[pc].i;
  goto *ltbl[prg[pc++].o];
l_end:
  free(env);
}
