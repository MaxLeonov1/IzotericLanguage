#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "../common/logs.h"
#include "syntax_analysis.h"
#include "../stack/stack.h"

/*=====================================================================================*/

#define INCR_PTR_   tokens->ptr++;
#define DECR_PTR_   tokens->ptr--;
#define CUR_TOKEN_  tokens->nodes[tokens->ptr]
#define NEXT_TOKEN_ tokens->nodes[tokens->ptr+1]
#define NAMES_STK_  (tokens->nametable_stk)
#define TOKEN_NAME_(token_) tokens->nametable->buff[token_->data.var_idx].name


#define RET_NULL_IF_ERR_                         \
    do {                                         \
        if(err->code != (int)FrontendErr::NONE)  \
            return nullptr;                      \
    } while(0);   
    
#define SYNTAX_ASSERT_(stat)                                                      \
    do {                                                                          \
        if (!(stat)) {                                                            \
            START_LOG_DUMP_(__FILE__, __LINE__, __FUNCTION__);                    \
            err->code = (int)FrontendErr::INVALID_SYNTAX;                         \
            SetErrMes(err,                                                        \
                    "SYNTAX ERR\n"                                                \
                    "IN FUNCTION: %s\n"                                           \
                    "IN FILE %s:%d\n"                                             \
                    "LINE IN SOURCE FILE: %d\n",                                  \
                    __FUNCTION__, __FILE__, __LINE__, CUR_TOKEN_->info.line);     \
            PrintLog(err->data);                                                  \
            END_LOG_DUMP_;                                                        \
            return nullptr;                                                       \
        }                                                                         \
    } while(0);

#define LOG_CHECK_MARK_                                         \
    do {                                                        \
        START_LOG_DUMP_(__FILE__, __LINE__, __FUNCTION__);      \
        PrintLog("ENTER FUNC");                                 \
    } while(0);
 

#define SKIP_BREAKS_                                                                            \
do {                                                                                            \                         
    while(CUR_TOKEN_->data.oper == Oper_t::LINE_END && CUR_TOKEN_->type == NodeType::OPER) {    \
        INCR_PTR_;                                                                              \ 
    }                                                                                           \                  
} while (0);                                                                                

/*=====================================================================================*/

static void TryInsertNameInStack(const Stack_t* stk, const char* str,
                                 bool* exists, bool* is_func, TokenType type);

static Node* GetOldFuncName  (TokenArr* tokens, Err_t* err);
static Node* GetNewFuncName  (TokenArr* tokens, Err_t* err);
static Node* GetNewVar       (TokenArr* tokens, Err_t* err);
static Node* GetOldVar       (TokenArr* tokens, Err_t* err);

static Node* GetProgram      (TokenArr* tokens, Err_t* err);

static Node* GetDefFunc      (TokenArr* tokens, Err_t* err);
static Node* GetFuncParams   (TokenArr* tokens, Err_t* err);

static Node* GetCallFunc     (TokenArr* tokens, Err_t* err);
static Node* GetFuncArgs     (TokenArr* tokens, Err_t* err);

static Node* GetBlock        (TokenArr* tokens, Err_t* err);
static Node* GetLine         (TokenArr* tokens, Err_t* err);

static Node* GetWhileSection (TokenArr* tokens, Err_t* err);
static Node* GetIfSection    (TokenArr* tokens, Err_t* err);
static Node* GetInit         (TokenArr* tokens, Err_t* err);
static Node* GetAssignment   (TokenArr* tokens, Err_t* err);
static Node* GetReturn       (TokenArr* tokens, Err_t* err);

static Node* GetExpression   (TokenArr* tokens, Err_t* err);
static Node* GetOperAND      (TokenArr* tokens, Err_t* err);
static Node* GetOperCompare  (TokenArr* tokens, Err_t* err);
static Node* GetOperADD_SUB  (TokenArr* tokens, Err_t* err);
static Node* GetOperMUL_DIV  (TokenArr* tokens, Err_t* err);
static Node* GetOperDEG      (TokenArr* tokens, Err_t* err);
static Node* GetElementary   (TokenArr* tokens, Err_t* err);

static Node* GetName         (TokenArr* tokens, Err_t* err);
static Node* GetNum          (TokenArr* tokens, Err_t* err);
static Node* GetLineEnd      (TokenArr* tokens, Err_t* err);

/*=====================================================================================*/

