enPAC:main.o Data_Structures.o Lexer.o pnml_parse.o product.o SBA.o Syntax_Tree.o TBA.o TGBA.o xml2ltl.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o
	g++ -g -o enPAC main.o Data_Structures.o Lexer.o pnml_parse.o product.o SBA.o Syntax_Tree.o TBA.o TGBA.o xml2ltl.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o -lpthread

main.o:main.cpp product.h xml2ltl.h
	g++ -g -c main.cpp -lpthread
xml2ltl.o:xml2ltl.cpp xml2ltl.h
	g++ -g -c xml2ltl.cpp -lpthread
Data_Structures.o:Data_Structures.cpp Data_Structures.h
	g++ -g -c Data_Structures.cpp -lpthread
Lexer.o:Lexer.cpp Lexer.h
	g++ -g -c Lexer.cpp -lpthread
pnml_parse.o:pnml_parse.cpp pnml_parse.h
	g++ -g -c pnml_parse.cpp -lpthread
product.o:product.cpp product.h
	g++ -g -c product.cpp -lpthread
SBA.o:SBA.cpp SBA.h
	g++ -g -c SBA.cpp -lpthread
Syntax_Tree.o:Syntax_Tree.cpp Syntax_Tree.h
	g++ -g -c Syntax_Tree.cpp -lpthread
TBA.o:TBA.cpp TBA.h
	g++ -g -c TBA.cpp -lpthread
TGBA.o:TGBA.cpp TGBA.h
	g++ -g -c TGBA.cpp -lpthread
tinystr.o:tinystr.cpp tinystr.h
	g++ -g -c tinystr.cpp -lpthread
tinyxml.o:tinyxml.cpp tinyxml.h
	g++ -g -c tinyxml.cpp -lpthread
tinyxmlerror.o:tinyxmlerror.cpp tinyxml.h
	g++ -g -c tinyxmlerror.cpp -lpthread
tinyxmlparser.o:tinyxmlparser.cpp tinyxml.h
	g++ -g -c tinyxmlparser.cpp -lpthread

clean:
	rm main.o Data_Structures.o Lexer.o pnml_parse.o product.o SBA.o Syntax_Tree.o TBA.o TGBA.o xml2ltl.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o
