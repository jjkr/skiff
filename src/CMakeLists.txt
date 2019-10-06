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
        source.hpp
        source.cpp
        compiler.hpp
        compiler.cpp
        wasm_code_gen.hpp
        wasm_code_gen.cpp
        wasm_compiler.hpp
        wasm_compiler.cpp
        )
add_executable(skc skc.cpp)
target_link_libraries(skc skiff ${SYSTEM_LIBRARIES})
add_executable(ski ski.cpp)
target_link_libraries(ski skiff ${SYSTEM_LIBRARIES})
add_executable(skic skic.cpp)
target_link_libraries(skic skiff binaryen::binaryen Threads::Threads)
add_executable(hello_llvm hello_llvm.cpp)
target_link_libraries(hello_llvm skiff ${SYSTEM_LIBRARIES})
add_executable(hello_llvm_c hello_llvm_c.cpp)
target_link_libraries(hello_llvm_c skiff ${SYSTEM_LIBRARIES})
add_executable(hello_binaryen hello_binaryen.cpp)
target_link_libraries(hello_binaryen skiff binaryen::binaryen Threads::Threads)
