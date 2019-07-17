%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MYast.h"
#include "symbol.h"
//#include <vector>
//#include <string>

void yyerror (const char *msg);

//vector<string> *ids; 
extern int linenumber;
ast t;
Type base_type;
%}

%union{
  ast a;
  char *s;
  char c;
  int n;
  Type t;
}

//%start program
%token T_and "and"
%token T_as "as"
%token T_begin "begin"
%token T_break "break"
%token T_byte "byte"
%token T_continue "continue"
%token T_decl "decl"
%token T_def "def"
%token T_elif "elif"
%token T_else "else"
%token T_end  "end"
%token T_exit "exit"
%token T_false "false"
%token T_if "if"
%token T_is "is"
%token T_int "int"
%token T_loop "loop"
%token T_not "not"
%token T_or "or"
%token T_ref "ref"
%token T_return "return"
%token T_skip "skip"
%token T_true "true"
%token T_var "var"
%token T_less '<'
%token T_greater '>'
%token T_lesseq "<="
%token T_greatereq ">="
%token T_noteq "<>"
%token T_assign ":="
%token<n> T_intconst
%token<s> T_id
%token<s> T_string
%token<c> T_char
%token T_auto_end

%left "or"
%left "and"
%left "not"
%nonassoc '<' '>' "<=" ">=" '=' "<>"
%left '+' '-'
%left '*' '/' '%'
%left UMINUS UPLUS

%type<a> program
%type<a> func_def
%type<a> header
%type<a> header_part
%type<a> fpar_def
%type<t> data_type
%type<t> type
%type<t> fpar_type
%type<t> fpar_part
%type<a> local_def
%type<a> func_decl
%type<a> var_def
%type<a> id
%type<a> stmt
%type<a> if_part
%type<a> block
%type<a> proc_call
%type<a> expr_part
%type<a> func_call
%type<a> l_value
%type<a> expr
%type<a> cond
%type<a> x_cond

%%

program:
  func_def  { t = $$ = $1; }
;

func_def:
  "def" header local_def block  { $$ = ast_func_def($2,$3,$4); }
;

header:
  T_id                                          { $$ = ast_header($1,typeVoid,NULL,NULL); }
| T_id "is" data_type                           { $$ = ast_header($1,$3,NULL,NULL); }
| T_id ':' fpar_def header_part                 { $$ = ast_header($1,typeVoid,$3,$4); }
| T_id "is" data_type ':' fpar_def header_part  { $$ = ast_header($1,$3,$5,$6); }
;

header_part:
  %empty  { $$ = NULL; }
| ',' fpar_def header_part  { $$ = ast_header_part($2,$3); }
;

fpar_def:
  id "as" fpar_type   { $$ = ast_fpar_def($1,$3); }
;

data_type:
  "int"   { $$ = typeInteger; }
| "byte"  { $$ = typeChar; }
;

type:
  data_type { $$ = $1; }
| type '[' T_intconst ']' { $$ = typeArray($3,$1); }
;

fpar_type:
  type  { $$ = $1; }
| "ref" data_type { $$ = typePointer($2); }
| fpar_part { $$ = $1; }
;

fpar_part:
  data_type '[' ']' { $$ = typeIArray($1); }
| data_type '[' T_intconst ']'  { $$ = typeArray($3,$1); }
| fpar_part '[' T_intconst ']'  { $$ = typeArray($3,$1); }
;

local_def:
  %empty  { $$ = NULL; }
| func_def local_def    { $$ = ast_seq($1,$2); }
| func_decl local_def   { $$ = ast_seq($1,$2); }
| var_def local_def     { $$ = ast_seq($1,$2); }
;

func_decl:
  "decl" header   { $$ = ast_decl($2); }
;

var_def:
  "var" id "is" type  { $$ = ast_var($2,$4); }
;

id:
  T_id    { $$ = ast_id($1,NULL); }
| T_id id { $$ = ast_id($1,$2); }
;

stmt:
  "skip"            { $$ = ast_skip(); }
