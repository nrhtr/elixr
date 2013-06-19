#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "internal.h"
#include "elixr.h"

#include "types.h"
#include "opcodes.h"
#include "compile.h"

#define X(a, b) b,
struct op_info op_info[] = { OPCODES };
#undef X

/* Genesis has an additional pass to add values (strings anyway) to the object
 * and all methods share the same string_table. We do it all in one pass (at
 * least for now) */

void xr_asm_op(struct XRAsm *m, u8 ins, int _a, int _b)
{
    if (m->alloc <= m->len) {
        m->ops = realloc(m->ops, sizeof(XR_OP) * (m->len + 1));
        m->alloc++;
    }


    XR_OP *p = m->ops + m->len;

    p->code = ins;
    p->a    = _a;
    p->b    = _b;

    m->len++;
}

struct XRMethod *xr_method_new(XR name, XR args)
{
    struct XRMethod *meth = malloc(sizeof(struct XRMethod));

    meth->code.len = 0;
    meth->code.alloc = 0;
    meth->code.ops = NULL;

    meth->locals = list_empty();
    meth->values = list_empty();
    meth->args = args;
    meth->name = name;

    meth->mt = method_vt;

    return meth;
}

struct XRMethod *xr_method_compile_new(XR name, XR args, XR ast)
{
    (void) name;
    (void) args;
    (void) ast;

    assert(0 && "FIXME: xr_method_compile_new unimplemented");

    /*xrListEach(args, index, item, {*/
        
    /*});*/
}

int find_local(XR locals, XR var)
{
    int found = -1;
    xrListEach(locals, index, item, {
        if (xr_sym_eq(0, item, var) == VAL_TRUE) {
            found = index;
        }
    });

    return found;
}

void xr_ast_compile_method(XR ast, struct XRMethod *m)
{
    /* compile args/sig. */
    int i = 0; //xrListLen(m->locals);
    xrListEach(m->args, index, item, {
        xr_asm_op(&m->code, OP_LSTORE, i, 0);
        i++;
    });

    xr_ast_compile(ast, m);
}

