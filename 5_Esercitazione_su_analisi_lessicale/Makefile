lexer-cpp: lexer-cpp.ll
	flex -o lexer-cpp.cc lexer-cpp.ll
	g++ -Wall -Wextra -O3 -o lexer-cpp lexer-cpp.cc

all: lexer-cpp

