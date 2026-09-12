#include "../include/white/compiler.h"
#include "../include/white/lexer.h"
#include "../include/white/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [command] [options] <file.white>\n", prog_name);
    fprintf(stderr, "\nCommands:\n");
    fprintf(stderr, "  (none)        Compile and run\n");
    fprintf(stderr, "  build         Compile to executable\n");
    fprintf(stderr, "  run           Compile and execute\n");
    fprintf(stderr, "  check         Check syntax only\n");
    fprintf(stderr, "  emit-c        Generate C source code\n");
    fprintf(stderr, "  version       Print version\n");
    fprintf(stderr, "  help          Show this help\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    CompileMode mode = COMPILE_RUN;
    const char *input_file = NULL;
    const char *output_file = NULL;
    bool verbose = false;
    
    int arg_idx = 1;
    
    /* Parse command */
    if (argv[arg_idx][0] != '-' && strchr(argv[arg_idx], '.') == NULL) {
        const char *cmd = argv[arg_idx];
        if (strcmp(cmd, "build") == 0) mode = COMPILE_BUILD;
        else if (strcmp(cmd, "run") == 0) mode = COMPILE_RUN;
        else if (strcmp(cmd, "check") == 0) mode = COMPILE_CHECK;
        else if (strcmp(cmd, "emit-c") == 0) mode = COMPILE_EMIT_C;
        else if (strcmp(cmd, "version") == 0) {
            printf("%s\n", compiler_version());
            return 0;
        } else if (strcmp(cmd, "help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown command: %s\n", cmd);
            return 1;
        }
        arg_idx++;
    }
    
    /* Parse options */
    while (arg_idx < argc) {
        if (strcmp(argv[arg_idx], "-v") == 0 || strcmp(argv[arg_idx], "--verbose") == 0) {
            verbose = true;
            arg_idx++;
        } else if (strcmp(argv[arg_idx], "-o") == 0) {
            if (arg_idx + 1 < argc) {
                output_file = argv[arg_idx + 1];
                arg_idx += 2;
            } else {
                fprintf(stderr, "Error: -o requires an argument\n");
                return 1;
            }
        } else if (argv[arg_idx][0] != '-') {
            input_file = argv[arg_idx];
            arg_idx++;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[arg_idx]);
            return 1;
        }
    }
    
    if (!input_file) {
        fprintf(stderr, "Error: no input file specified\n");
        print_usage(argv[0]);
        return 1;
    }
    
    CompileOptions opts = {
        .input_file = input_file,
        .output_file = output_file,
        .mode = mode,
        .verbose = verbose,
        .emit_ir = false
    };
    
    return compiler_compile(&opts);
}
if (lexer->source[lexer->position] == '=') {
                    type = TOK_NEQ;
                    lexer_advance(lexer);
                } else {
                    type = TOK_BANG;
                }
              else if (c == '<') {
                lexer_advance(lexer);
                if (lexer->source[lexer->position] == '=') {
                    type = TOK_LEQ;
                    lexer_advance(lexer);
                } else {
                    type = TOK_LT;
                }
            } else if (c == '>') {
                lexer_advance(lexer);
                if (lexer->source[lexer->position] == '=') {
                    type = TOK_GEQ;
                    lexer_advance(lexer);
                } else {
                    type = TOK_GT;
                }
            } else if (c == '&') {
                lexer_advance(lexer);
                type = lexer->source[lexer->position] == '&' ? TOK_AND : TOK_AMP;
                if (lexer->source[lexer->position] == '&') lexer_advance(lexer);
            } else if (c == '|') {
                lexer_advance(lexer);
                type = lexer->source[lexer->position] == '|' ? TOK_OR : TOK_PIPE;
                if (lexer->source[lexer->position] == '|') lexer_advance(lexer);
            } else if (c == '(') {
                type = TOK_LPAREN;
                lexer_advance(lexer);
            } else if (c == ')') {
                type = TOK_RPAREN;
                lexer_advance(lexer);
            } else if (c == '{') {
                type = TOK_LBRACE;
                lexer_advance(lexer);
            } else if (c == '}') {
                type = TOK_RBRACE;
                lexer_advance(lexer);
            } else if (c == '[') {
                type = TOK_LBRACKET;
                lexer_advance(lexer);
            } else if (c == ']') {
                type = TOK_RBRACKET;
                lexer_advance(lexer);
            } else if (c == ';') {
                type = TOK_SEMICOLON;
                lexer_advance(lexer);
            } else if (c == ',') {
                type = TOK_COMMA;
                lexer_advance(lexer);
            } else if (c == '.') {
                type = TOK_DOT;
                lexer_advance(lexer);
            } else if (c == ':') {
                type = TOK_COLON;
                lexer_advance(lexer);
            } else {
                fprintf(stderr, "Unexpected character '%c' at line %d, column %d\n", c, lexer->line, lexer->column);
            }

            
