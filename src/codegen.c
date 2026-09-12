#include "../include/white/codegen.h"
#include <stdio.h>
#include <string.h>

static void codegen_indent(CodeGenerator *cg) {
    for (int i = 0; i < cg->indent_level; i++) {
        fprintf(stdout, "  ");
    }
}

static void codegen_emit_line(CodeGenerator *cg, const char *fmt, ...) {
    char buffer[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    char *line = str_dup(buffer);
    array_push(cg->output, line);
}

static void codegen_emit_expr(CodeGenerator *cg, ASTNode *expr);

static void codegen_emit_expr(CodeGenerator *cg, ASTNode *expr) {
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
            codegen_emit_expr(cg, binop->left);
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
                default: break;
            }
            codegen_emit_expr(cg, binop->right);
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
            codegen_emit_expr(cg, unop->operand);
            break;
        }
        case AST_CALL: {
            ASTCall *call = (ASTCall *)expr->data;
            codegen_emit_expr(cg, call->func);
            printf("(");
            for (size_t i = 0; i < call->args->count; i++) {
                if (i > 0) printf(", ");
                codegen_emit_expr(cg, (ASTNode *)array_get(call->args, i));
            }
            printf(")");
            break;
        }
        default:
            break;
    }
}

static void codegen_emit_stmt(CodeGenerator *cg, ASTNode *stmt);

static void codegen_emit_block(CodeGenerator *cg, ASTNode *block) {
    if (!block || block->type != AST_BLOCK) return;
    
    ASTBlock *b = (ASTBlock *)block->data;
    printf("{\n");
    cg->indent_level++;
    
    for (size_t i = 0; i < b->statements->count; i++) {
        ASTNode *stmt = (ASTNode *)array_get(b->statements, i);
        codegen_emit_stmt(cg, stmt);
    }
    
    cg->indent_level--;
    codegen_indent(cg);
    printf("}\n");
}

static void codegen_emit_stmt(CodeGenerator *cg, ASTNode *stmt) {
    if (!stmt) return;
    
    codegen_indent(cg);
    
    switch (stmt->type) {
        case AST_EXPR_STMT:
        case AST_INT_LITERAL:
        case AST_FLOAT_LITERAL:
        case AST_BINARY_OP:
        case AST_UNARY_OP:
        case AST_CALL:
            codegen_emit_expr(cg, stmt);
            printf(";\n");
            break;
            
        case AST_VAR_DECL: {
            ASTVarDecl *var = (ASTVarDecl *)stmt->data;
            printf("double %s", var->name);
            if (var->init_value) {
                printf(" = ");
                codegen_emit_expr(cg, var->init_value);
            }
            printf(";\n");
            break;
        }
        
        case AST_IF_STMT: {
            ASTIfStmt *ifstmt = (ASTIfStmt *)stmt->data;
            printf("if (");
            codegen_emit_expr(cg, ifstmt->condition);
            printf(") ");
            codegen_emit_stmt(cg, ifstmt->then_branch);
            if (ifstmt->else_branch) {
                codegen_indent(cg);
                printf("else ");
                codegen_emit_stmt(cg, ifstmt->else_branch);
            }
            break;
        }
        
        case AST_WHILE_STMT: {
            ASTWhileStmt *whilestmt = (ASTWhileStmt *)stmt->data;
            printf("while (");
            codegen_emit_expr(cg, whilestmt->condition);
            printf(") ");
            codegen_emit_stmt(cg, whilestmt->body);
            break;
        }
        
        case AST_FOR_STMT: {
            ASTForStmt *forstmt = (ASTForStmt *)stmt->data;
            printf("for (");
            if (forstmt->init) {
                codegen_emit_expr(cg, forstmt->init);
            }
            printf("; ");
            if (forstmt->condition) {
                codegen_emit_expr(cg, forstmt->condition);
            }
            printf("; ");
            if (forstmt->update) {
                codegen_emit_expr(cg, forstmt->update);
            }
            printf(") ");
            codegen_emit_stmt(cg, forstmt->body);
            break;
        }
        
        case AST_RETURN_STMT: {
            ASTReturnStmt *ret = (ASTReturnStmt *)stmt->data;
            printf("return");
            if (ret->value) {
                printf(" ");
                codegen_emit_expr(cg, ret->value);
            }
            printf(";\n");
            break;
        }
        
        case AST_BREAK_STMT:
            printf("break;\n");
            break;
            
        case AST_CONTINUE_STMT:
            printf("continue;\n");
            break;
            
        case AST_PRINT_STMT: {
            DynamicArray *args = (DynamicArray *)stmt->data;
            printf("printf(");
            if (args->count > 0) {
                ASTNode *first = (ASTNode *)array_get(args, 0);
                codegen_emit_expr(cg, first);
                for (size_t i = 1; i < args->count; i++) {
                    printf(", ");
                    codegen_emit_expr(cg, (ASTNode *)array_get(args, i));
                }
            }
            printf(");\n");
            break;
        }
        
        case AST_BLOCK:
            codegen_emit_block(cg, stmt);
            break;
            
        default:
            break;
    }
}

CodeGenerator *codegen_create(void) {
    CodeGenerator *cg = xmalloc(sizeof(CodeGenerator));
    cg->output = array_create();
    cg->indent_level = 0;
    cg->temp_counter = 0;
    return cg;
}

void codegen_emit(CodeGenerator *cg, ASTNode *program) {
    if (!program || program->type != AST_PROGRAM) return;
    
    printf("#include <stdio.h>\n");
    printf("#include <stdint.h>\n");
    printf("#include <stdbool.h>\n");
    printf("#include <complex.h>\n");
    printf("#include \"white_runtime.h\"\n\n");
    
    ASTProgram *prog = (ASTProgram *)program->data;
    
    for (size_t i = 0; i < prog->statements->count; i++) {
        ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
        
        if (stmt->type == AST_FUNC_DEF) {
            ASTFuncDef *func = (ASTFuncDef *)stmt->data;
            printf("double %s()", func->name);
            codegen_emit_block(cg, func->body);
            printf("\n");
        }
    }
    
    printf("\nint main() {\n");
    for (size_t i = 0; i < prog->statements->count; i++) {
        ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
        if (stmt->type != AST_FUNC_DEF) {
            codegen_emit_stmt(cg, stmt);
        }
    }
    printf("    return 0;\n");
    printf("}\n");
}

char *codegen_get_output(CodeGenerator *cg) {
    size_t total = 0;
    for (size_t i = 0; i < cg->output->count; i++) {
        total += strlen((char *)array_get(cg->output, i)) + 1;
    }
    
    char *result = xmalloc(total + 1);
    result[0] = 0;
    for (size_t i = 0; i < cg->output->count; i++) {
        strcat(result, (char *)array_get(cg->output, i));
        strcat(result, "\n");
    }
    return result;
}

void codegen_free(CodeGenerator *cg) {
    for (size_t i = 0; i < cg->output->count; i++) {
        xfree(array_get(cg->output, i));
    }
    array_free(cg->output);
    xfree(cg);
}
int codegen_write_output_to_file(CodeGenerator *cg, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return -1;
    
    for (size_t i = 0; i < cg->output->count; i++) {
        fprintf(file, "%s\n", (char *)array_get(cg->output, i));
    }
    
    fclose(file);
    return 0;
}