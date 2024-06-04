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

const program = [
  ["assign", "a", 0],
  ["while", ["<", "a", 10], [
    ["if", ["==", "a", 4], [["assign", "a", 5], "continue"]],
    ["print_i", "a"],
    ["assign", "a", ["+", "a", 1]],
    ["if", ["==", "a", 7], ["break"]],
  ]],
];

const fs = require("fs");
const prg = compile(program);
fs.writeFileSync("prg.c", prg);
console.log(prg);
