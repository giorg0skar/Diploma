#ifndef __AST_H__
#define __AST_H__

#include "symbol.h"

typedef enum {
  PRINT, LET, FOR, IF, SEQ,
  ID, CONST, PLUS, MINUS, TIMES, DIV, MOD,
  LT, GT, LE, GE, EQ, NE, AND, OR, NOT, DECL, BLOCK
} kind;

typedef struct node {
  kind k;
  char id;
  int num;
  struct node *left, *right;
  Type type;
  int nesting_diff;  // ID and LET nodes
  int offset;        // ID and LET nodes
  int num_vars;      // BLOCK node
} *ast;

ast ast_id (char c);
ast ast_const (int n);
ast ast_op (ast l, kind op, ast r);
ast ast_print (ast l);
ast ast_let (char c, ast l);
ast ast_for (ast l, ast r);
ast ast_if (ast l, ast r);
ast ast_seq (ast l, ast r);
ast ast_decl (char c, Type t);
ast ast_block (ast l, ast r);

void ast_sem (ast t);

int ast_run (ast t);

#endif
