#include <libk/string.h>

#include <limits.h>

char *strcpy(char *strDest, char *strSrc)
{
    char *temp = strDest;
    while ((*strDest++ = *strSrc++))
        ; // or while((*strDest++=*strSrc++) != '\0');
    return temp;
}
void int_to_ascii(int n, char str[])
{
    int i, sign;
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do
    {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        str[i++] = '-';
    str[i] = '\0';
    reverse(str);
}
void hex_to_ascii(int n, char str[])
{
    append(str, '0');
    append(str, 'x');

    char zeros = 0;
    int32_t tmp;
    for (int i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0)
            continue;
        zeros -= 1;
        if (tmp >= 0xA)
            append(str, tmp - 0xA + 'a');
        else
            append(str, tmp + '0');
    }
    tmp = n & 0xF;
    if (tmp >= 0xA)
        append(str, tmp - 0xA + 'a');
    else
        append(str, tmp + '0');
}
void reverse(char s[])
{
    for (int i = 0, j = (int)(strlen(s) - 1); i < j; i++, j--)
    {
        int c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void backspace(char s[])
{
    int len = strlen(s);
    s[len - 1] = '\0';
}
void append(char s[], char n)
{
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}
int strcmp(char s1[], char s2[])
{
    for (; *s1 == *s2 && *s1; s1++, s2++)
        ;
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strdup(char *s)
{
    size_t len = strlen(s) + 1;
    char *d = malloc(len);
    if (d == NULL)
        return NULL;
    memset(d, 0, len);
    strcpy(d, s);
    return d;
}

#define ONES ((size_t)-1 / UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX / 2 + 1))
#define HASZERO(x) (((x)-ONES) & ~(x)&HIGHS)
#define BITOP(a, b, op) ((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1 << ((size_t)(b) % (8 * sizeof *(a))))

size_t strspn(const char *s, const char *c)
{
    const char *a = s;
    size_t byteset[32 / sizeof(size_t)] = {0};
    if (!c[0])
        return 0;
    if (!c[1])
    {
        for (; *s == *c; s++)
            ;
        return s - a;
    }

    //a[(size_t)(b) / (8 * sizeof*(a))] op (size_t)1 << ((size_t)(b) % (8 * sizeof *(a)))

    for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++)
        ;
    for (; *s && BITOP(byteset, *(unsigned char *)s, &); s++)
        ;
    return s - a;
}

char *strpbrk(const char *s, const char *b)
{
    s += strcspn(s, b);
    return *s ? (char *)s : 0;
}

size_t strcspn(const char *s, const char *c)
{
    const char *a = s;
    if (c[0] == c[1])
    {
        size_t byteset[32 / sizeof(size_t)] = {0};
        for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++)
            ;
        for (; *s && !BITOP(byteset, *(unsigned char *)s, &); s++)
            ;
        return s - a;
    }
    return strchrnul(s, *c) - a;
}

char *strchrnul(const char *s, int c)
{
    size_t *w;
    size_t k;
    c = (unsigned char)c;
    if (!c)
        return (char *)s + strlen(s);

    for (; (uintptr_t)s % sizeof(size_t); s++)
    {
        if (!*s || *(unsigned char *)s == c)
            return (char *)s;
    }

    k = ((size_t)-1 / UINT8_MAX) * c;
    for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w ^ k); w++)
        ;
    for (s = (void *)w; *s && *(unsigned char *)s != c; s++)
        ;
    return (char *)s;
}

char *strchr(const char *s, int c)
{
    char *r = strchrnul(s, c);
    return *(unsigned char *)r == (unsigned char)c ? r : 0;
}

char *strtok_r(char *str, const char *delim, char **saveptr)
{
    char *token;
    if (str == NULL)
        str = *saveptr;
    str += strspn(str, delim);
    if (*str == '\0')
    {
        *saveptr = str;
        return NULL;
    }
    token = str;
    str = strpbrk(token, delim);
    if (str == NULL)
        *saveptr = (char *)(size_t)strchr(token, '\0'); //(char*)lfind(token, '\0');
    else
    {
        *str = '\0';
        *saveptr = str + 1;
    }
    return token;
}