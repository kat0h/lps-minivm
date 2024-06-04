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

int main() {
  // 1000
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
  int len = sizeof(prg) / sizeof(p);
  for (int i = 0; i < len; i++) {
    unsigned int *ptr = (unsigned int *)&prg[i];
    printf("%#x, ", *ptr);
  }

  return 0;
}
