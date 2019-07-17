#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "MYast.h"

static ast ast_make (kind k, char *c, int n, ast l1, ast l2, ast l3, ast l4, Type t) {
	//printf("another node in ast\n");
	ast p;
	if ((p = malloc(sizeof(struct node))) == NULL) exit(1);
	p->k = k;
	p->id = (char *) malloc(sizeof(char)*(strlen(c)+1));
	//p->id = c;
	strcpy(p->id, c);
	p->num = n;
	p->branch1 = l1;
	p->branch2 = l2;
	p->branch3 = l3;
	p->branch4 = l4;    //each node in the ast has <=4 children
	p->type = t;
	//printf("node created\n");
	return p;
}

ast ast_func_def(ast header, ast local, ast block) {
  return ast_make(FUNC_DEF,"\0", 0, header, local, block, NULL, NULL);
}

ast ast_header(char *l1, Type t, ast l2, ast l3) {
  return ast_make(HEADER, l1, 0, l2, l3, NULL, NULL, t);
}

ast ast_header_part(ast l1, ast l2) {
  return ast_make(HEADER_PART, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_fpar_def(ast l1, Type t) {
  return ast_make(FPAR_DEF, "\0", 0, l1, NULL, NULL, NULL, t);
}

// ast ast_func_decl(ast l1) {
//   return ast_make(FUNC_DECL, "\0", 0, l1, NULL, NULL, NULL, NULL);
// }

ast ast_decl (ast l1) {
  return ast_make(DECL, "\0", 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_var(ast idlist, Type t) {
  return ast_make(VAR, "\0", 0, idlist, NULL, NULL, NULL, t);
}

ast ast_id(char *s, ast next) {
  return ast_make(ID, s, 0, next, NULL, NULL, NULL, NULL);
}

ast ast_op (ast l, kind op, ast r) {
  return ast_make(op, "\0", 0, l, r, NULL, NULL, NULL);
}

ast ast_skip() {
  return ast_make(SKIP, "\0", 0, NULL, NULL, NULL, NULL, NULL);
}

//assign a value to a variable
ast ast_assign(ast l, ast r) {
  return ast_make(ASSIGN, "\0", 0, l, r, NULL, NULL, NULL);
}

ast ast_exit() {
  return ast_make(EXIT, "\0", 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_return(ast l1) {
  return ast_make(RET, "\0", 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_if (ast l1, ast l2, ast l3) {
  return ast_make(IF, "\0", 0, l1, l2, l3, NULL, NULL);
}

ast ast_if_else (ast cond, ast blck, ast ifp, ast elblck) {
  return ast_make(IF_ELSE, "\0", 0, cond, blck, ifp, elblck, NULL);
}

ast ast_loop(char *s, ast l1) {
  return ast_make(LOOP, s, 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_break(char *s) {
  if (s == NULL) return ast_make(BREAK, "\0", 0, NULL, NULL, NULL, NULL, NULL);
  return ast_make(BREAK, s, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_continue(char *s) {
  if (s == NULL) return ast_make(CONT, "\0", 0, NULL, NULL, NULL, NULL, NULL);
  return ast_make(CONT, s, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_seq(ast l1, ast l2) {
  return ast_make(SEQ, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_block(ast l1) {
  return ast_make(BLOCK, "\0", 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_tid(char *s) {
  return ast_make(TID, s, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_string_lit(char *s) {
  return ast_make(STRING_LIT, s, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_arr(ast l1, ast l2) {
  return ast_make(ARR, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_int_const(int num) {
  return ast_make(INTCONST, "\0", num, NULL, NULL, NULL, NULL, NULL);
}

ast ast_char_const(char c) {
  char name[2];
  name[0] = c;
  return ast_make(CHARCONST, name, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_proc_call(char *s, ast l1, ast l2) {
  return ast_make(PROC_CALL, s, 0, l1, l2, NULL, NULL, NULL);
}

ast ast_func_call(char *s, ast l1, ast l2) {
  return ast_make(FUNC_CALL, s, 0, l1, l2, NULL, NULL, NULL);
}

ast ast_expr_part(ast l1, ast l2) {
  return ast_make(EXPR_PART, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_not_expr(ast l1) {
  return ast_make(EXPR_NOT, "\0", 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_and_expr(ast l1, ast l2) {
  return ast_make(EXPR_AND, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_or_expr(ast l1, ast l2) {
  return ast_make(EXPR_OR, "\0", 0, l1, l2, NULL, NULL, NULL);
}
//--------END OF COPIED STUFF FROM COMPILER----------------------------------------

struct rule_t {
    char *name;
    kind k;
    struct rule_t *rules;
};

int randomNum(int low, int high) {
    int num = (rand() % (high - low + 1)) + low;
    return num;
}

char *randomName(int length) {
    char *name = (char *) malloc(sizeof(char)*(length+1) );
    for(int i=0; i < length; i++) {
        char letter = 'a' + (rand() % 26 );
        name[i] = letter; 
    }
    name[length] = '\0';
    return name;
}

ast tree;
int TABS = 0;
Type functionType[10];
int sp = -1;

//push the function's type in the functionType stack
void pushType(Type t) {
    sp++;
    functionType[sp] = t;
    return;
}

//when we finish writing a grammar rule we enter newline and indent properly
void finishRule() {
    int i;
    printf("\n");
    for(i=0; i < TABS; i++) {
        printf("\t");
    }
}

//print code from given AST
void print_code_from_AST(ast t) {
    if (t == NULL) return;
    switch (t->k) {
    case FUNC_DEF: {
        printf("def ");
        //header
        print_code_from_AST(t->branch1);
        TABS++;
        finishRule();
        //local def
        print_code_from_AST(t->branch2);
        //block
        print_code_from_AST(t->branch3);
        TABS--;
        finishRule();
        return;
    }
    case HEADER: {
        printf("%s ", t->id);
        if (!equalType(t->type, typeVoid)) {
            if (equalType(t->type, typeInteger)) printf("is int ");
            else printf("is byte ");
        }
        if (t->branch1 != NULL) {
            printf(": ");
            print_code_from_AST(t->branch1);
        }
        print_code_from_AST(t->branch2);
        return;
    }
    case DECL: {
        printf("decl ");
        print_code_from_AST(t->branch1);
        return;
    }
    case VAR: {
        printf("var ");
        print_code_from_AST(t->branch1);
        printf("is ");
        printType(t->type);
        return;
    }
    case ID: {
        ast temp;
        printf("%s ", t->id);
        for(temp = t->branch1; temp != NULL; temp = temp->branch1) {
            printf("%s ", temp->id);
        }
        return;
    }
    case SKIP: {
        printf("skip");
        return;
    }
    case ASSIGN: {
        print_code_from_AST(t->branch1);
        printf(" := ");
        print_code_from_AST(t->branch2);
        return;
    }
    case EXIT: {
        printf("exit ");
        return;
    }
    case RET: {
        printf("return: ");
        print_code_from_AST(t->branch1);
        return;
    }
    case IF:
    {
        TABS++;
        printf("if ");
        //condition
        print_code_from_AST(t->branch1);
        printf(": ");
        //if-block
        finishRule();
        print_code_from_AST(t->branch2);
        TABS--;
        finishRule();
        //elif (if it exists) - NOT SUPPORTED RIGHT NOW
        //print_code_from_AST(t->branch3);
        return;
    }
    case IF_ELSE:
    {
        TABS++;
        printf("if ");
        //condition
        print_code_from_AST(t->branch1);
        printf(": ");
        //if-block
        finishRule();
        print_code_from_AST(t->branch2);
        TABS--;
        finishRule();

        printf("else: ");
        TABS++;
        //else-block
        print_code_from_AST(t->branch4);
        TABS--;
        finishRule();
        return;
    }
    case LOOP:
    {
        printf("loop: ");
        TABS++;
        finishRule();
        print_code_from_AST(t->branch1);
        TABS--;
        finishRule();
        return;
    }
    case BREAK: {
        printf("break ");
        return;
    }
    case CONT: {
        printf("continue ");
        return;
    }
    case SEQ: {
        print_code_from_AST(t->branch1);
        finishRule();
        print_code_from_AST(t->branch2);
        return;
    }
    case BLOCK: {
        print_code_from_AST(t->branch1);
        return;
    }
    case PROC_CALL: {
        printf("%s ", t->id);
        if (t->branch1 != NULL) {
            printf(": ");
            print_code_from_AST(t->branch1);
            print_code_from_AST(t->branch2);
        }
        return;
    }
    case FUNC_CALL: {
        printf("%s (", t->id);
        print_code_from_AST(t->branch1);
        print_code_from_AST(t->branch2);
        printf(")");
        return;
    }
    case TID: {
        printf("%s ", t->id);
        return;
    }
    case STRING_LIT: {
        printf("\"%s\" ", t->id);
        return;
    }
    case ARR: {
        print_code_from_AST(t->branch1);
        printf(" [");
        print_code_from_AST(t->branch2);
        printf("]");
        return;
    }
    case INTCONST: {
        printf("%d", t->num);
        return;
    }
    case CHARCONST: {
        printf("%c", (t->id)[0]);
        return;
    }
    case PLUS: 
    {
        print_code_from_AST(t->branch1);
        printf(" + ");
        print_code_from_AST(t->branch2);
        return;
    }
    case MINUS:
    {
        print_code_from_AST(t->branch1);
        printf(" - ");
        print_code_from_AST(t->branch2);
        return;
    }
    case TIMES:
    {
        print_code_from_AST(t->branch1);
        printf(" * ");
        print_code_from_AST(t->branch2);
        return;
    }
    case DIV:
    {
        print_code_from_AST(t->branch1);
        printf(" / ");
        print_code_from_AST(t->branch2);
        return;
    }
    case MOD:
    {
        // print_code_from_AST(t->branch1);
        // printf(" mod ");
        // print_code_from_AST(t->branch2);
        return;
    }
    case NOT:
    case AND:
    {
        print_code_from_AST(t->branch1);
        printf(" and ");
        print_code_from_AST(t->branch2);
        return;
    }
    case OR:
    {
        print_code_from_AST(t->branch1);
        printf(" or ");
        print_code_from_AST(t->branch2);
        return;
    }
    case EQ:
    {
        print_code_from_AST(t->branch1);
        printf(" = ");
        print_code_from_AST(t->branch2);
        return;
    }
    case LT:
    {
        print_code_from_AST(t->branch1);
        printf(" < ");
        print_code_from_AST(t->branch2);
        return;
    }
    case GT:
    {
        print_code_from_AST(t->branch1);
        printf(" > ");
        print_code_from_AST(t->branch2);
        return;
    }
    case LE:
    {
        print_code_from_AST(t->branch1);
        printf(" <= ");
        print_code_from_AST(t->branch2);
        return;
    }
    case GE:
    {
        print_code_from_AST(t->branch1);
        printf(" >= ");
        print_code_from_AST(t->branch2);
        return;
    }
    case NEQ:
    {
        print_code_from_AST(t->branch1);
        printf(" <> ");
        print_code_from_AST(t->branch2);
        return;
    }
    case EXPR_NOT:
    case EXPR_AND:
    {
        print_code_from_AST(t->branch1);
        printf(" & ");
        print_code_from_AST(t->branch2);
        return;
    }
    case EXPR_OR:
    {
        print_code_from_AST(t->branch1);
        printf(" | ");
        print_code_from_AST(t->branch2);
        return;
    }
    default:
        break;
    }
    return;
}

//create an AST with random values
ast createAST(kind k) {
    switch (k) {
    case FUNC_DEF: {
        char *id;
        ast header, local, block;
        //ast header = ast_header("h1", HEADER, NULL, NULL);
        header = createAST(HEADER);
        local = createAST(SEQ);
        block = createAST(BLOCK);
        ast t = ast_func_def(header, local, block);
        closeScope();
        return t;
    }
    case HEADER: {
        openScope();
        char *functionName;
        functionName = randomName(4);

        ast l2 = createAST(FPAR_DEF);
        //ast l3 = createAST(HEADER_PART);
        ast l3 = NULL;

        int type_choice = randomNum(0,2);
        Type ty;
        if (type_choice == 0) ty = typeVoid;
        else if (type_choice == 1) ty = typeInteger;
        else ty = typeChar;

        return ast_header(functionName, ty, l2, l3);
    }
    case FPAR_DEF: {
        Type ty;
        int type_choice = randomNum(0,1);
        if (type_choice == 0) ty = typeInteger;
        else ty = typeChar;
        char *var = randomName(3);
        return ast_fpar_def(ast_id(var, NULL), ty);
    }
    case DECL: {
        ast header = createAST(HEADER);
        closeScope();
        return ast_decl(header);
    }
    case VAR: {
        ast list = createAST(ID);
        int type_choice = randomNum(0,2);
        Type ty;
        if (type_choice == 0) ty = typeVoid;
        else if (type_choice == 1) ty = typeInteger;
        else ty = typeChar;
        return ast_var(list, ty);
    }
    case ID: {
        int num = randomNum(1, 3);
        int i;
        ast list, end;
        list = NULL;
        end = NULL;
        for(i=0; i < num; i++) {
            char *name = randomName(3);
            if (end == NULL) {
                list = ast_id(name, NULL);
                end = list;
            }
            else {
                ast temp = ast_id(name, NULL);
                end->branch1 = temp;
                end = temp;
            }
        }
        return list;
    }
    case SKIP: {
        return ast_skip();
    }
    case ASSIGN:
    case EXIT: {
        if (equalType(functionType[sp], typeVoid)) return ast_exit();
        else return NULL;
    }
    case RET: {
        if (equalType(functionType[sp], typeVoid)) return NULL;
        else {
            ast retval;
            int type_choice = randomNum(0,1);
            if (type_choice == 0) retval = createAST(INTCONST);
            else retval = createAST(CHARCONST);
        }
    }
    case IF:
    case IF_ELSE:
    case LOOP:
    case BREAK:
    case CONT:
    case SEQ:
    case BLOCK:
    case PROC_CALL:
    case FUNC_CALL:
    case TID:
    case STRING_LIT:
    case ARR:
    case INTCONST: {
        int num = randomNum(0, 20);
        return ast_int_const(num);
    }
    case CHARCONST: {
        char c = 'a' + (rand() % 26 );
        return ast_char_const(c);
    }
    case PLUS:
    case MINUS:
    case TIMES:
    case DIV:
    case MOD:
    case NOT:
    case AND:
    case OR:
    case EQ:
    case LT:
    case GT:
    case LE:
    case GE:
    case NEQ:
    case EXPR_NOT:
    case EXPR_AND:
    case EXPR_OR:
    default:
        break;
    }
    return NULL;
}

int main() {
    int i;
    srand(time(0));

    //simple example
    ast func, blck, sequence, simple_num;
    simple_num = ast_int_const(42);
    sequence = ast_seq(ast_skip(), ast_return(simple_num));
    blck = ast_block(sequence);
    ast head = ast_header("main", typeInteger, NULL, NULL);
    func = ast_func_def(head, NULL, blck);
    print_code_from_AST(func);

    return 0;
}