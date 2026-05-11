#include "hxs_lexer.h"

int main(int argc, char const *argv[])
{
    const char *source =
        "function main() {\n"
        "    print(\"hola\");\n"
        "}\n";
        
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

    return 0;
}
