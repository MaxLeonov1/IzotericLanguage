#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "tree.h"
#include "../common/functions.h"
#include "../common/errors.h"
#include "../common/input_output.h"
#include "../common/read_file.h"
#include "../common/logs.h"



#pragma GCC diagnostic ignored "-Wformat=2"

/*=====================================================================================*/

static Node* NodesPrefixRead              (TextStorage* text, Tree_t* tree, Err_t* error);
static void  DeleteClosingBracketFromWord (TextStorage* text, char* read);
static char  CheckOpeningBracketInInput   (TextStorage* text);
static void  GetNodeInfo                  (Tree_t* tree, char* read,
                                           NodeType* type, Val_t* val);
static Node* ReadNewNode                  (TextStorage* text, Tree_t* tree, Err_t* error);

/*=====================================================================================*/

void TreeCtor (Tree_t* tree) {

    assert(tree);

    tree->buffer = nullptr;
    tree->root = nullptr;
    tree->cpcty = 1;
    tree->nametable = NametableCtor();

}

/*=====================================================================================*/

TreeErr TreeDtor (Tree_t* tree) {

    assert(tree);

    if (!tree->root) return TreeErr::EMPTY_TREE_ACT_ERR;
    DeleteNode(tree->root);

    NametableDtor(tree->nametable);

    _RET_OK_

}

/*=====================================================================================*/

TreeErr AllocNode (Node** node) {

    assert(node);

    Node* node_ptr = (Node*)calloc(1, sizeof(node_ptr[0]));
    if (!node_ptr) return TreeErr::MEM_ALLOC_ERR;

    *node = node_ptr; 

    _RET_OK_

}

/*=====================================================================================*/

TreeErr DeleteNode(Node* node) {

    assert(node != nullptr);

    if (node->left != nullptr)
        DeleteNode (node->left);
    if (node->right != nullptr)
        DeleteNode (node->right);

    free(node);

    _RET_OK_

}

/*=====================================================================================*/

Node* MakeNode(NodeType type, Val_t val, NodeInfo info, Node* left, Node* right, Node* parent) {

    Node* node = (Node*)calloc(1, sizeof(node[0]));
    if ( !node ) return node;

    node->info = info;
    node->type = type;
    node->data = val;
    node->right = right;
    node->left = left;
    node->parent = parent;

    if(right)
        node->right->parent = node;

    if(left)
        node->left->parent = node;

    return node;

}

/*=====================================================================================*/

void ConnectNodes (Node* node, Node* left, Node* right) {

    assert(node);

    if (left != nullptr) {

        node->left   = left;
        left->parent = node;
    }

    if (right != nullptr) {
        
        node->right   = right;
        right->parent = node;
    }

}

/*=====================================================================================*/

Node* CreateNumNode (double val) {

    Node* node = (Node*)calloc(1, sizeof(Node));
    if (!node) return node;

    node->data.num = val;
    node->left = nullptr;
    node->right = nullptr;
    node->type = NodeType::NUM;

    return node;
}

/*=====================================================================================*/

void TreePrefixRead (TextStorage* text, Tree_t* tree, Err_t* error) {

    assert(text);
    assert(tree);
    assert(error);

    tree->root = NodesPrefixRead(text, tree, error);
}

/*=====================================================================================*/

static char CheckOpeningBracketInInput(TextStorage* text) {

    SkipSpaces(text);
    char opening_bracket_check = Buffgetc(text);
    SkipSpaces(text);

    return opening_bracket_check;
}

/*=====================================================================================*/

static void DeleteClosingBracketFromWord(TextStorage* text, char* read) {

    assert(read);

    size_t bracket_pos = strlen(NIL);
    if (read[bracket_pos] == ')') {

        read[bracket_pos] = '\0';
        text->ptr--;
    }
}

/*=====================================================================================*/

static void GetNodeInfo(Tree_t* tree, char* read, NodeType* type, Val_t* val) {

    int len = strlen(read);

    Oper_t oper = GetOperator(read);
    //printf("-%s %s\n", OperToStr(oper), read);
    if (oper != Oper_t::UNK) {

        *type = NodeType::OPER;
        *val  = {.oper = oper};
        return;
    }

    char* end = nullptr;
    double num = strtod(read, &end);
    if (!(num == 0 && end != read + len)) {

        *type = NodeType::NUM;
        *val = {.num = num};
        return;
    }

    if (read[0] == '\"' && read[len - 1] == '\"') {

        read[len - 1] = '\0';
        char* word = read + 1;

        int idx = InsertNameInTable(tree->nametable, word);

        *type = NodeType::VAR;
        *val  = {.var_idx = idx};
        return;
    }

    //printf("-%s\n", read);
}

/*=====================================================================================*/

static Node* ReadNewNode(TextStorage* text, Tree_t* tree, Err_t* error) {

    NodeType type = NodeType::POISON;
    Val_t    val = {};

    char read[MAX_STR_LEN_] = "";
    BufScanfWord(text, read);

    if (!strncmp(read, NIL, MAX_STR_LEN_))
        return nullptr;

    GetNodeInfo(tree, read, &type, &val);

    if (type == NodeType::POISON) {

        error->code = (int) TreeErr::INVALID_SYNTAX;
        PrintLog("UNKNOWN NODE TYPE<br>\n");
        return nullptr;
    }

    Node* node  = MakeNode(type, val);

    Node* left  = NodesPrefixRead(text, tree, error);
    Node* right = NodesPrefixRead(text, tree, error);

    ConnectNodes(node, left, right);

    SkipSpaces(text);

    return node;
}

/*=====================================================================================*/

static Node* NodesPrefixRead(TextStorage* text, Tree_t* tree, Err_t* error) {

    assert(text);
    assert(tree);
    assert(error);

    char opening_bracket_check = CheckOpeningBracketInInput(text);

    if (opening_bracket_check == '(') {

        Node* new_node = ReadNewNode(text, tree, error);
        SkipSpaces(text);

        char closing_bracket_check = Buffgetc(text);
        if (closing_bracket_check != ')') {

            error->code = (int) TreeErr::INVALID_SYNTAX;
            return nullptr;
        }

        return new_node;
    }
    else {

        text->ptr--;

        char read[MAX_STR_LEN_] = "";
        BufScanfWord(text, read);

        DeleteClosingBracketFromWord(text, read);

        if (strncmp(read, "nil", MAX_STR_LEN_))
            error->code = (int) TreeErr::INVALID_SYNTAX;
    }

    return nullptr;
}