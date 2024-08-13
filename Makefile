.PHONY: output clean

clean:
	rm -rf output/*

output:
	mkdir -p output

output/lib/utest:
	git clone git@github.com:sheredom/utest.h.git output/lib/utest

build_test: output output/lib/utest
	$(CXX) tests/test_main.cpp -Ioutput/lib -Iinclude -o output/run_tests

run_tests: build_test
	output/run_tests
