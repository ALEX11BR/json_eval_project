cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)

project(json_eval)

add_executable(${PROJECT_NAME} src/main.cpp src/eval-lexer.cpp src/eval-parser.cpp src/json-lexer.cpp src/json-parser.cpp src/print.cpp src/evaluate.cpp)
