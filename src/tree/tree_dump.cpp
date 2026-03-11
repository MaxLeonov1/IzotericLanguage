#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include "../common/logs.h"
#include "tree_dump.h"
#include "../stack/stack.h"

#pragma GCC diagnostic ignored "-Wformat=2"
#pragma GCC diagnostic ignored "-Wformat-overflow"
#pragma GCC diagnostic ignored "-Wformat-truncation"

/*=====================================================================================*/

#define NUM_COL_ "fillcolor=\"#4CAF50\", color=\"#2E7D32\""
#define VAR_COL_ "fillcolor=\"#FF9800\", color=\"#EF6C00\""
#define OP_COL_  "fillcolor=\"#2196F3\", color=\"#1565C0\""

static size_t      IMG_CNT        = 1;
static const char* IMG_FOLDER_DIR = "src/assets/img/";
static const char* TMP_DOT_FILE   = "src/assets/graph.txt"; 

/*=====================================================================================*/

static void TreeGraphCtor   (const Tree_t* tree);
static void StartDotGraph   (FILE* dot_file);
static int  PrintGraphNodes (Node* node, int rank, FILE* graph_text, const Tree_t* tree);
static void EndDotGraph     (FILE* dot_file);
static void MakeImgFromDot  (const char* dot_file);

static void NodesPrefixPrint  (FILE* fp, const Tree_t* tree, const Node* node);
static void PrintSyncNodeData (FILE* fp, const Tree_t* tree, const Node* node);

/*=====================================================================================*/

int TreeDump(FILE* stream, const void* nodes, const char* func, const char* file, const int line) {

    assert(nodes);
    START_LOG_DUMP_(file, line, func);

    const Tree_t* tree = (const Tree_t*)nodes;
    TreeGraphCtor(tree);
    //END_LOG_DUMP_;

    return (int)TreeErr::NONE;
}

/*=====================================================================================*/

static void TreeGraphCtor(const Tree_t* tree) {

    assert(tree);

    FILE* dot_file = fopen(TMP_DOT_FILE, "w");
    if (dot_file == nullptr) {

        PrintLog("CAN NOT DRAW TREE GRAPH<br>\n");
        return;
    }

    StartDotGraph(dot_file);
    //printf("abober:%p\n", tree->root);
    PrintGraphNodes(tree->root, 1, dot_file, tree);
    EndDotGraph(dot_file);

    fclose(dot_file);

    MakeImgFromDot(TMP_DOT_FILE);

}

/*=====================================================================================*/

static void StartDotGraph (FILE* dot_file) {

    fprintf ( dot_file, 
              "digraph structs {\n"
              "   rankdir = HR;\n"
              "   bgcolor = \"white\""
              /*"   splines = \"ortho\";\n"*/
              "   node [fontname=\"Helvetica-BoldOblique\", fontsize=\"11\","
              " style=\"filled,bold\", fillcolor = \"#fff0b3\", color = \"#ffe680\"];\n"
              );
    
}

/*=====================================================================================*/

static void EndDotGraph (FILE* dot_file) {

    fprintf ( dot_file, "}");

}

/*=====================================================================================*/

// static void PrintTreeDumpHeader (const Tree_t* tree, FILE* log_file) {

//     assert(tree);
//     assert(log_file);

//     fprintf( 
//         log_file,
//         "<h3>[TREE INFO]:</h3>\n"
//         "location: %s::%d, %s()\n"
//         "<h3>[TREE DATA]:</h3>\n"
//         "capacity: %lu\n"
//         "root[%lu]\n",
//         tree->info.file,
//         tree->info.line,
//         tree->info.func,
//         tree->cpcty,
//         tree->root
//     );

    
// }

/*=====================================================================================*/

// void CreateGraphImg (const Tree_t* tree, const char* graphname, const char* graph_dir ) {

//     assert(tree);
//     assert(graphname);
//     assert(graph_dir);

//     char graph_txt_path[MAX_STR_LEN_] = {0};
//     char graph_svg_path[MAX_STR_LEN_] = {0};
//     snprintf ( graph_svg_path, sizeof(graph_svg_path), "%s/%s", graph_dir, graphname );
//     snprintf ( graph_txt_path, sizeof(graph_txt_path), "%s/graph.txt", graph_dir );

//     FILE* graph_text = fopen ( graph_txt_path, "w" );

//     fprintf ( graph_text, 
//               "digraph structs {\n"
//               "   rankdir = HR;\n"
//               "   bgcolor = \"white\""
//               /*"   splines = \"ortho\";\n"*/
//               "   node [fontname=\"Helvetica-BoldOblique\", fontsize=\"11\","
//               " style=\"filled,bold\", fillcolor = \"#fff0b3\", color = \"#ffe680\"];\n"
//               );
    
