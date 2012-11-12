CXXFLAGS += -g -Wall -Wextra

TESTS = my_list_test

all : $(TESTS)

clean :
	rm -f $(TESTS) *.o

my_list_test.o : my_list_test.cpp my_list.hpp
	$(CXX) $(CXXFLAGS) -c my_list_test.cpp

$(TESTS): my_list_test.o
	$(CXX) $(CXXFLAGS) $^ -o $@
