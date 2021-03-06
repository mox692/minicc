// #include <ctype.h>
// #include <stdarg.h>
// #include <stdbool.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// // トークンの種類
// typedef enum {
//   TK_RESERVED, // 記号
//   TK_NUM,      // 整数トークン
//   TK_EOF,      // 入力の終わりを表すトークン
// } TokenKind;

// typedef struct Token Token;

// // トークン型
// struct Token {
//   TokenKind kind; // トークンの型
//   Token *next;    // 次の入力トークン
//   int val;        // kindがTK_NUMの場合、その数値
//   char *str;      // トークン文字列
//   int len;
// };

// // 現在着目しているトークン
// Token *token;

// // user入力
// char *user_input;

// typedef enum {
//   ND_ADD,
//   ND_SUB,
//   ND_MUL,
//   ND_DIV,
//   ND_NUM,
//   ND_EQ, // ==
//   ND_NE, // !=
//   ND_LT, // <
//   ND_LE, // <=
// } NodeKind;

// // MEMO: なぜこれがいるのだろう
// typedef struct Node Node;

// struct Node {
//   NodeKind kind;
//   Node *lhs;
//   Node *rhs;
//   int val;
// };

// // エラーを報告するための関数
// // printfと同じ引数を取る
// void error(char *fmt, ...) {
//   va_list ap;
//   va_start(ap, fmt);
//   vfprintf(stderr, fmt, ap);
//   fprintf(stderr, "\n");
//   exit(1);
// }

// void error_at(char *loc, char *fmt, ...) {
//   va_list ap;
//   va_start(ap, fmt);
//   int pos = loc - user_input;
//   fprintf(stderr, "%s\n", user_input);
//   fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
//   fprintf(stderr, "^ ");
//   vfprintf(stderr, fmt, ap);
//   fprintf(stderr, "\n");
//   exit(1);
// }

// // 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// // 真を返す。それ以外の場合には偽を返す。
// bool consume(char *op) {
//   if (token->kind != TK_RESERVED || strlen(op) != token->len ||
//       memcmp(token->str, op, token->len))
//     return false;
//   token = token->next;
//   return true;
// }

// // 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// // それ以外の場合にはエラーを報告する。
// void expect(char *op) {
//   if (token->kind != TK_RESERVED || strlen(op) != token->len ||
//       memcmp(token->str, op, token->len))
//     error_at(token->str, "'%c'ではありません", op);
//   token = token->next;
// }

// // 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// // それ以外の場合にはエラーを報告する。
// int expect_number() {
//   if (token->kind != TK_NUM)
//     error_at(token->str, "数ではありません");
//   int val = token->val;
//   token = token->next;
//   return val;
// }

// bool at_eof() { return token->kind == TK_EOF; }

// bool startswith(char *p, char *q, int size) { return memcmp(p, q, size) == 0;
// }

// // 新しいトークンを作成してcurに繋げる
// // MEMO: 具体的にはcurのnextを埋める、と言う作業をしてる
// Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
//   // token毎にcallocでmemを割り当てる
//   Token *tok = calloc(1, sizeof(Token));
//   tok->kind = kind;
//   tok->str = str;
//   tok->len = len;
//   cur->next = tok;
//   return tok;
// }

// // 入力文字列pをトークナイズしてそれを返す
// Token *tokenize() {
//   char *p = user_input;
//   Token head;
//   head.next = NULL;
//   Token *cur = &head;

//   // MEMO: NULL文字までloop
//   // MEMO: ここからはcurのみを書き換える。
//   while (*p) {
//     // 空白文字をスキップ
//     if (isspace(*p)) {
//       p++;
//       continue;
//     }

//     // 複数文字tokenかどうか
//     if (startswith(p, "==", 2) || startswith(p, "!=", 2) ||
//         startswith(p, "<=", 2) || startswith(p, ">=", 2)) {
//       cur = new_token(TK_RESERVED, cur, p, 2);
//       p += 2;
//     }

//     if (strchr("+-*/()<>", *p)) {
//       // MEMO: なぜp++ ?
//       // MEMO:
//       //
//       引数に渡したcurのアドレスが返り値で帰ってきてるcurのnextのアドレスになっている
//       cur = new_token(TK_RESERVED, cur, p++, 1);
//       continue;
//     }

//     if (isdigit(*p)) {
//       // curを書き換える
//       cur = new_token(TK_NUM, cur, p, 0);
//       char *q = p;
//       // curの数値を読んでcur.valに代入 & pを進める
//       cur->val = strtol(p, &p, 10);
//       // char pointerの差 = 文字数
//       cur->len = p - q;
//       continue;
//     }

//     error_at(p, "トークナイズできません");
//   }

//   new_token(TK_EOF, cur, p, 0);
//   // このhead.nextがリストの先頭のtokenを指しているはず？？...
//   return head.next;
// }

// // Num Node以外のNodeを受け取り、右辺値と左辺値から新しいNodeを生成する.
// Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
//   Node *node = calloc(1, sizeof(Node));
//   node->lhs = lhs;
//   node->rhs = rhs;
//   node->kind = kind;
//   return node;
// }

