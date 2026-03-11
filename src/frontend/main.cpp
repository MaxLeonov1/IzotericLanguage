#include "lexical_analysis.h"
#include "../common/input_output.h"
#include "../common/read_file.h"
#include "../common/name_table.h"
#include "../tree/tree_dump.h"
#include "../common/logs.h"
#include "syntax_analysis.h"




int main(const int argc, const char* argv[]) {

    Err_t err = {};
    Tree_t tree = {};

    OpenLogFile(argc, argv);

    const char* data_file = GetFileName(argc, argv, 1, "INPUT");

    TextStorage text = {};
    TextStorageCtor(&text, &err, data_file);

    TokenArr tokens = {};
    TokenArrCtor(&tokens);

    LexAnalysis(&text, &tokens, &err);

    EXIT_IF_FRONTEND_ERR_
    //DumpTokensStorage(stdout, &tokens);

    TreeCtor(&tree);
    CopyNameTable(tokens.nametable, tree.nametable);

    GetTreeFromTokens(&tokens, &tree, &err);
    EXIT_IF_FRONTEND_ERR_

    FILE* result = fopen(TREE_FILE, "w");
    PrintPrefixTree(result, &tree);
    fclose(result);

    TREE_DUMP_(&tree);
    TokenArrDtor(&tokens);
    TextStorageDtor(&text);
    TreeDtor(&tree);

    CloseLogFile();
    
    printf("ok\n");
    return 0;
}