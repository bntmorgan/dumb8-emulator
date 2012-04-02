all : asm_interpreter

test : asm_interpreter
	cat test.s | ./asm_interpreter

yacc : syntaxic_analyzer.y
	yacc -d -o syntaxic_analyzer.c syntaxic_analyzer.y

lex : lexical_analyzer.lex
	lex -o lexical_analyzer.c lexical_analyzer.lex

asm_interpreter : yacc lex 
	gcc -g -Wall -o asm_interpreter lexical_analyzer.c syntaxic_analyzer.c -ll -ly
