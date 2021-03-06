#include <stdio.h>
#include <stdlib.h>

#include "internal.h"
#include "elixr.h"
#include "opcodes.h"

#include "types/list.h"
#include "types/number.h"
#include "types/table.h"

#include "vm.h"

XR stack[256];
size_t pos;

#define PUSH(V)   stack[pos++] = V
#define POP()     stack[--pos]
#define VM_MATH(A, OP, MSG, B) ((xrIsNum(A) && xrIsNum(B))\
        ? xrNum(xrInt(A) OP xrInt(B))\
        : (xrIsNum(A)\
            ? number_##MSG(0, A, B)\
            : send(A, s_##MSG, B)))

/* Temp function to quickly run a method,
 * should work for now just to get some proper
 * debugging going. Fix up and optimise later. */
XR xr_run_method(XR method)
{
    size_t i;
    struct XRMethod *m = (struct XRMethod *) method;

    XR locals = list_new_len(xrListLen(m->locals) + xrListLen(m->args));

    XRScope new;
    new.locals = locals;
    m->running_scope = new;

    for (i = 0; i < m->code.len; i++) {
        XR_OP op = m->code.ops[i];
        assert(pos >= 0 && pos < 256);
        
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
                    XR val = table_at(0, ((struct XRObject*)m->object)->vars, name);
                    PUSH(val);
                }
                break;
            case OP_SETOBJVAR:
                {
                    XR name = POP();

                    if (table_at(0, xrObjVars(m->object), name) == VAL_NIL) {
                        log("Unable to find %s on object...\n", xrSymPtr(name));
                        exit(1);
                    }

                    XR val = POP();
                    table_put(0, xrObjVars(m->object), name, val);
                }
                break;
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

                    struct XRClosure *c = (struct XRClosure *) cl;
                    if (c->native == 0) {
                        /* TODO: reentry? */
                        XR val = xr_run_method(c->data[0]);
                        PUSH(val);
                        break;
                    }

                    /* FIXME: make this neater, implement @rest / varargs? */
                    XR *argv = &stack[pos - num_args];
                    pos -= num_args;

                    ret = call_method_args(cl, rcv, num_args, argv);
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
                fprintf(stderr, "ICE: Unimplemented opcode %d.\n", op.code);
                exit(1);
        }
    }

    return VAL_NIL;
}
