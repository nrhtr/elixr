#ifndef AST_H
#define AST_H

enum AST_TYPE {
    AST_TIMES,
    AST_DIVIDE,
    AST_PLUS,
    AST_MINUS,
    AST_NUMBER,
    AST_FLOAT,
    AST_STRING,
    AST_LIST,
    AST_VALUE, /* TODO: Should be able to replace NUMBER, STRING,
                * FLOAT, etc. Currently is just for OP_DVAL */
    AST_CODE,
    AST_ASSIGN,
    AST_VAR,
    AST_VDECL,
    AST_VINIT,
    AST_SEND,
    AST_SYMBOL,
    AST_IF,
    AST_IFELSE,
    AST_EQ,
    AST_NEQ,
    AST_GT,
    AST_LT,
    AST_GTE,
    AST_LTE,
    AST_NOT,
    AST_SELF,
    AST_WHILE,
    AST_EXPRSTMT,
    AST_FOR_IN,
    AST_AND,
    AST_OR,
    AST_ASSERT,
    AST_RETURN,
    AST_OBJVAR,
    AST_OBJASSIGN,
};

XR ast_node(enum AST_TYPE t, XR a, XR b, XR c);

void ast_graph_head(struct XRAst *tree);
void ast_graph_node(struct XRAst *node);

void xr_ast_methods(void);

void ast_print_digraph(XR tree);
void ast_traverse(struct XRAst *tree, void (*func)(XR));

/* Proper messages */
XR ast_name(XR cl, XR self);

#endif
