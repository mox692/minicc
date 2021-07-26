#include "minicc.h"

static int depth;

static void push(void) {
  printf("  push %%rax\n");
  depth++;
}

static void pop(char *arg) {
  printf("  pop %s\n", arg);
  depth--;
}

// Round up `n` to the nearest multiple of `align`. For instance,
// align_to(5, 8) returns 8 and align_to(11, 8) returns 16.
static int align_to(int n, int align) {
  return (n + align - 1) / align * align;
}

static void gen_addr(Node *node) {
  // 変数名(a-zの1文字)のoffsetを計算、そのアドレス値をraxに入れるアセンブリ命令を出力
  if (node->kind == ND_VAR) {
    printf("  lea %d(%%rbp), %%rax\n", node->var->offset);
    return;
  }

  error("not an lvalue");
}

static void gen_expr(Node *node) {
  switch (node->kind) {
  // 単項の場合はここで処理
  case ND_NUM:
    printf("  mov $%d, %%rax\n", node->val);
    return;
  case ND_NEG:
    gen_expr(node->lhs);
    printf("  neg %%rax\n");
    return;
  case ND_VAR:
    gen_addr(node);
    printf("  mov (%%rax), %%rax\n");
    return;
  case ND_ASSIGN:
    // 1.(左辺)変数に割り当てられたaddrをraxに格納&stackにpush(gen_addr(node->lhs);)
    // 2.右辺にある式を評価し、結果をraxに入れておく(gen_expr(node->rhs);)
    // 3.stackのtopにある変数のaddrをrdiにpop
    // 4.mov %%rax, (%%rdi)で右辺値を左辺変数のaddrに代入
    // (詳しくは出力してるアセンブリを参照)
    gen_addr(node->lhs);
    push();
    gen_expr(node->rhs);
    pop("%rdi");
    printf("  mov %%rax, (%%rdi)\n");
    return;
  }

  // 2項以上の場合はここから
  gen_expr(node->rhs); // MEMO: 何でrightから呼んでるのだろう...
  push();
  gen_expr(node->lhs);
  pop("%rdi");

  switch (node->kind) {
  case ND_ADD:
    printf("  add %%rdi, %%rax\n");
    return;
  case ND_SUB:
    printf("  sub %%rdi, %%rax\n");
    return;
  case ND_MUL:
    printf("  imul %%rdi, %%rax\n");
    return;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv %%rdi\n");
    return;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
    printf("  cmp %%rdi, %%rax\n");

    if (node->kind == ND_EQ)
      printf("  sete %%al\n");
    else if (node->kind == ND_NE)
      printf("  setne %%al\n");
    else if (node->kind == ND_LT)
      printf("  setl %%al\n");
    else if (node->kind == ND_LE)
      printf("  setle %%al\n");

    printf("  movzb %%al, %%rax\n");
    return;
  }

  error("invalid expression");
}

static void gen_stmt(Node *node) {
  if (node->kind == ND_EXPR_STMT) {
    // 左辺を展開(new_unartyがlhsに代入してるので。)
    gen_expr(node->lhs);
    return;
  }

  // 現段階ではND_EXPR_STMT以外はありえない
  error("invalid statement");
}

// Assign offsets to local variables.
static void assign_lvar_offsets(Function *prog) {
  int offset = 0;
  for (Obj *var = prog->locals; var; var = var->next) {
    offset += 8;
    var->offset = -offset;
  }
  prog->stack_size = align_to(offset, 16);
}

void codegen(Function *prog) {
  assign_lvar_offsets(prog);

  printf("  .globl main\n");
  printf("main:\n");

  // Prologue
  printf("  push %%rbp\n");
  printf("  mov %%rsp, %%rbp\n");
  printf("  sub $%d, %%rsp\n", prog->stack_size); // stackサイズ分だけ押し下げる

  for (Node *n = prog->body; n; n = n->next) {
    // stmtごとに.nextでNodeを進めていく
    gen_stmt(n);
    assert(depth == 0);
  }

  // Epilogue
  printf("  mov %%rbp, %%rsp\n");
  printf("  pop %%rbp\n");
  printf("  ret\n");
}