static Node* GetNewFuncName (TokenArr* tokens, Err_t* err) { 

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::VAR)
    Node* var = MakeNode(NodeType::VAR, {.var_idx = CUR_TOKEN_->data.var_idx});

    bool exists = false;
    bool is_func = false;

    TryInsertNameInStack(&NAMES_STK_, TOKEN_NAME_(CUR_TOKEN_), &exists, &is_func, TokenType::FUNC_NAME);
    
    if (exists) {

        err->code = (int) FrontendErr::INVALID_SYNTAX;
        if (is_func)
            SetErrMes(err, "FUNCTION ON LINE %d ALREADY EXISTS<br>\n", CUR_TOKEN_->info.line);
        else
            SetErrMes(err, "CAN NOT NAME FUNCTION ON LINE %d AS VARIABLE<br>\n", CUR_TOKEN_->info.line);
        return nullptr;
    }

    INCR_PTR_;
    return var;

}

/*=====================================================================================*/

static Node* GetOldFuncName (TokenArr* tokens, Err_t* err) {

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::VAR)

    Node* var = MakeNode(NodeType::VAR, {.var_idx = CUR_TOKEN_->data.var_idx});

    bool exists = false;
    bool is_func = false;

    TryInsertNameInStack(&NAMES_STK_, TOKEN_NAME_(CUR_TOKEN_), &exists, &is_func, TokenType::FUNC_NAME);

    if (!is_func) {

        err->code = (int) FrontendErr::INVALID_SYNTAX;
        SetErrMes(err, "CAN NOT GET VARIABLE AS FUNCTION ON LINE %d<br>\n", CUR_TOKEN_->info.line);
        return nullptr;
    }
    if (!exists) {

        err->code = (int) FrontendErr::INVALID_SYNTAX;
        SetErrMes(err, "UNKNOWN FUNCTION ON LINE %d<br>\n", CUR_TOKEN_->info.line);
        return nullptr;
    }

    INCR_PTR_;

    return var;
}

/*=====================================================================================*/

static Node* GetNewVar (TokenArr* tokens, Err_t* err) {

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::VAR)

    Node* var = MakeNode(NodeType::VAR, {.var_idx = CUR_TOKEN_->data.var_idx});

    bool exists = false;
    bool is_func = false;

    TryInsertNameInStack(&NAMES_STK_, TOKEN_NAME_(CUR_TOKEN_), &exists, &is_func, TokenType::VAR_NAME);

    if (exists) {

        err->code = (int) FrontendErr::INVALID_SYNTAX;
        if (!is_func)
            SetErrMes(err, "VARIABLE ON LINE %d ALREADY EXISTS<br>\n", CUR_TOKEN_->info.line);
        else
            SetErrMes(err, "CAN NOT NAME VARIABLE ON LINE %d AS FUNCTION<br>\n", CUR_TOKEN_->info.line);
        return nullptr;
    }

    INCR_PTR_;

    return var;
}


/*=====================================================================================*/

static Node* GetOldVar (TokenArr* tokens, Err_t* err) {

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::VAR)

    Node* var = MakeNode(NodeType::VAR, {.var_idx = CUR_TOKEN_->data.var_idx});

    // if (storage->tokens->all_names.list[CUR_TOKEN.info.name_id].name[0] == '\"')
    //     return var;

    bool exists = false;
    bool is_func = false;

    TryInsertNameInStack(&NAMES_STK_, TOKEN_NAME_(CUR_TOKEN_), &exists, &is_func, TokenType::VAR_NAME);

    if (is_func) {

        err->code = (int) FrontendErr::INVALID_SYNTAX;
        SetErrMes(err, "CAN NOT GET FUNCTION AS VARIABLE ON LINE %d<br>\n", CUR_TOKEN_->info.line);
        return nullptr;
    }
    if (!exists) {

        err->code = (int) FrontendErr::INVALID_SYNTAX;
        SetErrMes(err, "UNKNOWN VARIABLE ON LINE %d<br>\n", CUR_TOKEN_->info.line);
        return nullptr;
    }

    INCR_PTR_;

    return var;
}

/*=====================================================================================*/

void GetTreeFromTokens (TokenArr* tokens, Tree_t* tree, Err_t* err) {

    assert(tokens);
    assert(tree);
    assert(err);

    tree->root = GetProgram(tokens, err);

}

/*=====================================================================================*/

static Node* GetProgram (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::VAR)

    Node* funcs = nullptr;

    funcs = GetDefFunc(tokens, err);
    RET_NULL_IF_ERR_

    while(CUR_TOKEN_->type == NodeType::VAR) {

        Node* func = GetDefFunc(tokens, err);
        RET_NULL_IF_ERR_

        funcs = MakeNode(NodeType::OPER, {.oper = Oper_t::NEW_FUNC}, {.line = 0}, funcs, func);
    }

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::PROGRAM_END)

    return funcs;
}

