#include <stdio.h>
#include <stdlib.h>

#include "internal.h"

#include "elixr.h"
#include "xrstring.h"
#include "symbol.h"
#include "ast.h"
#include "table.h"
#include "closure.h"

/*
const char *ast_node_name(XR n)
{
    struct XRAst *node = (struct XRAst *) n;

    char *buf = malloc(64);
    int len;

    switch (node->type) {
    case AST_TIMES:
        sprintf(buf, "*");
        break;
    case AST_DIVIDE:
        sprintf(buf, "/");
        break;
    case AST_PLUS:
        sprintf(buf, "+");
        break;
    case AST_MINUS:
        sprintf(buf, "-");
        break;
    case AST_NUMBER:
        sprintf(buf, "%d", xrInt(node->n[0]));
        break;
    case AST_STRING:
        len = xrStrLen(node->n[0]);
        snprintf(buf, xrStrLen(node->n[0])+1, "%s", xrStrPtr(node->n[0]));
        buf[len] = '\0';
        break;
    default:
        sprintf(buf, "{%d}", node->type);
    }

    return buf;
}
*/

XR xr_ast_name(XR cl, XR self)
{
    (void) cl;

    struct XRAst *node = (struct XRAst *) self; 

    switch (node->type) {
    case AST_TIMES:
        return xr_str("times");
    case AST_DIVIDE:
        return xr_str("divide");
    case AST_PLUS:
        return xr_str("plus");
    case AST_MINUS:
        return xr_str("minus");
    case AST_NUMBER:
        return xr_str("number");
    case AST_FLOAT:
        return xr_str("decimal");
    case AST_STRING:
        return xr_str("string");
    case AST_LIST:
        return xr_str("list");
    case AST_VALUE:
        return xr_str("dval");
    case AST_CODE:
        return xr_str("code");
    case AST_ASSIGN:
        return xr_str("asssign");
    case AST_VAR:
        return xr_str("var");
    case AST_VDECL:
        return xr_str("vdecl");
    case AST_PRINT:
        return xr_str("print");
    case AST_VINIT:
        return xr_str("vinit");
    case AST_SEND:
        return xr_str("send");
    case AST_SYMBOL:
        return xr_str("symbol");
    case AST_IF:
        return xr_str("if");
    case AST_IFELSE:
        return xr_str("ifelse");
    case AST_EQ:
        return xr_str("eq");
    case AST_NEQ:
        return xr_str("neq");
    case AST_GT:
        return xr_str("gq");
    case AST_LT:
        return xr_str("lt");
    case AST_GTE:
        return xr_str("gte");
    case AST_LTE:
        return xr_str("lte");
    case AST_NOT:
        return xr_str("not");
    case AST_SELF:
        return xr_str("self");
    case AST_WHILE:
        return xr_str("while");
    case AST_DEBUG:
        return xr_str("debug");
    case AST_EXPRSTMT:
        return xr_str("expr");
    case AST_FOR_IN:
        return xr_str("for_in");
    default:
        printf("%d", node->type);
        return xr_str("???");
    }
}

