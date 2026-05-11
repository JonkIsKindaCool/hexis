#include <stdlib.h>
#include <string.h>

char *substring(char *src, int start, int end)
{
    if (src == NULL || start < 0 || end < start)
        return NULL;                

    int src_len = strlen(src);
    
    if (end >= src_len)
        end = src_len - 1;

    int len = end - start + 1;        
    if (len <= 0)
        return strdup("");           

    char *dst = malloc(len + 1);       
    if (dst == NULL)
        return NULL;

    strncpy(dst, src + start, len);
    dst[len] = '\0';

    return dst;
}