.PHONY: clean distclean default

# export SHELL := /bin/bash

# ifeq ($(shell uname -s),Darwin)
# export PATH := /usr/local/opt/llvm/bin:$(PATH)
# endif

CXXFLAGS=-Wall -g -std=c++11 `llvm-config --cxxflags`
LDFLAGS=`llvm-config --cxxflags --ldflags --libs all --system-libs`

all: dana simple_dana libs

libs: 
	cd edsger_lib; bash libs.sh

parser.hpp parser.cpp: parser.y
	bison -dv -o parser.cpp parser.y

simple_lexer.cpp: simple_lexer.l
	flex -s -o simple_lexer.cpp simple_lexer.l

lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

parser.o: parser.cpp parser.hpp ast.hpp
	$(CXX) $(CXXFLAGS) -c parser.cpp

simple_lexer.o: simple_lexer.cpp parser.hpp
	$(CXX) $(CXXFLAGS) -c simple_lexer.cpp

lexer.o: lexer.cpp parser.hpp
	$(CXX) $(CXXFLAGS) -c lexer.cpp

ast.o: ast.cpp ast.hpp
	$(CXX) $(CXXFLAGS) -c ast.cpp

symbol.o: symbol.c symbol.h
	gcc -c -Wall -g symbol.c

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<


# lexer: lexer.o
# 	$(CXX) $(CXXFLAGS) -o lexer $^ -lfl

# parser: lexer.o parser.o
# 	$(CXX) $(CXXFLAGS) -o parser $^ -lfl

simple_dana: simple_lexer.o parser.o ast.o symbol.o error.o general.o
	$(CXX) $(CXXFLAGS) -o simple_dana $^ $(LDFLAGS)

dana: lexer.o parser.o ast.o symbol.o error.o general.o
	$(CXX) $(CXXFLAGS) -o dana $^ $(LDFLAGS)

clean:
	$(RM) simple_lexer.cpp lexer.cpp parser.cpp parser.hpp parser.output a.* *.o *~ *.gch

distclean: clean
	$(RM) dana simple_dana