| l_value ":=" expr { $$ = ast_assign($1,$3); }
| proc_call         { $$ = $1; }
| "exit"            { $$ = ast_exit(); }
| "return" ':' expr { $$ = ast_return($3); }
| "if" cond ':' block if_part "else" ':' block  { $$ = ast_if_else($2,$4,$5,$8); }
| "if" cond ':' block if_part  { $$ = ast_if($2,$4,$5); }
| "loop" ':' block        { $$ = ast_loop("\0", $3); }
| "loop" T_id ':' block   { $$ = ast_loop($2,$4); }
| "break"           { $$ = ast_break(NULL); }
| "break" ':' T_id  { $$ = ast_break($3); }
| "continue"          { $$ = ast_continue(NULL); }
| "continue" ':' T_id { $$ = ast_continue($3); }
| stmt stmt   { $$ = ast_seq($1,$2); }
;

if_part:
  %empty  { $$ = NULL; }
| "elif" cond ':' block if_part   { $$ = ast_if($2,$4,$5); }
;

block:
  "begin" stmt "end"  { $$ = ast_block($2); }
| stmt T_end  { $$ = ast_block($1); }
;

proc_call:
  T_id                    { $$ = ast_proc_call($1,NULL,NULL); }
| T_id ':' expr expr_part { $$ = ast_proc_call($1,$3,$4); }
;

expr_part:
  %empty  { $$ = NULL; }
| ',' expr expr_part   { $$ = ast_expr_part($2,$3); }
;

func_call:
  T_id '(' ')'                { $$ = ast_func_call($1,NULL,NULL); }
| T_id '(' expr expr_part ')' { $$ = ast_func_call($1,$3,$4); }
;

l_value:
  T_id    { $$ = ast_tid($1); }
| T_string  { $$ = ast_string_lit($1); }
| l_value '[' expr ']'  { $$ = ast_arr($1,$3); }
;

expr:
  T_intconst    { $$ = ast_int_const($1); }
| T_char        { $$ = ast_char_const($1); }
| l_value       { $$ = $1; }
| '(' expr ')'  { $$ = $2; }
| func_call     { $$ = $1; }
| '+' expr      { $$ = ast_op(ast_int_const(0),PLUS,$2); }  %prec UPLUS
| '-' expr      { $$ = ast_op(ast_int_const(0),MINUS,$2); } %prec UMINUS
| expr '+' expr { $$ = ast_op($1,PLUS,$3); }
| expr '-' expr { $$ = ast_op($1,MINUS,$3); }
| expr '*' expr { $$ = ast_op($1,TIMES,$3); }
| expr '/' expr { $$ = ast_op($1,DIV,$3); }
| expr '%' expr { $$ = ast_op($1,MOD,$3); }
| "true"        { $$ = ast_char_const('\x01'); }
| "false"       { $$ = ast_char_const('\0'); }
| '!' expr      { $$ = ast_not_expr($2); }
| expr '&' expr { $$ = ast_and_expr($1,$3); }
| expr '|' expr { $$ = ast_or_expr($1,$3); }
;

cond:
  expr    { $$ = $1; }
| x_cond  { $$ = $1; }
;

x_cond:
  '(' x_cond ')'  { $$ = $2; }
| "not" cond      { $$ = ast_op(NULL,NOT,$2); }
| cond "and" cond { $$ = ast_op($1,AND,$3); }
| cond "or" cond  { $$ = ast_op($1,OR,$3); }
| expr '=' expr   { $$ = ast_op($1,EQ,$3); }
| expr "<>" expr  { $$ = ast_op($1,NEQ,$3); }
| expr T_less expr   { $$ = ast_op($1,LT,$3); }
| expr T_greater expr   { $$ = ast_op($1,GT,$3); }
| expr "<=" expr  { $$ = ast_op($1,LE,$3); }
| expr ">=" expr  { $$ = ast_op($1,GE,$3); }
;

%%

void yyerror (const char *msg) {
  fprintf(stderr, "Dana error: %s\n", msg);
  fprintf(stderr, "Aborting, I've had enough with line %d...\n",linenumber);
  //fprintf(stderr, "text is %s\n", yytext);
  exit(1);
}

int main() {
    if (yyparse()) return 1;
    printf("Parsing was succesful!\n");
    initSymbolTable(997);
    openScope();
    set_lib_functions();
    ast_sem(t);
    closeScope();
    //ast_run(t);
    destroySymbolTable();
    return 0;
}