/*=====================================================================================*/

static Node* GetDefFunc (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::VAR)
    Node* name = GetNewFuncName(tokens, err);
    SYNTAX_ASSERT_(name);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::L_BRACKET)
    INCR_PTR_

    NameTable_t* local_table = NametableCtor();
    StackPush(&NAMES_STK_, local_table);

    Node* params = GetFuncParams(tokens, err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::R_BRACKET)
    INCR_PTR_

    Node* block = GetBlock(tokens, err);
    SYNTAX_ASSERT_(block);

    StackPop(&NAMES_STK_);

    ConnectNodes(name, params, block);

    Node* func = MakeNode(NodeType::OPER, {.oper = Oper_t::NEW_FUNC}, {.line = 0}, name);
    
    return func;

}

/*=====================================================================================*/

static Node* GetFuncParams (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    if (CUR_TOKEN_->type != NodeType::VAR) return nullptr;

    Node* params = GetNewVar(tokens, err);
    SYNTAX_ASSERT_(params)

    while(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::COMMA) {

        INCR_PTR_

        Node* param = GetNewVar(tokens, err);
        SYNTAX_ASSERT_(param)

        params = MakeNode(NodeType::OPER, {.oper = Oper_t::COMMA}, {.line = 0}, params, param);

    }

    return params;

}

/*=====================================================================================*/

static Node* GetCallFunc (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::VAR)
    Node* name = GetOldFuncName(tokens, err);
    SYNTAX_ASSERT_(name);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::L_BRACKET)
    INCR_PTR_

    Node* args = GetFuncArgs(tokens, err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::R_BRACKET)
    INCR_PTR_

    Node* call_func = MakeNode(NodeType::OPER, {.oper = Oper_t::CALL_FUNC}, {.line = 0}, name, args);

    return call_func;

}

/*=====================================================================================*/

static Node* GetFuncArgs (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    if (CUR_TOKEN_->type != NodeType::VAR) return nullptr;

    Node* args = GetExpression(tokens, err);
    SYNTAX_ASSERT_(args)

    while(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::COMMA) {

        INCR_PTR_

        Node* arg = GetExpression(tokens, err);
        SYNTAX_ASSERT_(arg)

        args = MakeNode(NodeType::OPER, {.oper = Oper_t::COMMA}, {.line = 0}, args, arg);

    }

    return args;

}

/*=====================================================================================*/

static Node* GetBlock (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SKIP_BREAKS_

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::BLOCK_OPEN)
    INCR_PTR_

    Node* val = GetLine(tokens, err);
    RET_NULL_IF_ERR_

    Node* head = val;

    while(val) {

        Node* new_val = GetLine(tokens, err);
        RET_NULL_IF_ERR_

        ConnectNodes(val, nullptr, new_val);
        val = new_val;
        //printf("%p\n", val);

    }

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::BLOCK_END)
    INCR_PTR_

    return head;

}

/*=====================================================================================*/

static Node* GetLine (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    Node* val = nullptr;

    SKIP_BREAKS_

    if (CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::IF) {

        NameTable_t* local_table = NametableCtor();
        StackPush(&NAMES_STK_, local_table);

        val = GetIfSection(tokens, err);
        RET_NULL_IF_ERR_

        StackPop(&NAMES_STK_);

        Node* line_end = MakeNode(NodeType::OPER, {.oper = Oper_t::LINE_END});
        ConnectNodes(line_end, val, nullptr );
        return line_end;

    } else if (CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::WHILE) {

        NameTable_t* local_table = NametableCtor();
        StackPush(&NAMES_STK_, local_table);

        val = GetWhileSection(tokens, err);
        RET_NULL_IF_ERR_

        StackPop(&NAMES_STK_);

        Node* line_end = MakeNode(NodeType::OPER, {.oper = Oper_t::LINE_END});
        ConnectNodes(line_end, val, nullptr );
        return line_end;


    } else if (CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::RET) {

        val = GetReturn(tokens, err);

    } else if (CUR_TOKEN_->type == NodeType::VAR) {
        INCR_PTR_

        if (CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::INIT) {
            DECR_PTR_

            val = GetInit(tokens, err);

        } else if (CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::ASSIGN) {
            DECR_PTR_

            val = GetAssignment(tokens, err);

        } else {
            err->code = (int)FrontendErr::INVALID_SYNTAX;
            SYNTAX_ASSERT_(0)
        }

    } else 
        return nullptr;

    RET_NULL_IF_ERR_

    Node* line_end = GetLineEnd(tokens, err);
    RET_NULL_IF_ERR_

    ConnectNodes(line_end, val, nullptr );
    //printf("P1:%p\n", line_end);

    return line_end;

}

