#ifndef OPCODES_H
#define OPCODES_H

#define OPCODES \
    X(OP_NOP, "nop") \
    X(OP_PLUS, "plus") \
    X(OP_MINUS, "minus") \
    X(OP_MULT, "mult") \
    X(OP_DIV, "div") \
    X(OP_MKVAR, "mkvar") \
    X(OP_LLOAD, "lload") \
    X(OP_LSTORE, "lstore") \
    X(OP_IVAL, "ival") \
    X(OP_SEND, "send") \
    X(OP_NOTJMP, "notjmp") \
    X(OP_DVAL, "dval") \
    X(OP_EQ, "eq") \
    X(OP_NEQ, "neq") \
    X(OP_NOT, "not") \
    X(OP_JMP, "jmp") \
    X(OP_SELF, "self") \
    X(OP_GT, "gt") \
    X(OP_LT, "lt") \
    X(OP_GTE, "gte") \
    X(OP_LTE, "lte") \
    X(OP_POP, "pop") \
    X(OP_BUILD_LIST, "build_list") \
    X(OP_AND, "and") \
    X(OP_OR, "or") \
    X(OP_ASSERT, "assert") \
    X(OP_RETURN, "return") \

#define X(a, b) a,
enum XR_OPCODE { OPCODES };
#undef X

typedef struct {
	u8 code:8;
	s32 a:12;
	s32 b:12;
} XR_OP;

#endif
