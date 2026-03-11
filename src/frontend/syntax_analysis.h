#pragma once

#include "../tree/tree.h"
#include "lexical_analysis.h"
#include "../common/errors.h"



void GetTreeFromTokens (TokenArr* tokens, Tree_t* tree, Err_t* err);