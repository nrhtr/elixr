#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "elixr.h"
#include "opcodes.h"

#include "types/list.h"

#include "vm.h"

XR stack[256];
size_t pos;

#define PUSH(V)   stack[pos++] = V
#define POP()     stack[--pos]
#define VM_MATH(A, OP, MSG, B) ((xrIsNum(A) && xrIsNum(B))\
        ? xrNum(xrInt(A) OP xrInt(B))\
        : (xrIsNum(A)\
            ? number_##MSG(0, A, B)\
            : xr_send(A, s_##MSG, B)))

/* Temp function to quickly run a method,
 * should work for now just to get some proper
 * debugging going. Fix up and optimise later. */
XR xr_run_method(struct XRMethod *m)
{
    size_t i;
    log("####### Running VM ##############\n");
	
	XR locals = list_new_len(xrListLen(m->locals) + xrListLen(m->args));

    for (i = 0; i < m->code.len; i++) {
        XR_OP op = m->code.ops[i];
        
        switch (op.code) {
            case OP_IVAL:
                {
                    XR val = xrListAt(m->values, op.a);
                    PUSH(val);
                    break;
                }
                break;
            case OP_DVAL:
                {
                    PUSH(op.a);
                    break;
                }
                break;
            case OP_BUILD_LIST:
                {
                    int len = op.a;
                    XR list = list_new_len(len);
                    while (len--) {
                        XR val = POP();
                        xrListAt(list, len) = val;
                    }

                    PUSH(list);
                }
                break;
            case OP_EQ: case OP_NEQ: case OP_GT: case OP_LT: case OP_GTE: case OP_LTE:
                {
                    XR a = POP();
                    XR b = POP();
                    XR test;
                    switch (op.code) {
                        case OP_EQ:
                            test = xrMkBool(a == b);
                            break;
                        case OP_NEQ:
                            test = xrMkBool(a != b);
                            break;
                        case OP_GT:
                            test = xrMkBool(a > b);
                            break;
                        case OP_LT:
                            test = xrMkBool(a < b);
                            break;
                        case OP_GTE:
                            test = xrMkBool(a >= b);
                            break;
                        case OP_LTE:
                            test = xrMkBool(a <= b);
                            break;
                    }
                    PUSH(test);
                    break;
                }
            case OP_NOT:
                {
                    XR a = POP();

                    XR test = xrMkBool(!xrTest(a));
                    PUSH(test);
                }
                break;
            case OP_AND:
                {
                    XR a = POP();
                    XR b = POP();

                    XR test = xrMkBool(xrTest(a) && xrTest(b));
                    PUSH(test);
                }
                break;
            case OP_OR:
                {
                    XR a = POP();
                    XR b = POP();

                    XR test = xrMkBool(xrTest(a) || xrTest(b));
                    PUSH(test);
                }
                break;
            case OP_LSTORE:
                {
                    XR val = POP();
                    xrListAt(locals, op.a) = val;
                }
                break;
            case OP_LLOAD:
                {
                    XR val = xrListAt(locals, op.a);
                    PUSH(val);
                }
                break;
            case OP_GETOBJVAR:
                {
                    /* TODO: refactor to nice APIs */
                    XR name = POP();
                    XR val = xr_table_at(0, ((struct XRObject*)m->object)->vars, name);
                    PUSH(val);
                }
                break;
            case OP_SETOBJVAR:
                {
                    XR name = POP();

                    if (xr_table_at(0, xrObjectVars(m->object), name) == VAL_NIL) {
                        log("Unable to find %s on object...\n", xrSymPtr(name));
                        exit(1);
                    }

                    XR val = POP();
                    xr_table_put(0, xrObjectVars(m->object), name, val);
                }
            case OP_NOTJMP:
                {
                    XR val = POP();

                    if (!xrTest(val))
                    {
                        i += op.a; /* Make sure we run instr op.a next iteration. */
                    }
                }
                break;
            case OP_JMP:
                {
                    i += op.a;
                }
                break;
            case OP_POP:
                {
                    (void) POP();
                }
                break;
            case OP_SEND:
                {
                    XR rcv = POP();
                    XR msg = POP();

                    int num_args = op.a;
                    XR ret = VAL_NIL;

                    /* FIXME: seperate opcode for bind and remove later send's? */
                    XR cl = bind(rcv, msg);

                    struct XRClosure *c = cl;
                    if (c->native == 0) {
                        /* TODO: reentry? */
                        XR val = xr_run_method(c->data[0]);
                        PUSH(val);
                        break;
                    }

                    /* FIXME: make this neater, implement @rest / varargs? */
                    switch (num_args) {
                        case 0:
                            ret = send(rcv, msg);
                            break;
                        case 1:
                            {
                                XR a = POP();
                                ret = send(rcv, msg, a);
                            }
                            break;
                        case 2:
                            {
                                XR a = POP();
                                XR b = POP();
                                ret = send(rcv, msg, a, b);
                            }
                            break;
                        case 3:
                            {
                                XR a = POP();
                                XR b = POP();
                                XR c = POP();
                                ret = send(rcv, msg, a, b, c);
                            }
                            break;
                        default:
                            fprintf(stderr, "vm: OP_SEND: FIXME\n");
                            break;
                    }
                            
                    PUSH(ret);
                }
                break;
            case OP_SELF:
                {
                    XR self = m->object;
                    PUSH(self);
                }
                break;
            case OP_PLUS:
                {
                    XR a = POP();
                    XR b = POP();

                    XR c = VM_MATH(a, +, add, b);
                    PUSH(c);
                }
                break;
            case OP_MINUS:
                {
                    XR a = POP();
                    XR b = POP();

                    XR c = VM_MATH(a, -, sub, b);
                    PUSH(c);
                }
                break;
            case OP_MULT:
                {
                    XR a = POP();
                    XR b = POP();

                    XR c = VM_MATH(a, *, mul, b);
                    PUSH(c);
                }
                break;
            case OP_DIV:
                {
                    XR a = POP();
                    XR b = POP();

                    XR c;
                    if (b == VAL_ZERO) /* FIXME: will bypass obj / 0 */
                        c = VAL_NIL;
                    else {
                        c = VM_MATH(a, /, div, b);
                    }
                    PUSH(c);
                }
                break;
            case OP_ASSERT:
                {
                    XR a = POP();

                    if (!xrTest(a)) {
                        fprintf(stderr, "Assertion failed. Aborting.");
                        exit(1);
                    }
                }
                break;
            case OP_RETURN:
                {
                    XR a = POP();

                    /* TODO: reentry */
                    return a;
                }
                break;
            default:
                printf("Unimplemented opcode");
                break;
        }
    }

    return VAL_NIL;
}
