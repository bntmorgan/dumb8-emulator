all : asm_interpreter

test : asm_interpreter
	cat test.s | ./asm_interpreter

test_stack: asm_interpreter
	cat test_stack.s | ./asm_interpreter

test_reg : asm_interpreter
	cat test_reg.s | ./asm_interpreter

yacc : syntaxic_analyzer.y
	yacc -d -o syntaxic_analyzer.c syntaxic_analyzer.y

lex : lexical_analyzer.lex
	lex -o lexical_analyzer.c lexical_analyzer.lex

interpreter: interpreter.c
	gcc -g -Wall -c interpreter.c

asm_interpreter : yacc lex interpreter
	gcc -g -Wall -o asm_interpreter interpreter.o lexical_analyzer.c syntaxic_analyzer.c -ll -ly
