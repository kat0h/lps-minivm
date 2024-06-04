// o_end,
// o_push, // {.v = <number>}
// o_pop,
// o_add,     // +
// o_sub,     // -
// o_mul,     // *
// o_div,     // /
// o_eq,      // ==
// o_neq,     // !=
// o_lt,      // <
// o_leqt,    // <=
// o_gt,      // >
// o_geqt,    // >=
// o_setval,  // {.i = <int>}
// o_loadval, // {.i = <int>}
// o_print_f,
// o_print_i,
// o_print_cr,
// o_jmp,  // {.i = <int>}
// o_jmpr, // {.i = <int>}
// o_if,   // {.i = <int>}
// o_nif,  // {.i = <int>}
// o_ifr,  // {.i = <int>}
// o_nifr, // {.i = <int>}

let env = [];

function compile(prg) {
  env = [];
  return `p prg[] = {\n  ${
    compile_program(prg).join(",\n  ")
  },\n  o_end\n};\nvm(prg, 0);`;
}

function compile_program(prg) {
  // return prg.map((stmt) => [`//${stmt}`, ...compile_stmt(stmt)]).flat();
  return prg.map((stmt) => compile_stmt(stmt)).flat();
}

function compile_stmt(stmt) {
  if (stmt[0] == "print_f") {
    return [...compile_expr(stmt[1], env), "o_print_f"];
  } else if (stmt[0] == "print_i") {
    return [...compile_expr(stmt[1], env), "o_print_i"];
  } else if (stmt[0] == "print_cr") {
    return ["o_print_cr"];
  } else if (stmt[0] == "print_str") {
    return stmt[1].split("").map(
      (i) => ["o_push", `{.v = ${i.charCodeAt(0)}}`, "o_print_ascii"],
    ).flat();
  } else if (stmt[0] == "assign") {
    const p = [];
    p.push(...compile_expr(stmt[2]));
    p.push("o_setval");
    const index = env.indexOf(stmt[1]);
    if (index == -1) {
      env.push(stmt[1]);
      p.push(`{.i = ${env.length - 1}}`);
    } else {
      p.push(`{.i = ${index}}`);
    }
    return p;
  } else if (stmt[0] == "if") {
    const p = [];
    p.push(...compile_expr(stmt[1]));
    p.push("o_nifr");
    const s = compile_program(stmt[2]);
    p.push(
      `{.i = ${s.length + 1 + (stmt.length > 3 && stmt[3] == "else" ? 3 : 1)}}`,
    );
    p.push(...s);
    if (stmt.length > 3 && stmt[3] == "else") {
      const s = compile_program(stmt[4]);
      p.push("o_jmpr", `{.i = ${s.length + 2}}`);
      p.push(...s);
    }
    return p;
  } else if (stmt[0] == "while") {
    const p = [];
    const exp = compile_expr(stmt[1]);
    const stm = compile_program(stmt[2]);
    p.push(...exp);
    p.push("o_nifr", `{.i = ${stm.length + 4}}`);
    // break/continueの解決
    let i;
    while ((i = stm.indexOf("break")) != -1) {
      stm[i] = "o_jmpr";
      stm[i + 1] = `{.i = ${stm.length - i + 2}}`;
    }
    while ((i = stm.indexOf("continue")) != -1) {
      stm[i] = "o_jmpr";
      stm[i + 1] = `{.i = ${-i - exp.length - 2}}`;
    }
    p.push(...stm);
    p.push("o_jmpr", `{.i = ${-stm.length - exp.length - 2}}`);
    return p;
  } else if (stmt[0] == "for") {
    const p = [];
    const init = compile_stmt(stmt[1][0]);
    const cond = compile_expr(stmt[1][1]);
    const upda = compile_stmt(stmt[1][2]);
    const body = compile_program(stmt[2]);

    p.push(...init);
    p.push(...cond);
    p.push("o_nifr", `{.i = ${body.length + upda.length + 4}}`);
    // break/continueの解決
    let i;
    while ((i = body.indexOf("break")) != -1) {
      body[i] = "o_jmpr";
      body[i + 1] = `{.i = ${body.length - i + upda.length + 2}}`;
    }
    while ((i = body.indexOf("continue")) != -1) {
      body[i] = "o_jmpr";
      body[i + 1] = `{.i = ${-i - 2 - cond.length}}`;
    }
    p.push(...body);
    p.push(...upda);
    p.push("o_jmpr", `{.i = ${-cond.length - body.length - upda.length - 2}}`);
    return p;
  } else if (stmt == "break" || stmt == "continue") {
    // 後処理で数字分ズレないように
    return [stmt, null];
  }
  throw new Error(`Compile Error in compile_stmt ${stmt}`);
}

