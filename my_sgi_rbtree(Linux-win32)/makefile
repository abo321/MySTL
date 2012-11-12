CXXFLAGS += -g -Wall -Wextra

TESTS = my_rb_tree_test

all : $(TESTS)

clean :
	rm -f $(TESTS) *.o

my_rb_tree_test.o : my_rb_tree_test.cpp my_rb_tree.hpp
	$(CXX) $(CXXFLAGS) -c my_rb_tree_test.cpp

my_rb_tree.o : my_rb_tree.cpp my_rb_tree.hpp
	$(CXX) $(CXXFLAGS) -c my_rb_tree.cpp

$(TESTS): my_rb_tree_test.o my_rb_tree.o
	$(CXX) $(CXXFLAGS) $^ -o $@
