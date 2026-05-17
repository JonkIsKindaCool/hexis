#include "lexer/hxs_lexer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static char lx_peek(HxsLexer *l) { return l->src[l->pos]; }
static char lx_peek2(HxsLexer *l) { return l->src[l->pos + 1]; }
static bool lx_at_end(HxsLexer *l) { return l->src[l->pos] == '\0'; }

static char lx_advance(HxsLexer *l)
{
    char c = l->src[l->pos++];
    if (c == '\n')
    {
        l->line++;
        l->linePos = 1;
    }
    else
    {
        l->linePos++;
    }
    return c;
}

static void lx_skip_whitespace(HxsLexer *l)
{
    while (!lx_at_end(l))
    {
        char c = lx_peek(l);
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
            lx_advance(l);
        else if (c == '/' && lx_peek2(l) == '/')
        {
            while (!lx_at_end(l) && lx_peek(l) != '\n')
                lx_advance(l);
        }
        else if (c == '/' && lx_peek2(l) == '*')
        {
            lx_advance(l);
            lx_advance(l);
            while (!lx_at_end(l))
            {
                if (lx_peek(l) == '*' && lx_peek2(l) == '/')
                {
                    lx_advance(l);
                    lx_advance(l);
                    break;
                }
                lx_advance(l);
            }
        }
        else
            break;
    }
}

typedef struct
{
    const char *word;
    HxsTokenKind kind;
} Keyword;

static Keyword KEYWORDS[] = {
    {"function", FUNCTION_TOKEN},
    {"new", NEW_TOKEN},
    {"if", IF_TOKEN},
    {"else", ELSE_TOKEN},
    {"var", VAR_TOKEN},
    {"const", CONST_TOKEN},
    {"true", TRUE_TOKEN},
    {"false", FALSE_TOKEN},
    {"while", WHILE_TOKEN},
    {"for", FOR_TOKEN},
    {"switch", SWITCH_TOKEN},
    {"case", CASE_TOKEN},
    {"try", TRY_TOKEN},
    {"catch", CATCH_TOKEN},
    {"class", CLASS_TOKEN},
    {"return", RETURN_TOKEN},
    {NULL, EOF_TOKEN},
};

static HxsTokenKind keyword_or_ident(const char *word)
{
    for (int i = 0; KEYWORDS[i].word; i++)
        if (strcmp(KEYWORDS[i].word, word) == 0)
            return KEYWORDS[i].kind;
    return IDENTIFIER_TOKEN;
}

static HxsToken *lex_number(HxsLexer *l)
{
    size_t start = l->linePos;
    size_t line = l->line;
    bool is_float = false;

    while (!lx_at_end(l) && isdigit((unsigned char)lx_peek(l)))
        lx_advance(l);

    if (!lx_at_end(l) && lx_peek(l) == '.' && isdigit((unsigned char)lx_peek2(l)))
    {
        is_float = true;
        lx_advance(l);
        while (!lx_at_end(l) && isdigit((unsigned char)lx_peek(l)))
            lx_advance(l);
    }

    size_t len = l->linePos - start;
    char *buf = Hxs_Arena_alloc(l->arena, len + 1);
    memcpy(buf, l->src + (l->pos - len), len);
    buf[len] = '\0';

    HxsToken *tok;
    if (is_float)
        tok = make_float_token(l->arena, strtod(buf, NULL), line, start, l->linePos);
    else
        tok = make_int_token(l->arena, (int64_t)strtoll(buf, NULL, 10), line, start, l->linePos);

    return tok;
}

