CPPFLAGS += -std=c++11 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include

all : bin/c_compiler

src/parser.tab.cpp src/parser.tab.hpp : src/parser.y
	bison -v -d src/parser.y -o src/parser.tab.cpp

src/lexer.yy.cpp : src/lexer.flex src/parser.tab.hpp
	flex -o src/lexer.yy.cpp  src/lexer.flex

bin/c_compiler : src/compile.o src/parser.tab.o src/lexer.yy.o src/parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/compile $^
	chmod u+x ./bin/c_compiler
	make bin/c_translate
	chmod u+x ./test_compiler.sh
	chmod u+x ./test_translator.sh
	dos2unix bin/c_compiler

bin/c_translate : src/translate.o src/parser.tab.o src/lexer.yy.o src/parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/translate $^


clean :
	-rm bin/translate
	-rm bin/compile
	-rm -rf temp
	-rm src/*.tab.*
	-rm src/*.yy.*
	-rm src/*.o
	-rm -rf pyres
