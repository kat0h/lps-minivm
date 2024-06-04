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
  // 計算のサンプル
  // p prg[] = {o_push,       {.v = 114.0}, o_setval, {.i = 0},  o_push,
  //            {.v = 514.0}, o_setval,     {.i = 1}, o_loadval, {.i = 0},
  //            o_loadval,    {.i = 1},     o_add,    o_print_f,   o_end};
  // vm(prg, 2);

  // ループのサンプル
  // p prg[] = {o_push,   {.v = 0},  o_setval, {.i = 0}, o_loadval, {.i = 0},
  //            o_print_f,  o_loadval, {.i = 0}, o_push,   {.v = 1},  o_add,
  //            o_setval, {.i = 0},  o_jmp,    {.i = 4}};
  // vm(prg, 1);

  // 比較演算子のサンプル (1,0でOK)
  // p prg[] = {o_push,   {.v = 1}, o_push,   {.v = 2}, o_lt,    o_print_f,
  // o_push,
  //            {.v = 1}, o_push,   {.v = 2}, o_gt,     o_print_f, o_end};
  // vm(prg, 0);

  // スタックが空で終わっているか
  // p prg[] = {o_push, {.v = 1.0}, o_end};
  // vm(prg, 0);

  // 条件分岐のテスト(なにも表示されなければOK)
  // p prg[] = {o_push, {.v = 1.0},   o_if,    {.i = 8},
  //            o_push, {.v = 999.0}, o_print_f, o_end};
  // vm(prg, 0);

  // 出力のサンプル
  // p prg[] = {o_push, {.v = 1.1}, o_print_f, o_print_cr,
  //            o_push, {.v = 1.1}, o_print_i, o_print_cr};
  // vm(prg, 0);

  // relativeなjmp
  // p prg[] = {o_jmpr, {.i = 5}, o_push, {.v = 1}, o_print_i, o_end};
  // vm(prg, 0);

  p prg[] = {
      o_push,        {.v = 80},     o_print_ascii, o_push,        {.v = 51},
      o_print_ascii, o_push,        {.v = 32},     o_print_ascii, o_push,
      {.v = 10},     o_print_ascii, o_push,        {.v = 4},      o_setval,
      {.i = 0},      o_push,        {.v = 1000},   o_setval,      {.i = 1},
      o_loadval,     {.i = 1},      o_print_i,     o_loadval,     {.i = 1},
      o_print_i,     o_print_cr,    o_push,        {.v = 255},    o_print_i,
      o_push,        {.v = 0},      o_setval,      {.i = 2},      o_loadval,
      {.i = 1},      o_loadval,     {.i = 2},      o_gt,          o_nifr,
      {.i = 197},    o_loadval,     {.i = 2},      o_loadval,     {.i = 0},
      o_mul,         o_loadval,     {.i = 1},      o_div,         o_loadval,
      {.i = 0},      o_push,        {.v = 2},      o_div,         o_sub,
      o_setval,      {.i = 3},      o_push,        {.v = 0},      o_setval,
      {.i = 4},      o_loadval,     {.i = 1},      o_loadval,     {.i = 4},
      o_gt,          o_nifr,        {.i = 160},    o_loadval,     {.i = 4},
      o_loadval,     {.i = 0},      o_mul,         o_loadval,     {.i = 1},
      o_div,         o_loadval,     {.i = 0},      o_push,        {.v = 2},
      o_div,         o_sub,         o_setval,      {.i = 5},      o_push,
      {.v = 0},      o_setval,      {.i = 6},      o_push,        {.v = 0},
      o_setval,      {.i = 7},      o_push,        {.v = 0},      o_setval,
      {.i = 8},      o_push,        {.v = 0},      o_setval,      {.i = 9},
      o_push,        {.v = 50},     o_loadval,     {.i = 9},      o_gt,
      o_nifr,        {.i = 70},     o_loadval,     {.i = 6},      o_loadval,
      {.i = 6},      o_mul,         o_loadval,     {.i = 7},      o_loadval,
      {.i = 7},      o_mul,         o_sub,         o_loadval,     {.i = 3},
      o_add,         o_setval,      {.i = 10},     o_push,        {.v = 2},
      o_loadval,     {.i = 6},      o_mul,         o_loadval,     {.i = 7},
      o_mul,         o_loadval,     {.i = 5},      o_add,         o_setval,
      {.i = 11},     o_loadval,     {.i = 10},     o_setval,      {.i = 6},
      o_loadval,     {.i = 11},     o_setval,      {.i = 7},      o_loadval,
      {.i = 6},      o_loadval,     {.i = 6},      o_mul,         o_loadval,
      {.i = 7},      o_loadval,     {.i = 7},      o_mul,         o_add,
      o_push,        {.v = 4},      o_gt,          o_nifr,        {.i = 8},
      o_push,        {.v = 1},      o_setval,      {.i = 8},      o_jmpr,
      {.i = 11},     o_loadval,     {.i = 9},      o_push,        {.v = 1},
      o_add,         o_setval,      {.i = 9},      o_jmpr,        {.i = -73},
      o_loadval,     {.i = 8},      o_nifr,        {.i = 31},     o_loadval,
      {.i = 9},      o_push,        {.v = 12},     o_mul,         o_push,
      {.v = 255},    o_gt,          o_nifr,        {.i = 7},      o_push,
      {.v = 255},    o_print_i,     o_jmpr,        {.i = 8},      o_loadval,
      {.i = 9},      o_push,        {.v = 12},     o_mul,         o_print_i,
      o_push,        {.v = 0},      o_print_i,     o_push,        {.v = 0},
      o_print_i,     o_jmpr,        {.i = 11},     o_push,        {.v = 255},
      o_print_i,     o_push,        {.v = 255},    o_print_i,     o_push,
      {.v = 255},    o_print_i,     o_loadval,     {.i = 4},      o_push,
      {.v = 1},      o_add,         o_setval,      {.i = 4},      o_jmpr,
      {.i = -163},   o_print_cr,    o_loadval,     {.i = 2},      o_push,
      {.v = 1},      o_add,         o_setval,      {.i = 2},      o_jmpr,
      {.i = -200},   o_end};
  vm(prg, 12);

  return 0;
}
