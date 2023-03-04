assembler: pre_assembler.o first_pass.o encoding.o utils.o syntactical_analysis.o second_pass.o driver.o line_iterator.o symbol_table.o memory.o main.o
	gcc -ansi -Wall -pedantic pre_assembler.o first_pass.o encoding.o utils.o syntactical_analysis.o second_pass.o driver.o line_iterator.o symbol_table.o memory.o main.o -o assembler

pre_assembler.o: pre_assembler.c pre_assembler.h line_iterator.h utils.h constants.h
	gcc -c -ansi -pedantic -Wall pre_assembler.c

first_pass.o: first_pass.c first_pass.h syntactical_analysis.h encoding.h symbol_table.h line_iterator.h utils.h memory.h debug.h
	gcc -c -ansi -pedantic -Wall first_pass.c

encoding.o: encoding.c encoding.h syntactical_analysis.h line_iterator.h debug.h memory.h
	gcc -c -ansi -pedantic -Wall encoding.c

utils.o: utils.c utils.h syntactical_analysis.h constants.h
	gcc -c -ansi -pedantic -Wall utils.c

syntactical_analysis.o: syntactical_analysis.c syntactical_analysis.h line_iterator.h first_pass.h debug.h utils.h
	gcc -c -ansi -pedantic -Wall syntactical_analysis.c

second_pass.o: second_pass.c second_pass.h constants.h syntactical_analysis.h line_iterator.h symbol_table.h encoding.h memory.h debug.h utils.h constants.h
	gcc -c -ansi -pedantic -Wall second_pass.c

driver.o: driver.c driver.h pre_assembler.h memory.h debug.h first_pass.h second_pass.h
	gcc -c -ansi -pedantic -Wall driver.c

line_iterator.o: utils.h line_iterator.h line_iterator.c
	gcc -c -ansi -pedantic -Wall line_iterator.c

symbol_table.o: symbol_table.h symbol_table.c utils.h
	gcc -c -ansi -pedantic -Wall symbol_table.c

memory.o: memory.h memory.c constants.h
	gcc -c -ansi -pedantic -Wall memory.c

main.o: driver.h main.c
	gcc -c -ansi -pedantic -Wall main.c

clean:
	rm -f *.o