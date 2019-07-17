#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
//#include <cstdio>
//#include <cstdlib>
#include "ast.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#if defined(LLVM_VERSION_MAJOR) && LLVM_VERSION_MAJOR >= 4
#include <llvm/Transforms/Scalar/GVN.h>
#endif
extern "C"
{
#include "error.h"
#include "symbol.h"
}

using namespace llvm;
Value* ast_compile(ast t);

static ast ast_make(kind k, const char *c, int n, ast l1, ast l2, ast l3, ast l4, Type_h t)
{
    //printf("another node in ast\n");
    ast p;
    if ((p = (ast) malloc(sizeof(struct node))) == NULL)
        exit(1);
    p->k = k;
    p->id = (char *) malloc(sizeof(char)*(strlen(c) + 1));
    //p->id = c;
    strcpy(p->id, c);
    p->num = n;
    p->branch1 = l1;
    p->branch2 = l2;
    p->branch3 = l3;
    p->branch4 = l4; //each node in the ast has <=4 children
    p->type = t;
    //printf("node created\n");
    return p;
}


ast ast_func_def(ast header, ast local, ast block)
{
    return ast_make(FUNC_DEF, "\0", 0, header, local, block, NULL, NULL);
}

ast ast_header(const char *l1, Type_h t, ast l2, ast l3)
{
    return ast_make(HEADER, l1, 0, l2, l3, NULL, NULL, t);
}