/*=====================================================================================*/

static Node* GetWhileSection (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SKIP_BREAKS_

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::WHILE)
    Node* while_node = MakeNode(NodeType::OPER, {.oper = Oper_t::WHILE});
    INCR_PTR_

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::L_BRACKET)
    INCR_PTR_

    Node* expr = GetExpression(tokens, err);
    RET_NULL_IF_ERR_

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::R_BRACKET)
    INCR_PTR_

    SKIP_BREAKS_

    Node* body = GetBlock(tokens, err);
    RET_NULL_IF_ERR_

    ConnectNodes(while_node, expr, body);

    return while_node;

}

/*=====================================================================================*/


static Node* GetIfSection (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SKIP_BREAKS_

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::IF)
    Node* if_node = MakeNode(NodeType::OPER, {.oper = Oper_t::WHILE});
    INCR_PTR_

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::L_BRACKET)
    INCR_PTR_

    Node* expr = GetExpression(tokens, err);
    RET_NULL_IF_ERR_

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::R_BRACKET)
    INCR_PTR_

    SKIP_BREAKS_

    Node* body = GetBlock(tokens, err);
    RET_NULL_IF_ERR_

    ConnectNodes(if_node, expr, body);

    return if_node;

}

/*=====================================================================================*/

static Node* GetInit (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SKIP_BREAKS_

    Node* var = GetNewVar(tokens, err);
    RET_NULL_IF_ERR_

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::INIT)
    Node* init = MakeNode(NodeType::OPER, {.oper = Oper_t::INIT});
    INCR_PTR_

    Node* expr = GetExpression(tokens, err);
    RET_NULL_IF_ERR_

    ConnectNodes(init, var, expr);

    return init;

}

/*=====================================================================================*/

static Node* GetAssignment (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SKIP_BREAKS_

    Node* var = GetOldVar(tokens, err);
    RET_NULL_IF_ERR_

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::ASSIGN)
    Node* assign = MakeNode(NodeType::OPER, {.oper = Oper_t::ASSIGN});
    INCR_PTR_

    Node* expr = GetExpression(tokens, err);
    RET_NULL_IF_ERR_

    ConnectNodes(assign, var, expr);

    return assign;

}

/*=====================================================================================*/

static Node* GetReturn (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::RET)
    Node* ret = MakeNode(NodeType::OPER, {.oper = Oper_t::RET});
    INCR_PTR_

    Node* expr = GetExpression(tokens, err);
    RET_NULL_IF_ERR_

    ConnectNodes(ret, expr, nullptr);

    return ret;

}

/*=====================================================================================*/

static Node* GetExpression (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    Node* val = GetOperAND(tokens, err);
    RET_NULL_IF_ERR_

    while (CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::OR) {

        Node* op = MakeNode(NodeType::OPER, {.oper = Oper_t::OR});
        INCR_PTR_

        Node* new_val = GetOperAND(tokens, err);
        RET_NULL_IF_ERR_

        ConnectNodes(op, val, new_val);
        val = op;

    }

    return val;

}

/*=====================================================================================*/

static Node* GetOperAND (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    Node* val = GetOperCompare(tokens, err);
    RET_NULL_IF_ERR_

    while (CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::AND) {

        Node* op = MakeNode(NodeType::OPER, {.oper = Oper_t::AND});
        INCR_PTR_

        Node* new_val = GetOperCompare(tokens, err);
        RET_NULL_IF_ERR_

        ConnectNodes(op, val, new_val);
        val = op;

    }

    return val;

}

/*=====================================================================================*/

static Node* GetOperCompare (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    Node* val = GetOperADD_SUB(tokens, err);
    RET_NULL_IF_ERR_

    while (CUR_TOKEN_->type == NodeType::OPER && 
           (CUR_TOKEN_->data.oper == Oper_t::GREATER || CUR_TOKEN_->data.oper == Oper_t::GREATER_EQ ||
            CUR_TOKEN_->data.oper == Oper_t::LESS || CUR_TOKEN_->data.oper == Oper_t::LESS_EQ ||
            CUR_TOKEN_->data.oper == Oper_t::EQ || CUR_TOKEN_->data.oper == Oper_t::NOT_EQ)
          ) {

        Node* op = MakeNode(NodeType::OPER, {.oper = CUR_TOKEN_->data.oper});
        INCR_PTR_

        Node* new_val = GetOperADD_SUB(tokens, err);
        RET_NULL_IF_ERR_

        ConnectNodes(op, val, new_val);
        val = op;

    }

    return val;

}

