// MEMO
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;
//
// tokenize.c
//

typedef enum {
  TK_PUNCT, // Keywords or punctuators
  TK_IDENT, // 識別子
  TK_NUM,   // Numeric literals
  TK_EOF,   // End-of-file markers
} TokenKind;

// Token type
typedef struct Token Token;
struct Token {
  TokenKind kind; // Token kind
  Token *next;    // Next token
  int val;        // If kind is TK_NUM, its value
  char *loc;      // Token location
  int len;        // Token length
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
Token *tokenize(char *input);

// Local variable
typedef struct Obj Obj;
struct Obj {
  Obj *next;
  char *name; // Variable name
  int offset; // Offset from RBP
};

// Functionを表し、独自のlocal val(と、それに対応したstacksize)を保有する.
// 複数のNodeを持ちうる.
typedef struct Function Function;
struct Function {
  Node *body;  // 関数body
  Obj *locals; // そのscopeにおける変数
  int stack_size;
};

typedef enum {
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_NEG,       // unary -
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_NUM,       // Integer
  ND_EXPR_STMT, // stmt
  ND_ASSIGN,    // =
  ND_VAR,       // Variable
} NodeKind;

// AST node type
typedef struct Node Node;
struct Node {
  NodeKind kind; // Node kind
  Node *lhs;     // Left-hand side
  Node *rhs;     // Right-hand side
  Node *next;    // Next node
  int val;       // Used if kind == ND_NUM
  Obj *var;      // Used if kind == ND_VAR
};

Function *parse(Token *tok);

//
// codegen.c
//

void codegen(Function *prog);