#include "lexer/hxs_token.h"

HxsToken *create_base_token(HxsArena *arena, HxsTokenKind kind, size_t line, size_t start, size_t end)
{
    HxsToken *tok = Hxs_Arena_alloc(arena, sizeof(HxsToken));
    if (tok == NULL)
        return NULL;

    tok->kind  = kind;
    tok->value = (HxsTokenValue){0};
    tok->line  = line;
    tok->start = start;
    tok->end   = end;

    return tok;
}

HxsToken *create_op_token(HxsArena *arena, HxsOpKind op, size_t line, size_t start, size_t end)
{
    HxsToken *tok = create_base_token(arena, OP_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    tok->value.op = op;
    return tok;
}

HxsToken *make_int_token(HxsArena *arena, int64_t value, size_t line, size_t start, size_t end)
{
    HxsToken *tok = create_base_token(arena, INT_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    tok->value.int_val = value;
    return tok;
}

HxsToken *make_float_token(HxsArena *arena, double value, size_t line, size_t start, size_t end)
{
    HxsToken *tok = create_base_token(arena, FLOAT_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    tok->value.float_val = value;
    return tok;
}

HxsToken *make_identifier_token(HxsArena *arena, char *value, size_t line, size_t start, size_t end)
{
    if (value == NULL)
        return NULL;

    HxsToken *tok = create_base_token(arena, IDENTIFIER_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    tok->value.str_val = value;
    return tok;
}

HxsToken *make_string_token(HxsArena *arena, char *value, bool single, size_t line, size_t start, size_t end)
{
    if (value == NULL)
        return NULL;

    HxsToken *tok = create_base_token(arena,
        single ? SINGLE_QUOTE_STRING_TOKEN : DOUBLE_QUOTE_STRING_TOKEN,
        line, start, end);

    if (tok == NULL)
        return NULL;

    tok->value.str_val = value;
    return tok;
}

const char *op_kind_name(HxsOpKind op)
{
    switch (op)
    {
    case HXS_OP_ADD:          return "+";
    case HXS_OP_SUB:          return "-";
    case HXS_OP_MUL:          return "*";
    case HXS_OP_DIV:          return "/";
    case HXS_OP_MOD:          return "%";
    case HXS_OP_ASSIGN:       return "=";
    case HXS_OP_ADD_ASSIGN:   return "+=";
    case HXS_OP_SUB_ASSIGN:   return "-=";
    case HXS_OP_MUL_ASSIGN:   return "*=";
    case HXS_OP_DIV_ASSIGN:   return "/=";
    case HXS_OP_MOD_ASSIGN:   return "%=";
    case HXS_OP_EQ:           return "==";
    case HXS_OP_NEQ:          return "!=";
    case HXS_OP_LT:           return "<";
    case HXS_OP_LE:           return "<=";
    case HXS_OP_GT:           return ">";
    case HXS_OP_GE:           return ">=";
    case HXS_OP_AND:          return "&&";
    case HXS_OP_OR:           return "||";
    case HXS_OP_NOT:          return "!";
    case HXS_OP_BIT_AND:      return "&";
    case HXS_OP_BIT_OR:       return "|";
    case HXS_OP_BIT_XOR:      return "^";
    case HXS_OP_SPREAD:       return "...";
    case HXS_OP_SPREAD_ASSIGN: return "..=";
    case HXS_OP_NULLISH:      return "??";
    case HXS_OP_NULL_OPT:     return ".=";
    default:                  return "?op";
    }
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
    case CONST_TOKEN:                return "CONST";
    case TRUE_TOKEN:                 return "TRUE";
    case FALSE_TOKEN:                return "FALSE";
    case WHILE_TOKEN:                return "WHILE";
    case FOR_TOKEN:                  return "FOR";
    case SWITCH_TOKEN:               return "SWITCH";
    case CASE_TOKEN:                 return "CASE";
    case TRY_TOKEN:                  return "TRY";
    case CATCH_TOKEN:                return "CATCH";
    case CLASS_TOKEN:                return "CLASS";
    case RETURN_TOKEN:               return "RETURN";
    case OP_TOKEN:                   return "OP";
    case COLON_TOKEN:                return "COLON";
    case SEMICOLON_TOKEN:            return "SEMICOLON";
    case DOT_TOKEN:                  return "DOT";
    case COMMA_TOKEN:                return "COMMA";
    case LEFTPAREN_TOKEN:            return "LEFTPAREN";
    case RIGHTPAREN_TOKEN:           return "RIGHTPAREN";
    case LEFTBRACKET_TOKEN:          return "LEFTBRACKET";
    case RIGHTBRACKET_TOKEN:         return "RIGHTBRACKET";
    case LEFTCURLYBRACKET_TOKEN:     return "LEFTCURLYBRACKET";
    case RIGHTCURLYBRACKET_TOKEN:    return "RIGHTCURLYBRACKET";
    case QUESTION_TOKEN:             return "QUESTION";
    case EOF_TOKEN:                  return "EOF";
    default:                         return "UNKNOWN";
    }
}

char *Hxs_token_to_string(HxsToken *token)
{
    if (!token)
        return strdup("(null token)");

    const char *kind = token_kind_name(token->kind);
    char *result = malloc(256);
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
    case OP_TOKEN:
        snprintf(result, 256, "OP(%s)", op_kind_name(token->value.op));
        break;
    default:
        snprintf(result, 256, "%s", kind);
        break;
    }

    return result;
}