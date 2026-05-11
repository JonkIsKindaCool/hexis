#include "hxs_lexer.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static char lx_peek(HxsLexer *l)               { return l->src[l->pos]; }
static char lx_peek2(HxsLexer *l)              { return l->src[l->pos + 1]; }
static bool lx_at_end(HxsLexer *l)             { return l->src[l->pos] == '\0'; }

static char lx_advance(HxsLexer *l)
{
    char c = l->src[l->pos++];
    if (c == '\n') { l->line++; l->linePos = 1; }
    else           { l->linePos++; }
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
            lx_advance(l); lx_advance(l);                
            while (!lx_at_end(l))
            {
                if (lx_peek(l) == '*' && lx_peek2(l) == '/')
                { lx_advance(l); lx_advance(l); break; }
                lx_advance(l);
            }
        }
        else break;
    }
}

typedef struct { const char *word; HxsTokenKind kind; } Keyword;

static Keyword KEYWORDS[] = {
    { "function", FUNCTION_TOKEN },
    { "new",      NEW_TOKEN      },
    { "if",       IF_TOKEN       },
    { "else",     ELSE_TOKEN     },
    { "var",      VAR_TOKEN      },
    { "final",    FINAL_TOKEN    },
    { "true",     TRUE_TOKEN     },
    { "false",    FALSE_TOKEN    },
    { "while",    WHILE_TOKEN    },
    { "for",      FOR_TOKEN      },
    { "switch",   SWITCH_TOKEN   },
    { "case",     CASE_TOKEN     },
    { "try",      TRY_TOKEN      },
    { "catch",    CATCH_TOKEN    },
    { NULL,       EOF_TOKEN      },
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
    size_t line  = l->line;
    bool is_float = false;

    while (!lx_at_end(l) && isdigit((unsigned char)lx_peek(l)))
        lx_advance(l);

    if (!lx_at_end(l) && lx_peek(l) == '.' && isdigit((unsigned char)lx_peek2(l)))
    {
        is_float = true;
        lx_advance(l);                               // consume '.'
        while (!lx_at_end(l) && isdigit((unsigned char)lx_peek(l)))
            lx_advance(l);
    }

    size_t len = l->linePos - start;
    char *buf = malloc(len + 1);
    memcpy(buf, l->src + (l->pos - len), len);
    buf[len] = '\0';

    HxsToken *tok;
    if (is_float)
        tok = make_float_token(strtod(buf, NULL), line, start, l->linePos);
    else
        tok = make_int_token((int64_t)strtoll(buf, NULL, 10), line, start, l->linePos);

    free(buf);
    return tok;
}

static HxsToken *lex_string(HxsLexer *l)
{
    size_t line  = l->line;
    size_t start = l->linePos;
    char quote   = lx_advance(l);                 
    bool single  = (quote == '\'');

    size_t cap = 64, len = 0;
    char *buf = malloc(cap);

    while (!lx_at_end(l) && lx_peek(l) != quote)
    {
        if (len + 2 >= cap) { cap *= 2; buf = realloc(buf, cap); }

        char c = lx_advance(l);
        if (c == '\\' && !lx_at_end(l))
        {
            char esc = lx_advance(l);
            switch (esc)
            {
            case 'n':  buf[len++] = '\n'; break;
            case 't':  buf[len++] = '\t'; break;
            case 'r':  buf[len++] = '\r'; break;
            case '\\': buf[len++] = '\\'; break;
            default:   buf[len++] = esc;  break;
            }
        }
        else buf[len++] = c;
    }

    if (!lx_at_end(l)) lx_advance(l);         
    buf[len] = '\0';

    HxsToken *tok = make_string_token(buf, single, line, start, l->linePos);
    free(buf);
    return tok;
}

static HxsToken *lex_identifier(HxsLexer *l)
{
    size_t line  = l->line;
    size_t start = l->linePos;

    while (!lx_at_end(l) && (isalnum((unsigned char)lx_peek(l)) || lx_peek(l) == '_'))
        lx_advance(l);

    size_t len = l->linePos - start;
    char *buf = malloc(len + 1);
    memcpy(buf, l->src + (l->pos - len), len);
    buf[len] = '\0';

    HxsTokenKind kind = keyword_or_ident(buf);
    HxsToken *tok;

    if (kind == IDENTIFIER_TOKEN)
        tok = make_identifier_token(buf, line, start, l->linePos);
    else
        tok = create_base_token(kind, line, start, l->linePos);

    free(buf);
    return tok;
}

HxsLexer *make_lexer(const char *src)
{
    HxsLexer *l = malloc(sizeof(HxsLexer));
    l->src     = src;
    l->line    = 1;
    l->pos     = 0;
    l->linePos = 1;
    return l;
}

