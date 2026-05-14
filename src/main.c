#include "hxs_lexer.h"
#include "hxs_parser.h"

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
    const char *source = (const char *)read_file("../test.hxs");

    HxsLexer *lexer = make_lexer(source);
    while (true)
    {
        HxsToken *token = get_token(lexer, true);

        if (token->kind == EOF_TOKEN)
        {
            freeToken(token);
            break;
        }

        char *info = token_to_string(token);

        printf("%s\n", info);

        free(info);
        freeToken(token);
    }

    free_lexer(lexer);

    HxsParser *parser = init_parser(source);
    HxsStmt *stmt = HxsParser_parse(parser);

    StringBuffer* buf = print_stmt(stmt, 0);

    printf("%s\n", buf->buf);

    freeBuffer(buf);

    free_stmt(stmt);
    freeParser(parser);

    free((void *)source);

    return 0;
}
