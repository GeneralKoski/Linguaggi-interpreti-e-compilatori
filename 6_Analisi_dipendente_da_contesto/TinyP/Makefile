CXX = clang++
CXXFLAGS = -std=c++14 -Wall -Wextra -g -O3

INC_FILES = \
ast.hh \
ast_dumper.hh \
ast_visitor.hh \
collecting.hh \
interpreter.hh \
pretty_printer.hh \
utils.hh

CC_FILES = \
ast_dumper.cc \
collecting.cc \
interpreter.cc \
pretty_printer.cc \
tinyP-concrere.cc \
tinyP-pp.cc \
tinyP-run.cc \
utils.cc

BASIC_OBJ_FILES = \
lexer.o \
parser.o \
utils.o

RUN_OBJ_FILES = \
${BASIC_OBJ_FILES} \
interpreter.o \
tinyP-run.o

CONCRETE_OBJ_FILES = \
${BASIC_OBJ_FILES} \
pretty_printer.o \
collecting.o \
tinyP-concrete.o

ASTDUMP_OBJ_FILES = \
${BASIC_OBJ_FILES} \
ast_dumper.o \
tinyP-astdump.o

PP_OBJ_FILES = \
${BASIC_OBJ_FILES} \
pretty_printer.o \
tinyP-pp.o

all: tinyP-run tinyP-astdump tinyP-pp tinyP-concrete

lexer.cc: lexer.ll parser.hh
	flex -o lexer.cc lexer.ll

parser.cc parser.hh: parser.yy
	bison --report=all --defines=parser.hh -o parser.cc parser.yy

lexer.o: lexer.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
parser.o: parser.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
utils.o: utils.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<

# AST dumper
ast_dumper.o: ast_dumper.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
tinyP-astdump.o: tinyP-astdump.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
tinyP-astdump: ${ASTDUMP_OBJ_FILES}
	g++ -o tinyP-astdump ${ASTDUMP_OBJ_FILES} -lgmpxx -lgmp

# pretty printer
pretty_printer.o: pretty_printer.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
tinyP-pp.o: tinyP-pp.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
tinyP-pp: ${PP_OBJ_FILES}
	g++ -o tinyP-pp ${PP_OBJ_FILES} -lgmpxx -lgmp

# interpreter
interpreter.o: interpreter.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
tinyP-run.o: tinyP-run.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
tinyP-run: ${RUN_OBJ_FILES}
	g++ -o tinyP-run ${RUN_OBJ_FILES} -lgmpxx -lgmp

# collecting
collecting.o: collecting.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
tinyP-concrete.o: tinyP-concrete.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<
tinyP-concrete: ${CONCRETE_OBJ_FILES}
	g++ -o tinyP-concrete ${CONCRETE_OBJ_FILES} -lgmpxx -lgmp

clean:
	rm -f tinyP-pp tinyP-astdump tinyP-run tinyP-concrete *.o parser.hh parser.cc parser.output lexer.cc
