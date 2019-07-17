%{
#include <stdio.h>
#include "MYast.h"
#include "parser.h"
// #include "symbol.h"
//#include "y.tab.h"

//----defining to test lexer alone
#define T_eof 0
// #define T_and 1000
// #define T_as 1001
// #define T_begin 1002
// #define T_break 1003
// #define T_byte 1004
// #define T_continue 1005
// #define T_decl 1006
// #define T_def 1007
// #define T_elif 1008
// #define T_else 1009
// #define T_end 1010
// #define T_exit 1011
// #define T_false 1012
// #define T_if 1013
// #define T_is 1014
// #define T_int 1015
// #define T_loop 1016
// #define T_not 1017
// #define T_or 1018
// #define T_ref 1019
// #define T_return 1020
// #define T_skip 1021
// #define T_true 1022
// #define T_var 1023
// #define T_intconst 1024
// #define T_char 1025
// #define T_id 1026
// #define T_string 1027
// #define T_greater 1028
// #define T_less 1029
// #define T_greatereq 1030
// #define T_lesseq 1031
// #define T_noteq 1032
// #define T_assign 1033

//----defining done

int choice=0;              //if choice==1 then we have read a keyword that begins a block
int blockstart=0;
int i;
int commentcounter;
int linenumber = 1;
int spacecounter=0;
int blockcounter=0;
int newline_block = 0;		//if set then the following block begins in the next line
int sameline_block = 0;		//if set then the following block begins in the same line after character ':'
int mystack[100];			//stack contains how right in the editor each block starts
int sp = -1;                 //sp points to the latest element pushed into the stack

void push(int element);
int pop();
void isNextEOF(void);
void yyerror (const char *msg);
%}

Id [[:alpha:]][[:alnum:]]*
Intconst [0-9]+
W [ \t\r]
asciizero \0
backslash \\
singlequote \'
doublequote \"
asciichar \x[0-9a-fA-F][0-9a-fA-F]
Char {singlequote}{1}.{singlequote}{1}

%x SLCOMMENT
%x MLCOMMENT
%x STRING
%x FINAL
%s SIMPLE
%x NEWLINE

%%

 /*------------------------layout-------------------------------------- */

[ ]		{ isNextEOF(); spacecounter++; }
\t		{ isNextEOF(); spacecounter += 4; }
\n		{ spacecounter=0; linenumber++;
			/* def blocks always begin after newline character*/
			BEGIN(NEWLINE);
			isNextEOF();
		}
:		{ if (sameline_block) {
			if (choice == 0) {
				/* when we encounter : before a block begins we set choice to 1 to return a begin token*/
				choice = 1;
				char *yycopy = strdup( yytext );
				for ( int i = yyleng - 1; i >= 0; --i ) unput( yycopy[i] );
				free(yycopy);
				char t=yytext[0];
				isNextEOF();
				return t;
			}
			else {
				sameline_block = 0;
				choice = 0;
				isNextEOF();
				return T_begin;
			}
			}
			else {
				char t=yytext[0];
				isNextEOF();
				return t;
			}
		}
"def"	{ blockstart=1; newline_block=1;  push(spacecounter); blockcounter++; isNextEOF(); return T_def; }
"if"	{ blockstart=1; sameline_block=1; push(spacecounter); blockcounter++; isNextEOF(); return T_if; }
"elif"	{ blockstart=1; sameline_block=1; push(spacecounter); blockcounter++; isNextEOF(); return T_elif; }
"else"	{ blockstart=1; sameline_block=1; push(spacecounter); blockcounter++; isNextEOF(); return T_else; }
"loop"	{ blockstart=1; sameline_block=1; push(spacecounter); blockcounter++; isNextEOF(); return T_loop; }

 /* in NEWLINE each non-whitespace token we encounter may end a block*/
<NEWLINE>[ ]	{ isNextEOF(); spacecounter++; }
<NEWLINE>\t		{ isNextEOF(); spacecounter += 4; }
<NEWLINE>\r		{ isNextEOF(); spacecounter = 0; }
<NEWLINE>\n		{ isNextEOF(); spacecounter=0; linenumber++; }
<NEWLINE>[^ \t\r\n]	{ 	if ((spacecounter <= mystack[sp]) && (sp > -1) ) {
							sp--; blockcounter--;
							/* copy the token we scanned back to be scanned again*/
							char *yycopy = strdup( yytext );
							for ( int i = yyleng - 1; i >= 0; --i ) unput( yycopy[i] );
							free(yycopy);
							isNextEOF();

							return T_end;
					  	}
					  	char *yycopy = strdup( yytext );
					  	for ( int i = yyleng - 1; i >= 0; --i ) unput( yycopy[i] );
						free(yycopy);
						BEGIN(INITIAL);
						isNextEOF();
					}

