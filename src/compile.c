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
    struct XRMethod *meth = xr_alloc(sizeof(struct XRMethod));

    meth->code.len = 0;
    meth->code.alloc = 0;
    meth->code.ops = NULL;

    meth->locals = list_empty();
    meth->values = list_empty();
    meth->args = args;
    meth->name = name;

    meth->mt[-1] = method_vt;

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

int find_var(XR locals, XR var)
{
    int found = -1;
    xrListEach(locals, index, item, {
        if (symbol_eq(0, item, var) == VAL_TRUE) {
            found = index;
        }
    });

    return found;
}

bool has_objvar(XR obj, XR var)
{
    XR vars = xrObjVars(obj);

    if (table_at(0, vars, var) != VAL_NIL)
        return 1;

    return 0;
}

void ast_compile_method(XR ast, struct XRMethod *m)
{
    /* compile args/sig. */
    int i = 0; //xrListLen(m->locals);
    xrListEach(m->args, index, item, {
        xr_asm_op(&m->code, OP_LSTORE, i, 0);
        i++;
    });

    ast_compile(ast, m);
}

/* TODO: factor out common code from simple ops (i.e. plus/minus/etc). */
/* TODO: proper error handling */
void ast_compile(XR ast, struct XRMethod *m)
{
    assert(ast);

    /*printf("\n------\n");*/
    /*send(send(ast, s_string), s_show);*/
    /*printf("\n------\n");*/

    struct XRAst *n = (struct XRAst*) ast;
    XR *oper = n->n;

    switch (n->type) {
        case AST_CODE:
            {
                XR stmts = ((struct XRAst*)ast)->n[0];

                xrListEach(stmts, index, stmt, {
                    ast_compile(stmt, m);
                });

            }
            break;
        case AST_VDECL:
        case AST_VINIT:
            {
                xrListEach(m->args, index, item, {
                    if (symbol_eq(0, oper[0], item) == VAL_TRUE) {
                        fprintf(stderr, "Var already defined as parameter.\n");
                        return;
                    }
                });

                xrListEach(m->locals, index, item, {
                    if (symbol_eq(0, oper[0], item) == VAL_TRUE) {
                        fprintf(stderr, "Var already defined as local.\n");
                        return;
                    }
                });

                list_append(0, m->locals, oper[0]);
                if (n->type == AST_VINIT) {
                    ast_compile(oper[1], m);
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
                if (oper[2] && xrIsPtr(oper[2]))
                    args = xrListLen(oper[2]);
                
                if (args) {
                    /* If we POP each arg for methods we need to emit PUSH in reverse,
                     * currently, however, we read the args off the stack and update the
                     * stack pointer so its easiest to just push them in order */
                    xrListEach(oper[2], index, item, {
                        ast_compile(item, m);
                    });
                }

                ast_compile(oper[1], m);
                ast_compile(oper[0], m);

                xr_asm_op(&m->code, OP_SEND, args, 0);
            }
            break;
        case AST_IF:
            {
                ast_compile(oper[0], m); /* emit conditional */
                int jmp = m->code.len; /* index to next opcode */
                xr_asm_op(&m->code, OP_NOTJMP, 0, 0); /* skip over true block if false */
                ast_compile(oper[1], m); /* emit true block */

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
                ast_compile(oper[0], m); /* emit conditional */

                int fjmp = m->code.len; /* index to next opcode */
                xr_asm_op(&m->code, OP_NOTJMP, 0, 0); /* skip over true block if false */

                ast_compile(oper[1], m); /* emit true block */

                int tjmp = m->code.len;
                xr_asm_op(&m->code, OP_JMP, 0, 0); /* now skip over false block */

                m->code.ops[fjmp].a = m->code.len - fjmp - 1; /*NOTJMP goes here*/
                ast_compile(oper[2], m); /* emit false block */
                m->code.ops[tjmp].a = m->code.len - tjmp - 1; /*JMP goes here*/
            }
            break;
        case AST_WHILE:
            {
                int loop = m->code.len;
                ast_compile(oper[0], m); /* conditional */
                int exit = m->code.len;
                xr_asm_op(&m->code, OP_NOTJMP, 0, 0); /* if false exit loop */

                ast_compile(oper[1], m);

                xr_asm_op(&m->code, OP_JMP, loop - m->code.len - 1, 0); /* hopefully jmp back to conditional */
                m->code.ops[exit].a = m->code.len - exit - 1; /* loop exit jumps here */
            }
            break;
        case AST_ASSIGN:
            {
                // Look for method parameters as locals first.
                int var_index = find_var(m->locals, oper[0]);
                if (var_index != -1) {
                    ast_compile(oper[1], m);
                    xr_asm_op(&m->code, OP_LSTORE, xrListLen(m->args) + var_index, 0);
                    break;
                }

                /* FIXME!!*/

                // Look in locals proper
                var_index = find_var(m->locals, oper[0]);
                if (var_index != -1) {
                    ast_compile(oper[1], m);
                    xr_asm_op(&m->code, OP_LSTORE, var_index, 0);
                    break;
                }


                fprintf(stderr, "No such var '%s' for assignment\n", xrSymPtr(oper[0]));
                exit(1);
            }
            break;
        case AST_OBJASSIGN:
            {
                bool found = has_objvar(m->object, oper[0]);
                if (!found) {
                    printf("No such objvar '%s' for assignment.\n", xrSymPtr(oper[0]));
                    exit(1);
                }

                int val_index = -1;
                xrListEach(m->values, index, item, {
                    if (xrIsPtr(item) && xrMTable(item) == symbol_vt
                     && strcmp(xrSymPtr(oper[0]), xrSymPtr(item)) == 0) {
                        val_index = index;
                        break;
                    }
                });

                if (val_index == -1) {
                    list_append(0, m->values, oper[0]);
                    val_index = xrListLen(m->values) - 1;
                }

                ast_compile(oper[1], m);
                xr_asm_op(&m->code, OP_IVAL, val_index, 0);
                xr_asm_op(&m->code, OP_SETOBJVAR, 0, 0);
            }
            break;
        case AST_EXPRSTMT:
            {
                /* Just a single expression statement.
                 * Compile the expr node then pop the stack to discard
                 * the result. */
                ast_compile(oper[0], m);
                xr_asm_op(&m->code, OP_POP, 0, 0);
            }
            break;
        case AST_VALUE:
            {
                /* Store the value directly in the operand */
                xr_asm_op(&m->code, OP_DVAL, oper[0], 0);
            }
            break;
        case AST_EQ: case AST_NEQ: case AST_GT: case AST_LT: case AST_GTE: case AST_LTE:
            ast_compile(oper[1], m);
            ast_compile(oper[0], m);

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
                ast_compile(oper[0], m);
                xr_asm_op(&m->code, OP_NOT, 0, 0);
            }
            break;
        case AST_AND:
            {
                ast_compile(oper[0], m);
                ast_compile(oper[1], m);
                xr_asm_op(&m->code, OP_AND, 0, 0);
            }
            break;
        case AST_OR:
            {
                ast_compile(oper[0], m);
                ast_compile(oper[1], m);
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
                    if (xrIsNum(item) && oper[0] == item) {
                        val_index = index;
                        break;
                    }
                });

                if (val_index == -1) {
                    /* Values are stored on the persistent Method object */
                    list_append(0, m->values, oper[0]);
                    val_index = xrListLen(m->values) - 1;
                }

                xr_asm_op(&m->code, OP_IVAL, val_index, 0);
            }
            break;
        case AST_STRING:
            {
                int val_index = -1;
                xrListEach(m->values, index, item, {
                    if (xrIsPtr(item) && xrMTable(item) == string_vt && strcmp(xrStrPtr(oper[0]), xrStrPtr(item)) == 0) {
                        val_index = index;
                        break;
                    }
                });

                if (val_index == -1) {
                    list_append(0, m->values, oper[0]);
                    val_index = xrListLen(m->values) - 1;
                }

                xr_asm_op(&m->code, OP_IVAL, val_index, 0);
            }
            break;
        case AST_SYMBOL:
            {
                int val_index = -1;
                xrListEach(m->values, index, item, {
                    if (xrIsPtr(item) && xrMTable(item) == symbol_vt && strcmp(xrSymPtr(oper[0]), xrSymPtr(item)) == 0) {
                        val_index = index;
                        break;
                    }
                });

                if (val_index == -1) {
                    list_append(0, m->values, oper[0]);
                    val_index = xrListLen(m->values) - 1;
                }

                xr_asm_op(&m->code, OP_IVAL, val_index, 0);
            }
            break;
        case AST_LIST:
            {
                XR expr_list = oper[0];
                xrListEach(expr_list, index, item, {
                    ast_compile(item, m);
                });

                int len = xrListLen(expr_list);

                xr_asm_op(&m->code, OP_BUILD_LIST, len, 0);
            }
            break;
        case AST_VAR:
            {
                int var = find_var(m->locals, oper[0]);
                if (var != -1) {
                    xr_asm_op(&m->code, OP_LLOAD, xrListLen(m->args) + var, 0);
                    break;
                }

                var = find_var(m->args, oper[0]);
                if (var != -1) {
                    xr_asm_op(&m->code, OP_LLOAD, var, 0);
                    break;
                }

                printf("No such var '%s'\n", xrStrPtr(oper[0]));
                exit(1);
            }
            break;
        case AST_OBJVAR:
            {
                bool found = has_objvar(m->object, oper[0]);
                if (!found) {
                    printf("No such objvar '%s'.\n", xrSymPtr(oper[0]));
                    exit(1);
                }

                /* Have to use an IVAL as we do a lookup
                 * using an full-blown symbol */

                /* FIXME: copy-pasted code to add value to use in IVAL */
                int val_index = -1;
                xrListEach(m->values, index, item, {
                    if (xrIsPtr(item) && xrMTable(item) == symbol_vt
                                      && strcmp(xrSymPtr(oper[0]), xrSymPtr(item)) == 0) {
                        val_index = index;
                        break;
                    }
                });

                if (val_index == -1) {
                    list_append(0, m->values, oper[0]);
                    val_index = xrListLen(m->values) - 1;
                }

                xr_asm_op(&m->code, OP_IVAL, val_index, 0);
                xr_asm_op(&m->code, OP_GETOBJVAR, 0, 0);
                break;
            }
        case AST_PLUS:
            {
                ast_compile(oper[1], m);
                ast_compile(oper[0], m);
                xr_asm_op(&m->code, OP_PLUS, 0, 0);
            }
            break;
        case AST_MINUS:
            {
                ast_compile(oper[1], m);
                ast_compile(oper[0], m);
                xr_asm_op(&m->code, OP_MINUS, 0, 0);
            }
            break;
        case AST_TIMES:
            {
                ast_compile(oper[1], m);
                ast_compile(oper[0], m);
                xr_asm_op(&m->code, OP_MULT, 0, 0);
            }
            break;
        case AST_DIVIDE:
            {
                ast_compile(oper[1], m);
                ast_compile(oper[0], m);
                xr_asm_op(&m->code, OP_DIV, 0, 0);
            }
            break;
        case AST_ASSERT:
            {
                ast_compile(oper[0], m);
                xr_asm_op(&m->code, OP_ASSERT, 0, 0);
            }
            break;
        case AST_RETURN:
            {
                ast_compile(oper[0], m);
                xr_asm_op(&m->code, OP_RETURN, 0, 0);
            }
            break;
        default:
            printf("No codegen path for this AST: %d\n", n->type);
    }
}
