#include "minicc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }
  Token *tok = tokenize(argv[1]);
  Function *prog = parse(tok);
  // 関数ごとに(今は1つだが)codegenする.
  codegen(prog);
  return 0;
}