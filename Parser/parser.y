%{
#include <stdio.h>
#include <stdlib.h>
//#include "ast.h"
//#include "symbol.h"
#include <string.h>
//#include <vector>
//#include <string>

void yyerror (const char *msg);

//vector<string> *ids; 
extern int linenumber;
//ast t;
%}

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
%token T_name 
%token T_char
%token T_intconst
%token T_string
%token T_id
%token T_char_const
%token T_string_literal
%token T_auto_end

%left '+' '-'
%left '*' '/' '%'
%left UMINUS UPLUS

%%


program:
  func_def
;

func_def:
  "def" header local_def block
;

header:
  T_id                                          
| T_id "is" data_type                           
| T_id ':' fpar_def header_part                 
| T_id "is" data_type ':' fpar_def header_part  
;

header_part:
  %empty
| ',' fpar_def header_part
;

fpar_def:
  id "as" fpar_type
;

data_type:
  "int"   
| "byte"  
;

type:
  data_type                 
| type '[' T_intconst ']'   
;

fpar_type:
  type           
| "ref" data_type
| fpar_part      
;

fpar_part:
  data_type '[' ']'
| data_type '[' T_intconst ']'  
| fpar_part '[' T_intconst ']'  
;

local_def:
  %empty
| func_def local_def    
| func_decl local_def   
| var_def local_def
;

func_decl:
  "decl" header
;

var_def:
  "var" id "is" type
;

id:
  T_id
| T_id id
;

stmt:
  "skip"            
| l_value ":=" expr 
| proc_call         
| "exit"            
| "return" ':' expr 
| "if" cond ':' block if_part "else" ':' block  
| "if" cond ':' block if_part                   
| "loop" ':' block        
| "loop" T_id ':' block    
| "break"                  
| "break" ':' T_id        
| "continue"              
| "continue" ':' T_id     
| stmt stmt
;

if_part:
  %empty
| "elif" cond ':' block if_part
;

block:
  "begin" stmt "end"
| stmt T_end
;

proc_call:
  T_id                    
| T_id ':' expr expr_part 
;

expr_part:
  %empty  
| ',' expr expr_part   
;

func_call:
  T_id '(' ')'                
| T_id '(' expr expr_part ')' 
;

l_value:
  T_id                  
| T_string              
| l_value '[' expr ']'
;

expr:
  T_intconst    
| T_char        
| l_value       
| '(' expr ')'  
| func_call     
| '+' expr      
| '-' expr      
| expr '+' expr 
| expr '-' expr 
| expr '*' expr 
| expr '/' expr 
| expr '%' expr 
| "true"        
| "false"       
| '!' expr      
| expr '&' expr 
| expr '|' expr 
;

cond:
  expr    
| x_cond  
;

x_cond:
  '(' x_cond ')'  
| "not" cond      
| cond "and" cond 
| cond "or" cond  
| expr '=' expr   
| expr "<>" expr  
| expr T_less expr   
| expr T_greater expr   
| expr "<=" expr  
| expr ">=" expr  
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
    return 0;
}