// // 数値をNum Nodeとして返す.
// Node *new_node_num(int val) {
//   Node *node = calloc(1, sizeof(Node));
//   node->kind = ND_NUM;
//   node->val = val;
//   return node;
// }

// Node *expr();
// Node *equality();
// Node *relational();
// Node *add();

// // primary Nodeを返す関数.
// Node *primary() {
//   // primaryが処理するのはnumか(expr)の2パターンである

//   // exprの時
//   if (consume("(")) {
//     Node *node = expr();
//     expect(")");
//     return node;
//   }
//   // それ以外の時(num)
//   return new_node_num(expect_number());
// }

// // unary Nodeを返す関数.
// Node *unary() {
//   if (consume("+")) {
//     return primary();
//   } else if (consume("-")) {
//     // 引き算として扱ってる
//     return new_node(ND_SUB, new_node_num(0), primary());
//   }
//   return primary();
// }

// // mul Nodeを返す関数.
// // *か/のみのtokenを読み進め、それ以外のtokenが出現した場合はbreakする
// Node *mul() {
//   Node *node = unary();

//   for (;;) {
//     if (consume("*")) {
//       node = new_node(
//           ND_MUL, node,
//           primary()); //
//           第3引数でprimary()を呼んで、さらにtokenを読み進めてる点に注意.
//     } else if (consume("/")) {
//       node = new_node(
//           ND_DIV, node,
//           primary()); //
//           第3引数でprimary()を呼んで、さらにtokenを読み進めてる点に注意.
//     } else {
//       return node;
//     }
//   }
// }

// Node *add() {
//   Node *node = mul();

//   for (;;) {
//     if (consume("+")) {
//       node = new_node(ND_ADD, node, mul());
//     } else if (consume("-")) {
//       node = new_node(ND_SUB, node, mul());
//     } else {
//       return node;
//     }
//   }
// }

// Node *relational() {
//   Node *node = add();

//   for (;;) {
//     if (consume("<")) {
//       node = new_node(ND_LT, node, add());
//     } else if (consume("<=")) {
//       node = new_node(ND_LE, node, add());
//     } else if (consume(">")) {
//       node = new_node(ND_LT, add(), node);
//     } else if (consume(">=")) {
//       node = new_node(ND_LE, add(), node);
//     } else {
//       return node;
//     }
//   }
// }

// Node *equality() {
//   Node *node = relational();

//   for (;;) {
//     if (consume("==")) {
//       node = new_node(ND_EQ, node, relational());
//     } else if (consume("!=")) {
//       node = new_node(ND_NE, node, relational());
//     } else {
//       return node;
//     }
//   }
// }

// // expr Nodeを返す関数.
// Node *expr() { equality(); }

// // Nodeを引数にとり、そのNodeに対応したasmを標準出力する
// void genAsm(Node *node) {
//   // 1.ND_NUMのとき(最後はここにきて処理が終了するはず)
//   if (node->kind == ND_NUM) {
//     printf("  push %d\n", node->val);
//     return;
//   }

//   // 2.ND_NUMでない時
//   // 左辺Nodeと右辺Nodeに再起的にgenAsmをかけてasmを生成
//   genAsm(node->lhs);
//   genAsm(node->rhs);

//   printf("  pop rdi\n");
//   printf("  pop rax\n");

//   switch (node->kind) {
//   case ND_ADD:
//     printf("  add rax, rdi\n");
//     break;
//   case ND_SUB:
//     printf("  sub rax, rdi\n");
//     break;
//   case ND_MUL:
//     printf("  imul rax, rdi\n");
//     break;
//   case ND_DIV:
//     printf("  cqo\n");
//     printf("  idiv rdi\n");
//     break;
//   case ND_EQ:
//     printf("  cmp rax, rdi\n");
//     printf("  sete al\n");
//     printf("  movzb rax, al\n");
//     break;
//   case ND_NE:
//     printf("  cmp rax, rdi\n");
//     printf("  setne al\n");
//     printf("  movzb rax, al\n");
//     break;
//   case ND_LT:
//     printf("  cmp rax, rdi\n");
//     printf("  setl al\n");
//     printf("  movzb rax, al\n");
//     break;
//   case ND_LE:
//     printf("  cmp rax, rdi\n");
//     printf("  setle al\n");
//     printf("  movzb rax, al\n");
//     break;
//   }

//   printf("  push rax\n");
// }

// int main(int argc, char **argv) {
//   if (argc != 2) {
//     error("引数の個数が正しくありません");
//     return 1;
//   }

//   // トークナイズする
//   user_input = argv[1];
//   token = tokenize();
//   Node *node = expr();

//   // アセンブリの前半部分を出力
//   printf(".intel_syntax noprefix\n");
//   printf(".globl main\n");
//   printf("main:\n");

//   // asmコードを出力.
//   genAsm(node);

//   printf("  pop rax\n");
//   printf("  ret\n");
//   return 0;
// }