/* TODO: factor out common code from simple ops (i.e. plus/minus/etc). */
/* TODO: proper error handling */
void xr_ast_compile(XR ast, struct XRMethod *m)
{
    assert(ast);

    /*printf("\n------\n");*/
    /*send(send(ast, s_string), s_print);*/
    /*printf("\n------\n");*/

    struct XRAst *n = (struct XRAst*) ast;

    switch (n->type) {
        case AST_CODE:
            {
                XR stmts = ((struct XRAst*)ast)->n[0];

                xrListEach(stmts, index, stmt, {
                    xr_ast_compile(stmt, m);
                });

            }
            break;
        case AST_VDECL:
        case AST_VINIT:
            {
                xrListEach(m->args, index, item, {
                    if (xr_sym_eq(0, n->n[0], item) == VAL_TRUE) {
                        fprintf(stderr, "Var already defined as parameter.\n");
                        return;
                    }
                });

                xrListEach(m->locals, index, item, {
                    if (xr_sym_eq(0, n->n[0], item) == VAL_TRUE) {
                        fprintf(stderr, "Var already defined as local.\n");
                        return;
                    }
                });

                list_append(0, m->locals, n->n[0]);
                if (n->type == AST_VINIT) {
                    xr_ast_compile(n->n[1], m);
                    xr_asm_op(&m->code, OP_LSTORE, xrListLen(m->args) + xrListLen(m->locals)-1, 0);
                }
            }
            break;
        case AST_SELF:
            {
                xr_asm_op(&m->code, OP_SELF, 0, 0);
            }
            break;
        case AST_SEND:
            {
                /* FIXME: arguments */
                int args = 0;
                if (n->n[2] && xrIsPtr(n->n[2]))
                    args = xrListLen(n->n[2]);
                
                if (args) {
                    xrListEachR(n->n[2], index, item, {
                        xr_ast_compile(item, m);
                    });
                }

                xr_ast_compile(n->n[1], m);
                xr_ast_compile(n->n[0], m);

                xr_asm_op(&m->code, OP_SEND, args, 0);
            }
            break;
        case AST_IF:
            {
                xr_ast_compile(n->n[0], m); /* emit conditional */
                int jmp = m->code.len; /* index to next opcode */
                xr_asm_op(&m->code, OP_NOTJMP, 0, 0); /* skip over true block if false */
                xr_ast_compile(n->n[1], m); /* emit true block */

                /* update emitted notjmp, telling it to jump to next opcode after true block. */
                m->code.ops[jmp].a = m->code.len - jmp - 1;
            }
            break;
        case AST_FOR_IN:
            {
                break;
            }
            break;
        case AST_IFELSE:
            {
                xr_ast_compile(n->n[0], m); /* emit conditional */

                int fjmp = m->code.len; /* index to next opcode */
                xr_asm_op(&m->code, OP_NOTJMP, 0, 0); /* skip over true block if false */

                xr_ast_compile(n->n[1], m); /* emit true block */

                int tjmp = m->code.len;
                xr_asm_op(&m->code, OP_JMP, 0, 0); /* now skip over false block */

                m->code.ops[fjmp].a = m->code.len - fjmp - 1; /*NOTJMP goes here*/
                xr_ast_compile(n->n[2], m); /* emit false block */
                m->code.ops[tjmp].a = m->code.len - tjmp - 1; /*JMP goes here*/
            }
            break;
        case AST_WHILE:
            {
                int loop = m->code.len;
                xr_ast_compile(n->n[0], m); /* conditional */
                int exit = m->code.len;
                xr_asm_op(&m->code, OP_NOTJMP, 0, 0); /* if false exit loop */

                xr_ast_compile(n->n[1], m);

                xr_asm_op(&m->code, OP_JMP, loop - m->code.len - 1, 0); /* hopefully jmp back to conditional */
                m->code.ops[exit].a = m->code.len - exit - 1; /* loop exit jumps here */
            }
            break;
        case AST_ASSIGN:
            {
                int var_index = -1;
                xrListEach(m->locals, index, item, {
                    if (xr_sym_eq(0, n->n[0], item) == VAL_TRUE) {
                        var_index = xrListLen(m->args) + index;
                        break;
                    }
                });

                if (var_index == -1) {
                xrListEach(m->args, index, item, {
                    if (xr_sym_eq(0, n->n[0], item) == VAL_TRUE) {
                        var_index = index;
                        break;
                    }
                });
                }

                xr_ast_compile(n->n[1], m);
                xr_asm_op(&m->code, OP_LSTORE, var_index, 0);
            }
            break;
        case AST_EXPRSTMT:
            {
                /* Just a single expression statement.
                 * Compile the expr node then pop the stack to discard
                 * the result. */
                xr_ast_compile(n->n[0], m);
                xr_asm_op(&m->code, OP_POP, 0, 0);
            }
            break;
        case AST_VALUE:
            {
                /* Store the value directly in the operand */
                xr_asm_op(&m->code, OP_DVAL, n->n[0], 0);
            }
            break;
        case AST_EQ: case AST_NEQ: case AST_GT: case AST_LT: case AST_GTE: case AST_LTE:
            xr_ast_compile(n->n[1], m);
            xr_ast_compile(n->n[0], m);

            switch (n->type) {
                case AST_EQ:
                    xr_asm_op(&m->code, OP_EQ, 0, 0);
                    break;
                case AST_NEQ:
                    xr_asm_op(&m->code, OP_NEQ, 0, 0);
                    break;
                case AST_GT:
                    xr_asm_op(&m->code, OP_GT, 0, 0);
                    break;
                case AST_LT:
                    xr_asm_op(&m->code, OP_LT, 0, 0);
                    break;
                case AST_GTE:
                    xr_asm_op(&m->code, OP_GTE, 0, 0);
                    break;
                case AST_LTE:
                    xr_asm_op(&m->code, OP_LTE, 0, 0);
                    break;
            }
            break;
        case AST_NOT:
            {
                xr_ast_compile(n->n[0], m);
                xr_asm_op(&m->code, OP_NOT, 0, 0);
            }
            break;
        case AST_AND:
            {
                xr_ast_compile(n->n[0], m);
                xr_ast_compile(n->n[1], m);
                xr_asm_op(&m->code, OP_AND, 0, 0);
            }
            break;
        case AST_OR:
            {
                xr_ast_compile(n->n[0], m);
                xr_ast_compile(n->n[1], m);
                xr_asm_op(&m->code, OP_OR, 0, 0);
            }
            break;
        case AST_NUMBER:
            {
                /* TODO: Generalise these into AST_VALUE which is currently used for
                 * in-opcode parameters. */
                int val_index = -1;

                /* Search for existing fixnum value */
                xrListEach(m->values, index, item, {
                    if (xrIsNum(item) && n->n[0] == item) {
                        val_index = index;
                        break;
                    }
                });

                if (val_index == -1) {
                    /* Values are stored on the persistent Method object */
                    list_append(0, m->values, n->n[0]);
                    val_index = xrListLen(m->values) - 1;
                }

                xr_asm_op(&m->code, OP_IVAL, val_index, 0);
            }
            break;
        case AST_STRING:
            {
                int val_index = -1;
                xrListEach(m->values, index, item, {
                    if (xrIsPtr(item) && val_vtable(item) == string_vt && strcmp(xrStrPtr(n->n[0]), xrStrPtr(item)) == 0) {
                        val_index = index;
                        break;
                    }
                });

                if (val_index == -1) {
                    list_append(0, m->values, n->n[0]);
                    val_index = xrListLen(m->values) - 1;
                }

                xr_asm_op(&m->code, OP_IVAL, val_index, 0);
            }
            break;
        case AST_SYMBOL:
            {
                int val_index = -1;
                xrListEach(m->values, index, item, {
                    if (xrIsPtr(item) && val_vtable(item) == symbol_vt && strcmp(xrSymPtr(n->n[0]), xrSymPtr(item)) == 0) {
                        val_index = index;
                        break;
                    }
                });

                if (val_index == -1) {
                    list_append(0, m->values, n->n[0]);
                    val_index = xrListLen(m->values) - 1;
                }

                xr_asm_op(&m->code, OP_IVAL, val_index, 0);
            }
            break;
        case AST_LIST:
            {
                XR expr_list = n->n[0];
                xrListEach(expr_list, index, item, {
                    xr_ast_compile(item, m);
                });

                int len = xrListLen(expr_list);

                xr_asm_op(&m->code, OP_BUILD_LIST, len, 0);
            }
            break;
        case AST_VAR:
            {
                int var = find_local(m->locals, n->n[0]);

                if (var == -1) {
                    var = find_local(m->args, n->n[0]);
                    if (var == -1) {
                        printf("No such variable as '%s'\n", xrStrPtr(n->n[0]));
                        exit(1);
                    } else {
                        xr_asm_op(&m->code, OP_LLOAD, var, 0);
                    }
                } else {
                    xr_asm_op(&m->code, OP_LLOAD, xrListLen(m->args) + var, 0);
                }
            }
            break;
        case AST_PLUS:
            {
                xr_ast_compile(n->n[1], m);
                xr_ast_compile(n->n[0], m);
                xr_asm_op(&m->code, OP_PLUS, 0, 0);
            }
            break;
        case AST_MINUS:
            {
                xr_ast_compile(n->n[1], m);
                xr_ast_compile(n->n[0], m);
                xr_asm_op(&m->code, OP_MINUS, 0, 0);
            }
            break;
        case AST_TIMES:
            {
                xr_ast_compile(n->n[1], m);
                xr_ast_compile(n->n[0], m);
                xr_asm_op(&m->code, OP_MULT, 0, 0);
            }
            break;
        case AST_DIVIDE:
            {
                xr_ast_compile(n->n[1], m);
                xr_ast_compile(n->n[0], m);
                xr_asm_op(&m->code, OP_DIV, 0, 0);
            }
            break;
        case AST_ASSERT:
            {
                xr_ast_compile(n->n[0], m);
                xr_asm_op(&m->code, OP_ASSERT, 0, 0);
            }
            break;
        case AST_RETURN:
            {
                xr_ast_compile(n->n[0], m);
                xr_asm_op(&m->code, OP_RETURN, 0, 0);
            }
            break;
        default:
            printf("No codegen path for this AST: %d\n", n->type);
    }
}
