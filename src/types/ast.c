#include <stdio.h>
#include <stdlib.h>

#include "internal.h"

#include "elixr.h"
#include "xrstring.h"
#include "symbol.h"
#include "ast.h"
#include "table.h"
#include "closure.h"

char *ast_node_names[] = {
    "times",
    "divide"
    "plus",
    "minus",
    "number",
    "float",
    "string",
    "symbol",
    "list",
    "value",
    "code",
    "assign",
    "var",
    "vdecl",
    "vinit,"
    "send,"
    "if,"
    "ifelse,"
    "eq,"
    "neq,"
    "gt,"
    "lt,"
    "gte,"
    "lte,"
    "not,"
    "self,"
    "while,"
    "exprstmt,"
    "for_in,"
    "and,"
    "or,"
    "assert,"
    "return,"
    "objvar,"
    "objassign,"
};

#define AST_NAME(NODE) xr_str(ast_node_names[((struct XRAst*)NODE)->type])

XR ast_name(XR cl, XR self)
{
    (void) cl;

    return AST_NAME(self);
}

XR ast_string(XR cl, XR self)
{
    (void) cl;

    XR str = xr_str_alloc(64);
    struct XRAst * s = ((struct XRAst *)self);
    str = xr_str_append(0, str, AST_NAME(self));

    int i;
    if (s->n[0])
        str = xr_str_append_chars(0, str, " (");
    for (i = 0; i < 3; i++) {
        XR n = s->n[i];
        if (!n) continue;

        if (xrIsNum(n)) {
            str = xr_str_append(0, str, xr_str_sprintf("%d", xrInt(n)));
        /*} else if (xrMTable(n) == list_vt) {*/
            /*xrListEach(n, index, item, {*/
                /*str = xr_str_append(0, str, send(item, s_string));*/
                /*if (index != xrListLen(n) - 1)*/
                    /*str = xr_str_append_chars(0, str, ", ");*/
            /*});*/
        } else {
            str = xr_str_append(0, str, send(n, s_string));
        }
        if (i < 3 && s->n[i+1] != 0)
            str = xr_str_append_chars(0, str, " ");
    }
    if (s->n[0])
        str = xr_str_append_chars(0, str, ")");


    return str;
}

void ast_graph_string_each(XR self)
{

    struct XRAst * s = ((struct XRAst *)self);
    send(AST_NAME(self), s_show);

    int i;
    for (i = 0; i < 3; i++) {
        XR n = s->n[i];
        if (n) {
            if (xrIsNum(n)) {
                printf("%ld", xrInt(n));
            } else {
                send(send(n, s_string), s_show);
            }
        }
        printf(" ");
    }
}

void ast_graph_nodex(XR self)
{
    struct XRAst * s = ((struct XRAst *)self);
    switch (s->type) {
    case AST_TIMES:
        printf("node%p -> node%p;\nnode%p -> node%p;\n", s, (void *) s->n[0], s, (void *) s->n[1]);
        ast_graph_nodex(s->n[0]); ast_graph_nodex(s->n[1]); 
        break;
    case AST_DIVIDE:
        printf("node%p -> node%p;\nnode%p -> node%p;\n", s, (void *) s->n[0], s, (void *) s->n[1]);
        ast_graph_nodex(s->n[0]); ast_graph_nodex(s->n[1]); 
        break;
    case AST_PLUS:
        printf("node%p -> node%p;\nnode%p -> node%p;\n", s, (void *) s->n[0], s, (void *) s->n[1]);
        ast_graph_nodex(s->n[0]); ast_graph_nodex(s->n[1]); 
        break;
    case AST_MINUS:
        printf("node%p -> node%p;\nnode%p -> node%p;\n", s, (void *) s->n[0], s, (void *) s->n[1]);
        ast_graph_nodex(s->n[0]); ast_graph_nodex(s->n[1]); 
        break;
    case AST_NUMBER:
        break;
    case AST_STRING:
        break;
    case AST_CODE:
        xrListEach(s->n[0], i, v, {
            printf("node%p -> node%p;\n", s, (void*) v);
            ast_graph_nodex(v);
        });
        break;
    default:
        break;
    };

    XR label = AST_NAME(self);
    printf("node%p [label=\"", s); send(label, s_show); printf("\"];\n");
}

void ast_graph_head(struct XRAst *tree)
{
    printf("digraph AST {\n");

    ast_graph_nodex((XR) tree);

    printf("}\n");
}

void ast_traverse(struct XRAst *tree, void (*func)(XR))
{
    struct XRAst *t = tree;

    func((XR)t);

    int i = 0;
    while (t->n[i] && xrIsPtr(t->n[i])) {
        ast_traverse((struct XRAst *)t->n[i], func);
        i++;
    }
}

XR ast_node(enum AST_TYPE t, XR a, XR b, XR c)
{
    /* alloc mem for Z object + 3 object refs */
    struct XRAst *node = malloc(sizeof(struct XRAst) + 3 * sizeof(XR));

    node->mt = ast_vt;

    node->type = t;
    node->n[0] = a;

    /* TODO: only set if needed */
    node->n[1] = b;
    node->n[2] = c;

    return (XR)node;
}

