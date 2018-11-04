all:
	@tup upd

check: all
	@cd ./test/api && ./run_tests
	@cd ./test/elixr && ./all_tests.sh

clean:
	@rm -f src/*.o
	@rm -f src/types/*.o
	@rm -f src/grammar.c
	@rm -f ./elixr
	@rm -f test/*.o
	@rm -f test/run_tests

rebuild: clean all check

.PHONY: all clean rebuild
