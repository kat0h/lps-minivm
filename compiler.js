function compile(prg) {
  return `p prg[] = {\n  ${
    compile_program(prg).join(",\n  ")
  },\n  o_end\n};\nvm(prg, 0);`;
}

function compile_program(prg) {
  return prg.map((stmt) => compile_stmt(stmt)).flat();
}

function compile_stmt(stmt) {
  if (stmt[0] == "print_f") {
    return [...compile_expr(stmt[1]), "o_print_f"];
  } else if (stmt[0] == "print_i") {
    return [...compile_expr(stmt[1]), "o_print_i"];
  } else if (stmt[0] == "print_cr") {
    return ["o_print_cr"];
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
  }
  throw new Error(`Compile Error in compile_expr ${expr}`);
}

const program = [
  ["print_f", ["+", ["*", 3, 4], 2]],
  ["print_cr"],
  ["print_i", ["+", ["*", 3, 4], 2]],
  ["print_cr"],
];
console.log(compile(program));
