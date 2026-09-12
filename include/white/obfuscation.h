#ifndef WHITE_OBFUSCATION_H
#define WHITE_OBFUSCATION_H

#include "common.h"
#include "ast.h"

/* Code Obfuscation Support
 *
 * Legitimate defensive security feature for:
 * - Anti-reverse engineering
 * - Binary integrity protection
 * - Code analysis resistance
 * - Research and academic study
 *
 * NOT for malware, persistence, or evasion.
 */

typedef enum {
    OBFUSCATE_NONE = 0,
    OBFUSCATE_NAMES = 1,       /* Rename variables */
    OBFUSCATE_CONTROL_FLOW = 2, /* Flatten control flow */
    OBFUSCATE_DATA_FLOW = 4,    /* Obfuscate data dependencies */
    OBFUSCATE_STRINGS = 8,      /* Encode string literals */
    OBFUSCATE_ALL = 15,
} ObfuscationLevel;

typedef struct {
    ObfuscationLevel level;
    uint32_t seed;
    bool enable_string_encoding;
    bool enable_name_mangling;
    bool enable_control_flow_flattening;
    bool keep_debug_symbols;
} ObfuscationOptions;

typedef struct {
    DynamicArray *name_mappings;  /* Original -> Obfuscated names */
    ObfuscationOptions opts;
    uint32_t obfuscation_seed;
} ObfuscationEngine;

/* Create obfuscation engine */
ObfuscationEngine *obfuscation_create(ObfuscationOptions *opts);

/* Apply obfuscation to AST */
void obfuscation_apply(ObfuscationEngine *engine, ASTNode *ast);

/* Get obfuscation report */
const char *obfuscation_get_report(ObfuscationEngine *engine);

/* Cleanup */
void obfuscation_free(ObfuscationEngine *engine);

#endif
typedef struct {
    ObfuscationLevel level;
    uint32_t seed;
    bool enable_string_encoding;
    bool enable_name_mangling;
    bool enable_control_flow_flattening;
    bool keep_debug_symbols;
} ObfuscationOptions;