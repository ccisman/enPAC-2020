cmake_minimum_required(VERSION 3.12)
project(enPAC_on_the_fly)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(enPAC_on_the_fly
        Data_Structures.cpp
        Data_Structures.h
        Lexer.cpp
        Lexer.h
        main.cpp
        pnml_parse.cpp
        pnml_parse.h
        product.cpp
        product.h
        SBA.cpp
        SBA.h
        Syntax_Tree.cpp
        Syntax_Tree.h
        TBA.cpp
        TBA.h
        TGBA.cpp
        TGBA.h
        tinystr.cpp
        tinystr.h
        tinyxml.cpp
        tinyxml.h
        tinyxmlerror.cpp
        tinyxmlparser.cpp
        xml2ltl.cpp
        xml2ltl.h)

find_package(Threads REQUIRED)
target_link_libraries(enPAC_on_the_fly Threads::Threads)