static HxsToken *lex_string(HxsLexer *l)
{
    size_t line = l->line;
    size_t start = l->linePos;
    char quote = lx_advance(l);
    bool single = (quote == '\'');

    size_t cap = 64, len = 0;
    char *buf = Hxs_Arena_alloc(l->arena, cap);

    while (!lx_at_end(l) && lx_peek(l) != quote)
    {
        if (len + 2 >= cap)
        {
            cap *= 2;
            buf = realloc(buf, cap);
        }

        char c = lx_advance(l);
        if (c == '\\' && !lx_at_end(l))
        {
            char esc = lx_advance(l);
            switch (esc)
            {
            case 'n':
                buf[len++] = '\n';
                break;
            case 't':
                buf[len++] = '\t';
                break;
            case 'r':
                buf[len++] = '\r';
                break;
            case '\\':
                buf[len++] = '\\';
                break;
            default:
                buf[len++] = esc;
                break;
            }
        }
        else
            buf[len++] = c;
    }

    if (!lx_at_end(l))
        lx_advance(l);
    buf[len] = '\0';

    HxsToken *tok = make_string_token(l->arena, buf, single, line, start, l->linePos);
    return tok;
}

static HxsToken *lex_identifier(HxsLexer *l)
{
    size_t line = l->line;
    size_t start = l->linePos;

    while (!lx_at_end(l) && (isalnum((unsigned char)lx_peek(l)) || lx_peek(l) == '_'))
        lx_advance(l);

    size_t len = l->linePos - start;
    char *buf = Hxs_Arena_alloc(l->arena, len + 1);
    memcpy(buf, l->src + (l->pos - len), len);
    buf[len] = '\0';

    HxsTokenKind kind = keyword_or_ident(buf);
    HxsToken *tok;

    if (kind == IDENTIFIER_TOKEN)
        tok = make_identifier_token(l->arena, buf, line, start, l->linePos);
    else
        tok = create_base_token(l->arena, kind, line, start, l->linePos);

    return tok;
}

HxsLexer *Hxs_make_lexer(HxsArena *arena, const char *src)
{
    HxsLexer *l = Hxs_Arena_alloc(arena, sizeof(HxsLexer));
    l->src = src;
    l->arena = arena;
    l->line = 1;
    l->pos = 0;
    l->linePos = 1;
    return l;
}