ast ast_header_part(ast l1, ast l2)
{
    return ast_make(HEADER_PART, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_fpar_def(ast l1, Type_h t)
{
    return ast_make(FPAR_DEF, "\0", 0, l1, NULL, NULL, NULL, t);
}

// ast ast_func_decl(ast l1) {
//   return ast_make(FUNC_DECL, "\0", 0, l1, NULL, NULL, NULL, NULL);
// }

ast ast_decl(ast l1)
{
    return ast_make(DECL, "\0", 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_var(ast idlist, Type_h t)
{
    return ast_make(VAR, "\0", 0, idlist, NULL, NULL, NULL, t);
}

ast ast_id(const char *s, ast next)
{
    return ast_make(ID, s, 0, next, NULL, NULL, NULL, NULL);
}

ast ast_op(ast l, kind op, ast r)
{
    return ast_make(op, "\0", 0, l, r, NULL, NULL, NULL);
}

ast ast_skip()
{
    return ast_make(SKIP, "\0", 0, NULL, NULL, NULL, NULL, NULL);
}

//assign a value to a variable
ast ast_assign(ast l, ast r)
{
    return ast_make(ASSIGN, "\0", 0, l, r, NULL, NULL, NULL);
}

ast ast_exit()
{
    return ast_make(EXIT, "\0", 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_return(ast l1)
{
    return ast_make(RET, "\0", 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_if(ast l1, ast l2, ast l3)
{
    return ast_make(IF, "\0", 0, l1, l2, l3, NULL, NULL);
}

ast ast_if_else(ast cond, ast blck, ast ifp, ast elblck)
{
    return ast_make(IF_ELSE, "\0", 0, cond, blck, ifp, elblck, NULL);
}

ast ast_loop(const char *s, ast l1)
{
    return ast_make(LOOP, s, 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_break(const char *s)
{
    if (s == NULL)
        return ast_make(BREAK, "\0", 0, NULL, NULL, NULL, NULL, NULL);
    return ast_make(BREAK, s, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_continue(const char *s)
{
    if (s == NULL)
        return ast_make(CONT, "\0", 0, NULL, NULL, NULL, NULL, NULL);
    return ast_make(CONT, s, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_seq(ast l1, ast l2)
{
    return ast_make(SEQ, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_block(ast l1)
{
    return ast_make(BLOCK, "\0", 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_tid(const char *s)
{
    return ast_make(TID, s, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_string_lit(const char *s)
{
    return ast_make(STRING_LIT, s, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_arr(ast l1, ast l2)
{
    return ast_make(ARR, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_int_const(int num)
{
    return ast_make(INTCONST, "\0", num, NULL, NULL, NULL, NULL, NULL);
}

ast ast_char_const(char c)
{
    char name[] = {c, '\0'};
    return ast_make(CHARCONST, name, 0, NULL, NULL, NULL, NULL, NULL);
}

ast ast_proc_call(const char *s, ast l1, ast l2)
{
    return ast_make(PROC_CALL, s, 0, l1, l2, NULL, NULL, NULL);
}

ast ast_func_call(const char *s, ast l1, ast l2)
{
    return ast_make(FUNC_CALL, s, 0, l1, l2, NULL, NULL, NULL);
}

ast ast_expr_part(ast l1, ast l2)
{
    return ast_make(EXPR_PART, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_not_expr(ast l1)
{
    return ast_make(EXPR_NOT, "\0", 0, l1, NULL, NULL, NULL, NULL);
}

ast ast_and_expr(ast l1, ast l2)
{
    return ast_make(EXPR_AND, "\0", 0, l1, l2, NULL, NULL, NULL);
}

ast ast_or_expr(ast l1, ast l2)
{
    return ast_make(EXPR_OR, "\0", 0, l1, l2, NULL, NULL, NULL);
}

// Global LLVM variables related to the LLVM suite.
static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::unique_ptr<legacy::FunctionPassManager> TheFPM;

// Global LLVM variables related to the generated code.
static Function *TheWriteInteger;
static Function *TheWriteString;
static Function *TheWriteChar;
static Function *TheWriteByte;
static Function *TheReadInteger;
static Function *TheReadString;
static Function *TheReadChar;
static Function *TheReadByte;
static Function *extend;
static Function *shrink;
static Function *TheStrlen;
static Function *TheStrcmp;
static Function *TheStrcpy;
static Function *TheStrcat;
StructType *current_AR = NULL;
AllocaInst *currentAlloca = NULL;
AllocaInst *retAlloca;
BasicBlock *returnBlock;
std::multimap<char *, BasicBlock *> afterLoopMap;
std::multimap<char *, BasicBlock *> LoopMap;
std::vector<BasicBlock *> blockNames;
std::map<Value *, int> paramsIndex;
int passByReference = 0;
int loadByReference = 0;

// Useful LLVM types.
static Type *i1 = IntegerType::get(TheContext, 1);
static Type *i8 = IntegerType::get(TheContext, 8);
static Type *i32 = IntegerType::get(TheContext, 32);
static Type *i64 = IntegerType::get(TheContext, 64);

// Useful LLVM helper functions.
inline ConstantInt *c1(int n)
{
    return ConstantInt::get(TheContext, APInt(1, n, true));
}
inline ConstantInt *c8(char c)
{
    return ConstantInt::get(TheContext, APInt(8, c, true));
}
inline ConstantInt *c32(int n)
{
    return ConstantInt::get(TheContext, APInt(32, n, true));
}

Type *translateType(Type_h type)
{
    if (equalType(type, typeInteger))
        return i32;
    if (equalType(type, typeChar))
        return i8;
    if (equalType(type, typeBoolean))
        return i1;
    if (equalType(type, typeVoid))
        return Type::getVoidTy(TheContext);
    if (isArray(type))
    {
        Type_h ty = type;
        std::vector<int> array_sizes;
        while (isArray(ty))
        {
            array_sizes.push_back(ty->size);
            ty = ty->refType;
        }
        //the size of the last dimension
        //array_sizes.push_back(ty->size);

        auto base = translateType(ty);
        std::reverse(array_sizes.begin(), array_sizes.end());
        for (auto size : array_sizes)
        {
            base = ArrayType::get(base, size);
        }
        //std::cout << base << std::endl;
        //printType(type);
        return base;
    }
    if (isIArray(type)) 
    {
        Type_h ty = type;
        std::vector<int> array_sizes;
        ty = ty->refType;
        while (isArray(ty))
        {
            array_sizes.push_back(ty->size);
            ty = ty->refType;
        }
        //the size of the last dimension
        //array_sizes.push_back(ty->size);

        auto base = translateType(ty);
        std::reverse(array_sizes.begin(), array_sizes.end());
        for (auto size : array_sizes)
        {
            base = ArrayType::get(base, size);
        }
        //std::cout << base << std::endl;
        //printType(type);
        base = PointerType::get(base, 0);
        return base;
    }
    if (isPointer(type))
    {
        Type_h ty = type->refType;
        Type *ref = translateType(ty);
        return PointerType::get(ref, 0);
    }
    return nullptr;
}


//if we're in a condition we convert \x01 and \0 chars to true and false respectively
void checkForBool(ast tr)
{
    // if (equalType(tr->type, typeChar)) {
    //     if ((tr->id)[0] == '\x01') tr->type = typeBoolean;
    //     if ((tr->id)[0] == '\0') tr->type = typeBoolean;
    //     if (tr->num == 1) {
    //         tr->type = typeBoolean;
    //     }
    // }
    if (equalType(tr->type, typeInteger)) {
        if (tr->num == 0 || tr->num == 1) tr->type = typeBoolean;
    }
    return;
}


void ast_sem(ast t)
{
    if (t == NULL)
        return;
    switch (t->k)	{
    case FUNC_DEF:
    {
        //printf("entered func def\n");
        //openScope();
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        t->num_vars = currentScope->negOffset;
        ast_sem(t->branch3);

        closeScope();
        return;
    }
    case HEADER:
    {
        ;
        //printf("entered header\n");
        SymbolEntry *f = newFunction(t->id);
        openScope();

        //branch1: fpar_def , branch2: header_part i.e multiple fpar_defs
        //ast_sem(t->branch1);
        ast temp;
        Type_h parType;
        PassMode mode;
        if (t->branch1 != NULL)
        {
            temp = t->branch1; //temp now shows to fpar_def node. branch1 of that node is the beginning of the list of id nodes
            parType = temp->type;
            if (isPointer(temp->type) || isArray(temp->type) || isIArray(temp->type))
                mode = PASS_BY_REFERENCE;
            else
                mode = PASS_BY_VALUE;
            for (temp = temp->branch1; temp != NULL; temp = temp->branch1)
            {
                //we evaluate the semantics of each id node before we create the new entry. this is to avoid same name conflicts in the parameter definition
                ast_sem(temp);
                temp->type = parType;
                SymbolEntry *par = newParameter(temp->id, parType, mode, f);
            }
        }

        //ast_sem(t->branch2);

        //if t->branch2 is NULL, the following loop exits immediately so i don't need an if-check
        temp = t->branch2; //temp now points to the header_part node. branch1 is a fpar_node and branch2 is another header_part node
        ast headerpart;
        for (headerpart = t->branch2; headerpart != NULL; headerpart = headerpart->branch2)
        {
            //for each headerpart we evaluate a fpar_def node like before
            temp = headerpart->branch1; //in every iteration temp points to the fpar_def node of the header_part node
            parType = temp->type;
            if (isPointer(temp->type) || isArray(temp->type) || isIArray(temp->type))
                mode = PASS_BY_REFERENCE;
            else
                mode = PASS_BY_VALUE;
            ast idnode;
            for (idnode = temp->branch1; idnode != NULL; idnode = idnode->branch1)
            {
                ast_sem(idnode);
                idnode->type = parType;
                SymbolEntry *par = newParameter(idnode->id, parType, mode, f);
            }
        }

        endFunctionHeader(f, t->type);
        return;
    }
    case DECL:
    {
        //printf("entered decl %s\n", t->branch1->id);
        //DECL is the same as HEADER + we declare the function as forward 
        //branch1 points to header node
        //parameter declarations have their own scope
        //we assume that there are not any functions with the same name and different headers in the same scope

        //t_header points to the header of the function we wish to declare
        ast t_header = t->branch1;
        SymbolEntry *f = newFunction(t_header->id);
        forwardFunction(f);

        openScope();

        //branch1: fpar_def , branch2: header_part i.e multiple fpar_defs
        //ast_sem(t->branch1);
        ast temp;
        Type_h parType;
        PassMode mode;
        if (t_header->branch1 != NULL) {
            temp = t_header->branch1;    //temp now shows to fpar_def node. branch1 of that node is the beginning of the list of id nodes
            parType = temp->type;
            if (isPointer(temp->type) || isArray(temp->type) || isIArray(temp->type) ) mode = PASS_BY_REFERENCE;
            else mode = PASS_BY_VALUE;
            for(temp=temp->branch1; temp!=NULL; temp=temp->branch1) {
                //we evaluate the semantics of each id node before we create the new entry. this is to avoid same name conflicts in the parameter definition
                ast_sem(temp);
                temp->type = parType;
                SymbolEntry *par = newParameter(temp->id, parType, mode, f);
            }
        }

        //if t_header->branch2 is NULL the following loop exits immediatly so i don't need an if-check
        temp = t_header->branch2;    //temp now points to the header_part node. branch1 is a fpar_node and branch2 is another header_part node
        ast headerpart;
        for(headerpart = t_header->branch2; headerpart!=NULL; headerpart = headerpart->branch2) {
            //for each headerpart we evaluate a fpar_def node like before
            temp = headerpart->branch1;   //in every iteration temp points to the fpar_def node of the header_part node
            parType = temp->type;
            if (isPointer(temp->type) || isArray(temp->type) || isIArray(temp->type) ) mode = PASS_BY_REFERENCE;
            else mode = PASS_BY_VALUE;
            ast idnode;
            for(idnode=temp->branch1; idnode!=NULL; idnode=idnode->branch1) {
                ast_sem(idnode);
                idnode->type = parType;
                SymbolEntry *par = newParameter(idnode->id, parType, mode, f);
            }
        }

        endFunctionHeader(f, t_header->type);
        closeScope();

        return;
    }
    case VAR:
    {
        //var definitions
        ;
        ast temp;
        for (temp = t->branch1; temp != NULL; temp = temp->branch1)
        {
            ast_sem(temp);
            temp->type = t->type;
            SymbolEntry *v = newVariable(temp->id, t->type);
        }
        return;
    }
    case ID:
    {
        ;
        //printf("entered id -> %s\n", t->id);
        //check if there's already a variable with the same name in current scope
        char c = t->id[0];
        if (!isalpha(c))
        {
            error("variable names have to start with a letter. variable is: %s\n", t->id);
        }
        SymbolEntry *e = lookupEntry(t->id, LOOKUP_CURRENT_SCOPE, false);
        if (e != NULL)
        {
            //strcat(t->id, "\\0");
            error("there is already a variable with the name: $s\n", t->id);
        }
        return;
    }
    case SKIP:
        return;
    case ASSIGN:
    {
        //assign a value to a variable
        ast_sem(t->branch1);
        // if (isArray(t->branch1->type) || isIArray(t->branch1->type))
        //     error("cannot assign a value to an array variable");
        kind k = t->branch1->k;
        ast_sem(t->branch2);
        //check if types are the same
        if (!equalType(t->branch1->type, t->branch2->type))
        {
            if (isPointer(t->branch1->type)) {
                //check if refType of pointer is the same as the right hand value
                if(isPointer(t->branch2->type)) {
                    if (!equalType(t->branch1->type->refType,t->branch2->type->refType))
                        error("type mismatch in assigning value to variable");
                }
                else {
                    if (!equalType(t->branch1->type->refType,t->branch2->type))
                        error("type mismatch in assigning value to variable");
                }
            }
            else if (isPointer(t->branch2->type)) {
                //check if refType of pointer is the same as the right hand value
                if(isPointer(t->branch1->type)) {
                    if (!equalType(t->branch1->type->refType,t->branch2->type->refType))
                        error("type mismatch in assigning value to variable");
                }
                else {
                    if (!equalType(t->branch2->type->refType,t->branch1->type))
                        error("type mismatch in assigning value to variable");
                }
            }
            else {
                printf("left hand type is: ");
                printType(t->branch1->type);
                printf("\nright hand type is: ");
                printType(t->branch2->type);
                printf("\n");
                error("type mismatch in assigning value to variable");
            }
        }
        if (k == TID)
        {
            //we first check if a variable with that name exists
            SymbolEntry *v = lookupEntry(t->branch1->id, LOOKUP_ALL_SCOPES, true);
            if (v == NULL)
            {
                error("no variable with name %s\n", t->branch1->id);
            }
            if (!equalType(v->u.eVariable.type, t->branch1->type))
                error("type mismatch in assignment");
            t->nesting_diff = currentScope->nestingLevel - v->nestingLevel;
            t->offset = v->u.eVariable.offset;

            // if (equalType(t->branch2->type, typeChar)) {
            //     if((t->branch2->id)[0]=='\0' || (t->branch2->id)[0]=='\x01') {
            //         if (v->entryType == ENTRY_VARIABLE) v->u.eVariable.type = typeBoolean;
            //         if (v->entryType == ENTRY_PARAMETER) v->u.eParameter.type = typeBoolean;
            //     }
            // }
        }
        else if (k == ARR)
        {
            //l_value is t[n]
            ast temp = t->branch1;
            while (temp->k != ID && temp->k != TID)
                temp = temp->branch1;
            SymbolEntry *v = lookupEntry(temp->id, LOOKUP_ALL_SCOPES, true);
            t->nesting_diff = currentScope->nestingLevel - v->nestingLevel;
            t->offset = v->u.eVariable.offset;

            // if (equalType(t->branch2->type, typeChar)) {
            //     if((t->branch2->id)[0]=='\0' || (t->branch2->id)[0]=='\x01') {
            //         if (v->entryType == ENTRY_VARIABLE) v->u.eVariable.type = typeBoolean;
            //         if (v->entryType == ENTRY_PARAMETER) v->u.eParameter.type = typeBoolean;
            //     }
            // }
        }
        return;
    }
    case EXIT:
    {
        //we exit a block. it must not have a return type
        //we must make sure exit is inside a function with return type: void
        //closeScope();
        return;
    }
    case RET:
    {
        //printf("entered return\n");
        //we return an expr and leave the function
        //we must make sure return is inside a function with the same type as the return value
        ast_sem(t->branch1);
        //SymbolEntry *e;
        Scope *loop_scope;
        int found = 0;
        int foundAFunction = 0;
        for (loop_scope = currentScope; loop_scope != NULL; loop_scope = loop_scope->parent)
        {
            SymbolEntry *e;
            for (e = loop_scope->entries; e != NULL; e = e->nextInScope)
            {
                if (e->entryType == ENTRY_FUNCTION)
                {
                    foundAFunction = 1;
                    if (equalType(e->u.eFunction.resultType, t->branch1->type))
                    {
                        //we found the function
                        found = 1;
                        break;
                    }
                }
            }
            if (found)
                break;
        }
        //foundAFunction exists solely to print the correct error statement
        if (found == 0)
        {
            if (foundAFunction)
                error("function doesn't have the same type as the return value");
            else
                error("return command used but no function found");
        }
        //printf("finished return\n");
        return;
    }
    case IF:
    {
        //printf("entered if\n");
        ast_sem(t->branch1);
        // checkForBool(t->branch1);
        // if (!equalType(t->branch1->type, typeBoolean) && !equalType(t->branch1->type, typeChar)) {
        //     if (t->branch1->k != FUNC_CALL)
        //         error("if expects a boolean condition");
        // }
        ast_sem(t->branch2);

        ast_sem(t->branch3);
        return;
    }
    case IF_ELSE:
    {
        //printf("entered if else\n");
        ast_sem(t->branch1);
        // checkForBool(t->branch1);
        // if ((!equalType(t->branch1->type, typeBoolean)) && (!equalType(t->branch1->type, typeChar))) {
        //     if (t->branch1->k != FUNC_CALL)
        //         error("if expects a boolean condition");
        // }
        //openScope();
        ast_sem(t->branch2);
        //closeScope();
        ast_sem(t->branch3);
        //openScope();
        ast_sem(t->branch4);
        //closeScope();
        return;
    }
    case LOOP:
    {
        //we need to remember the loop's name if it has one
        if (strcmp(t->id, "\0") != 0)
        {
            Type_h ctype = typeArray(strlen(t->id) + 1, typeChar);
            SymbolEntry *e = newConstant(t->id, ctype, t->id);
        }

        ast_sem(t->branch1);
        return;
    }
    case BREAK:
    {
        if (strcmp(t->id, "\0") != 0)
        {
            //the break stops a specific loop
            SymbolEntry *l = lookupEntry(t->id, LOOKUP_ALL_SCOPES, true);
            if (l->entryType != ENTRY_CONSTANT)
                error("break given string that's not a loop name");
            if (l->nestingLevel > currentScope->nestingLevel)
                error("break isn't located inside the %s loop", t->id);

            t->nesting_diff = currentScope->nestingLevel - l->nestingLevel;
        }
        
        return;
    }
    case CONT:
    {
        if (strcmp(t->id, "\0") != 0)
        {
            //continue begins the next iteration of a specific loop
            SymbolEntry *l = lookupEntry(t->id, LOOKUP_ALL_SCOPES, true);
            if (l->entryType != ENTRY_CONSTANT)
                error("break given string that's not a loop name");
            if (currentScope->nestingLevel < l->nestingLevel)
                error("continue isn't located inside the %s loop", t->id);

            t->nesting_diff = currentScope->nestingLevel - l->nestingLevel;
        }
        return;
    }
    case SEQ:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        return;
    }
    case BLOCK:
    {
        //we begin a new block.
        t->num_vars = currentScope->negOffset;
        //printf("block begins\n");
        ast_sem(t->branch1);
        //printf("block ends\n");
        return;
    }
    case PROC_CALL: {
		;
		//calling a previously defined function (with no return value)
		//branch1-> expr , branch2-> expr_part (more expressions)
		//we check if an entry with the given name exists, if it's a function with void return type
		//printf("calling procedure %s\n", t->id);
		SymbolEntry *f = lookupEntry(t->id, LOOKUP_ALL_SCOPES, true);
		if (f->entryType != ENTRY_FUNCTION) error("name given is not a function");
		if (!equalType(f->u.eFunction.resultType, typeVoid)) error("type mismatch, called function is not void");
		t->type = typeVoid;
		ast_sem(t->branch1);
		if (t->branch1 == NULL) {
            if (f->u.eFunction.firstArgument != NULL) error("function has parameters and none were given");
            else {
                //function has no parameters and we called it using none, so everything's wrapped up real nice
                return;
            }
		}
		if (t->branch1 != NULL && f->u.eFunction.firstArgument == NULL) error("function has no parameters, however some were given");
		
		SymbolEntry *params = f->u.eFunction.firstArgument;
		if (!equalType(t->branch1->type, params->u.eParameter.type)) {
            //if the type of the typical parameter is an IArray then it can be matched with an Arraytype of any length
            //if (isIArray(t->branch1->type) && isArray(params->u.eParameter.type)) ;
            if (isArray(t->branch1->type) && isIArray(params->u.eParameter.type)) {
                //we need to check if the referenced types of the arrays match
                if (!equalType(t->branch1->type->refType, params->u.eParameter.type->refType))
                    error("parameter type mismatch");
            }
            else if (isPointer(params->u.eParameter.type)) {
                //if parameter is passed by reference we check if argument is l_value
                if ((t->branch1->k != TID) && (t->branch1->k != ARR) && (t->branch1->k != STRING_LIT))
                    error("parameter type mismatch");
            }
            else error("parameter type mismatch");
		}
		if ((params->u.eParameter.mode == PASS_BY_REFERENCE) && (t->branch1->k != TID) 
			&& (t->branch1->k != ARR) && (t->branch1->k != STRING_LIT))
		error("parameter passing mode mismatch");

		ast temp = t->branch2;
		params = params->u.eParameter.next;
		//we check each real parameter to see if they match with the function's typical parameters
		while(temp != NULL && params != NULL) {
			ast_sem(temp->branch1);

			if (!equalType(temp->branch1->type, params->u.eParameter.type)) {
				//if one of the types is an IArray then it can be matched with an Arraytype of any length
				if (isArray(temp->branch1->type) && isIArray(params->u.eParameter.type)) {
				//we need to check if the referenced types of the arrays match
				    if (!equalType(temp->branch1->type->refType, params->u.eParameter.type->refType))
					    error("parameter type mismatch");
				}
                else if (isPointer(params->u.eParameter.type)) {
                    //if parameter is passed by reference we check if argument is l_value
                    if ((temp->branch1->k != TID) && (temp->branch1->k != ARR ) && (temp->branch1->k != STRING_LIT))
                        error("parameter type mismatch");
                }
				else error("parameter type mismatch");
			}
			if ((params->u.eParameter.mode == PASS_BY_REFERENCE) && (temp->branch1->k != TID) 
				&& (temp->branch1->k != ARR ) && (temp->branch1->k != STRING_LIT))
				    error("parameter passing mode mismatch");
			params = params->u.eParameter.next;
			temp = temp->branch2;
		}

		if (temp!=NULL && params==NULL) error("proc call was given too many parameters");
		if (temp==NULL && params!=NULL) error("proc call was given too few parameters");

        t->nesting_diff = currentScope->nestingLevel - f->nestingLevel;
		//printf("leaving procedure %s\n", t->id);
		return;
    }
	case FUNC_CALL: {
		;
		//printf("calling function %s\n", t->id);
		//calling a previously defined function (with return value)
		//branch1-> expr , branch2-> expr_part (more expressions)
		//we check if an entry with the given name exists, if it's a function with non-void return type
		SymbolEntry *f = lookupEntry(t->id, LOOKUP_ALL_SCOPES, true);
		if (f->entryType != ENTRY_FUNCTION) error("name given is not a function");
		if (equalType(f->u.eFunction.resultType, typeVoid)) error("type mismatch, called function is void");

		//we make the type of ast node the same as the function's. this is needed in order to use the called function in math calculations
		t->type = f->u.eFunction.resultType;
		ast_sem(t->branch1);
		if (t->branch1 == NULL) {
			if (f->u.eFunction.firstArgument != NULL) error("function has parameters and none were given");
			else {
			    //function has no parameters and we called it using none, so everything's wrapped up real nice
			    return;
			}
		}
		if (t->branch1 != NULL && f->u.eFunction.firstArgument == NULL) error("function has no parameters, however some were given");

		SymbolEntry *params = f->u.eFunction.firstArgument;
		if (!equalType(t->branch1->type, params->u.eParameter.type)) {
			//if one of the types is an IArray then it can be matched with an Arraytype of any length
			if (isArray(t->branch1->type) && isIArray(params->u.eParameter.type)) {
                //we need to check if the referenced types of the arrays match
                if (!equalType(t->branch1->type->refType, params->u.eParameter.type->refType))
                    error("parameter type mismatch");
			}
            else if (isPointer(params->u.eParameter.type)) {
                //if parameter is passed by reference we check if argument is l_value
                if ((t->branch1->k != TID) && (t->branch1->k != ARR) && (t->branch1->k != STRING_LIT))
                    error("parameter type mismatch");
            }
			else error("parameter type mismatch");
		}
		if ((params->u.eParameter.mode == PASS_BY_REFERENCE) && (t->branch1->k != TID) 
			&& (t->branch1->k != ARR) && (t->branch1->k != STRING_LIT))
			error("parameter passing mode mismatch");

		ast temp = t->branch2;
		params = params->u.eParameter.next;
		//we check each real parameter to see if they match with the function's typical parameters
		while(temp!=NULL && params!=NULL) {
			ast_sem(temp->branch1);
			if (!equalType(temp->branch1->type, params->u.eParameter.type)) {
                //if one of the types is an IArray then it can be matched with an Arraytype of any length
                if (isArray(temp->branch1->type) && isIArray(params->u.eParameter.type)) {
                    //we need to check if the referenced types of the arrays match
                    if (!equalType(temp->branch1->type->refType, params->u.eParameter.type->refType))
                    error("parameter type mismatch");
                }
                else if (isPointer(params->u.eParameter.type)) {
                        //if parameter is passed by reference we check if argument is l_value
                        if ((temp->branch1->k != TID) && (temp->branch1->k != ARR ) && (temp->branch1->k != STRING_LIT))
                            error("parameter type mismatch");
                }
                else error("parameter type mismatch");
			}
			if ((params->u.eParameter.mode == PASS_BY_REFERENCE) && (temp->branch1->k != TID) 
				&& (temp->branch1->k != ARR ) && (temp->branch1->k != STRING_LIT))
			error("parameter passing mode mismatch");
			params = params->u.eParameter.next;
			temp = temp->branch2;
		}

		if (temp!=NULL && params==NULL) error("func call was given too many parameters");
		if (temp==NULL && params!=NULL) error("func call was given too few parameters");

        t->nesting_diff = currentScope->nestingLevel - f->nestingLevel;
		//printf("finished function call\n");
		return;
	}
    case TID:
    {
        ;
        //printf("accesing variable %s\n", t->id);
        char c1 = t->id[0];
        if (!isalpha(c1))
        {
            error("variable names have to start with a letter");
        }
        SymbolEntry *e = lookupEntry(t->id, LOOKUP_ALL_SCOPES, true);
        if (e->entryType == ENTRY_PARAMETER)
        {
            t->type = e->u.eParameter.type;
            t->nesting_diff = currentScope->nestingLevel - e->nestingLevel;
            t->offset = e->u.eParameter.offset;
            //printf("parameter %s nesting diff is %d\n", t->id, t->nesting_diff);
            //printf("parameter %s offset is: %d\n", t->id, t->offset);
        }
        else if (e->entryType == ENTRY_VARIABLE)
        {
            t->type = e->u.eVariable.type;
            t->nesting_diff = currentScope->nestingLevel - e->nestingLevel;
            t->offset = e->u.eVariable.offset;
            //printf("variable %s nesting diff is %d\n", t->id, t->nesting_diff);
            //printf("variable %s offset is: %d\n", t->id, t->offset);
        }
        return;
    }
    case STRING_LIT:
    {
        ;
        //string constant
        int len = strlen(t->id);
        // char *strconst;
        // strconst = (char *) malloc(sizeof(char)*(len + 1);
        // strcpy(strconst, t->id);
        t->type = typeArray(len + 1, typeChar);
        SymbolEntry *e = lookupEntry(t->id, LOOKUP_ALL_SCOPES, false);
        if (e == NULL) {
            Type_h ctype = typeArray(len + 1, typeChar);
            e = newConstant(t->id, ctype, t->id);
        }
        if (e->entryType == ENTRY_CONSTANT) {
            t->nesting_diff = currentScope->nestingLevel - e->nestingLevel;
            t->offset = e->u.eVariable.offset;
        }
        return;
    }
    case ARR:
    {
        //branch1-> l_value, branch2-> expr
        //case is an access to a[i] (element i of array a)
        //we check if 'a' is an array, if it exists, if i is int and if 0 <= i < N , N being the size of the array
        ast_sem(t->branch1);
        ast temp = t->branch1;
        while ((temp->k != TID) && (temp->k != STRING_LIT))
            temp = temp->branch1;
        SymbolEntry *e2 = lookupEntry(temp->id, LOOKUP_ALL_SCOPES, true);
        if (!isArray(t->branch1->type) && !isIArray(t->branch1->type))
            error("l_value is not an array");
        
        ast_sem(t->branch2);
        if (!equalType(t->branch2->type, typeInteger)) 
        {
            //we check if branch1 is a pointer
            if (isPointer(t->branch2->type)) {
                Type_h indexType = t->branch2->type;
                if (!equalType(indexType->refType, typeInteger)) 
                    error("tried to access an array with index not being integer");
            }
            else error("tried to access an array with index not being integer");
        }
        else {
            if (t->branch2->num < 0)
                error("index below 0");
            if (isArray(t->branch1->type))
            {
                if (t->branch2->num >= t->branch1->type->size)
                    error("index exceeds array size");
            }
        }

        t->type = t->branch1->type->refType;
        return;
    }
    case INTCONST:
    {
        //printf("number %d\n", t->num);
        t->type = typeInteger;
        return;
    }
    case CHARCONST:
    {
        //characters \x01 and \0 represent the true and false keywords. but we don't know if they will be used as bool or char
        //solution: we consider them chars for now unless we notice that they are used in a condition

        // if (strcmp(t->id, "\0")==0) t->type = typeBoolean;
        // else if(strcmp(t->id, "\x01")==0) t->type = typeBoolean;
        // else t->type = typeChar;
        t->type = typeChar;
        return;
    }
    case PLUS:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        Type_h opType;
        if (isPointer(t->branch1->type)) {
            opType = t->branch1->type;
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in + operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in + operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in + operator");
        }
        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeInteger;
            else
                error("type mismatch in + operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeChar;
            else
                error("type mismatch in + operator");
        }
        else error("no recognisable type in + operation");
        return;
    }
    case MINUS:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in - operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in - operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in - operator");
        }
        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeInteger;
            else
                error("type mismatch in - operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeChar;
            else
                error("type mismatch in - operator");
        }
        return;
    }
    case TIMES:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in * operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in * operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in * operator");
        }

        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeInteger;
            else
                error("type mismatch in * operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeChar;
            else
                error("type mismatch in * operator");
        }
        else
            error("unknown type for * operation");
        return;
    }
    case DIV:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in / operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in / operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in / operator");
        }

        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeInteger;
            else
                error("type mismatch in / operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeChar;
            else
                error("type mismatch in / operator");
        }
        return;
    }
    case MOD:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in % operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in % operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in % operator");
        }

        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeInteger;
            else
                error("type mismatch in mod operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeChar;
            else
                error("type mismatch in mod operator");
        }
        return;
    }
    case NOT:
    {
        ast_sem(t->branch2);
        //checkForBool(t->branch2);
        // if (!equalType(t->branch2->type, typeBoolean)) {
        //     printType(t->branch2->type);
        //     printf(" %s\n", t->branch2->id);
        //     error("type mismatch in not operation");
        // }
        t->type = typeBoolean;
        return;
    }
    case AND:
    {
        ast_sem(t->branch1);
        //checkForBool(t->branch1);
        ast_sem(t->branch2);
        //checkForBool(t->branch2);
        // if ((!equalType(t->branch1->type, typeBoolean)) || (!equalType(t->branch2->type, typeBoolean)))
        //     error("type mismatch in and operator");
        t->type = typeBoolean;
        return;
    }
    case OR:
    {
        ast_sem(t->branch1);
        //checkForBool(t->branch1);
        ast_sem(t->branch2);
        //checkForBool(t->branch2);
        // if ((!equalType(t->branch1->type, typeBoolean)) || (!equalType(t->branch2->type, typeBoolean)))
        //     error("type mismatch in or operator");
        t->type = typeBoolean;
        return;
    }
    case EQ:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in = operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in = operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in = operator");
        }

        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeBoolean;
            else
                error("type mismatch in = operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeBoolean;
            else
                error("type mismatch in = operator");
        }
        t->type = typeBoolean;
        return;
    }
    case LT:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in = operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in = operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in = operator");
        }

        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeBoolean;
            else
                error("type mismatch in < operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeBoolean;
            else
                error("type mismatch in < operator");
        }
        t->type = typeBoolean;
        return;
    }
    case GT:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in = operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in = operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in = operator");
        }

        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeBoolean;
            else
                error("type mismatch in > operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeBoolean;
            else
                error("type mismatch in > operator");
        }
        t->type = typeBoolean;
        return;
    }
    case LE:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in = operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in = operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in = operator");
        }

        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeBoolean;
            else
                error("type mismatch in <= operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeBoolean;
            else
                error("type mismatch in <= operator");
        }
        t->type = typeBoolean;
        return;
    }
    case GE:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in = operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in = operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in = operator");
        }

        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeBoolean;
            else
                error("type mismatch in >= operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeBoolean;
            else
                error("type mismatch in >= operator");
        }
        t->type = typeBoolean;
        return;
    }
    case NEQ:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if (isPointer(t->branch1->type)) 
        {
            if (isPointer(t->branch2->type)) {
                if (equalType(t->branch1->type, t->branch2->type)) t->type = t->branch1->type->refType;
                else error("type mismatch in = operator");
            }
            else if (equalType(t->branch1->type->refType, t->branch2->type)) {
                t->type = t->branch2->type;
            }
            else error("type mismatch in = operator");
        }
        else if (isPointer(t->branch2->type)) {
            if (equalType(t->branch1->type, t->branch2->type->refType)) t->type = t->branch1->type;
            else error("type mismatch in = operator");
        }

        else if (equalType(t->branch1->type, typeInteger))
        {
            if (equalType(t->branch2->type, typeInteger))
                t->type = typeBoolean;
            else
                error("type mismatch in <> operator");
        }
        else if (equalType(t->branch1->type, typeChar))
        {
            if (equalType(t->branch2->type, typeChar))
                t->type = typeBoolean;
            else
                error("type mismatch in <> operator");
        }
        t->type = typeBoolean;
        return;
    }
    case EXPR_NOT:
    {
        //not operation for expressions
        ast_sem(t->branch1);
        if (!equalType(t->branch1->type, typeChar))
            error("type mismatch in ! operation");
        t->type = typeChar;
        return;
    }
    case EXPR_AND:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if ((!equalType(t->branch1->type, typeChar)) || (!equalType(t->branch2->type, typeChar)))
            error("type mismatch in & operation");
        t->type = typeChar;
        return;
    }
    case EXPR_OR:
    {
        ast_sem(t->branch1);
        ast_sem(t->branch2);
        if ((!equalType(t->branch1->type, typeChar)) || (!equalType(t->branch2->type, typeChar)))
            error("type mismatch in | operation");
        t->type = typeChar;
        return;
    }
    }
}

void set_lib_functions() {
  //initialize the following dana functions in the symbol table

  ast fdecl, fpardef;
  //decl writeInteger: n as int
  fpardef = ast_fpar_def(ast_id("n", NULL), typeInteger);
  fdecl = ast_header("writeInteger", typeVoid, fpardef, NULL);
  ast_sem(ast_decl(fdecl));
  
  //decl writeByte: b as byte
  fpardef = ast_fpar_def(ast_id("b", NULL), typeChar);
  fdecl = ast_header("writeByte", typeVoid, fpardef, NULL);
  ast_sem(ast_decl(fdecl));

  //decl writeChar: b as byte
  fpardef = ast_fpar_def(ast_id("b", NULL), typeChar);
  fdecl = ast_header("writeChar", typeVoid, fpardef, NULL);
  ast_sem(ast_decl(fdecl));

  //decl writeString: s as byte []
  fpardef = ast_fpar_def(ast_id("s", NULL), typeIArray(typeChar));
  fdecl = ast_header("writeString", typeVoid, fpardef, NULL);
  ast_sem(ast_decl(fdecl));

  //decl readInteger is int
  fdecl = ast_header("readInteger", typeInteger, NULL, NULL);
  ast_sem(ast_decl(fdecl));

  //decl readByte is byte
  fdecl = ast_header("readByte", typeChar, NULL, NULL);
  ast_sem(ast_decl(fdecl));

  //decl readChar is byte
  fdecl = ast_header("readChar", typeChar, NULL, NULL);
  ast_sem(ast_decl(fdecl));

  //decl readString: n as int, s as byte []
  fpardef = ast_fpar_def(ast_id("s", NULL), typeIArray(typeChar));
  ast fpardef2 = ast_header_part(fpardef, NULL);
  fpardef = ast_fpar_def(ast_id("n", NULL), typeInteger);
  fdecl = ast_header("readString", typeVoid, fpardef, fpardef2);
  ast_sem(ast_decl(fdecl));

  //decl extend is int: b as byte
  fpardef = ast_fpar_def(ast_id("b", NULL), typeChar);
  fdecl = ast_header("extend", typeInteger, fpardef, NULL);
  ast_sem(ast_decl(fdecl));

  //decl shrink is byte: n as int
  fpardef = ast_fpar_def(ast_id("n", NULL), typeInteger);
  fdecl = ast_header("shrink", typeChar, fpardef, NULL);
  ast_sem(ast_decl(fdecl));

  //decl strlen is int: s as byte []
  fpardef = ast_fpar_def(ast_id("s", NULL), typeIArray(typeChar));
  fdecl = ast_header("strlen", typeInteger, fpardef, NULL);
  ast_sem(ast_decl(fdecl));

  //decl strcmp is int: s1 s2 as byte []
  fpardef = ast_fpar_def(ast_id("s1", ast_id("s2", NULL)), typeIArray(typeChar));
  fdecl = ast_header("strcmp", typeInteger, fpardef, NULL);
  ast_sem(ast_decl(fdecl));

  //decl strcpy: trg src as byte []
  fpardef = ast_fpar_def(ast_id("trg", ast_id("src", NULL)), typeIArray(typeChar));
  fdecl = ast_header("strcpy", typeVoid, fpardef, NULL);
  ast_sem(ast_decl(fdecl));

  //decl strcat: trg src as byte []
  fpardef = ast_fpar_def(ast_id("trg", ast_id("src", NULL)), typeIArray(typeChar));
  fdecl = ast_header("strcat", typeVoid, fpardef, NULL);
  ast_sem(ast_decl(fdecl));
}

//---------------------end of sem analysis----------------------------

char makeEscapeChar(char c) {
    switch (c)
    {
        case '\\':
            return '\\';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case '0':
            return '\0';
        default:
            break;
    }
    return c;
}

int isLibFunction(char *name) {
    if (strcmp(name,"writeInteger") == 0) return 1;
    if (strcmp(name,"writeByte") == 0) return 1;
    if (strcmp(name,"writeChar") == 0) return 1;
    if (strcmp(name,"writeString") == 0) return 1;

    if (strcmp(name,"readInteger") == 0) return 1;
    if (strcmp(name,"readByte") == 0) return 1;
    if (strcmp(name,"readChar") == 0) return 1;
    if (strcmp(name,"readString") == 0) return 1;

    if (strcmp(name,"extend") == 0) return 1;
    if (strcmp(name,"shrink") == 0) return 1;

    if (strcmp(name,"strlen") == 0) return 1;
    if (strcmp(name,"strcmp") == 0) return 1;
    if (strcmp(name,"strcpy") == 0) return 1;
    if (strcmp(name,"strcat") == 0) return 1;

    return 0;
}

Value *compile_function(ast f)
{
    //case of FUNC_DEF
    if (f == NULL) return nullptr;

    StructType *old = current_AR;
    BasicBlock *OldRetBlock;
    StructType *new_frame = TheModule->getTypeByName(f->branch1->id);
    if (new_frame == NULL) {
        char *structname = (char *) malloc(sizeof(char)*(strlen(f->branch1->id) + 8) );
        strcpy(structname, "struct_");
        strcat(structname,f->branch1->id);
        new_frame = StructType::create(TheContext, structname );
    }
    std::vector<Type *> parameters;
    std::vector<Type *> frame_fields;

    int isLib = isLibFunction(f->branch1->id);
    if(!isLib) {
        parameters.push_back(PointerType::get(current_AR, 0));
    }
    frame_fields.push_back(PointerType::get(current_AR, 0));
    current_AR = new_frame;

    //iterate through the parameters and push the types in the vector
    ast params = f->branch1->branch1;
    Type *par_type;
    if (params) {
        par_type = translateType(params->type);
        if (par_type->isArrayTy()) {
            ArrayType *arr = cast<ArrayType>(par_type);
            //par_type = PointerType::get(par_type, 0);
            par_type = PointerType::get(arr->getElementType(), 0);
        }
        for(ast temp=params->branch1; temp!=NULL; temp=temp->branch1) 
        {
            parameters.push_back(par_type);
            frame_fields.push_back(par_type);
        }
    }
    for(ast defs = f->branch1->branch2; defs!=NULL; defs=defs->branch2) {
        params = defs->branch1;
        par_type = translateType(params->type);
        if (par_type->isArrayTy()) {
            ArrayType *ar = cast<ArrayType>(par_type);
            par_type = PointerType::get(ar->getElementType(), 0);
        }
        for(ast temp = params->branch1; temp!=NULL; temp=temp->branch1) 
        {
            parameters.push_back(par_type);
            frame_fields.push_back(par_type);
        }
    }

    //local variable definitions
    for(ast local_defs = f->branch2; local_defs != NULL; local_defs=local_defs->branch2) {
        if (local_defs->branch1->k == VAR) {
            //we found a var def
            ast var_def = local_defs->branch1;
            Type *var_type = translateType(var_def->type);
            for(ast temp = var_def->branch1; temp != NULL; temp = temp->branch1) {
                frame_fields.push_back(var_type);
                // printf("variable added to frame\n");
            }
        }
    }

    if (new_frame->isOpaque()) {
        //this if is used only in the case of forwarding functions
        new_frame->setBody(frame_fields, false);
    }
    //printf("point 2\n");
    //if the function doesn't already exist we create it
    FunctionType *ftype = FunctionType::get(translateType(f->branch1->type), parameters, false);
    Function *NewFunction = TheModule->getFunction(f->branch1->id);
    if (NewFunction == NULL) {
        NewFunction = Function::Create(ftype, GlobalValue::ExternalLinkage, f->branch1->id, TheModule.get());
    }
    //printf("point 3\n");
    //we change the names of the parameters in the Function to their original ones
    Function::arg_iterator iter = NewFunction->arg_begin();
    iter->setName("previous");
    iter++;

    params= f->branch1->branch1;
    if (params) {
        for(ast temp=params->branch1; temp!=NULL; temp=temp->branch1, iter++) {
            iter->setName(temp->id);
        }
        for(ast defs = f->branch1->branch2; defs!=NULL; defs=defs->branch2) {
            params = defs->branch1;
            for(ast temp = params->branch1; temp!=NULL; temp=temp->branch1, iter++) {
                iter->setName(temp->id);
            }
        }
    }

    //printf("point 4\n");
    //we execute the local defs and decls
    ast_compile(f->branch2);
    BasicBlock *BB = BasicBlock::Create(TheContext, "entry", NewFunction);
    Builder.SetInsertPoint(BB);

    AllocaInst *oldAlloca = currentAlloca;
    currentAlloca = Builder.CreateAlloca(new_frame, 0, "new_frame");
    if (!equalType(f->branch1->type, typeVoid)) {
        //we allocate memory for the return value
        retAlloca = Builder.CreateAlloca(translateType(f->branch1->type), 0, "");
    }
    OldRetBlock = returnBlock;
    returnBlock = BasicBlock::Create(TheContext, "return", NewFunction);

    //store Function parameters in the current frame
    iter = NewFunction->arg_begin();
    //std::vector<Type *> idxlist;
    Value *gep = Builder.CreateGEP(currentAlloca, std::vector<Value *>{c32(0), c32(0)}, "");
    Value *element = & (*iter);
    Builder.CreateStore(element, gep, false);
    iter++;
    for(int i=1; i < parameters.size(); i++, iter++) {
        gep = Builder.CreateGEP(currentAlloca, std::vector<Value *>{c32(0), c32(i)}, "");
        element = & (*iter);
        Builder.CreateStore(element, gep, false);
    }

    //we store the highest index of parameters in this frame
    if (!isLib) paramsIndex.insert(std::pair<Value *, int>(currentAlloca, parameters.size()-1));
    else paramsIndex.insert(std::pair<Value *, int>(currentAlloca, parameters.size()));
    //we start executing the block of the function
    ast_compile(f->branch3);

    //if we encountered no return instruction, the function has void return type
    // Function *endFunction = Builder.GetInsertBlock()->getParent();
    Builder.CreateBr(returnBlock);
    Builder.SetInsertPoint(returnBlock);

    if (!equalType(f->branch1->type, typeVoid)) {
        Value *returnValue = Builder.CreateLoad(retAlloca, "");
        Builder.CreateRet(returnValue);
    }
    else Builder.CreateRetVoid();
    returnBlock = OldRetBlock;

    //function finished. we return to the previous AR
    current_AR = old;
    currentAlloca = oldAlloca;
    // returnBlock = OldRetBlock;
    return nullptr;
}

Value *ast_compile(ast t)
{
    if (t == nullptr) return nullptr;
    switch (t->k) {
    case FUNC_DEF:
    {
        compile_function(t);
        //execute function header actions, create a new activation record
        //ast_compile(t->branch1);

        //execute local def instructions
        //ast_compile(t->branch2);

        //execute function block
        //ast_compile(t->branch3);
        return nullptr;
    }
    case HEADER:
    {
        //CASE ALREADY COVERED
        return nullptr;
    }
    case DECL:
    {
        StructType *new_frame = TheModule->getTypeByName(t->branch1->id);
        if (new_frame == NULL) {
            char *structname = (char *) malloc(sizeof(char)*(strlen(t->branch1->id) + 8) );
            strcpy(structname, "struct_");
            strcat(structname,t->branch1->id);
            new_frame = StructType::create(TheContext, structname );
        }
        std::vector<Type *> parameters;
        std::vector<Type *> frame_fields;

        if(!isLibFunction(t->branch1->id)) {
            parameters.push_back(PointerType::get(current_AR, 0));
        }
        frame_fields.push_back(PointerType::get(current_AR, 0));

        //parameter definition
        ast params= t->branch1->branch1;
        Type *par_type;
        if (params) {
            par_type = translateType(params->type);
            for(ast temp=params->branch1; temp!=NULL; temp=temp->branch1) {
                parameters.push_back(par_type);
                frame_fields.push_back(par_type);
            }
        }
        for(ast defs = t->branch1->branch2; defs!=NULL; defs=defs->branch2) {
            params = defs->branch1;
            par_type = translateType(params->type);
            for(ast temp = params->branch1; temp!=NULL; temp=temp->branch1) {
                parameters.push_back(par_type);
                frame_fields.push_back(par_type);
            }
        }

        //if the function doesn't already exist we create it
        FunctionType *ftype = FunctionType::get(translateType(t->branch1->type), parameters, false);
        Function *NewFunction = TheModule->getFunction(t->branch1->id);
        if (NewFunction == NULL) {
            NewFunction = Function::Create(ftype, GlobalValue::ExternalLinkage, t->branch1->id, TheModule.get());
        }
        return nullptr;
    }
    case VAR:
    {
        //CASE ALREADY COVERED
        return nullptr;
    }
    case ID:
    {
        //CASE ALREADY COVERED
        return nullptr;
    }
    case SKIP:
    {
        return nullptr;
    }
    case ASSIGN:
    {
        Value *val = ast_compile(t->branch2);
        //printf("%s var ofset is %d\n", t->branch1->id, t->branch1->offset);
        ast node;
        std::vector<Value *> idxlist;
        //in case left side is access to an array element, we iterate until we find the base array node
        for(node=t->branch1; node->branch1 != NULL; node=node->branch1) {
            Value *i = ast_compile(node->branch2);
            idxlist.push_back(i);
        }

        int frame_diff = node->nesting_diff;
        int indx = node->offset;
        Value *record = currentAlloca;
        Value *gep;
        for(int i=0; i < frame_diff; i++) {
            gep = Builder.CreateGEP(record, std::vector<Value *>{c32(0), c32(0)}, "");
            record = Builder.CreateLoad(gep, "previous");
        }

        gep =  Builder.CreateGEP(record, std::vector<Value *>{c32(0), c32(indx)}, "");
        if (isArray(node->type) || isIArray(node->type)) {
            // Function *TheFunction = Builder.GetInsertBlock()->getParent();
            // Function::arg_iterator arg_it = TheFunction->arg_begin();
            // while(arg_it != TheFunction->arg_end()) {

            //     if (strcmp(arg_it->getName().data(), node->id) == 0) {
            //         //array is function argument. that means we need the array's address
            //         gep = Builder.CreateLoad(gep, "");
            //         break;
            //     }
            //     arg_it++;
            // }
            //if indx <= paramsIndex then the array is a parameter and thus it passes by reference
            auto iter = idxlist.rbegin();
            if (indx <= paramsIndex.find(record)->second) {
                gep = Builder.CreateLoad(gep, "");
                Type *arrType = translateType(node->type->refType);
                gep = Builder.CreateInBoundsGEP(arrType, gep, *iter, "");
                iter++;
            }

            for(; iter != idxlist.rend(); ++iter) {
                gep = Builder.CreateInBoundsGEP(gep, std::vector<Value *>{c32(0), *iter}, "");
            }
        }
        if (isPointer(node->type)) {
            gep = Builder.CreateLoad(gep, "");
        }

        Builder.CreateStore(val, gep, false);
        //printf("finished assign\n");
        return nullptr;
    }
    case EXIT:
    {
        //we exit the closest block with void return value
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *ExitBlock = BasicBlock::Create(TheContext, "exit", TheFunction);
        // int index = blockNames.size() -1;
        // BasicBlock *ExitPoint = blockNames.at(index);

        Builder.CreateBr(returnBlock);
        Builder.SetInsertPoint(ExitBlock);
        //blockNames.pop_back();
        return nullptr;
    }
    case RET:
    {
        Value *retval = ast_compile(t->branch1);
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *AfterBB = BasicBlock::Create(TheContext, "after_return", TheFunction);

        //Value *gep = Builder.CreateGEP(retAlloca, 0, "");
        Builder.CreateStore(retval, retAlloca, false);
        Builder.CreateBr(returnBlock);
        
        Builder.SetInsertPoint(AfterBB);
        return nullptr;
    }
    case IF:
    {
        Value *cond;
        Value *v = ast_compile(t->branch1);

        ConstantInt *CI;
        if (CI = dyn_cast<ConstantInt>(v) ) {
            int num = CI->getSExtValue();
            cond = Builder.CreateICmpNE(c32(num), c32(0), "if_cond");
        }
        else {
            // if (t->branch1->k == FUNC_CALL) {
            //     if (equalType(t->branch1->type, typeInteger)) cond = Builder.CreateICmpNE(v, c32(0), "if_cond");
            //     if (equalType(t->branch1->type, typeChar)) cond = Builder.CreateICmpNE(v, c8(0), "if_cond");
            //     else {
            //         printType(t->branch1->type);
            //         printf("\n");
            //         error("not supposed to happen");
            //     }
            // }
            // else {
            //     if (equalType(t->branch1->type, typeInteger)) cond = Builder.CreateICmpNE(v, c32(0), "if_cond");
            //     else if (equalType(t->branch1->type, typeChar)) cond = Builder.CreateICmpNE(v, c8(0), "if_cond");
            //     else {
            //         cond = v;
            //     }
            // }
            Type *ty = translateType(t->branch1->type);
            if (ty->isIntegerTy(32)) cond = Builder.CreateICmpNE(v, c32(0), "if_cond");
            else if (ty->isIntegerTy(8)) cond = Builder.CreateICmpNE(v, c8(0), "if_cond");
            else if (ty->isIntegerTy(1)) cond = v;
            else cond = v;
        }

        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *InsideBB = BasicBlock::Create(TheContext, "then", TheFunction);
        BasicBlock *EndifBB = BasicBlock::Create(TheContext, "endif", TheFunction);
        BasicBlock *NextBB;
        if (t->branch3 != NULL) NextBB = BasicBlock::Create(TheContext, "elif", TheFunction);
        else NextBB = EndifBB;

        Builder.CreateCondBr(cond, InsideBB, NextBB);
        Builder.SetInsertPoint(InsideBB);
        //we execute the block inside if
        ast_compile(t->branch2);

        Builder.CreateBr(EndifBB);
        Builder.SetInsertPoint(NextBB);

        //we execute all elif nodes
        for(ast elif_node=t->branch3; elif_node!=NULL; elif_node = elif_node->branch3) {
            Value *val = ast_compile(elif_node->branch1);
            Value *cond1;
            ConstantInt *CI;
            if (CI = dyn_cast<ConstantInt>(val) ) {
                int num = CI->getSExtValue();
                cond1 = Builder.CreateICmpNE(c32(num), c32(0), "elif_cond");
            }
            else {
                Type *ty = translateType(elif_node->branch1->type);
                if (ty->isIntegerTy(32)) cond1 = Builder.CreateICmpNE(val, c32(0), "if_cond");
                else if (ty->isIntegerTy(8)) cond1 = Builder.CreateICmpNE(val, c8(0), "if_cond");
                else if (ty->isIntegerTy(1)) cond1 = val;
                else cond1 = val;
            }

            Function *TheFunction = Builder.GetInsertBlock()->getParent();
            BasicBlock *InElifBB = BasicBlock::Create(TheContext, "in_elif", TheFunction);
            BasicBlock *NextElifBB;
            if (elif_node->branch3 != NULL) NextElifBB = BasicBlock::Create(TheContext, "next_elif", TheFunction);
            else                            NextElifBB = EndifBB;
            Builder.CreateCondBr(cond1, InElifBB, NextElifBB);
            Builder.SetInsertPoint(InElifBB);
            ast_compile(elif_node->branch2);

            Builder.CreateBr(EndifBB);
            Builder.SetInsertPoint(NextElifBB);
        }

        return nullptr;
    }
    case IF_ELSE:
    {
        Value *cond;
        Value *v = ast_compile(t->branch1);

        //if Value v is a condition we don't need to do one more comparison. if it's a numerical value the comparison is needed
        ConstantInt *CI;
        if (CI = dyn_cast<ConstantInt>(v) ) {
            int num = CI->getSExtValue();
            cond = Builder.CreateICmpNE(c32(num), c32(0), "if_cond");
        }
        else {
            Type *ty = translateType(t->branch1->type);
            if (ty->isIntegerTy(32)) cond = Builder.CreateICmpNE(v, c32(0), "if_cond");
            else if (ty->isIntegerTy(8)) cond = Builder.CreateICmpNE(v, c8(0), "if_cond");
            else if (ty->isIntegerTy(1)) cond = v;
            else cond = v;
        }
        
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *InsideBB = BasicBlock::Create(TheContext, "then", TheFunction);
        BasicBlock *EndifBB = BasicBlock::Create(TheContext, "endif", TheFunction);
        BasicBlock *ElseBB =  BasicBlock::Create(TheContext, "else", TheFunction);
        BasicBlock *NextBB;
        if (t->branch3 != NULL) NextBB = BasicBlock::Create(TheContext, "elif", TheFunction);
        else NextBB = ElseBB;

        Builder.CreateCondBr(cond, InsideBB, NextBB);
        Builder.SetInsertPoint(InsideBB);
        //we execute the block inside 'if'
        ast_compile(t->branch2);
        
        Builder.CreateBr(EndifBB);
        Builder.SetInsertPoint(NextBB);

        //we execute all elif nodes
        for(ast elif_node=t->branch3; elif_node!=NULL; elif_node = elif_node->branch3) {
            Value *val = ast_compile(elif_node->branch1);
            Value *cond1;
            ConstantInt *CI;
            if (CI = dyn_cast<ConstantInt>(val) ) {
                int num = CI->getSExtValue();
                cond1 = Builder.CreateICmpNE(c32(num), c32(0), "elif_cond");
            }
            else {
                Type *ty = translateType(elif_node->branch1->type);
                if (ty->isIntegerTy(32)) cond1 = Builder.CreateICmpNE(val, c32(0), "if_cond");
                else if (ty->isIntegerTy(8)) cond1 = Builder.CreateICmpNE(val, c8(0), "if_cond");
                else if (ty->isIntegerTy(1)) cond1 = val;
                else cond1 = val;
            }

            Function *TheFunction1 = Builder.GetInsertBlock()->getParent();
            BasicBlock *InElifBB = BasicBlock::Create(TheContext, "in_elif", TheFunction1);
            BasicBlock *NextElifBB;
            if (elif_node->branch3 != NULL) NextElifBB = BasicBlock::Create(TheContext, "next_elif", TheFunction1);
            else                            NextElifBB = ElseBB;
            Builder.CreateCondBr(cond1, InElifBB, NextElifBB);
            Builder.SetInsertPoint(InElifBB);
            ast_compile(elif_node->branch2);

            Builder.CreateBr(EndifBB);
            Builder.SetInsertPoint(NextElifBB);
        }

        //we execute the 'else' block
        ast_compile(t->branch4);

        Builder.CreateBr(EndifBB);
        Builder.SetInsertPoint(EndifBB);
        return nullptr;
    }
    case LOOP:
    {
        // Make the new basic block for the loop.
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *LoopBB = BasicBlock::Create(TheContext, "loop", TheFunction);
        BasicBlock *AfterBB = BasicBlock::Create(TheContext, "after_loop", TheFunction);

        //if the loop has a name we store it
        char *name;
        if ((t->id)[0] != '\0') {
            name = (char *) malloc((strlen(t->id)+1)*sizeof(char));
            strcpy(name, t->id);
            afterLoopMap.insert(std::pair<char *, BasicBlock*>(name, AfterBB));
            LoopMap.insert(std::pair<char *, BasicBlock*>(name, LoopBB));
        }
        else {
            name = NULL;
            afterLoopMap.insert(std::pair<char *, BasicBlock*>("\0", AfterBB));
            LoopMap.insert(std::pair<char *, BasicBlock*>("\0", LoopBB));
        }

        // Insert an explicit fall-through from the current block.
        Builder.CreateBr(LoopBB);
        // Start insertion in the loop.
        Builder.SetInsertPoint(LoopBB);
        //we execute the body of the loop
        ast_compile(t->branch1);
        //loop ends only when instructions break, exit are encountered inside

        //we remove the Loop and AfterLoop blocks from the map
        std::multimap<char *, BasicBlock*>::iterator map_it;
        if ((t->id)[0] != '\0') {
            for(auto it = afterLoopMap.find(name); strcmp(it->first,name)==0; it++) map_it = it;
            afterLoopMap.erase(map_it, afterLoopMap.end());
            for(auto it=LoopMap.find(name); strcmp(it->first,name)==0; it++) map_it = it;
            LoopMap.erase(map_it, LoopMap.end());
            //free(name);
        }
        else {
            for(auto it=afterLoopMap.begin(); it !=afterLoopMap.end(); it++) map_it = it;
            afterLoopMap.erase(map_it);
            for(auto it=LoopMap.begin(); it !=LoopMap.end(); it++) map_it = it;
            LoopMap.erase(map_it);
        }

        Builder.CreateBr(LoopBB);
        //end of loop
        Builder.SetInsertPoint(AfterBB);
        return nullptr;
    }
    case BREAK:
    {
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *AfterBlock;
        BasicBlock *BreakBlock = BasicBlock::Create(TheContext, "break", TheFunction);
        std::multimap<char *, BasicBlock*>::iterator iter;
        std::multimap<char *, BasicBlock*>::reverse_iterator closestLoop;
        //auto list = & TheFunction->getBasicBlockList();

        if ((t->id)[0] != '\0') {
            //if break is given a loop name we break out of that specific loop.
            //name = (char *) malloc((strlen(t->id)+1)*sizeof(char));
            iter = afterLoopMap.find(t->id);
            while(iter != afterLoopMap.end()) {
                if (strcmp(iter->first, t->id)==0) AfterBlock = iter->second;
                iter++;
            }
        }
        else {
            //otherwise we break the closest loop
            closestLoop = afterLoopMap.rbegin();
            if (closestLoop != afterLoopMap.rend()) AfterBlock = closestLoop->second;
            else printf("shouldn't be empty\n");
        }

        Builder.CreateBr(AfterBlock);
        Builder.SetInsertPoint(BreakBlock);
        return nullptr;
    }
    case CONT:
    {
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *LoopBlock;
        BasicBlock *ContBlock = BasicBlock::Create(TheContext, "cont", TheFunction);
        std::multimap<char *, BasicBlock*>::iterator iter;
        std::multimap<char *, BasicBlock*>::reverse_iterator closestLoop;

        if ((t->id)[0] != '\0') {
            //continue is given a specific name
            iter = LoopMap.find(t->id);
            while(iter != LoopMap.end()) {
                if (strcmp(iter->first, t->id) == 0) LoopBlock = iter->second;
                iter++;
            }
        }
        else {
            closestLoop = LoopMap.rbegin();
            LoopBlock = closestLoop->second;
        }

        Builder.CreateBr(LoopBlock);
        Builder.SetInsertPoint(ContBlock);
        return nullptr;
    }
    case SEQ:
    {
        ast_compile(t->branch1);
        ast_compile(t->branch2);
        return nullptr;
    }
    case BLOCK:
    {
        //we push the current basic block to the vector
        // BasicBlock *currentBlock = Builder.GetInsertBlock();
        // blockNames.push_back(currentBlock);
        ast_compile(t->branch1);
        return nullptr;
    }
    case PROC_CALL:
    {
        //Function *TheFunction = Builder.GetInsertBlock()->getParent();
        Function *Callee = TheModule->getFunction(t->id);
        std::vector<Value *> args;
        Value *record = currentAlloca;

        int isLib = isLibFunction(t->id);
        
        if (!isLib) {
            for(int i=0; i < t->nesting_diff; i++) {
                Value *gep = Builder.CreateGEP(record, std::vector<Value *>{c32(0), c32(0)}, "");
                record = Builder.CreateLoad(gep, "previous");
            }
            args.push_back(record);
        }

        //if the call has arguments we insert them in the args vector
        if (t->branch1) {
            Function::arg_iterator arg_it = Callee->arg_begin();
            if (!isLib) arg_it++;
            if (arg_it->getType()->isPointerTy() ) passByReference = 1;
            else passByReference = 0;
            arg_it++;
            args.push_back(ast_compile(t->branch1));

            for(ast temp=t->branch2; temp != NULL; temp=temp->branch2) {
                if (arg_it->getType()->isPointerTy() ) passByReference = 1;
                else passByReference = 0;
                arg_it++;

                Value *v = ast_compile(temp->branch1);
                args.push_back(v);
            }
        }
        
        Builder.CreateCall(Callee, args, "");
        return nullptr;
    }
    case FUNC_CALL:
    {
        Function *Callee = TheModule->getFunction(t->id);
        std::vector<Value *> args;
        Value *record = currentAlloca;

        int isLib = isLibFunction(t->id);

        if (!isLib) {
            for(int i=0; i < t->nesting_diff; i++) {
                Value *gep = Builder.CreateGEP(record, std::vector<Value *>{c32(0), c32(0)}, "");
                record = Builder.CreateLoad(gep, "previous");
            }
            args.push_back(record);
        }

        //if func call has arguments we push them in the args vector
        if (t->branch1) {
            Function::arg_iterator arg_it = Callee->arg_begin();
            if (!isLib) arg_it++;
            if (arg_it->getType()->isPointerTy() ) passByReference = 1;
            else passByReference = 0;
            args.push_back(ast_compile(t->branch1));
            arg_it++;

            for(ast temp=t->branch2; temp != NULL; temp=temp->branch2) {
                //real_param_type = translateType(temp->branch1->type);
                if (arg_it->getType()->isPointerTy() ) passByReference = 1;
                else passByReference = 0;
                arg_it++;

                Value *v = ast_compile(temp->branch1);
                args.push_back(v);
            }
        }
        if (!isLib) return Builder.CreateCall(Callee, args, t->id);
        else return Builder.CreateCall(Callee, args, "");
    }
    case TID:
    {
        ///we search the AR where the variable/param was defined and load it's value
        int frame_diff = t->nesting_diff;
        Value *record = currentAlloca;
        Value *gep;
        for(int i=0; i < frame_diff; i++) {
            gep = Builder.CreateGEP(record, std::vector<Value *>{c32(0), c32(0)}, "");
            record = Builder.CreateLoad(gep, "previous");
        }
        int index = t->offset;
        gep = Builder.CreateGEP(record, std::vector<Value *>{c32(0), c32(index)}, "");
        // printf("finished variable %s\n", t->id);
        if (passByReference) {
            Type *ty = translateType(t->type);
            if (ty->isArrayTy() ) {
                //if an array is to be passed by ref, we pass a pointer to it's first element

                gep = Builder.CreateInBoundsGEP(gep, std::vector<Value *>{c32(0), c32(0)}, "");
            }
            if (ty->isPointerTy()) {
                gep = Builder.CreateLoad(gep, "");
            }
            passByReference = 0;
            return gep;
        }

        if (isPointer(t->type)) {
            gep = Builder.CreateLoad(gep, "");
        }
        // if (loadByReference) {
        //     Value *ptr = Builder.CreateLoad(gep, "");
        //     loadByReference = 0;
        //     return Builder.CreateLoad(ptr, t->id);
        // }
        return Builder.CreateLoad(gep, t->id);
    }
    case ARR:
    {
        int passMode = passByReference;
        //int loadMode = loadByReference;
        passByReference = 0;
        // if (isPointer(t->branch2->type)) loadByReference = 1;
        // else loadByReference = 0;
        Value *val = ast_compile(t->branch2);
        //loadByReference = 0;

        std::vector<Value *> idxlist;
        idxlist.push_back(val);
        ast temp;
        //loop until we reach the base node. we store the index for each array dimension
        for(temp = t->branch1; temp->branch1 != NULL; temp=temp->branch1) {
            idxlist.push_back(ast_compile(temp->branch2));
        }

        passByReference = passMode;
        //loadByReference = loadMode;
        int frame_diff = temp->nesting_diff;
        int offset = temp->offset;
        Value *record = currentAlloca;
        Value *gep;
        for(int i=0; i < frame_diff; i++) {
            gep = Builder.CreateGEP(record, std::vector<Value *>{c32(0), c32(0)}, "");
            record = Builder.CreateLoad(gep, "previous");
        }
        
        gep = Builder.CreateGEP(record, std::vector<Value *>{c32(0), c32(offset)}, "");
        Value *array_value;

        if (offset <= paramsIndex.find(record)->second) {
            //if offset <= paramsIndex then the array is a parameter and thus it passes by reference. so we load it's location
            Type_h astType = temp->type->refType;
            array_value = Builder.CreateLoad(gep, "");
            Type *arrType = translateType(astType);
            auto iter = idxlist.rbegin();
            gep = Builder.CreateInBoundsGEP(arrType, array_value, *iter, "");
            iter++;
            for(; iter != idxlist.rend(); iter++) {
                // astType = astType->refType;
                // arrType = translateType(astType);
                gep = Builder.CreateInBoundsGEP(gep, std::vector<Value *>{c32(0), *iter}, "");
                //gep = Builder.CreateInBoundsGEP(arrType, array_value, *iter, "");
                //array_value = gep;
            }
            array_value = Builder.CreateLoad(gep, "");
        }
        else {
            //array is not a parameter
            for(auto iter = idxlist.rbegin(); iter != idxlist.rend(); iter++) {
                gep = Builder.CreateInBoundsGEP(gep, std::vector<Value *>{c32(0), *iter}, "");
                //gep = Builder.CreateInBoundsGEP(array_value, std::vector<Value *>{c32(0), *iter}, "");
                //array_value = Builder.CreateLoad(gep, "");
            }
            array_value = Builder.CreateLoad(gep, "");
        }

        if (passByReference) {
            passByReference = 0;
            return gep;
        }
            
        //return Builder.CreateLoad(array_value, "");
        return array_value;
    }
    case STRING_LIT:
    {
        //printf("the string is: %s\n", t->id);
        //printf("len is %d\n", strlen(t->id));
        char *str = (char *) malloc(sizeof(char)*(strlen(t->id) + 1) );
        strcpy(str, t->id);
        std::vector<Constant *> str_vector;
        int escape_char = 0;
        for(int i=0; i < strlen(str); i++) {
            if (str[i] == '\\' && escape_char==0) escape_char = 1;
            else if (escape_char) {
                char c = makeEscapeChar(str[i]);
                escape_char = 0;
                str_vector.push_back(c8(c));
            }
            else str_vector.push_back(c8(str[i]));
        }
        str_vector.push_back(c8('\0'));

        //string literals are arrays of typeChar
        Type_h ty = t->type;
        Type *ref = translateType(ty->refType);
        //int size = ty->size;
        int size = str_vector.size();
        ArrayType *str_type = ArrayType::get(ref, size);

        GlobalVariable *TheString;
        TheString = new GlobalVariable(*TheModule, str_type, true, GlobalValue::PrivateLinkage, ConstantArray::get(str_type, str_vector));
        passByReference = 0;
        //loadByReference = 0;
        return Builder.CreateGEP(TheString, std::vector<Value *>{c32(0), c32(0)}, "");
    }
    case INTCONST:
    {
        return c32(t->num);
    }
    case CHARCONST:
    {
        char c = (t->id)[0];
        return c8(c);
    }
    case PLUS:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateAdd(l, r, "addtmp");
    }
    case MINUS:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateSub(l, r, "subtmp");
    }
    case TIMES:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateMul(l, r, "multmp");
    }
    case DIV:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateSDiv(l, r, "divtmp");
    }
    case MOD:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateSRem(l, r, "modtmp");
    }
    case NOT:
    {
        //Boolean not, and, or cases (!, &&, ||)
        Value *l = ast_compile(t->branch2);
        ConstantInt *res;
        if (equalType(t->branch2->type, typeChar)) {
            return Builder.CreateICmpEQ(l, c8(0), "cond_nottmp");
        }

        if (res = dyn_cast<ConstantInt>(l)) {
            return Builder.CreateICmpEQ(l, c32(0), "cond_nottmp");
        }
        //PHINode *phi_iter = Builder.CreatePHI(i32, 2, "not");
        return Builder.CreateICmpEQ(l, c1(0), "cond_nottmp");
    }
    case AND:
    {
        ConstantInt *res1, *res2;
        Value *l, *r;
        l = ast_compile(t->branch1);
        if (res1 = dyn_cast<ConstantInt>(l) ) {
            if (res1->isZero()) return c32(0);

            //we have determined that left hand is non zero
            r = ast_compile(t->branch2);
            if (res2 = dyn_cast<ConstantInt>(r) ) {
                //if right hand is number we compare with 0
                return Builder.CreateICmpNE(r, c32(0), "cond_andtmp");
            }
            //if r s a comparison we just return it
            else return r;
        }
        //left hand is a boolean condition
        r = ast_compile(t->branch2);
        if (res2 = dyn_cast<ConstantInt>(r) ) {
            if (res2->isZero()) return c32(0);
            
            return l;
        }
        //now we have determined that both l and r are conditions
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *RightBB = BasicBlock::Create(TheContext, "right_cond_and", TheFunction);
        BasicBlock *ResultBB = BasicBlock::Create(TheContext, "result_and", TheFunction);
        BasicBlock *BB = Builder.GetInsertBlock();

        Builder.CreateCondBr(l, RightBB, ResultBB);
        Builder.SetInsertPoint(RightBB);

        //Value *right_op;
        Builder.CreateBr(ResultBB);
        Builder.SetInsertPoint(ResultBB);

        PHINode *phi_iter = Builder.CreatePHI(i1, 2, "and");
        Value *itsfalse = Builder.CreateICmpNE(c32(0), c32(0), "falsetmp");
        phi_iter->addIncoming(itsfalse, BB);
        phi_iter->addIncoming(r, RightBB);

        Value *and_result = phi_iter;
        return and_result;
    }
    case OR:
    {
        ConstantInt *res1, *res2;
        Value *l, *r;
        l = ast_compile(t->branch1);
        if (res1 = dyn_cast<ConstantInt>(l) ) {
            if (res1->isOne()) return c32(1);
            if (res1->getZExtValue() > 0) return c32(1);

            //we have determined that left hand is zero
            r = ast_compile(t->branch2);
            if (res2 = dyn_cast<ConstantInt>(r) ) {
                //if right hand is number we compare with 0
                return Builder.CreateICmpNE(r, c32(0), "cond_ortmp");
            }
            //if r s a comparison we just return it
            else return r;
        }
        //left hand is a boolean condition
        r = ast_compile(t->branch2);
        if (res2 = dyn_cast<ConstantInt>(r) ) {
            if (res2->isOne()) return c32(1);
            if (res2->getZExtValue() > 0) return c32(1);
            
            return l;
        }
        //now we have determined that both l and r are conditions
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *RightBB = BasicBlock::Create(TheContext, "right_cond_or", TheFunction);
        BasicBlock *ResultBB = BasicBlock::Create(TheContext, "result_or", TheFunction);
        BasicBlock *BB = Builder.GetInsertBlock();

        Builder.CreateCondBr(l, ResultBB, RightBB);
        Builder.SetInsertPoint(RightBB);

        //Value *right_op;
        Builder.CreateBr(ResultBB);
        Builder.SetInsertPoint(ResultBB);

        PHINode *phi_iter = Builder.CreatePHI(i1, 2, "or");
        Value *itstrue = Builder.CreateICmpEQ(c32(0), c32(0), "truetmp");
        phi_iter->addIncoming(itstrue, BB);
        phi_iter->addIncoming(r, RightBB);

        Value *or_result = phi_iter;
        return or_result;
    }
    case EQ:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateICmpEQ(l, r, "equaltmp");
    }
    case LT:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateICmpSLT(l, r, "lowertmp");
    }
    case GT:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateICmpSGT(l, r, "greatertmp");
    }
    case LE:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateICmpSLE(l, r, "lowerorequaltmp");
    }
    case GE:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateICmpSGE(l, r, "greaterorequaltmp");
    }
    case NEQ:
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateICmpNE(l, r, "nequaltmp");
    }
    case EXPR_NOT: 
    {
        Value *l = ast_compile(t->branch1);
        return Builder.CreateNot(l, "nottmp");
    }
    case EXPR_AND: 
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateAnd(l, r, "andtmp");
    }
    case EXPR_OR: 
    {
        Value *l = ast_compile(t->branch1);
        Value *r = ast_compile(t->branch2);
        return Builder.CreateOr(l, r, "ortmp");
    }
    }
    return nullptr;
}

