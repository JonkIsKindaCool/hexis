#include <stdio.h>
#include <stdlib.h>
#include "lexer/hxs_lexer.h"
#include "parser/hxs_parser.h"
#include "core/hxs_utils.h"

char *read_file(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f)
    {
        perror("fopen");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *source = (char *)malloc(size + 1);
    if (!source)
    {
        perror("malloc");
        exit(1);
    }

    fread(source, 1, size, f);
    source[size] = '\0';

    fclose(f);
    return source;
}

int main(int argc, char const *argv[])
{
    const char *source = read_file("../test.hxs");
    HxsArena *arena = Hxs_Arena_create(1024 * 1024);

    HxsLexer *lex = Hxs_make_lexer(arena, source);

    while (true)
    {
        HxsToken* token = Hxs_get_token(lex, true);
        char* info = Hxs_token_to_string(token);

        printf("%s\n", info);
        free(info);

        if (token->kind == EOF_TOKEN){
            break;
        }
    }
    

    HxsLexer *lexer = Hxs_make_lexer(arena, source);
    HxsParser *parser = Hxs_Parser_new(arena, lexer, "../test.hxs");
    HxsExpr *program = Hxs_Parser_parse_program(parser);
    Hxs_printExpr(program);
    Hxs_Arena_destroy(arena);
    free((void *)source);
    return 0;
}