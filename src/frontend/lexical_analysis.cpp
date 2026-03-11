#include <ctype.h>
#include <assert.h>
#include <stdio.h>

#include "lexical_analysis.h"
#include "../common/colors.h"



enum class CharType {

    DIGIT,
    ALPHA,
    OPER,
    OTHER,
    UNK,

};


static CharType GetCharType   (const int ch);
static CharType CheckCharType (TextStorage* text);

static void   ReadWord   (TextStorage* text, char* buffer);
static void   HandleWord (TextStorage* text, TokenArr* tokens);
static void   HandleNum  (TextStorage* text, TokenArr* tokens);
static double ReadNum    (TextStorage* text);

static void SkipSymbols  (TextStorage* text);
static bool SkipComments (TextStorage* text);



void TokenArrCtor(TokenArr* tokens) {

    assert(tokens);

    tokens->nodes = (Node**)calloc(DEF_TOKENS_NUM_, sizeof(Node*));
    assert(tokens->nodes);

    tokens->nametable = NametableCtor();
    FillNameTableWithKeyWords(tokens->nametable);

    StackCtor(&(tokens->nametable_stk), 5);
    StackPush(&(tokens->nametable_stk), NametableCtor());
    FillNameTableWithKeyWords(tokens->nametable_stk.data[0]);
    //DumpNametable(stderr, tokens->nametable_stk.data[0]);

    tokens->size = 0;
    tokens->ptr = 0;

}



void TokenArrDtor(TokenArr* tokens) {

    assert(tokens);

    for (size_t idx = 0; idx <= tokens->size; idx++) {

        if (tokens->nodes[idx] != nullptr)
            free(tokens->nodes[idx]);

    }

    if (tokens->nametable)
        NametableDtor(tokens->nametable);

    if (&(tokens->nametable_stk)) {
        for (size_t i = 0; i < tokens->nametable_stk.size; i++) {
            if (tokens->nametable_stk.data[i]) {
                NametableDtor(tokens->nametable_stk.data[i]);
            }
        }
        StackDtor(&(tokens->nametable_stk));
    }

}



static CharType CheckCharType(TextStorage* text) {

    assert(text);

    int ch = Buffgetc(text);
    text->ptr--;

    return GetCharType(ch);

}



static CharType GetCharType(const int ch) {

    if (isalpha(ch))
        return CharType::ALPHA;

    if (isdigit(ch))
        return CharType::DIGIT;

    switch (ch) {

        // case '(':
        // case ')':
        case '\'':
        case '.':
        case '!':
        case '?':
            return CharType::OPER;
        case '_':
            return CharType::OTHER;
        default:
            return CharType::UNK;
    }
}



FrontendErr LexAnalysis (TextStorage* text, TokenArr* tokens, Err_t* err) {

    assert(text);
    assert(tokens);
    assert(err);

    const int ch = *(text->buffer);
    CharType type = GetCharType(ch);
    
    while(text->ptr < text->text_len) {

        //printf("BEFORE: ptr=%lu, len=%lu, line=%lu\n", text->ptr, text->text_len, text->cur_line);
        SkipSymbols(text);
        //printf("AFTER:  ptr=%lu, len=%lu, line=%lu\n", text->ptr, text->text_len, text->cur_line);

        type = CheckCharType(text);

        if (type == CharType::DIGIT) {

            HandleNum(text, tokens);

        } else if (type != CharType::UNK) {
            
            HandleWord(text, tokens);

        } else {

            SET_FRONTEND_ERR_DATA_(FrontendErr::INVALID_SYNTAX, "UNKNOWN SYMBOL IN LINE %lu", text->cur_line);

        }

    }

    NodeInfo info = {.line = text->cur_line};
    Val_t val = {.oper = Oper_t::PROGRAM_END};
    Node* end_token = MakeNode(NodeType::OPER, val, info);
    tokens->nodes[tokens->size] = end_token;
    tokens->size++;

    return (FrontendErr)(err->code);

}



