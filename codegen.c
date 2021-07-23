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

void codegen(Node *node) {
  printf("  .globl main\n");
  printf("main:\n");

  for (Node *n = node; n; n = n->next) {
    gen_stmt(n);
    assert(depth == 0);
  }
  printf("  ret\n");
}