const op = {
  "+": "o_add",
  "-": "o_sub",
  "*": "o_mul",
  "/": "o_div",
  "%": "o_mod",
  "==": "o_eq",
  "!=": "o_neq",
  "<": "o_lt",
  "<=": "o_leqt",
  ">": "o_gt",
  ">=": "o_geqt",
};
function compile_expr(expr) {
  if (typeof expr == "number") {
    return ["o_push", `{.v = ${expr}}`];
  } else if (typeof expr == "object") {
    return [...compile_expr(expr[1]), ...compile_expr(expr[2]), op[expr[0]]];
  } else if (typeof expr == "string") {
    return ["o_loadval", `{.i = ${env.indexOf(expr)}}`];
  }
  throw new Error(`Compile Error in compile_expr ${expr}`);
}

// [
//   "for",
//   [["assign", "i", 0], ["<", "i", 20], ["assign", "i", ["+", 1, "i"]]],
//   [
//     ["if", ["==", ["%", "i", 2], 0], [
//       ["print_i", "i"],
//       ["if", ["==", "i", 8], [
//         ["assign", "i", 15],
//         "continue"
//       ]],
//     ]],
//   ],
// ],
const program = [
  ["print_str", "P3 \n"],

  ["assign", "size", 4],
  ["assign", "pixel", 1],

  ["print_i", "pixel"],
  ["print_i", "pixel"],
  ["print_cr"],
  ["print_i", 255],

  ["for", [["assign", "i", 0], [">", "pixel", "i"], ["assign", "i", ["+", "i", 1]]], [
    ["assign", "x", ["-", ["/", ["*", "i", "size"], "pixel"], ["/", "size", 2]]],
    ["for", [["assign", "j", 0], [">", "pixel", "j"], ["assign", "j", ["+", "j", 1]]], [
      ["assign", "y", ["-", ["/", ["*", "j", "size"], "pixel"], ["/", "size", 2]]],
      ["assign", "a", 0],
      ["assign", "b", 0],
      ["assign", "d", 0],
      ["for", [["assign", "k", 0], [">", 50, "k"], ["assign", "k", ["+", "k", 1]]], [
        ["assign", "_a", ["+", ["-", ["*", "a", "a"], ["*", "b", "b"]], "x"]],
        ["assign", "_b", ["+", ["*", ["*", 2, "a"], "b"], "y"]],
        ["assign", "a", "_a"],
        ["assign", "b", "_b"],
        ["if", [">", ["+", ["*", "a", "a"], ["*", "b", "b"]], 4], [
          ["assign", "d", 1],
          "break",
        ]],
      ]],
      ["if", "d", [
        ["if", [">", ["*", "k", 12], 255], [
          ["print_i", 255],
        ], "else", [
          ["print_i", ["*", "k", 12]],
        ]],
        ["print_i", 0],
        ["print_i", 0],
      ], "else", [
        ["print_i", 255],
        ["print_i", 255],
        ["print_i", 255],
      ]],
    ]],
    ["print_cr"],
  ]],
];

const fs = require("fs");
const prg = compile(program);
fs.writeFileSync("prg.c", prg);
console.log(prg);
