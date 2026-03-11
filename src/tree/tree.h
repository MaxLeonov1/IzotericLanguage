#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "../stack/stack.h"
#include "../common/errors.h"
#include "../common/read_file.h"

#define _DEBUG

/*=====================================================================================*/

typedef struct TreeNode Node;
typedef struct Tree Tree_t;
typedef struct TreeDebugInfo TreeDebugInfo_t;

/*=====================================================================================*/

enum class TreeErr{

    NONE = 0,
    EMPTY_TREE_ACT_ERR,
    MEM_ALLOC_ERR,
    FILE_OPEN_ERR,
    READ_SYNTAX_ERR,
    READ_DATA_ERR,
    INVALID_SYNTAX,

};

enum class NodeType {

    OPER,
    VAR,
    NUM,

    POISON

};

typedef union {

    double num;
    int    var_idx;
    Oper_t oper;

} Val_t;

struct NodeInfo {

    size_t line;

};

struct TreeNode {

    NodeInfo info;

    NodeType type;
    Val_t    data;

    Node* parent;
    Node* left;
    Node* right;

};

struct TreeDebugInfo {

    const char* func;
    const char* file;
    int line;

};

struct Tree {

    #ifdef _DEBUG
    TreeDebugInfo_t info;
    #endif

    Node* root;
    size_t cpcty; 
    NameTable_t* nametable;
    char* buffer;

};

/*=====================================================================================*/

void        TreeCtor      (Tree_t* tree);
TreeErr     TreeDtor      (Tree_t* tree);
TreeErr     AllocNode     (Node** node);
TreeErr     DeleteNode    (Node* node);
Node*       MakeNode      (NodeType type, Val_t val, NodeInfo info = {.line = 0}, 
                           Node* left = nullptr, Node* right = nullptr, Node* parent = nullptr);
void        ConnectNodes  (Node* node, Node* left, Node* right);
Node*       CreateNumNode (double val);

void TreePrefixRead (TextStorage* text, Tree_t* tree, Err_t* error);

/*=====================================================================================*/

#define _left_ 0
#define _right_ 1
#define _nil_len_ 4
#define _buff_byte_padding_ 5

#define FILE_MODE_ 0755

#define EDGE_STD_SET_ "penwidth = 2.0, arrowsize = 0.5, constraint = \"true\""
#define DEF_DISK_NAME_ "treedisk.bin"

#define OLD_NODE_COL_ "fillcolor = \"#fff0b3\", color = \"#ffe680\""
#define NEW_NODE_COL_ "fillcolor = \"#ccccff\", color = \"#b3b3ff\""

static const char* TREE_FILE = "src/assets/tmp.txt";
static const char* NIL       = "nil";

/*=====================================================================================*/

#ifndef _DEBUG
    #define TREE_INIT(name) Tree_t name = { nullptr, 0 };
    #define TREE_INIT_LIST(name) { nullptr, 0 }
#else
    #define TREE_INIT(name) Tree_t name = { {#name, __FUNCTION__, __FILE__, __LINE__}, nullptr, 0};
    #define TREE_INIT_LIST(name) { {#name, __FUNCTION__, __FILE__, __LINE__}, nullptr, 0}
#endif

#define _OK_STAT_ TreeErr status = TreeErr::NONE;
#define _RET_OK_ return TreeErr::NONE;
#define _DUMP_IF_ERR_(tree, status, mes) if(status = TreeErr::NONE) TreeDump(&tree, status, mes);

#define TREE_STAT_CHECK_DUMP_(tree_p, status)   \
    if ( status != TreeErr::NONE ) {  \
        TreeDump(tree_p, status);          \
        return status;                     \
    }

#define STAT_CHECK_                              \
    if ( status != TreeErr::NONE ) return status; \

#define SKIP_SPACE_ while (buffer[*pos] == ' ') (*pos)++;

/*=====================================================================================*/