void declare_library_functions() {
    // declare void @writeInteger(i32)
    FunctionType *writeInteger_type =
        FunctionType::get(Type::getVoidTy(TheContext),
                          std::vector<Type *>{i32}, false);
    TheWriteInteger =
        Function::Create(writeInteger_type, Function::ExternalLinkage,
                         "writeInteger", TheModule.get());

    // declare void @writeChar(i8)
    FunctionType *writeChar_type = FunctionType::get(Type::getVoidTy(TheContext), std::vector<Type *>{i8}, false);
    TheWriteChar = Function::Create(writeChar_type, Function::ExternalLinkage, "writeChar", TheModule.get());

    //declare void @writyByte(i8)
    FunctionType *writeByte_type = FunctionType::get(Type::getVoidTy(TheContext), std::vector<Type *>{i8}, false);
    TheWriteByte = Function::Create(writeByte_type, Function::ExternalLinkage, "writeByte", TheModule.get());

    // declare void @writeString(i8*)
    FunctionType *writeString_type =
        FunctionType::get(Type::getVoidTy(TheContext),
                          std::vector<Type *>{PointerType::get(i8, 0)}, false);
    TheWriteString =
        Function::Create(writeString_type, Function::ExternalLinkage,
                         "writeString", TheModule.get());


    //declare int @readInteger()
    FunctionType *readInteger_type = FunctionType::get(Type::getInt32Ty(TheContext), std::vector<Type *>(), false);
    TheReadInteger = Function::Create(readInteger_type, Function::ExternalLinkage, "readInteger", TheModule.get());

    //declare byte @readChar()
    FunctionType *readChar_type = FunctionType::get(Type::getInt8Ty(TheContext), std::vector<Type *>(), false);
    TheReadChar = Function::Create(readChar_type, Function::ExternalLinkage, "readChar", TheModule.get());

    //declare byte @readByte()
    FunctionType *readByte_type = FunctionType::get(Type::getInt8Ty(TheContext), std::vector<Type *>(), false);
    TheReadByte = Function::Create(readByte_type, Function::ExternalLinkage, "readByte", TheModule.get());

    //declare void @readString(i32, *i8)
    FunctionType *readString_type = 
        FunctionType::get(Type::getVoidTy(TheContext), std::vector<Type *>{i32, PointerType::get(i8, 0)}, false);
    TheReadString = Function::Create(readString_type, Function::ExternalLinkage, "readString", TheModule.get());

    //declare int @extend(i8)
    FunctionType *extend_type = FunctionType::get(Type::getInt32Ty(TheContext), std::vector<Type *>{i8}, false);
    extend = Function::Create(extend_type, Function::ExternalLinkage, "extend", TheModule.get());
    //declare byte @shrink(i32)
    FunctionType *shrink_type = FunctionType::get(Type::getInt8Ty(TheContext), std::vector<Type *>{i32}, false);
    shrink = Function::Create(shrink_type, Function::ExternalLinkage, "shrink", TheModule.get());


    //declare int @strlen(*i8)
    FunctionType *strlen_type = FunctionType::get(Type::getInt32Ty(TheContext), std::vector<Type *>{PointerType::get(i8, 0)}, false);
    TheStrlen = Function::Create(strlen_type, Function::ExternalLinkage, "strlen", TheModule.get());
    //declare int @strcmp(*i8,*i8)
    FunctionType *strcmp_type = 
        FunctionType::get(Type::getInt32Ty(TheContext), std::vector<Type *>{PointerType::get(i8, 0), PointerType::get(i8, 0)}, false);
    TheStrcmp = Function::Create(strcmp_type, Function::ExternalLinkage, "strcmp", TheModule.get());
    //declare void @strcpy(*i8,*i8)
    FunctionType *strcpy_type = 
        FunctionType::get(Type::getVoidTy(TheContext), std::vector<Type *>{PointerType::get(i8, 0), PointerType::get(i8, 0)}, false);
    TheStrcpy = Function::Create(strcpy_type, Function::ExternalLinkage, "strcpy", TheModule.get());
    //declare void @strcat(*i8,*i8)
    TheStrcat = Function::Create(strcpy_type, Function::ExternalLinkage, "strcat", TheModule.get());
    return;
}

