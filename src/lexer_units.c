#include "../include/white/lexer.h"
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Unit multiplier constants */
static const double UNIT_MULTIPLIERS[] = {
    [TOK_HZ] = 1.0,
    [TOK_KHZ] = 1e3,
    [TOK_MHZ] = 1e6,
    [TOK_GHZ] = 1e9,
    [TOK_MS] = 1e-3,
    [TOK_US] = 1e-6,
    [TOK_NS] = 1e-9,
};

static const char *UNIT_NAMES[] = {
    [TOK_HZ] = "Hz",
    [TOK_KHZ] = "kHz",
    [TOK_MHZ] = "MHz",
    [TOK_GHZ] = "GHz",
    [TOK_MS] = "ms",
    [TOK_US] = "us",
    [TOK_NS] = "ns",
};

/* Parse a number with unit suffix like 2.4GHz or 10MHz */
double lexer_parse_number_with_unit(const char *lexeme, size_t length, TokenType unit_type) {
    char buffer[256];
    size_t num_len = length;
    
    /* Find where the unit starts (first non-digit after decimal point) */
    for (size_t i = 0; i < length; i++) {
        if (!isdigit(lexeme[i]) && lexeme[i] != '.' && lexeme[i] != 'e' && lexeme[i] != 'E' && 
            lexeme[i] != '+' && lexeme[i] != '-') {
            num_len = i;
            break;
        }
    }
    
    strncpy(buffer, lexeme, num_len < sizeof(buffer) ? num_len : sizeof(buffer) - 1);
    buffer[num_len < sizeof(buffer) ? num_len : sizeof(buffer) - 1] = '\0';
    
    double value = strtod(buffer, NULL);
    return value * UNIT_MULTIPLIERS[unit_type];
}

/* Check if identifier is a unit and return token type */
TokenType lexer_check_unit(const char *lexeme, size_t length) {
    if (length == 2 && strncmp(lexeme, "Hz", 2) == 0) return TOK_HZ;
    if (length == 3 && strncmp(lexeme, "kHz", 3) == 0) return TOK_KHZ;
    if (length == 3 && strncmp(lexeme, "MHz", 3) == 0) return TOK_MHZ;
    if (length == 3 && strncmp(lexeme, "GHz", 3) == 0) return TOK_GHZ;
    if (length == 2 && strncmp(lexeme, "ms", 2) == 0) return TOK_MS;
    if (length == 2 && strncmp(lexeme, "us", 2) == 0) return TOK_US;
    if (length == 2 && strncmp(lexeme, "ns", 2) == 0) return TOK_NS;
    return TOK_ERROR;
}

const char *lexer_unit_name(TokenType type) {
    if (type >= TOK_HZ && type <= TOK_NS) {
        return UNIT_NAMES[type];
    }
    return "unknown";
}

double lexer_unit_multiplier(TokenType type) {
    if (type >= TOK_HZ && type <= TOK_NS) {
        return UNIT_MULTIPLIERS[type];
    }
    return 1.0;
}
const bool lexer_is_unit(TokenType type) {
    return type >= TOK_HZ && type <= TOK_NS;
}
void lexer_print_unit(TokenType type) {
    if (type >= TOK_HZ && type <= TOK_NS) {
        printf("%s", UNIT_NAMES[type]);
    } else {
        printf("unknown");
    }
}
void lexer_print_number_with_unit(double value, TokenType unit_type) {
    if (unit_type >= TOK_HZ && unit_type <= TOK_NS) {
        printf("%g%s", value / UNIT_MULTIPLIERS[unit_type], UNIT_NAMES[unit_type]);
    } else {
        printf("%g", value);
    }
}
