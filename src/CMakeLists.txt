add_library(skiff
        util/logger.hpp
        util/logger.cpp
        util/string_view.hpp
        util/visitor.hpp
        ast.hpp
        ast.cpp
        ast_printer.hpp
        ast_printer.cpp
        ast_visitor.cpp
        ast_visitor.hpp
        code_gen.hpp
        code_gen.cpp
        lexer.hpp
        lexer.cpp
        parser.hpp
        parser.cpp
        )
add_executable(skc skc.cpp)
target_link_libraries(skc skiff ${SYSTEM_LIBRARIES})
add_executable(ski ski.cpp)
target_link_libraries(ski skiff ${SYSTEM_LIBRARIES})
add_executable(hello_llvm hello_llvm.cpp)
target_link_libraries(hello_llvm skiff ${SYSTEM_LIBRARIES})
add_executable(hello_llvm_c hello_llvm_c.cpp)
target_link_libraries(hello_llvm_c skiff ${SYSTEM_LIBRARIES})
