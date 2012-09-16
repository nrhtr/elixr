all:
	@tup upd

check: all
	@./test/run_tests

clean:
	@rm -f src/*.o
	@rm -f src/types/*.o
	@rm -f src/grammar.c
	@rm -f ./elixr
	@rm -f test/*.o
	@rm -f test/run_tests

rebuild: clean all check

.PHONY: all clean rebuild