/*=====================================================================================*/

static Node* GetOperADD_SUB (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    Node* val = GetOperMUL_DIV(tokens, err);
    RET_NULL_IF_ERR_

    while (CUR_TOKEN_->type == NodeType::OPER && 
           (CUR_TOKEN_->data.oper == Oper_t::ADD || 
            CUR_TOKEN_->data.oper == Oper_t::SUB)
          ) {

        Node* op = MakeNode(NodeType::OPER, {.oper = CUR_TOKEN_->data.oper});
        INCR_PTR_

        Node* new_val = GetOperMUL_DIV(tokens, err);
        RET_NULL_IF_ERR_

        ConnectNodes(op, val, new_val);
        val = op;

    }

    return val;

}

/*=====================================================================================*/

static Node* GetOperMUL_DIV (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    Node* val = GetOperDEG(tokens, err);
    RET_NULL_IF_ERR_

    while (CUR_TOKEN_->type == NodeType::OPER && 
           (CUR_TOKEN_->data.oper == Oper_t::MUL || 
            CUR_TOKEN_->data.oper == Oper_t::DIV)
          ) {

        Node* op = MakeNode(NodeType::OPER, {.oper = CUR_TOKEN_->data.oper});
        INCR_PTR_

        Node* new_val = GetOperDEG(tokens, err);
        RET_NULL_IF_ERR_

        ConnectNodes(op, val, new_val);
        val = op;

    }

    return val;

}

/*=====================================================================================*/

static Node* GetOperDEG (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    Node* val = GetElementary(tokens, err);
    RET_NULL_IF_ERR_

    while (CUR_TOKEN_->type == NodeType::OPER && 
           CUR_TOKEN_->data.oper == Oper_t::DEG
          ) {

        Node* op = MakeNode(NodeType::OPER, {.oper = CUR_TOKEN_->data.oper});
        INCR_PTR_

        Node* new_val = GetElementary(tokens, err);
        RET_NULL_IF_ERR_

        ConnectNodes(op, val, new_val);
        val = op;

    }

    return val;

}

/*=====================================================================================*/

static Node* GetElementary (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    Node* val = nullptr;

    if (CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::L_BRACKET) {

        INCR_PTR_
        val = GetExpression(tokens, err);
        RET_NULL_IF_ERR_

        SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::R_BRACKET)
        INCR_PTR_

        return val;

    } else {

        SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::NUM || CUR_TOKEN_->type == NodeType::VAR)

        if (CUR_TOKEN_->type == NodeType::NUM) {

            val = GetNum(tokens, err);

        } else {

            if (NEXT_TOKEN_->type == NodeType::OPER && NEXT_TOKEN_->data.oper == Oper_t::L_BRACKET) {

                val = GetCallFunc(tokens, err);
                RET_NULL_IF_ERR_

                return val;

            } else 
                val = GetOldVar(tokens, err);

        }

        RET_NULL_IF_ERR_

    }

    return val;

}

/*=====================================================================================*/

static Node* GetNum (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::NUM)

    Node* num = MakeNode(NodeType::NUM, {.num = CUR_TOKEN_->data.num});
    //printf("lf1:%lf lf2:%lf\n", num->data.num, CUR_TOKEN_->data.num);

    INCR_PTR_

    return num;

}

/*=====================================================================================*/

static Node* GetName (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::VAR)

    Node* num = MakeNode(NodeType::VAR, {.var_idx = CUR_TOKEN_->data.var_idx});
    //printf("--%d\n", CUR_TOKEN_->data.var_idx);

    INCR_PTR_

    return num;

}

/*=====================================================================================*/

static Node* GetLineEnd (TokenArr* tokens, Err_t* err) {

    LOG_CHECK_MARK_

    assert(tokens);
    assert(err);

    SYNTAX_ASSERT_(CUR_TOKEN_->type == NodeType::OPER && CUR_TOKEN_->data.oper == Oper_t::LINE_END)

    Node* num = MakeNode(NodeType::OPER, {.oper = CUR_TOKEN_->data.oper});

    INCR_PTR_

    return num;

}



static void TryInsertNameInStack(const Stack_t* stk, const char* str,
                                 bool* exists, bool* is_func, TokenType type) {

    assert(stk);
    assert(str);
    assert(is_func);
    assert(exists);

    bool found = false;

    for (int i = 0; i < stk->size; i++) {

        found = FindNameInTable(stk->data[i], str, exists, is_func);
        if (found) break;
    }

    if (!found)
        InsertNameInTable(stk->data[stk->size - 1], str, type);

}