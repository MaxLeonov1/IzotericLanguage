#include <math.h>

#include "../tree/tree.h"
#include "../tree/tree_dump.h"
#include "../common/functions.h"
#include "middleend.h"



static int   TryEvalConstTree (Node* node, double* result);
static Node* SimplTrivleCases (Node* node, int* simpl);

/*=====================================================================================*/

Node* SimplConstTree(Node* node, int* simpl) {

    if (!node) return nullptr;
    
    if (node->left) node->left = SimplConstTree(node->left, simpl);
    if (node->right) node->right = SimplConstTree(node->right, simpl);
    
    double const_value = 0;
    if (TryEvalConstTree(node, &const_value)) {
        simpl++;
        DeleteNode(node);
        Node* new_node = CreateNumNode(const_value);
        return new_node;
    }

    return node;
}

/*=====================================================================================*/

#define EvL_ TryEvalConstTree(node->left, &left_val)
#define EvR_ TryEvalConstTree(node->right, &right_val)

static int TryEvalConstTree(Node* node, double* result) {
    if (!node) {
        *result = 0;
        return 0;
    }
    
    switch (node->type) {
        case NodeType::NUM:
            *result = node->data.num;
            return 1;
            
        case NodeType::VAR:
            return 0;

        case NodeType::OPER: {
            double left_val = 0, right_val = 0;
            
            if (EvL_ && EvR_) {
                switch (node->data.oper) {
                    case Oper_t::ADD:
                        *result = left_val + right_val;
                        return 1;
                    case Oper_t::SUB:
                        *result = left_val - right_val;
                        return 1;
                    case Oper_t::MUL:
                        *result = left_val * right_val;
                        return 1;
                    case Oper_t::DIV:
                        if (double_cmp(0, right_val)) {
                            *result = 0;
                            return 1;
                        }
                        *result = left_val / right_val;
                        return 1;
                    case DEG:
                        *result = pow(left_val, right_val);
                        return 1;
                    case SIN:
                        *result = sin(right_val);
                        return 1;
                    case COS:
                        *result = cos(right_val);
                        return 1;
                    default:
                        return 0;
                }
            }
            return 0;
        }    
        default:
            return 0;
    }
}

#undef EvL_
#undef EvR_

/*=====================================================================================*/

#define SIMPL_NODE_(new_node)     \
    *simpl++;                     \
    Node* res = new_node;         \
    new_node = nullptr;           \
    DeleteNode(node);             \
    return res;
    
#define _ADD_X_(num_, child) node->child  && node->child->type  == NodeType::NUM && double_cmp(num_, node->child->data.num)
#define _MUL_X_(num_, child) node->child && node->child->type  == NodeType::NUM && double_cmp(num_, node->child->data.num)
#define _DIV_X_(num_, child) node->child && node->child->type  == NodeType::NUM && double_cmp(num_, node->child->data.num)
#define _X_DEG_(num_) node->right && node->right->type == NodeType::NUM && double_cmp(num_, node->right->data.num)

static Node* SimplTrivleCases(Node* node, int* simpl){

    if(node->left) node->left  = SimplTrivleCases(node->left, simpl);
    if(node->right) node->right = SimplTrivleCases(node->right, simpl);

    if (node->type == NodeType::OPER) {

        switch(node->data.oper) {

            case Oper_t::SUB:
            case Oper_t::ADD:
                if (_ADD_X_(0, left)) {
                    SIMPL_NODE_(node->right)
                }
                if (_ADD_X_(0, right)) {
                    SIMPL_NODE_(node->left)
                }
                break;

            case Oper_t::MUL:
                if (_MUL_X_(1, left)) {
                    SIMPL_NODE_(node->right)
                }
                if (_MUL_X_(0, left)) {
                    SIMPL_NODE_(node->left)
                }
                if (_MUL_X_(1, right)) {
                    SIMPL_NODE_(node->left)
                }
                if (_MUL_X_(0, right)) {
                    SIMPL_NODE_(node->right)
                }
                break;

            case Oper_t::DIV:
                if (_DIV_X_(0, left)) {
                    SIMPL_NODE_(node->right)
                }
                if (_DIV_X_(1, right)) {
                    SIMPL_NODE_(node->left)
                }
                break;
            
            case Oper_t::DEG:
                if (_X_DEG_(0)) {
                    Node* new_node = node->right;
                    new_node->data.num = 1;
                    node->right = nullptr;
                    DeleteNode(node);
                    return new_node;
                } 
                if (_X_DEG_(1)) {
                    SIMPL_NODE_(node->left)
                }
                break;
        }
    }
    return node;
}

#undef SIMPL_NODE_
#undef _ADD_X_
#undef _MUL_X_
#undef _DIV_X_
#undef _X_DEG_

/*=====================================================================================*/

MiddleendErr SimplTree(Tree_t* tree) {

    int simpl = 0;
    do {
        simpl = 0;
        tree->root = SimplConstTree(tree->root, &simpl);
        tree->root = SimplTrivleCases(tree->root, &simpl);

    } while(simpl);

    return MiddleendErr::NONE;
}