HxsToken *Hxs_get_token(HxsLexer *lexer, bool advance)
{
    int saved_pos = lexer->pos;
    int saved_line = lexer->line;
    int saved_linePos = lexer->linePos;

    if (setjmp(lexer->error_jmp) != 0)
    {
        fprintf(stderr, "[LexerError] line %d character %d: %s\n", lexer->line, lexer->linePos, lexer->error_msg);
        return NULL;
    }

    lx_skip_whitespace(lexer);

    if (lx_at_end(lexer))
    {
        HxsToken *t = create_base_token(lexer->arena, EOF_TOKEN, lexer->line, lexer->linePos, lexer->linePos);
        if (!advance)
        {
            lexer->pos = saved_pos;
            lexer->line = saved_line;
            lexer->linePos = saved_linePos;
        }
        return t;
    }

    size_t line = lexer->line;
    size_t start = lexer->linePos;
    char c = lx_peek(lexer);

    HxsToken *tok = NULL;

#define SINGLE(kind)                                                          \
    lx_advance(lexer);                                                        \
    tok = create_base_token(lexer->arena, kind, line, start, lexer->linePos); \
    break

#define DOUBLE(next, kind_double, kind_single)                                           \
    lx_advance(lexer);                                                                   \
    if (lx_peek(lexer) == (next))                                                        \
    {                                                                                    \
        lx_advance(lexer);                                                               \
        tok = create_base_token(lexer->arena, kind_double, line, start, lexer->linePos); \
    }                                                                                    \
    else                                                                                 \
        tok = create_base_token(lexer->arena, kind_single, line, start, lexer->linePos); \
    break

    switch (c)
    {
    case '+':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_ADD_ASSIGN, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_ADD, line, start, lexer->linePos);
        break;

    case '-':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_SUB_ASSIGN, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_SUB, line, start, lexer->linePos);
        break;

    case '*':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_MUL_ASSIGN, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_MUL, line, start, lexer->linePos);
        break;

    case '/':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_DIV_ASSIGN, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_DIV, line, start, lexer->linePos);
        break;

    case '%':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_MOD_ASSIGN, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_MOD, line, start, lexer->linePos);
        break;

    case '=':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_EQ, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_ASSIGN, line, start, lexer->linePos);
        break;

    case '!':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_NEQ, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_NOT, line, start, lexer->linePos);
        break;

    case '<':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_LE, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_LT, line, start, lexer->linePos);
        break;

    case '>':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_GE, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_GT, line, start, lexer->linePos);
        break;

    case '&':
        lx_advance(lexer);
        if (lx_peek(lexer) == '&')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_AND, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_BIT_AND, line, start, lexer->linePos);
        break;

    case '|':
        lx_advance(lexer);
        if (lx_peek(lexer) == '|')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_OR, line, start, lexer->linePos);
        }
        else
            tok = create_op_token(lexer->arena, HXS_OP_BIT_OR, line, start, lexer->linePos);
        break;

    case '^':
        lx_advance(lexer);
        tok = create_op_token(lexer->arena, HXS_OP_BIT_XOR, line, start, lexer->linePos);
        break;

    case '.':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_NULL_OPT, line, start, lexer->linePos);
        }
        else if (lx_peek(lexer) == '.')
        {
            lx_advance(lexer);
            if (lx_peek(lexer) == '=')
            {
                lx_advance(lexer);
                tok = create_op_token(lexer->arena, HXS_OP_SPREAD_ASSIGN, line, start, lexer->linePos);
            }
            else if (lx_peek(lexer) == '.')
            {
                lx_advance(lexer);
                tok = create_op_token(lexer->arena, HXS_OP_SPREAD, line, start, lexer->linePos);
            }
        }
        else
            tok = create_base_token(lexer->arena, DOT_TOKEN, line, start, lexer->linePos);
        break;

    case '?':
        lx_advance(lexer);
        if (lx_peek(lexer) == '?')
        {
            lx_advance(lexer);
            tok = create_op_token(lexer->arena, HXS_OP_NULLISH, line, start, lexer->linePos);
        }
        else
            tok = create_base_token(lexer->arena, QUESTION_TOKEN, line, start, lexer->linePos);
        break;
    case '~':
        lx_advance(lexer);
        tok = create_op_token(lexer->arena, HXS_OP_BIT_NOT, line, start, lexer->linePos);
        break;
    case ';':
        SINGLE(SEMICOLON_TOKEN);
    case ',':
        SINGLE(COMMA_TOKEN);
    case '(':
        SINGLE(LEFTPAREN_TOKEN);
    case ')':
        SINGLE(RIGHTPAREN_TOKEN);
    case '[':
        SINGLE(LEFTBRACKET_TOKEN);
    case ']':
        SINGLE(RIGHTBRACKET_TOKEN);
    case '{':
        SINGLE(LEFTCURLYBRACKET_TOKEN);
    case '}':
        SINGLE(RIGHTCURLYBRACKET_TOKEN);
    case ':':
        SINGLE(COLON_TOKEN);
    case '"':
    case '\'':
        tok = lex_string(lexer);
        break;

    default:
        if (isdigit((unsigned char)c))
            tok = lex_number(lexer);
        else if (isalpha((unsigned char)c) || c == '_')
            tok = lex_identifier(lexer);
        break;
    }

#undef SINGLE
#undef DOUBLE

    if (tok == NULL)
    {
        Hxs_lexer_throw(lexer, "Unexpected Character %c", c);
    }

    if (!advance)
    {
        lexer->pos = saved_pos;
        lexer->line = saved_line;
        lexer->linePos = saved_linePos;
        return tok;
    }

    return tok;
}

void Hxs_lexer_throw(HxsLexer *lexer, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(lexer->error_msg, sizeof(lexer->error_msg), fmt, args);
    va_end(args);

    lexer->has_error = true;
    longjmp(lexer->error_jmp, 1);
}