<FINAL><<EOF>>	{ return T_eof; }
<FINAL>.		{	while (blockcounter > 0) {
						blockcounter--;
						char *yycopy = strdup( yytext );
					  	for ( int i = yyleng - 1; i >= 0; --i ) unput( yycopy[i] );
						free(yycopy);
						return T_end;
					}
				}

 /*------------------------end of layout--------------------------------- */

<<EOF>> 		{ printf("EOF shouldn't be encounteed here\n"); return T_eof; }

"and"			{ isNextEOF(); return T_and; }
"as"			{ isNextEOF(); return T_as; }
"begin"			{ isNextEOF(); return T_begin; }
"break"			{ isNextEOF(); return T_break; }
"byte"			{ isNextEOF(); return T_byte; }
"continue"		{ isNextEOF(); return T_continue; }
"decl"			{ isNextEOF(); return T_decl; }
 /*"def"			{ return T_def; } */		
 /*"elif"			{ return T_elif; } */
 /*"else"			{ return T_else; } */
"end"			{ isNextEOF(); sp--; return T_end; }
"exit"			{ isNextEOF(); return T_exit; }
"false"			{ isNextEOF(); return T_false; }
 /*"if"			{ return T_if; } */
"is"			{ isNextEOF(); return T_is; }
"int"			{ isNextEOF(); return T_int; }
 /*"loop"			{ return T_loop; } */
"not"			{ isNextEOF(); return T_not; }
"or"			{ isNextEOF(); return T_or; }
"ref"			{ isNextEOF(); return T_ref; }
"return"		{ isNextEOF(); return T_return;}
"skip"			{ isNextEOF(); return T_skip; }
"true"			{ isNextEOF(); return T_true; }
"var"			{ isNextEOF(); return T_var; }

	/*comment handling*/
"#"					{ BEGIN(SLCOMMENT); }
<SLCOMMENT>[^\n]*	{ /* nothing */ }
<SLCOMMENT>\n		{ linenumber++; spacecounter=0; isNextEOF(); BEGIN(INITIAL); }

"(*"				{ commentcounter = 1; BEGIN(MLCOMMENT); }
<MLCOMMENT>"(*"		{ commentcounter++; }
<MLCOMMENT>"*)"		{ if(--commentcounter==0) BEGIN(INITIAL); }
<MLCOMMENT>\n		{ linenumber++; spacecounter=0;}
<MLCOMMENT>"*"		{ /* nothing */ }
<MLCOMMENT>[^*\n(]*	{ /* nothing */ }

"\""				{ BEGIN(STRING); }
<STRING>\n			{ yyerror("string cannot span over a single line"); }
<STRING>[^"\n]*		{ return T_string; }
<STRING>\"			{ BEGIN(INITIAL); isNextEOF(); }


"<="			{ isNextEOF(); return T_lesseq; }
">="			{ isNextEOF(); return T_greatereq; }
"<>"			{ isNextEOF(); return T_noteq; }
":="			{ isNextEOF(); return T_assign; }

	/*extra regular expression-whenever you find the following return the ascii character*/

[(),=+\-*/%!&|><]	{ char t=yytext[0]; isNextEOF(); 
					   if (t == '<') return T_less;
					   else if (t == '>') return T_greater;
					   return t; 
					}

"["			{ char t=yytext[0]; isNextEOF(); return t; }
"]"			{ char t=yytext[0]; isNextEOF(); return t; }


	/*identifiers*/

{Intconst}		{ yylval.n = atoi(yytext); isNextEOF(); return T_intconst; }
{Char}			{ yylval.c = yytext[0]; isNextEOF(); return T_char; }
{Id}			{ yylval.s = strdup(yytext); isNextEOF(); return T_id; }
\r				{ isNextEOF(); /* nothing */}

 /*\n			{ linenumber++; } */

.			{ yyerror("illegal character "); }

%%


//if the next character from input is EOF then go to FINAL state to prepare to close any open blocks 
void isNextEOF(void) {
	int c;
	c=input();
	if (c != 0) {
		//printf("1\n");
		unput(c);
		return;
	}
	else {
		//printf("2");
		BEGIN(FINAL);
		unput('0');
		return;
	}
	return;
}

void push(int element) {
	sp++;
	mystack[sp]=element;
	return;
}

int pop(void) {
	int n= mystack[sp];
	sp--;
	return n;
}
