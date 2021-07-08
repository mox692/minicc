#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
};

char *user_input;

// 現在着目しているトークン
Token *token;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char* loc, char* fmt, ...) {
    va_list ap;
  va_start(ap, fmt);
    int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error_at(token->str, "'%c'ではありません", op);
  token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str,"数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
// MEMO: 具体的にはcurのnextを埋める、と言う作業をしてる
Token *new_token(TokenKind kind, Token *cur, char *str) {
  // token毎にcallocでmemを割り当てる
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  // MEMO: NULL文字までloop
  // MEMO: ここからはcurのみを書き換える。
  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strchr("+-*/()", *p)){
      // MEMO: なぜp++ ?
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      // curを書き換える
      cur = new_token(TK_NUM, cur, p);
      // curの数値を読んでcur.valに代入 & pを進める
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p,"トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  // このhead.nextがリストの先頭のtokenを指しているはず？？...
  return head.next;
}

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
} NodeKind;

// MEMO: なぜこれがいるのだろう
typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

// Num Node以外のNodeを受け取り、右辺値と左辺値から新しいNodeを生成する.
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->lhs = lhs;
  node->rhs = rhs;
  node->kind = kind;
  return node;
}


// 数値をNum Nodeとして返す.
Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(int));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *expr();
Node *mul();
Node *primary();

// expr Nodeを返す関数.
Node *expr() {
  Node *node = mul();

  for(;;) {
    if(consume('+')) {
      node = new_node(ND_ADD, node, mul());
    } else if(consume('-')) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

// primary Nodeを返す関数.
// mul Nodeを返す関数.
// *か/のみのtokenを読み進め、それ以外のtokenが出現した場合はbreakする
Node *mul() {
  Node *node = primary();

  for(;;) {
    if(consume('*')) {
      node = new_node(ND_MUL, node, primary()); // 第3引数でprimary()を呼んで、さらにtokenを読み進めてる点に注意.
    } else if(consume('/')) {
      node = new_node(ND_DIV, node, primary()); // 第3引数でprimary()を呼んで、さらにtokenを読み進めてる点に注意.
    } else {
      return node;
    }
  }
}

Node *primary() {
  // primaryが処理するのはnumか(expr)の2パターンである
  
  // exprの時
  if(consume('(')) {
    Node *node = expr();
    expect(')');
    return node;
  }
  // それ以外の時(num)
  return new_node_num(expect_number());
}



// Nodeを引数にとり、そのNodeに対応したasmを標準出力する
void genAsm(Node *node) {
  // 1.ND_NUMのとき(最後はここにきて処理が終了するはず)
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  // 2.ND_NUMでない時
  // 左辺Nodeと右辺Nodeに再起的にgenAsmをかけてasmを生成
  genAsm(node->lhs);
  genAsm(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  }
  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  // トークナイズする
  user_input = argv[1];
  token = tokenize();
  Node *node = expr();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // asmコードを出力.
  genAsm(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}