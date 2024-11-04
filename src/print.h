#pragma once

#include "json-parser.h"
#include "eval-parser.h"

void printJson(const JsonAstNode &ast, std::ostream &outputStream);
void printToUser(const JsonAstNode &ast, std::ostream &outputStream);
void debugEval(const EvalAstNode &ast, std::ostream &outputStream, int level);
