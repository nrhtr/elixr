#ifndef COMPILE_H
#define COMPILE_H

void compile_ast(XR tree);
void compile_expr(XR expr_node);

void ast_compile(XR ast, struct XRMethod *m);
void ast_compile_method(XR ast, struct XRMethod *m);
void xr_asm_op(struct XRAsm *m, u8 ins, int _a, int _b);

XR xr_parse_dump_from_stdin(void);
XR xr_parse_dump_file(char *);
XR xr_parse_code_from_stdin(void);
XR xr_parse_method_from_stdin(void);

struct XRMethod *xr_method_new(XR name, XR args);

#endif
