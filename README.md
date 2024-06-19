# StackVM

gcc拡張を利用して命令ディスパッチの計算量をO(1)にしたC言語製のスタックVM実装.

スタックは数値スタック1本，変数の数は実行前に決定する.

## 実行方法
```
gcc main.c -o vm
./vm
```

## 命令セット

1命令32bit固定長.

```
o_end,
o_push,    // {.v = <float>}
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
o_jmp,     // {.i = <int>}
o_jmpr,    // {.i = <int>}
o_if,      // {.i = <int>}
o_nif,     // {.i = <int>}
o_ifr,     // {.i = <int>}
o_nifr,    // {.i = <int>}
```

## プログラムのサンプル

```c
// 計算のサンプル
p prg[] = {o_push,       {.v = 114.0}, o_setval, {.i = 0},  o_push,
           {.v = 514.0}, o_setval,     {.i = 1}, o_loadval, {.i = 0},
           o_loadval,    {.i = 1},     o_add,    o_print_f, o_end};
vm(prg, 2);

// ループのサンプル
p prg[] = {o_push,    {.v = 0},  o_setval, {.i = 0}, o_loadval, {.i = 0},
           o_print_f, o_loadval, {.i = 0}, o_push,   {.v = 1},  o_add,
           o_setval,  {.i = 0},  o_jmp,    {.i = 4}};
vm(prg, 1);

// 比較演算子のサンプル (1,0でOK)
p prg[] = {o_push,    {.v = 1},  o_push,   {.v = 2}, o_lt,
           o_print_f, o_push,    {.v = 1}, o_push,   {.v = 2},
           o_gt,      o_print_f, o_end};
vm(prg, 0);

// スタックが空で終わっているか
p prg[] = {o_push, {.v = 1.0}, o_end};
vm(prg, 0);

// 条件分岐のテスト(なにも表示されなければOK)
p prg[] = {o_push, {.v = 1.0},   o_if,      {.i = 8},
           o_push, {.v = 999.0}, o_print_f, o_end};
vm(prg, 0);

// 出力のサンプル
p prg[] = {o_push, {.v = 1.1}, o_print_f, o_print_cr,
           o_push, {.v = 1.1}, o_print_i, o_print_cr};
vm(prg, 0);

// relativeなjmp
p prg[] = {o_jmpr, {.i = 5}, o_push, {.v = 1}, o_print_i, o_end};
vm(prg, 0);
```

## コンパイラ

S式風のASTからVMの命令セット用のプログラムを吐き出すコンパイラを添付した.

nodejsで実行すると標準出力にプログラムを吐き出すので，main.cを書き換える．

下記はマンデルブロ集合の画像をppm形式で吐き出すプログラムのサンプル:

```js
const program = [
  ["print_str", "P3 \n"],

  ["assign", "size", 4],
  ["assign", "pixel", 1000],

  ["print_i", "pixel"],
  ["print_i", "pixel"],
  ["print_cr"],
  ["print_i", 255],

  ["for", [["assign", "i", 0], [">", "pixel", "i"], ["assign", "i", [ "+", "i", 1, ]]], [
    ["assign", "x", ["-", ["/", ["*", "i", "size"], "pixel"], [ "/", "size", 2, ]]],
    ["for", [["assign", "j", 0], [">", "pixel", "j"], ["assign", "j", [ "+", "j", 1, ]]], [
      ["assign", "y", ["-", ["/", ["*", "j", "size"], "pixel"], [
        "/",
        "size",
        2,
      ]]],
      ["assign", "a", 0],
      ["assign", "b", 0],
      ["assign", "d", 0],
      ["for", [["assign", "k", 0], [">", 50, "k"], ["assign", "k", [ "+", "k", 1, ]]], [
        ["assign", "_a", ["+", ["-", ["*", "a", "a"], ["*", "b", "b"]], "x"]],
        ["assign", "_b", ["+", ["*", ["*", 2, "a"], "b"], "y"]],
        ["assign", "a", "_a"],
        ["assign", "b", "_b"],
        ["if", [">", ["+", ["*", "a", "a"], ["*", "b", "b"]], 4], [
          ["assign", "d", 1],
          "break",
        ]],
      ]],
      [ "if", "d", [
          [ "if", [">", ["*", "k", 12], 255], [
              ["print_i", 255],
            ], "else", [
              ["print_i", ["*", "k", 12]],
            ],
          ],
          ["print_i", 0],
          ["print_i", 0],
        ], "else", [
          ["print_i", 255],
          ["print_i", 255],
          ["print_i", 255],
        ],
      ],
    ]],
    ["print_cr"],
  ]],
];
```

C言語で書いた同様のプログラム(mand.cを参照)の約1/5倍速程度で動作する.

ASTの形式はおおむね以下の通り.

```
program = [ statement* ]
statement = ["print_f", expression]
          | ["print_i", expression]
          | ["print_cr"]
          | ["print_str", STRING]
          | ["assign", "NAME", expression]
          | ["if", expression, program]
          | ["if", expression, program, "else", program]
          | ["while", expression, program]
          | ["for", [statement, expression, statement], program]
          | "break"
          | "continue"
expression = NUMBER
           | [operator, expression, expression]
           | "NAME"
operator = "+" | "-" | "*" | "/" | "%" | "==" | "!=" | "<" | "<=" | ">" | ">="
```
