.PHONY:ALL
ALL: test example1 example2
test:test.cc
	g++ -g -o $@ $^ -std=c++11
example1:example1.cc
	g++ -g -o $@ $^ -std=c++11
example2:example2.cc
	g++ -g -o $@ $^ -std=c++11
.PHONY clean:
clean:	
	rm -f test example1