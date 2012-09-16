#ifndef OPCODES_H
#define OPCODES_H

struct op_info {
    char *name;
    /* Add other shit */
};

typedef struct {
	u8 code:8;
	s32 a:12;
	s32 b:12;
} XR_OP;

enum XR_OPCODE {
	OP_NOP,
	OP_PLUS,
	OP_MINUS,
    OP_MULT,
    OP_DIV,
    OP_MKVAR,
    OP_LLOAD,
    OP_LSTORE,
    OP_IVAL, /* Index into array of values */
    OP_SEND,
    OP_NOTJMP,
    OP_DVAL, /* Stored directly in opcode */
    OP_EQ,
    OP_NEQ,
    OP_NOT,
    OP_JMP,
    OP_SELF,
    OP_GT,
    OP_LT,
    OP_GTE,
    OP_LTE,
    OP_BRK,
    OP_POP,
    OP_BUILD_LIST,
    OP_AND,
    OP_OR
};

extern struct op_info op_info[];

#endif
