#include <stdlib.h>
#include <assert.h>

#include "backend.h"



static void AssembleNum  (Node* node, FILE* dest);
static void AssembleVar  (Node* node, FILE* dest);
static void AssembleOper (Node* node, Err_t* err, FILE* dest);

static void AssembleIF    (Node* node, Err_t* err, FILE* dest);
static void AssembleWhile (Node* node, Err_t* err, FILE* dest);
static void AssembleInit  (Node* node, Err_t* err, FILE* dest);



void TreeToAsm (Tree_t* tree, Err_t* err, FILE* dest) {

    assert(tree);
    assert(err);
    assert(dest);

    fprintf(dest, "CALL :main\n"
                  "HLT\n");

}



static void AssembleNode (Node* node, Err_t* err, FILE* dest) {

    assert(node);
    assert(err);
    assert(dest);

    switch(node->type) {

        case NodeType::OPER:

            break;

        case NodeType::NUM:
            AssembleNum(node, dest);
            break;

        case NodeType::VAR:
            AssembleVar(node, dest);
            break;

        case NodeType::POISON:
            SetErrMes(err, "INCORRECT TREE NODE");
            break;

        default:
            SetErrMes(err, "UNKNOWN NODE TYPE");

    }

}



static void AssembleNum (Node* node, FILE* dest) {

    assert(node);
    assert(dest);

    fprintf(
        dest,
        "PUSH %g\n", 
        node->data.num);

}



static void AssembleVar (Node* node, FILE* dest) {

    assert(node);
    assert(dest);

    fprintf(
        dest,
        "PUSH %d\n"
        "POPR RAX\n"
        "PUSHM [RAX]\n",
        node->data.var_idx);

}



static void AssembleOper (Node* node, Err_t* err, FILE* dest) {

    assert(node);
    assert(err);
    assert(dest);

    switch(node->data.oper) {

        case Oper_t::ADD:
            fprintf (dest, "ADD\n\n");
            break;

        case Oper_t::SUB:
            fprintf (dest, "SUB\n\n");
            break;

        case Oper_t::MUL:
            fprintf (dest, "MUL\n\n");
            break;

        case Oper_t::DIV:
            fprintf (dest, "DIV\n\n");
            break;

        case Oper_t::IF:
            AssembleIF(node, err, dest);
            break;

        case Oper_t::WHILE:
            AssembleIF(node, err, dest);
            break;

        case Oper_t::ASSIGN:
        case Oper_t::INIT:
            AssembleInit(node, err, dest);
            break;

    }

}



static void AssembleIF (Node* node, Err_t* err, FILE* dest) {

    assert(node);
    assert(err);
    assert(dest);

    static size_t if_counter = 0;

    AssembleNode(node->left, err, dest);

    fprintf(
        dest,
        "PUSH 0\n"
        "JNE :endif_%lu\n\n",
        if_counter);

    AssembleNode(node->right, err, dest);

    fprintf(
        dest,
        ":endif_%lu\n\n",
        if_counter);

    if_counter++;

}



static void AssembleWhile (Node* node, Err_t* err, FILE* dest) {

    assert(node);
    assert(err);
    assert(dest);

    static size_t while_counter = 0;

    fprintf(
        dest,
        ":beginwhile_%lu",
        while_counter);

    AssembleNode(node->left, err, dest);

    fprintf(
        dest,
        "PUSH 0\n"
        "JNE :endwhile_%lu\n\n",
        while_counter);

    AssembleNode(node->right, err, dest);

    fprintf(
        dest,
        "JMP :beginwhile_%lu"
        ":endif_%lu\n\n",
        while_counter,
        while_counter);

    while_counter++;

}



static void AssembleInit (Node* node, Err_t* err, FILE* dest) {

    assert(node);
    assert(err);
    assert(dest);

    AssembleNode(node->right, err, dest);

    if (node->left->type != NodeType::VAR) {
        SetErrMes(err, "INCORRECT INITIAL(ASSIGNMENT) EXPRESSION");
        return;
    }

    fprintf(
        dest,
        "PUSH %d\n"
        "POPR RAX\n"
        "PUSHM [RAX]\n",
        node->left->data.var_idx);

}