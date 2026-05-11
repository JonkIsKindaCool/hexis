#include "hxs_token.h"

HxsToken *create_base_token(HxsTokenKind kind, size_t line, size_t start, size_t end)
{
    HxsToken *tok = malloc(sizeof(HxsToken));
    if (tok == NULL)
        return NULL;

    tok->kind        = kind;
    tok->value       = (HxsTokenValue){0};
    tok->line        = line;
    tok->start       = start;
    tok->end         = end;

    return tok;
}

HxsToken *make_int_token(int64_t value, size_t line, size_t start, size_t end)
{
    HxsToken *tok = create_base_token(INT_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    tok->value.int_val = value;
    return tok;
}

HxsToken *make_float_token(double value, size_t line, size_t start, size_t end)
{
    HxsToken *tok = create_base_token(FLOAT_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    tok->value.float_val = value;
    return tok;
}

HxsToken *make_identifier_token(const char *value, size_t line, size_t start, size_t end)
{
    if (value == NULL)
        return NULL;

    HxsToken *tok = create_base_token(IDENTIFIER_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    size_t len = strlen(value);
    tok->value.str_val = malloc(len + 1);
    if (tok->value.str_val == NULL)
    {
        free(tok);
        return NULL;
    }

    memcpy(tok->value.str_val, value, len + 1);
    return tok;
}

HxsToken *make_string_token(const char *value, bool single, size_t line, size_t start, size_t end)
{
    if (value == NULL)
        return NULL;

    HxsToken *tok = create_base_token(
        single ? SINGLE_QUOTE_STRING_TOKEN : DOUBLE_QUOTE_STRING_TOKEN,
        line, start, end);

    if (tok == NULL)
        return NULL;

    size_t len = strlen(value);
    tok->value.str_val = malloc(len + 1);
    if (tok->value.str_val == NULL)
    {
        free(tok);
        return NULL;
    }

    memcpy(tok->value.str_val, value, len + 1);
    return tok;
}

void freeToken(HxsToken *token)
{
    if (token == NULL)
        return;

    if (token->kind == IDENTIFIER_TOKEN        ||
        token->kind == SINGLE_QUOTE_STRING_TOKEN ||
        token->kind == DOUBLE_QUOTE_STRING_TOKEN)
    {
        free(token->value.str_val);
    }

    free(token);
}

const char *token_kind_name(HxsTokenKind kind)
{
    switch (kind)
    {
    case IDENTIFIER_TOKEN:           return "IDENTIFIER";
    case INT_TOKEN:                  return "INT";
    case FLOAT_TOKEN:                return "FLOAT";
    case DOUBLE_QUOTE_STRING_TOKEN:  return "STRING_DOUBLE";
    case SINGLE_QUOTE_STRING_TOKEN:  return "STRING_SINGLE";
    case FUNCTION_TOKEN:             return "FUNCTION";
    case NEW_TOKEN:                  return "NEW";
    case IF_TOKEN:                   return "IF";
    case ELSE_TOKEN:                 return "ELSE";
    case VAR_TOKEN:                  return "VAR";
    case FINAL_TOKEN:                return "FINAL";
    case TRUE_TOKEN:                 return "TRUE";
    case FALSE_TOKEN:                return "FALSE";
    case WHILE_TOKEN:                return "WHILE";
    case FOR_TOKEN:                  return "FOR";
    case SWITCH_TOKEN:               return "SWITCH";
    case CASE_TOKEN:                 return "CASE";
    case TRY_TOKEN:                  return "TRY";
    case CATCH_TOKEN:                return "CATCH";
    case ADD_TOKEN:                  return "ADD";
    case MINUS_TOKEN:                return "MINUS";
    case MULTIPLY_TOKEN:             return "MULTIPLY";
    case DIVIDE_TOKEN:               return "DIVIDE";
    case MODULO_TOKEN:               return "MODULO";
    case EQUAL_TOKEN:                return "EQUAL";
    case NOT_EQUAL_TOKEN:            return "NOT_EQUAL";
    case LESS_TOKEN:                 return "LESS";
    case GREATER_TOKEN:              return "GREATER";
    case LESS_EQUAL_TOKEN:           return "LESS_EQUAL";
    case GREATER_EQUAL_TOKEN:        return "GREATER_EQUAL";
    case LOGICAL_AND_TOKEN:          return "LOGICAL_AND";
    case LOGICAL_OR_TOKEN:           return "LOGICAL_OR";
    case LOGICAL_NOT_TOKEN:          return "LOGICAL_NOT";
    case AND_BIT_TOKEN:              return "AND_BIT";
    case OR_BIT_TOKEN:               return "OR_BIT";
    case XOR_BIT_TOKEN:              return "XOR_BIT";
    case ASSIGN_TOKEN:               return "ASSIGN";
    case ADD_ASSIGN_TOKEN:           return "ADD_ASSIGN";
    case MINUS_ASSIGN_TOKEN:         return "MINUS_ASSIGN";
    case MULTIPLY_ASSIGN_TOKEN:      return "MULTIPLY_ASSIGN";
    case DIVIDE_ASSIGN_TOKEN:        return "DIVIDE_ASSIGN";
    case MODULO_ASSIGN_TOKEN:        return "MODULO_ASSIGN";
    case SPREAD_TOKEN:               return "SPREAD";
    case SPREAD_EQUAL_TOKEN:         return "SPREAD_EQUAL";
    case NULLISH_TOKEN:              return "NULLISH";
    case NULL_OPTIONAL_TOKEN:        return "NULL_OPTIONAL";
    case COMMA_TOKEN:                return "COMMA";
    case LEFTPAREN_TOKEN:            return "LEFTPAREN";
    case RIGHTPAREN_TOKEN:           return "RIGHTPAREN";
    case LEFTCURLYBRACKET_TOKEN:     return "LEFTCURLYBRACKET";
    case RIGHTCURLYBRACKET_TOKEN:    return "RIGHTCURLYBRACKET";
    case LEFTBRACKET_TOKEN:          return "LEFTBRACKET";
    case RIGHTBRACKET_TOKEN:         return "RIGHTBRACKET";
    case COLON_TOKEN:                return "COLON";
    case SEMICOLON_TOKEN:            return "SEMICOLON";
    case DOT_TOKEN:                  return "DOT";
    case EOF_TOKEN:                  return "EOF";
    default:                         return "UNKNOWN";
    }
}

char *token_to_string(HxsToken *token)
{
    if (!token)
        return strdup("(null token)");

    const char *kind   = token_kind_name(token->kind);
    char       *result = malloc(256);
    if (!result)
        return NULL;

    switch (token->kind)
    {
    case INT_TOKEN:
        snprintf(result, 256, "%s(%ld)", kind, token->value.int_val);
        break;
    case FLOAT_TOKEN:
        snprintf(result, 256, "%s(%g)", kind, token->value.float_val);
        break;
    case IDENTIFIER_TOKEN:
    case DOUBLE_QUOTE_STRING_TOKEN:
    case SINGLE_QUOTE_STRING_TOKEN:
        snprintf(result, 256, "%s(\"%s\")", kind, token->value.str_val);
        break;
    default:
        snprintf(result, 256, "%s", kind);
        break;
    }

    return result;
}