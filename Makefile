all : asm_interpreter

test : asm_interpreter
	./asm_interpreter test.s

test_stack: asm_interpreter
	./asm_interpreter test_stack.s

test_reg : asm_interpreter
	./asm_interpreter cat test_reg.s

test_call : asm_interpreter
	./asm_interpreter cat test_call.s

yacc : syntaxic_analyzer.y
	bison -d -o syntaxic_analyzer.c syntaxic_analyzer.y

lex : lexical_analyzer.lex
	lex -o lexical_analyzer.c lexical_analyzer.lex

interpreter: interpreter.c
	gcc -g -Wall -c interpreter.c

options: options.c
	gcc -g -Wall -c options.c

asm_interpreter : yacc lex interpreter options
	gcc -g -Wall -o asm_interpreter interpreter.o options.o lexical_analyzer.c syntaxic_analyzer.c -ll -ly