HxsToken *get_token(HxsLexer *lexer, bool advance)
{
    int  saved_pos     = lexer->pos;
    int  saved_line    = lexer->line;
    int  saved_linePos = lexer->linePos;

    lx_skip_whitespace(lexer);

    if (lx_at_end(lexer))
    {
        HxsToken *t = create_base_token(EOF_TOKEN, lexer->line, lexer->linePos, lexer->linePos);
        if (!advance) { lexer->pos = saved_pos; lexer->line = saved_line; lexer->linePos = saved_linePos; }
        return t;
    }

    size_t line  = lexer->line;
    size_t start = lexer->linePos;
    char   c     = lx_peek(lexer);

    HxsToken *tok = NULL;

#define SINGLE(kind) \
    lx_advance(lexer); tok = create_base_token(kind, line, start, lexer->linePos); break

#define DOUBLE(next, kind_double, kind_single)                            \
    lx_advance(lexer);                                                    \
    if (lx_peek(lexer) == (next)) { lx_advance(lexer);                   \
        tok = create_base_token(kind_double, line, start, lexer->linePos); } \
    else tok = create_base_token(kind_single, line, start, lexer->linePos); \
    break

    switch (c)
    {
    case '+':
        lx_advance(lexer);
        if      (lx_peek(lexer) == '=') { lx_advance(lexer); tok = create_base_token(ADD_ASSIGN_TOKEN,   line, start, lexer->linePos); }
        else    tok = create_base_token(ADD_TOKEN, line, start, lexer->linePos);
        break;

    case '-':
        lx_advance(lexer);
        if      (lx_peek(lexer) == '=') { lx_advance(lexer); tok = create_base_token(MINUS_ASSIGN_TOKEN, line, start, lexer->linePos); }
        else    tok = create_base_token(MINUS_TOKEN, line, start, lexer->linePos);
        break;

    case '*':
        lx_advance(lexer);
        if      (lx_peek(lexer) == '=') { lx_advance(lexer); tok = create_base_token(MULTIPLY_ASSIGN_TOKEN, line, start, lexer->linePos); }
        else    tok = create_base_token(MULTIPLY_TOKEN, line, start, lexer->linePos);
        break;

    case '/':
        lx_advance(lexer);
        if      (lx_peek(lexer) == '=') { lx_advance(lexer); tok = create_base_token(DIVIDE_ASSIGN_TOKEN, line, start, lexer->linePos); }
        else    tok = create_base_token(DIVIDE_TOKEN, line, start, lexer->linePos);
        break;

    case '%':
        lx_advance(lexer);
        if      (lx_peek(lexer) == '=') { lx_advance(lexer); tok = create_base_token(MODULO_ASSIGN_TOKEN, line, start, lexer->linePos); }
        else    tok = create_base_token(MODULO_TOKEN, line, start, lexer->linePos);
        break;

    case '=': DOUBLE('=', EQUAL_TOKEN,         ASSIGN_TOKEN);
    case '!': DOUBLE('=', NOT_EQUAL_TOKEN,     LOGICAL_NOT_TOKEN);
    case '<': DOUBLE('=', LESS_EQUAL_TOKEN,    LESS_TOKEN);
    case '>': DOUBLE('=', GREATER_EQUAL_TOKEN, GREATER_TOKEN);

    case '&':
        lx_advance(lexer);
        if (lx_peek(lexer) == '&') { lx_advance(lexer); tok = create_base_token(LOGICAL_AND_TOKEN, line, start, lexer->linePos); }
        else tok = create_base_token(AND_BIT_TOKEN, line, start, lexer->linePos);
        break;

    case '|':
        lx_advance(lexer);
        if (lx_peek(lexer) == '|') { lx_advance(lexer); tok = create_base_token(LOGICAL_OR_TOKEN, line, start, lexer->linePos); }
        else tok = create_base_token(OR_BIT_TOKEN, line, start, lexer->linePos);
        break;

    case '^': SINGLE(XOR_BIT_TOKEN);

    case '.':
        lx_advance(lexer);
        if (lx_peek(lexer) == '=')
        {
            lx_advance(lexer);
            tok = create_base_token(NULL_OPTIONAL_TOKEN, line, start, lexer->linePos);
        }
        else if (lx_peek(lexer) == '.')
        {
            lx_advance(lexer);
            if (lx_peek(lexer) == '=') { lx_advance(lexer); tok = create_base_token(SPREAD_EQUAL_TOKEN, line, start, lexer->linePos); }
            else if (lx_peek(lexer) == '.') { lx_advance(lexer); tok = create_base_token(SPREAD_TOKEN, line, start, lexer->linePos); }
        }
        break;

    case '?':
        lx_advance(lexer);
        if (lx_peek(lexer) == '?') { lx_advance(lexer); tok = create_base_token(NULLISH_TOKEN, line, start, lexer->linePos); }
        break;

    case '"': case '\'':
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

    if (!advance)
    {
        freeToken(tok);                            
        lexer->pos     = saved_pos;
        lexer->line    = saved_line;
        lexer->linePos = saved_linePos;
        return get_token(lexer, true);
    }

    return tok;
}

void free_lexer(HxsLexer *lexer)
{
    free(lexer);
}