void llvm_compile_and_dump(ast t)
{
    // Initialize the module and the optimization passes.
    TheModule = make_unique<Module>("dana program", TheContext);
    TheFPM = make_unique<legacy::FunctionPassManager>(TheModule.get());
    TheFPM->add(createPromoteMemoryToRegisterPass());
    TheFPM->add(createInstructionCombiningPass());
    TheFPM->add(createReassociatePass());
    TheFPM->add(createGVNPass());
    TheFPM->add(createCFGSimplificationPass());
    TheFPM->add(createConstantPropagationPass());
    TheFPM->add(createLoopUnrollPass());
    TheFPM->add(createDeadCodeEliminationPass());
    TheFPM->add(createDeadInstEliminationPass());
    TheFPM->doInitialization();

    declare_library_functions();
                         
    // Define and start the main function.
    // Constant *c = TheModule->getOrInsertFunction("main", i32, NULL);
    // Function *main = cast<Function>(c);
    // BasicBlock *BB = BasicBlock::Create(TheContext, "entry", main);
    // Builder.SetInsertPoint(BB);

    //we initialize the global current_AR
    current_AR = StructType::create(TheContext, "start");
    //currentAlloca = Builder.CreateAlloca(current_AR, 0, "");
    // Emit the program code.
    ast_compile(t);

    Function *main = TheModule->getFunction("main");

    // Verify and optimize the main function.
    bool bad = verifyModule(*TheModule, &errs());
    if (bad)
    {
        fprintf(stderr, "The faulty IR is:\n");
        fprintf(stderr, "------------------------------------------------\n\n");
        TheModule->print(outs(), nullptr);
        return;
    }
    TheFPM->run(*main);
    // Print out the IR.
    TheModule->print(outs(), nullptr);
}