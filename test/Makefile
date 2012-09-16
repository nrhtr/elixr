CC = gcc
CFLAGS = -std=c99 -lcheck
OBJS = ../src/ast.o ../src/dbpack.o ../src/closure.o ../src/compile.o ../src/init.o ../src/lookup3.o ../src/list.o ../src/number.o ../src/objmodel.o ../src/string.o ../src/symbol.o ../src/table.o

TESTS = \
	check_main.o   \
	check_list.o   \
	check_string.o \
	check_symbol.o \
	check_table.o  \

PARSER = lexer.o parser.o
MAIN = driver.o

all: ./run_tests
	./run_tests

./run_tests: $(TESTS) $(OBJS)
	$(CC) $(CFLAGS) $(TESTS) $(OBJS) -o run_tests