XR ast_literal(XR cl, XR self)
{
    (void) cl;

    return AST_NAME(self);
}

void print_spaces(int spaces)
{
    while (spaces--)
        printf("-");
}

XR ast_source(XR cl, XR self, XR indent)
{
    (void) cl;

    assert(self);

    struct XRAst *ast = (struct XRAst*) self;

    int _indent = xrInt(indent);

    switch (ast->type) {
        case AST_TIMES:
            ast_source(0, ast->n[0], indent);
            printf(" * ");
            ast_source(0, ast->n[1], indent);
            break;
        case AST_DIVIDE:
            ast_source(0, ast->n[0], indent);
            printf(" / ");
            ast_source(0, ast->n[1], indent);
            break;
        case AST_PLUS:
            ast_source(0, ast->n[0], indent);
            printf(" + ");
            ast_source(0, ast->n[1], indent);
            break;
        case AST_MINUS:
            ast_source(0, ast->n[0], indent);
            printf(" - ");
            ast_source(0, ast->n[1], indent);
            break;
        case AST_NUMBER:
        case AST_FLOAT:
            {
                /* need to make sure this keep precision :s seems fragile */
                XR lit = send(ast->n[0], s_literal);
                printf("%s", xrStrPtr(lit));
            }
            break;
        case AST_STRING:
            {
                XR literal = send(ast->n[0], s_literal);
                printf("%s", xrStrPtr(literal));
            }
            break;
        case AST_SYMBOL:
            /* FIXME: see note in AST_SEND */
            printf(":%s", xrSymPtr(ast->n[0]));
            break;
        case AST_LIST:
            printf("[");
            xrListEach(ast->n[0], index, item, {
                ast_source(0, item, indent);
                if (index != xrListLen(ast->n[0]) - 1) {
                    printf(", ");
                }
            });
            printf("]");
            break;
        case AST_VALUE:
            /* We don't store the actual names in the AST */
            if (ast->n[0] == VAL_TRUE)
                printf("true");
            else if (ast->n[1] == VAL_FALSE)
                printf("false");
            else if (ast->n[1] == VAL_NIL)
                printf("nil");
            break;

        case AST_CODE:
            printf("{\n");
            xrListEach(ast->n[0], index, item, {
                ast_source(0, item, xrNum(_indent + 4));
            });
            print_spaces(_indent);
            printf("}\n");
            break;
        case AST_ASSIGN:
            printf("%s = ", xrSymPtr(ast->n[0]));
            ast_source(0, ast->n[1], indent);
            printf("\n");
            break;
        case AST_VAR:
            printf("%s", xrSymPtr(ast->n[0]));
            break;
        case AST_VDECL:
            printf("var %s\n", xrSymPtr(ast->n[0]));
            break;
        case AST_VINIT:
            printf("var %s = ", xrSymPtr(ast->n[0]));
            ast_source(0, ast->n[1], indent);
            printf("\n");
            break;

        case AST_IF:
            printf("if (");
            ast_source(0, ast->n[0], indent);
            printf(")\n");
            if (((struct XRAst *)ast->n[1])->type != AST_CODE)
                print_spaces(_indent);
            ast_source(0, ast->n[1], indent);
            break;
        case AST_EQ:
            ast_source(0, ast->n[0], indent);
            printf(" == ");
            ast_source(0, ast->n[1], indent);
            break;
        case AST_SELF:
            printf("self");
            break;
        case AST_EXPRSTMT:
            ast_source(0, ast->n[0], indent);
            printf("\n");
            break;
        case AST_SEND:
            ast_source(0, ast->n[0], indent);
            printf(" ");

            //ast_source(0, ast->n[1], indent);
            //we dont want the ':'
            printf("%s", xrSymPtr(((struct XRAst *)ast->n[1])->n[0]));

            /* if args */
            if (ast->n[2]) {
                printf("(");
                xrListEach(ast->n[2], index, item, {
                    ast_source(0, item, indent);
                    if (index != xrListLen(ast->n[2]) - 1) {
                        printf(", ");
                    }
                });
                printf(")");
            }
            break;
        case AST_OBJVAR: 
            printf("/%s", xrSymPtr(ast->n[0]));
            break;
        case AST_OBJASSIGN:
            printf("/%s", xrSymPtr(ast->n[0]));
            printf(" = ");
            ast_source(0, ast->n[1], indent);
            printf("\n");
            break;

        default:
            printf("[%d]\n", ast->type);
            break;
    }

    return VAL_NIL;
}

void xr_ast_methods()
{
    def_method(ast_vt, s_literal, ast_literal);
    def_method(ast_vt, s_string, ast_string);
#define m(NAME) qdef_method(string_vt, #NAME, ast_##NAME)
    m(name);
    m(source);
#undef m
}
