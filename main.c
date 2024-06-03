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
  o_eq,      // ==
  o_neq,     // !=
  o_lt,      // <
  o_leqt,    // <=
  o_gt,      // >
  o_geqt,    // >=
  o_setval,  // {.i = <size_t>}
  o_loadval, // {.i = <size_t>}
  o_print,
  o_jmp, // {.i = <size_t>}
  o_if,  // {.i = <size_t>}
  o_nif, // {.i = <size_t>}
} opc;

typedef union {
  opc o;
  number v;
  size_t i;
} p;

void vm(p *prg, size_t vc);

int main() {
  // 計算のサンプル
  // p prg[] = {o_push,       {.v = 114.0}, o_setval, {.i = 0},  o_push,
  //            {.v = 514.0}, o_setval,     {.i = 1}, o_loadval, {.i = 0},
  //            o_loadval,    {.i = 1},     o_add,    o_print,   o_end};
  // vm(prg, 2);

  // ループのサンプル
  // p prg[] = {o_push,   {.v = 0},  o_setval, {.i = 0}, o_loadval, {.i = 0},
  //            o_print,  o_loadval, {.i = 0}, o_push,   {.v = 1},  o_add,
  //            o_setval, {.i = 0},  o_jmp,    {.i = 4}};
  // vm(prg, 1);

  // 比較演算子のサンプル (1,0でOK)
  // p prg[] = {o_push,   {.v = 1}, o_push,   {.v = 2}, o_lt,    o_print,
  // o_push,
  //            {.v = 1}, o_push,   {.v = 2}, o_gt,     o_print, o_end};
  // vm(prg, 0);

  // スタックが空で終わっているか
  // p prg[] = {o_push, {.v = 1.0}, o_end};
  // vm(prg, 0);

  // 条件分岐のテスト(なにも表示されなければOK)
  // p prg[] = {o_push, {.v = 1.0},   o_if,    {.i = 8},
  //            o_push, {.v = 999.0}, o_print, o_end};
  // vm(prg, 0);

  return 0;
}

// prg: プログラム
// vc : 変数の数
void vm(p *prg, size_t vc) {
  static const void *ltbl[] = {
      &&l_end,     &&l_push,  &&l_pop, &&l_add,  &&l_sub, &&l_mul,  &&l_div,
      &&l_eq,      &&l_neq,   &&l_lt,  &&l_leqt, &&l_gt,  &&l_geqt, &&l_setval,
      &&l_loadval, &&l_print, &&l_jmp, &&l_if,   &&l_nif};
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
l_print:
  printf("%f\n", *--sp);
  goto *ltbl[prg[pc++].o];
l_jmp:
  pc = prg[pc].i;
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
l_end:
  // check stack is empty
  if (sp != stack)
    fprintf(stderr, "Internal Error: Stack is not empty.\n");
  free(env);
}
