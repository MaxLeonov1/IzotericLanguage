#include <stdlib.h>
#include <stdio.h>

#include "middleend.h"
#include "../common/logs.h"
#include "../common/errors.h"
#include "../tree/tree_dump.h"
#include "../common/input_output.h"
#include "../common/read_file.h"
#include "../tree/tree.h"



int main(int argc, char const *argv[]) {

    Err_t err = {};
    Tree_t tree = {};

    OpenLogFile(argc, argv);

    TextStorage text = {};
    TextStorageCtor(&text, &err, TREE_FILE);

    TreeCtor(&tree);

    TreePrefixRead(&text, &tree, &err);

    SimplTree(&tree);
    TREE_DUMP_(&tree);

    FILE* result = fopen(TREE_FILE, "w");
    PrintPrefixTree(result, &tree);
    fclose(result);

    TreeDtor(&tree);
    TextStorageDtor(&text);
    CloseLogFile();

    return 0;
}

