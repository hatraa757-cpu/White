#include "../include/white/codegen.h"
#include "../include/white/ast.h"
#include "../include/white/common.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static int temp_var_counter = 0;

static char *codegen_gen_temp_var(void) {
    char *var = xmalloc(32);
    snprintf(var, 32, "__tmp_%d", temp_var_counter++);
    return var;
}

static void codegen_emit_c_includes(void) {
    printf("#include <stdio.h>\n");
    printf("#include <stdlib.h>\n");
    printf("#include <stdint.h>\n");
    printf("#include <stdbool.h>\n");
    printf("#include <string.h>\n");
    printf("#include <math.h>\n");
    printf("#include <complex.h>\n");
    printf("#include \"white_runtime.h\"\n\n");
}

static void codegen_emit_c_preamble(void) {
    printf("/* Physics Constants */\n");
    printf("const double SPEED_OF_LIGHT = 299792458.0;\n");
    printf("const double PI = 3.14159265358979323846;\n");
    printf("const double E = 2.71828182845904523536;\n\n");
}

static void codegen_emit_expr_to_c(ASTNode *expr) {
    if (!expr) return;
    
    switch (expr->type) {
        case AST_INT_LITERAL: {
            ASTIntLiteral *lit = (ASTIntLiteral *)expr->data;
            printf("%lld", (long long)lit->value);
            break;
        }
        case AST_FLOAT_LITERAL: {
            ASTFloatLiteral *lit = (ASTFloatLiteral *)expr->data;
            printf("%f", lit->value);
            break;
        }
        case AST_STRING_LITERAL: {
            ASTStringLiteral *lit = (ASTStringLiteral *)expr->data;
            printf("\"%s\"", lit->value);
            break;
        }
        case AST_BOOL_LITERAL: {
            ASTBoolLiteral *lit = (ASTBoolLiteral *)expr->data;
            printf("%s", lit->value ? "true" : "false");
            break;
        }
        case AST_IDENT: {
            ASTIdent *ident = (ASTIdent *)expr->data;
            printf("%s", ident->name);
            break;
        }
        case AST_BINARY_OP: {
            ASTBinaryOp *binop = (ASTBinaryOp *)expr->data;
            printf("(");
            codegen_emit_expr_to_c(binop->left);
            
            switch (binop->op) {
                case TOK_PLUS: printf(" + "); break;
                case TOK_MINUS: printf(" - "); break;
                case TOK_STAR: printf(" * "); break;
                case TOK_SLASH: printf(" / "); break;
                case TOK_PERCENT: printf(" %% "); break;
                case TOK_EQ: printf(" == "); break;
                case TOK_NE: printf(" != "); break;
                case TOK_LT: printf(" < "); break;
                case TOK_LE: printf(" <= "); break;
                case TOK_GT: printf(" > "); break;
                case TOK_GE: printf(" >= "); break;
                case TOK_AND: printf(" && "); break;
                case TOK_OR: printf(" || "); break;
                default: printf(" ?? "); break;
            }
            
            codegen_emit_expr_to_c(binop->right);
            printf(")");
            break;
        }
        case AST_UNARY_OP: {
            ASTUnaryOp *unop = (ASTUnaryOp *)expr->data;
            switch (unop->op) {
                case TOK_MINUS: printf("-"); break;
                case TOK_PLUS: printf("+"); break;
                case TOK_NOT: printf("!"); break;
                case TOK_AMP: printf("&"); break;
                case TOK_STAR: printf("*"); break;
                default: break;
            }
            codegen_emit_expr_to_c(unop->operand);
            break;
        }
        case AST_CALL: {
            ASTCall *call = (ASTCall *)expr->data;
            codegen_emit_expr_to_c(call->func);
            printf("(");
            for (size_t i = 0; i < call->args->count; i++) {
                if (i > 0) printf(", ");
                codegen_emit_expr_to_c((ASTNode *)array_get(call->args, i));
            }
            printf(")");
            break;
        }
        default:
            break;
    }
}

