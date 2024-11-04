#pragma once

#include "eval-parser.h"
#include "json-parser.h"

JsonAstNode evaluate(const EvalAstNode &evalAst, const JsonAstNode &jsonAst, const JsonAstNode &jsonRoot);