//     PrintGraphNodes(tree->root, 1, graph_text, tree);

//     fprintf ( graph_text, "}" );
//     fclose ( graph_text );

//     char cmd_line[MAX_STR_LEN_] = {0};
//     snprintf ( cmd_line, sizeof(cmd_line), "dot -Tsvg %s -o %s", graph_txt_path, graph_svg_path );
//     system ( cmd_line );

// }

/*=====================================================================================*/

static int PrintGraphNodes(Node* node, int rank, FILE* graph_text, const Tree_t* tree) {

    assert(node);
    assert(graph_text);

    static int idx = 0;
    int idx_left = 0;
    int idx_right = 0;

    char data[MAX_STR_LEN_] = "";
    const char* type  = "";
    const char* color = "";

    if(rank == 1) idx = 0;

    if (node->left)  idx_left  = PrintGraphNodes(node->left,  rank+1, graph_text, tree);
    if (node->right) idx_right = PrintGraphNodes(node->right, rank+1, graph_text, tree);

    switch(node->type){
        case NodeType::NUM:
            type = "NUM";
            color = NUM_COL_;
            sprintf(
                data, 
                "[%lf]", 
                node->data.num
            );
            break;

        case NodeType::OPER:
            type = "OPER";
            color = OP_COL_;
            sprintf(
                data, 
                "%s",
                OperToStr(node->data.oper)
            );
            break;

        case NodeType::VAR:
            type = "VAR";
            color = VAR_COL_;
            sprintf(
                data,
                "[ %d '%s')]",
                node->data.var_idx,
                tree->nametable->buff[node->data.var_idx].name
            );
            break;
    }

    fprintf(
        graph_text, 
        "node_%d[shape=Mrecord, rank=%d, %s"
        "label=\" { %p | type: %s | data: %s | { Left: %p | Right: %p } } \",];\n",
        idx,
        rank,
        color,
        node,
        type,
        data,
        node->left,
        node->right
    );

    if (node->left) {
        fprintf(
            graph_text,
            "node_%d -> node_%d["EDGE_STD_SET_"];\n",
            idx, idx_left
        );
            
    }

    if (node->right) {
        fprintf(
            graph_text,
            "node_%d -> node_%d["EDGE_STD_SET_"];\n",
            idx, idx_right
        );
        
    }

    return idx++;

}

/*=====================================================================================*/

static void MakeImgFromDot(const char* dot_file) {

    char img_name[MAX_STR_LEN_] = "";
    snprintf(img_name, MAX_STR_LEN_, "%simg%lu_%lu.svg", IMG_FOLDER_DIR, IMG_CNT++, clock());

    char dot_command[MAX_STR_LEN_] = "";
    snprintf(dot_command, MAX_STR_LEN_, "dot %s -T svg -o %s", dot_file, img_name);
    system(dot_command);

    PrintLog("<img src=\"%s\"><br>", img_name);
}

/*=====================================================================================*/

void PrintPrefixTree(FILE* fp, const Tree_t* tree) {

    assert(tree);

    fprintf(fp, "(");
    NodesPrefixPrint(fp, tree, tree->root);
    fprintf(fp, " )\n");
}

/*=====================================================================================*/

static void NodesPrefixPrint(FILE* fp, const Tree_t* tree, const Node* node) {

    if (!node) { fprintf(fp, " "); fprintf(fp, "nil"); fprintf(fp, " "); return; }

    PrintSyncNodeData(fp, tree, node);

    if (node->left != nullptr) fprintf(fp, " (");
    NodesPrefixPrint(fp, tree, node->left);
    if (node->left != nullptr) fprintf(fp, " )");

    if (node->right != nullptr) fprintf(fp, " (");
    NodesPrefixPrint(fp, tree, node->right);
    if (node->right != nullptr) fprintf(fp, " )");
}

/*=====================================================================================*/

static void PrintSyncNodeData(FILE* fp, const Tree_t* tree, const Node* node) {

    if (!node)
        return;

    switch(node->type)
    {
        case (NodeType::NUM):
            fprintf(fp, " %g", node->data.num);
            break;
        case (NodeType::VAR):
            fprintf(fp, " \"%s\"", tree->nametable->buff[node->data.var_idx].name);
            break;
        case (NodeType::OPER):
            fprintf(fp, " ");
            PrintOper(fp, node->data.oper);
            break;
        default:
            fprintf(fp, " undef ");
    }
}
