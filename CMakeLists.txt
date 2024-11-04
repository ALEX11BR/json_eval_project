cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)

project(json_eval)

set(sources
    src/app-exception.cpp
    src/evaluate.cpp
    src/eval-lexer.cpp
    src/eval-parser.cpp
    src/json-lexer.cpp
    src/json-parser.cpp
    src/print.cpp
)

add_executable(${PROJECT_NAME} src/main.cpp ${sources})

enable_testing()

add_executable(test-runner src/test-runner.cpp ${sources})
add_test(NAME test1 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "a.b[1]" "2")
add_test(NAME test2 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "a.b[2].c" "test")
add_test(NAME test3 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "a.b" "[ 1, 2, { \"c\": \"test\" }, [ 11, 12 ] ]")
add_test(NAME test4 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "a.b[a.b[1]].c" "test")
add_test(NAME test5 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "max(a.b[0], a.b[1])" "2")
add_test(NAME test6 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "min(a.b[3])" "11")
add_test(NAME test7 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "size(a)" "1")
add_test(NAME test8 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "size(a.b)" "4")
add_test(NAME test9 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "size(a.b[a.b[1]].c)" "4")
add_test(NAME test10 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "max(a.b[0], 10, a.b[1], 15)" "15")
add_test(NAME test11 COMMAND $<TARGET_FILE:test-runner> ${PROJECT_SOURCE_DIR}/test-util/input.json "sum(a.b[3])" "23")