static void codegen_emit_stmt_to_c(ASTNode *stmt, int indent_level) {
    if (!stmt) return;
    
    char indent[128] = {0};
    for (int i = 0; i < indent_level; i++) strcat(indent, "    ");
    
    switch (stmt->type) {
        case AST_VAR_DECL: {
            ASTVarDecl *var = (ASTVarDecl *)stmt->data;
            printf("%sdouble %s", indent, var->name);
            if (var->init_value) {
                printf(" = ");
                codegen_emit_expr_to_c(var->init_value);
            }
            printf(";\n");
            break;
        }
        case AST_RETURN_STMT: {
            ASTReturnStmt *ret = (ASTReturnStmt *)stmt->data;
            printf("%sreturn", indent);
            if (ret->value) {
                printf(" ");
                codegen_emit_expr_to_c(ret->value);
            }
            printf(";\n");
            break;
        }
        case AST_IF_STMT: {
            ASTIfStmt *ifstmt = (ASTIfStmt *)stmt->data;
            printf("%sif (", indent);
            codegen_emit_expr_to_c(ifstmt->condition);
            printf(") {\n");
            codegen_emit_stmt_to_c(ifstmt->then_branch, indent_level + 1);
            if (ifstmt->else_branch) {
                printf("%s} else {\n", indent);
                codegen_emit_stmt_to_c(ifstmt->else_branch, indent_level + 1);
            }
            printf("%s}\n", indent);
            break;
        }
        case AST_WHILE_STMT: {
            ASTWhileStmt *whilestmt = (ASTWhileStmt *)stmt->data;
            printf("%swhile (", indent);
            codegen_emit_expr_to_c(whilestmt->condition);
            printf(") {\n");
            codegen_emit_stmt_to_c(whilestmt->body, indent_level + 1);
            printf("%s}\n", indent);
            break;
        }
        case AST_FOR_STMT: {
            ASTForStmt *forstmt = (ASTForStmt *)stmt->data;
            printf("%sfor (", indent);
            if (forstmt->init) codegen_emit_expr_to_c(forstmt->init);
            printf("; ");
            if (forstmt->condition) codegen_emit_expr_to_c(forstmt->condition);
            printf("; ");
            if (forstmt->update) codegen_emit_expr_to_c(forstmt->update);
            printf(") {\n");
            codegen_emit_stmt_to_c(forstmt->body, indent_level + 1);
            printf("%s}\n", indent);
            break;
        }
        case AST_PRINT_STMT: {
            DynamicArray *args = (DynamicArray *)stmt->data;
            printf("%sprintf(", indent);
            if (args && args->count > 0) {
                for (size_t i = 0; i < args->count; i++) {
                    if (i > 0) printf(", ");
                    codegen_emit_expr_to_c((ASTNode *)array_get(args, i));
                }
            }
            printf(");\n");
            break;
        }
        case AST_BLOCK: {
            ASTBlock *block = (ASTBlock *)stmt->data;
            for (size_t i = 0; i < block->statements->count; i++) {
                codegen_emit_stmt_to_c((ASTNode *)array_get(block->statements, i), indent_level);
            }
            break;
        }
        default:
            break;
    }
}

void codegen_emit_to_c_file(ASTNode *program, const char *output_file) {
    FILE *out = fopen(output_file, "w");
    if (!out) return;
    
    FILE *old_stdout = stdout;
    stdout = out;
    
    codegen_emit_c_includes();
    codegen_emit_c_preamble();
    
    if (program && program->type == AST_PROGRAM) {
        ASTProgram *prog = (ASTProgram *)program->data;
        
        /* Emit function declarations */
        printf("/* Forward declarations */\n");
        for (size_t i = 0; i < prog->statements->count; i++) {
            ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
            if (stmt->type == AST_FUNC_DEF) {
                ASTFuncDef *func = (ASTFuncDef *)stmt->data;
                printf("double %s();\n", func->name);
            }
        }
        printf("\n");
        
        /* Emit functions */
        for (size_t i = 0; i < prog->statements->count; i++) {
            ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
            if (stmt->type == AST_FUNC_DEF) {
                ASTFuncDef *func = (ASTFuncDef *)stmt->data;
                printf("double %s() {\n", func->name);
                codegen_emit_stmt_to_c(func->body, 1);
                printf("}\n\n");
            }
        }
        
        /* Emit main */
        printf("int main() {\n");
        for (size_t i = 0; i < prog->statements->count; i++) {
            ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
            if (stmt->type != AST_FUNC_DEF) {
                codegen_emit_stmt_to_c(stmt, 1);
            }
        }
        printf("    return 0;\n");
        printf("}\n");
    }
    
    stdout = old_stdout;
    fclose(out);
}
void codegen_emit_to_c(ASTNode *program) {
    codegen_emit_c_includes();
    codegen_emit_c_preamble();

    if (program && program->type == AST_PROGRAM) {
        ASTProgram *prog = (ASTProgram *)program->data;

        /* Emit function declarations */
        printf("/* Forward declarations */\n");
        for (size_t i = 0; i < prog->statements->count; i++) {
            ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
            if (stmt->type == AST_FUNC_DEF) {
                ASTFuncDef *func = (ASTFuncDef *)stmt->data;
                printf("double %s();\n", func->name);
            }
        }
        printf("\n");

        /* Emit functions */
        for (size_t i = 0; i < prog->statements->count; i++) {
            ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
            if (stmt->type == AST_FUNC_DEF) {
                ASTFuncDef *func = (ASTFuncDef *)stmt->data;
                printf("double %s() {\n", func->name);
                codegen_emit_stmt_to_c(func->body, 1);
                printf("}\n\n");
            }
        }

        /* Emit main */
        printf("int main() {\n");
        for (size_t i = 0; i < prog->statements->count; i++) {
            ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
            if (stmt->type != AST_FUNC_DEF) {
                codegen_emit_stmt_to_c(stmt, 1);
            }
        }
        printf("    return 0;\n");
        printf("}\n");
    }
}
