#ifndef __AST_H__
#define __AST_H__

#include "symbol.h"

typedef enum {
  PROGRAM, HEADER, HEADER_PART, FUNC_DEF, FPAR_DEF,
  VAR, ID, SKIP, BREAK, CONT, LOOP, SEQ, IF, IF_ELSE, BLOCK,
  PLUS, MINUS, TIMES, DIV, MOD,
  NOT, AND, OR, EQ, NEQ, LT, GT, LE, GE,
  EXIT, ASSIGN, PROC_CALL, FUNC_CALL, RET, DECL,
  STRING_LIT, TID, ARR,
  INTCONST, CHARCONST,
  EXPR_PART, EXPR_NOT, EXPR_OR, EXPR_AND
} kind;

typedef struct node {
  kind k;
  char *id;
  int num;
  struct node *branch1, *branch2, *branch3, *branch4;
  Type type;
  int nesting_diff;  // ID and LET nodes
  int offset;        // ID and LET nodes
  int num_vars;      // BLOCK node
} *ast;

ast ast_func_def(ast header, ast local, ast block);
ast ast_header(char *l1, Type t, ast l2, ast l3);
ast ast_header_part(ast l1, ast l2);
ast ast_fpar_def(ast l1, Type t);
ast ast_decl (ast l1);
ast ast_var(ast idlist, Type t);
ast ast_id (char *c, ast next);
ast ast_op (ast l, kind op, ast r);
ast ast_skip();
ast ast_assign(ast l, ast r);
ast ast_exit();
ast ast_return(ast l1);
ast ast_if (ast l1, ast l2, ast l3);
ast ast_if_else (ast cond, ast blck, ast ifp, ast elblck);
ast ast_loop(char *l, ast r);
ast ast_break(char *s);
ast ast_continue(char *s);
ast ast_seq(ast l1, ast l2);
ast ast_block(ast l1);
ast ast_tid(char *s);
ast ast_string_lit(char *s);
ast ast_arr(ast l1, ast l2);
ast ast_int_const (int n);
ast ast_char_const(char c);
ast ast_proc_call(char *s, ast l1, ast l2);
ast ast_func_call(char *s, ast l1, ast l2);
ast ast_expr_part(ast l1, ast l2);
ast ast_not_expr(ast l1);
ast ast_and_expr(ast l1, ast l2);
ast ast_or_expr(ast l1, ast l2);

void ast_sem (ast t);

int ast_run (ast t);

#endif
