#ifndef _JOJ_KEYWORDS_H
#define _JOJ_KEYWORDS_H

#define KEYWORD_COLON ":"
#define KEYWORD_SEMICOLON ";"
#define KEYWORD_ASSIGN "="
#define KEYWORD_EQUAL "=="
#define KEYWORD_NOT_EQUAL "!="
#define KEYWORD_GREATER_THAN ">="
#define KEYWORD_LESS_THAN "<="
#define KEYWORD_AND "&&"
#define KEYWORD_OR "||"
#define KEYWORD_ADD_SIGN "+"
#define KEYWORD_SUB_SIGN "-"
#define KEYWORD_DIV_SIGN "/"
#define KEYWORD_MUL_SIGN "*"
#define KEYWORD_OPEN_PAR "("
#define KEYWORD_CLOSE_PAR ")"
#define KEYWORD_OPEN_BRAC "["
#define KEYWORD_CLOSE_BRAC "]"
#define KEYWORD_OPEN_CURLY_BRAC "{"
#define KEYWORD_CLOSE_CURLY_BRAC "}"
#define KEYWORD_COUNTER_BAR "'\\'"
#define KEYWORD_PIPE "|"


#ifdef LANGUAGE_PORTUGUESE
#define KEYWORD_MAIN_FUNC "principal"
#define KEYWORD_BOOL "booleano"
#define KEYWORD_CHAR "letra"
#define KEYWORD_INT "int"
#define KEYWORD_FLOAT "dec"
#define KEYWORD_STRING "frase"
#define KEYWORD_LET "var"
#define KEYWORD_FUNC "func"
#define KEYWORD_IF "se"
#define KEYWORD_ELSEIF "ouse"
#define KEYWORD_ELSE "senao"
#define KEYWORD_MATCH "igualar"
#define KEYWORD_FOR "para"
#define KEYWORD_IN "em"
#define KEYWORD_WHILE "enquanto"
#define KEYWORD_LOOP "loop"
#define KEYWORD_BREAK "quebre"
#define KEYWORD_TRUE "verdade"
#define KEYWORD_FALSE "falso"
#define KEYWORD_VOID "vazio"
#elif defined(LANGUAGE_ENGLISH)
#define KEYWORD_MAIN_FUNC "principal"
#define KEYWORD_BOOL "booleano"
#define KEYWORD_CHAR "letra"
#define KEYWORD_INT "int"
#define KEYWORD_FLOAT "dec"
#define KEYWORD_STRING "frase"
#define KEYWORD_LET "var"
#define KEYWORD_FUNC "func"
#define KEYWORD_IF "se"
#define KEYWORD_ELSEIF "ouse"
#define KEYWORD_ELSE "senao"
#define KEYWORD_MATCH "igualar"
#define KEYWORD_FOR "para"
#define KEYWORD_IN "em"
#define KEYWORD_WHILE "enquanto"
#define KEYWORD_LOOP "loop"
#define KEYWORD_BREAK "quebre"
#define KEYWORD_TRUE "verdade"
#define KEYWORD_FALSE "falso"
#define KEYWORD_VOID "vazio"
#else // English is the Default Language
#define KEYWORD_MAIN_FUNC "main"
#define KEYWORD_BOOL "boolean"
#define KEYWORD_CHAR "char"
#define KEYWORD_INT "int"
#define KEYWORD_FLOAT "float"
#define KEYWORD_STRING "string"
#define KEYWORD_LET "let"
#define KEYWORD_FUNC "func"
#define KEYWORD_IF "if"
#define KEYWORD_ELSEIF "elif"
#define KEYWORD_ELSE "else"
#define KEYWORD_MATCH "match"
#define KEYWORD_FOR "for"
#define KEYWORD_IN "in"
#define KEYWORD_WHILE "while"
#define KEYWORD_LOOP "loop"
#define KEYWORD_BREAK "break"
#define KEYWORD_TRUE "true"
#define KEYWORD_FALSE "false"
#endif

#endif // _JOJ_KEYWORDS_H