XR xr_ast_string(XR cl, XR self)
{
    (void) cl;

    XR str = xr_str_alloc(64);
    struct XRAst * s = ((struct XRAst *)self);
    str = xr_str_append(0, str, xr_ast_name(0, self));

    int i;
    if (s->n[0])
        str = xr_str_append_chars(0, str, " (");
    for (i = 0; i < 3; i++) {
        XR n = s->n[i];
        if (!n) continue;

        if (xrIsNum(n)) {
            str = xr_str_append(0, str, xr_str_sprintf("%d", xrInt(n)));
        /*} else if (val_vtable(n) == list_vt) {*/
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
    send(xr_ast_name(0, self), s_print);

    int i;
    for (i = 0; i < 3; i++) {
        XR n = s->n[i];
        if (n) {
            if (xrIsNum(n)) {
                printf("%ld", xrInt(n));
            } else {
                send(send(n, s_string), s_print);
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

    XR label = xr_ast_name(0, self);
    printf("node%p [label=\"", s); send(label, s_print); printf("\"];\n");
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

    return xr_ast_name(0, self);
}

/*
XR ast_emit(XR node)
{
    struct XRAst *n = node;

    switch(n->type) {
        case AST_PLUS:
            printf("");
            break;
    }
}

*/

void print_spaces(int spaces)
{
    while (spaces--)
        printf(" ");
}

XR xr_ast_source(XR cl, XR self, XR indent)
{
    (void) cl;

    assert(self);

    struct XRAst *ast = (struct XRAst*) self;

    int _indent = xrInt(indent);

    switch (ast->type) {
        case AST_VAR: case AST_VALUE: case AST_STRING:
        case AST_NUMBER: case AST_EQ: case AST_SEND:
        case AST_LIST: case AST_MINUS: case AST_PLUS:
        case AST_TIMES: case AST_DIVIDE:
            break;
        default:
            print_spaces(_indent);
    }


    switch (ast->type) {
        case AST_IF:
            printf("if (");
            xr_ast_source(0, ast->n[0], indent);
            printf(")\n");
            if (((struct XRAst *)ast->n[1])->type != AST_CODE)
                print_spaces(_indent);
            xr_ast_source(0, ast->n[1], indent);
            break;
        case AST_EQ:
            xr_ast_source(0, ast->n[0], indent);
            printf(" == ");
            xr_ast_source(0, ast->n[1], indent);
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
        case AST_SELF:
            printf("self");
            break;
        case AST_VAR:
            printf("%s", xrSymPtr(ast->n[0]));
            break;
        case AST_STRING:
            {
                XR literal = send(ast->n[0], s_literal);
                printf("%s", xrStrPtr(literal));
            }
            break;
        case AST_EXPRSTMT:
            xr_ast_source(0, ast->n[0], indent);
            printf("\n");
            break;
        case AST_CODE:
            printf("{\n");
            xrListEach(ast->n[0], index, item, {
                xr_ast_source(0, item, xrNum(_indent + 4));
            });
            print_spaces(_indent);
            printf("}\n");
            break;
        case AST_VDECL:
            printf("var %s\n", xrSymPtr(ast->n[0]));
            break;
        case AST_VINIT:
            printf("var %s = ", xrSymPtr(ast->n[0]));
            xr_ast_source(0, ast->n[1], indent);
            printf("\n");
            break;
        case AST_ASSIGN:
            /* FIXME: BLAH BLAH BLAH SYMBOLS MSGS AND SHIT */
            printf("%s = ", xrSymPtr(ast->n[0]));
            xr_ast_source(0, ast->n[1], indent);
            printf("\n");
            break;
        case AST_SEND:
            xr_ast_source(0, ast->n[0], indent);
            printf(" ");
            /*xr_ast_source(0, ast->n[1], indent);*/

            /* FIXME: sort out literals vs messages, and toliteral() type stuff */
            printf("%s", xrSymPtr(((struct XRAst *)ast->n[1])->n[0]));
            /* if args */
            if (ast->n[2]) {
                printf("(");
                xrListEach(ast->n[2], index, item, {
                    xr_ast_source(0, item, indent);
                    if (index != xrListLen(ast->n[2]) - 1) {
                        printf(", ");
                    }
                });
                printf(")");
            }
            break;
        case AST_LIST:
            printf("[");
            xrListEach(ast->n[0], index, item, {
                xr_ast_source(0, item, indent);
                if (index != xrListLen(ast->n[0]) - 1) {
                    printf(", ");
                }
            });
            printf("]");
            break;
        case AST_NUMBER:
            {
                /* need to make sure this keep precision :S seems fragile */
                XR lit = send(ast->n[0], s_literal);
                printf("%s", xrStrPtr(lit));
            }
            break;
        case AST_SYMBOL:
            /* FIXME: see note in AST_SEND */
            printf(":%s", xrSymPtr(ast->n[0]));
            break;
        case AST_TIMES:
            xr_ast_source(0, ast->n[0], indent);
            printf(" * ");
            xr_ast_source(0, ast->n[1], indent);
            break;
        case AST_DIVIDE:
            xr_ast_source(0, ast->n[0], indent);
            printf(" / ");
            xr_ast_source(0, ast->n[1], indent);
            break;
        case AST_PLUS:
            xr_ast_source(0, ast->n[0], indent);
            printf(" + ");
            xr_ast_source(0, ast->n[1], indent);
            break;
        case AST_MINUS:
            xr_ast_source(0, ast->n[0], indent);
            printf(" - ");
            xr_ast_source(0, ast->n[1], indent);
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
    def_method(ast_vt, s_string, xr_ast_string);

    qdef_method(ast_vt, "name", xr_ast_name);
    qdef_method(ast_vt, "source", xr_ast_source);
}
