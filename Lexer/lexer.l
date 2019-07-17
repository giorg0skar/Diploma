%{
#include <stdio.h>

//----defining to test lexer alone
#define T_eof 0
#define T_and 1000
#define T_as 1001
#define T_begin 1002
#define T_break 1003
#define T_byte 1004
#define T_continue 1005
#define T_decl 1006
#define T_def 1007
#define T_elif 1008
#define T_else 1009
#define T_end 1010
#define T_exit 1011
#define T_false 1012
#define T_if 1013
#define T_is 1014
#define T_int 1015
#define T_loop 1016
#define T_not 1017
#define T_or 1018
#define T_ref 1019
#define T_return 1020
#define T_skip 1021
#define T_true 1022
#define T_var 1023
#define T_intconst 1024
#define T_char 1025
#define T_id 1026
#define T_string 1027
#define T_greater 1028
#define T_less 1029
#define T_greatereq 1030
#define T_lesseq 1031
#define T_noteq 1032
#define T_assign 1033

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

[ ]		{ spacecounter++; }
\t		{ spacecounter += 4; }
\n		{ spacecounter=0; linenumber++;
			/* def blocks always begin after newline character*/
			if (newline_block) {
				newline_block = 0;
			
				return T_begin;
			}
			else {
				BEGIN(NEWLINE);
			
			}
		}
:		{ if (sameline_block) {
			if (choice == 0) {
				/* when we encounter : before a block begins we set choice to 1 to return a begin token*/
				choice = 1;
				char *yycopy = strdup( yytext );
				for ( int i = yyleng - 1; i >= 0; --i ) unput( yycopy[i] );
				//strcpy(yytext, yycopy);
				free(yycopy);
				char t=yytext[0];
			
				return t;
			}
			else {
				sameline_block = 0;
				choice = 0;
			
				return T_begin;
			}
			}
			else {
			char t=yytext[0];
			return t;
			}
		}
"def"	{ blockstart=1; newline_block=1;  push(spacecounter); blockcounter++; return T_def; }
"if"	{ blockstart=1; sameline_block=1; push(spacecounter); blockcounter++; return T_if; }
"elif"	{ blockstart=1; sameline_block=1; push(spacecounter); blockcounter++; return T_elif; }
"else"	{ blockstart=1; sameline_block=1; push(spacecounter); blockcounter++; return T_else; }
"loop"	{ blockstart=1; sameline_block=1; push(spacecounter); blockcounter++; return T_loop; }

 /* in NEWLINE each non-whitespace token we encounter may end a block*/
<NEWLINE>[ ]	{ spacecounter++; }
<NEWLINE>\t		{ spacecounter += 4; }
<NEWLINE>\n		{ spacecounter=0; linenumber++; }
<NEWLINE>[^ \t\n]	{ 	if ((spacecounter <= mystack[sp]) && (sp > -1) ) {
							sp--; blockcounter--;
							/* copy the token we scanned back to be scanned again*/
							char *yycopy = strdup( yytext );
							for ( int i = yyleng - 1; i >= 0; --i ) unput( yycopy[i] );
							//strcpy(yytext, yycopy);
							free(yycopy);
						

							return T_end;
					  	}
					  	char *yycopy = strdup( yytext );
					  	for ( int i = yyleng - 1; i >= 0; --i ) unput( yycopy[i] );
						//strcpy(yytext, yycopy);
						free(yycopy);
						BEGIN(INITIAL);
					}

<FINAL><<EOF>>	{ return T_eof; }
<FINAL>.		{	if (blockcounter > 0) {
						blockcounter--;
						char *yycopy = strdup( yytext );
					  	for ( int i = yyleng - 1; i >= 0; --i ) unput( yycopy[i] );
						//strcpy(yytext, yycopy);
						free(yycopy);
						return T_end;
					}
					else if (blockcounter == 0) return T_eof;
				}

 /*------------------------end of layout--------------------------------- */

<<EOF>> 			{ 
						if (blockcounter > 0) {
							blockcounter--;
							char *yycopy = strdup(yytext);
							unput('0');
							BEGIN(FINAL);
							//strcpy(yytext, yycopy);
							free(yycopy);
							return T_end;
						}
						return T_eof;
					}

"and"			{ return T_and; }
"as"			{ return T_as; }
"begin"			{ return T_begin; }
"break"			{ return T_break; }
"byte"			{ return T_byte; }
"continue"		{ return T_continue; }
"decl"			{ return T_decl; }
 /*"def"			{ return T_def; } */		
 /*"elif"			{ return T_elif; } */
 /*"else"			{ return T_else; } */
"end"			{ sp--; return T_end; }
"exit"			{ return T_exit; }
"false"			{ return T_false; }
 /*"if"			{ return T_if; } */
"is"			{ return T_is; }
"int"			{ return T_int; }
 /*"loop"			{ return T_loop; } */
"not"			{ return T_not; }
"or"			{ return T_or; }
"ref"			{ return T_ref; }
"return"		{ return T_return;}
"skip"			{ return T_skip; }
"true"			{ return T_true; }
"var"			{ return T_var; }

	/*comment handling*/
"#"					{ BEGIN(SLCOMMENT); }
<SLCOMMENT>[^\n]*	{ /* nothing */ }
<SLCOMMENT>\n		{ linenumber++; spacecounter=0; BEGIN(INITIAL); }

"(*"				{ commentcounter = 1; BEGIN(MLCOMMENT); }
<MLCOMMENT>"(*"		{ commentcounter++; }
<MLCOMMENT>"*)"		{ if(--commentcounter==0) BEGIN(INITIAL); }
<MLCOMMENT>\n		{ linenumber++; spacecounter=0;}
<MLCOMMENT>"*"		{ /* nothing */ }
<MLCOMMENT>[^*\n(]*	{ /* nothing */ }

"\""				{ BEGIN(STRING); }
<STRING>[^"]*		{ return T_string; }
<STRING>\"			{ BEGIN(INITIAL); }

"<"				{ return T_less; }
">"				{ return T_greater; }
"<="			{ return T_lesseq; }
">="			{ return T_greatereq; }
"<>"			{ return T_noteq; }
":="			{ return T_assign; }

	/*extra regular expression-whenever you find the following return the ascii character*/

[(),=+\-*/%!&|]	{ char t=yytext[0]; return t; }

"["			{ char t=yytext[0]; return t; }
"]"			{ char t=yytext[0]; return t; }


	/*identifiers*/

{Intconst}		{ return T_intconst; }
{Char}			{ return T_char; }
{Id}			{ return T_id; }
\r				{ /* nothing */}

 /*\n			{ linenumber++; } */

.			{ yyerror("illegal character"); }

%%

void yyerror (const char *msg) {
  fprintf(stderr, "Dana error: %s\n", msg);
  fprintf(stderr, "Aborting, I've had enough with line %d ...\n",linenumber);
  exit(1);
}

//if the next character form input is EOF then go to FINAL state to prepare to close any open blocks 
void isNextEOF(void) {
	register int c;
	c=input();
	if (c!=0) {
		unput(c);
		return;
	}
	else {
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

int main() {
	int token;
  	do {
    	token = yylex();
    	//printf("token=%d, lexeme=\"%s\"\n", token, yytext);
		printf("%d\n", token);
  	} while (token != T_eof);
	return 0;
}