static void HandleWord (TextStorage* text, TokenArr* tokens) {

    assert(text);
    assert(tokens);

    Node* token = nullptr;

    char buffer[MAX_STR_LEN_] = "";
    ReadWord(text, buffer);

    int id = InsertNameInTable(tokens->nametable, buffer);

    if (tokens->nametable->buff[id].type == TokenType::TOKEN) {

        NodeInfo info = {.line = text->cur_line};
        Oper_t op = KeyWordToType(buffer);
        Val_t val = {.oper = op};
        token = MakeNode(NodeType::OPER, val, info);

    } else {

        // DumpNametable(stderr, tokens->nametable);
        // DumpNametable(stderr, tokens->nametable_stk.data[0]);

        NodeInfo info = {.line = text->cur_line};
        Val_t val = {.var_idx = id};
        token = MakeNode(NodeType::VAR, val, info);

    }

    tokens->nodes[tokens->size] = token;
    tokens->size++;

}



static void HandleNum (TextStorage* text, TokenArr* tokens) {

    assert(text);
    assert(text);

    Node* token = nullptr;
    double buffer = ReadNum(text);

    NodeInfo info = {.line = text->cur_line};
    Val_t val = {.num = buffer};
    token = MakeNode(NodeType::NUM, val, info,
                     nullptr, nullptr, nullptr);

    tokens->nodes[tokens->size] = token;
    tokens->size++;

}



static double ReadNum(TextStorage* text) {

    assert(text);
    
    double result   = 0;
    double fraction = 0;
    int ch          = 0;
    
    while (isspace(ch = Buffgetc(text)));
    
    while (isdigit(ch)) {
        result = result * 10 + (ch - '0');
        ch = Buffgetc(text);
    }
    
    if (ch == '.') {
        ch = Buffgetc(text);
        double divisor = 10;
        
        while (isdigit(ch)) {
            fraction += (ch - '0') / divisor;
            divisor *= 10;
            ch = Buffgetc(text);
        }
    }

    if (ch != EOF) text->ptr--;
    
    result += fraction;
    
    return result;
}


static void ReadWord (TextStorage* text, char* buffer) {

    assert(text);
    assert(buffer);

    int idx = 0;
    int ch = Buffgetc(text);

    if (GetCharType(ch) != CharType::DIGIT) {

        do {

            buffer[idx] = (char)ch;
            idx++;
            ch = Buffgetc(text);

        } while(GetCharType(ch) != CharType::UNK);

    }

    buffer[idx] = '\0';
    text->ptr--;

}



static void SkipSymbols (TextStorage* text) {

    assert(text);

    bool need_to_skip = true;

    while (need_to_skip)
        need_to_skip = SkipSpaces(text) || SkipComments(text);
}



static bool SkipComments(TextStorage* text) {

    int ch_1 = Buffgetc(text);
    int ch_2 = Buffgetc(text);
    bool skipped = false;

    if (ch_1 == '/' && ch_2 == '*') {

        ch_1 = Buffgetc(text);
        ch_2 = Buffgetc(text);

        while (ch_1 != '*' && ch_2 != '/') {

            ch_1 = ch_2;
            ch_2 = Buffgetc(text);
        }
        skipped = true;

    } else {

        text->ptr -= 2;
    }

    return skipped;
}



void DumpTokensStorage(FILE* fp, TokenArr* tokens) {

    assert(tokens);

    for (size_t i = 0; i < tokens->size; i++) {

        fprintf(fp, "[%lu]===========\n", i);
        DumpToken(fp, tokens->nodes[i], tokens->nametable);
        fprintf(fp, "==============\n");
    }

    DumpNametable(fp, tokens->nametable);
}



void DumpToken(FILE* fp, Node* token, NameTable_t* nametable) {

    assert(token);

    fprintf(fp, "---------------\n");

    switch (token->type) {

        case NodeType::NUM:
            fprintf(fp, "TYPE > NUMBER\n"
                        "VAL  > %lg\n"
                        "LINE > %lu\n"
                        "---------------\n", token->data.num, token->info.line);
            return;
        case NodeType::OPER:
            fprintf(fp, "TYPE > TOKEN ");
            PrintOper(fp, token->data.oper);
            fprintf(fp, "\nLINE > %lu\n"
                        "---------------\n", token->info.line);
            return;
        case NodeType::VAR:
            fprintf(fp, "TYPE > NAME\n"
                        "ID   > %d\n"
                        "LINE > %lu\n"
                        "INFO > %s\n"
                        "---------------\n",
                        token->data.var_idx,
                        token->info.line, 
                        nametable->buff[token->data.var_idx]);
            return;
        default:
            fprintf(fp, "POISONED TOKEN\n"
                        "---------------\n");
            return;
    }
}