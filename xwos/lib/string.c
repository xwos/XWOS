/**
 * @file
 * @brief XWOS通用库：string
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/string.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief Fill a region of memory with the given value
 * @param s: (I) Pointer to the start of the area.
 * @param c: (I) The byte to fill the area with
 * @param count: (I) The size of the area.
 * @retval pointer: the start of the area
 */
__xwlib_code
void * memset(void * s, int c, xwsz_t count)
{
        char * xs = s;

        while (count--) {
                *xs++ = (char)c;
        }
        return s;
}

/**
 * @brief Compare two areas of memory
 * @param cs: (I) One area of memory
 * @param ct: (I) Another area of memory
 * @param count: (I) The size of the area.
 * @retval 0: same
 * @retval >0: cs > ct
 * @retval <0: cs < ct
 */
__xwlib_code
int memcmp(const void * cs, const void * ct, xwsz_t count)
{
        const unsigned char * su1, * su2;
        int res = 0;

        for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--) {
                if ((res = *su1 - *su2) != 0) {
                        break;
                }
        }
        return res;
}

/**
 * @brief Copy one area of memory to another
 * @param dest: (I) Where to copy to
 * @param src: (I) Where to copy from
 * @param count: (I) The size of the area.
 * @retval pointer: dest address
 */
__xwlib_code
void * memcpy(void * restrict dest, const void * restrict src, xwsz_t count)
{
        char * tmp = dest;
        const char * s = src;

        while (count--) {
                *tmp = *s;
                tmp++;
                s++;
        }
        return dest;
}

/**
 * @brief Copy one area of memory to another
 * @param dest: (I) Where to copy to
 * @param src: (I) Where to copy from
 * @param count: (I) The size of the area.
 * @retval pointer: dest address
 */
__xwlib_code
void * memmove(void * dest, const void * src, xwsz_t count)
{
        char * tmp;
        const char * s;

        if (dest <= src) {
                tmp = dest;
                s = src;
                while (count--) {
                        *tmp++ = *s++;
                }
        } else {
                tmp = dest;
                tmp += count;
                s = src;
                s += count;
                while (count--) {
                        *--tmp = *--s;
                }
        }
        return dest;
}

/**
 * @brief Find a character in an area of memory.
 * @param s: (I) The memory area
 * @param c: (I) The byte to search for
 * @param n: (I) The size of the area.
 * @retval pointer: the address of the first occurrence of <b><em>c</em></b>, or
 *         NULL if <b><em>c</em></b> is not found
 */
__xwlib_code
void * memchr(const void * s, int c, xwsz_t n)
{
        const unsigned char * p = s;
        while (n-- != 0) {
                if ((unsigned char)c == *p++) {
                        return (void *)(p - 1);
                }
        }
        return NULL;
}

/**
 * @brief Find the length of a string
 * @param s: (I) The string to be sized
 * @retval xwsz_t: length of the string, excluding the terminating null byte ('\0')
 */
__xwlib_code
xwsz_t strlen(const char * s)
{
        const char * sc;

        for (sc = s; *sc != '\0'; ++sc) {
        }
        return (xwsz_t)(sc - s);
}

/**
 * @brief Find the length of a length-limited string
 * @param s: (I) The string to be sized
 * @param count: (I) The maximum number of bytes to search
 * @retval xwsz_t: length of the string, excluding the terminating null byte ('\0')
 */
__xwlib_code
xwsz_t strnlen(const char * s, xwsz_t count)
{
        const char * sc;

        for (sc = s; count-- && *sc != '\0'; ++sc) {
        }
        return (xwsz_t)(sc - s);
}

/**
 * @brief Find the first occurrence of a set of characters
 * @param cs: (I) The string to be searched
 * @param ct: (I) The characters to search for
 * @retval pointer: the byte in <b><em>cs</em></b> that matches one of the bytes
 *                  in <b><em>ct</em></b>, or NULL if no such byte is found
 */
__xwlib_code
char * strpbrk(const char * cs, const char * ct)
{
        const char * sc1, * sc2;

        for (sc1 = cs; *sc1 != '\0'; ++sc1) {
                for (sc2 = ct; *sc2 != '\0'; ++sc2) {
                        if (*sc1 == *sc2) {
                                return (char *)sc1;
                        }
                }
        }
        return NULL;
}

/**
 * @brief Split a string into tokens
 * @param s: (I) The string to be searched
 *           (O)  updated to point past the token
 * @param ct: (I) The token characters to search for
 * @retval pointer: the beginning of located token or NULL
 * @note
 * - strsep() updates <b><em>s</em></b> to point after the token,
 *   ready for the next call. It returns empty tokens, too,
 *   behaving exactly like the libc function of that name.
 */
__xwlib_code
char * strsep(char ** s, const char * ct)
{
        char * sbegin = *s;
        char * end;

        if (sbegin == NULL) {
                return NULL;
        }

        end = strpbrk(sbegin, ct);
        if (end) {
                *end++ = '\0';
        }
        *s = end;
        return sbegin;
}

/**
 * @brief Find the first substring in a %NUL terminated string
 * @param s1: (I) The string to be searched
 * @param s2: (I) The string to search for
 * @retval pointer: the beginning of the located substring
 */
__xwlib_code
char * strstr(const char * s1, const char * s2)
{
        xwsz_t l1, l2;

        l2 = strlen(s2);
        if (!l2) {
                return (char *)s1;
        }
        l1 = strlen(s1);
        while (l1 >= l2) {
                l1--;
                if (!memcmp(s1, s2, l2)) {
                        return (char *)s1;
                }
                s1++;
        }
        return NULL;
}

/**
 * @brief Find the first substring in a length-limited string
 * @param s1: (I) The string to be searched
 * @param s2: (I) The string to search for
 * @param len: (I) the maximum number of characters to search
 * @retval pointer: the beginning of the located substring
 */
__xwlib_code
char * strnstr(const char * s1, const char * s2, xwsz_t len)
{
        xwsz_t l2;

        l2 = strlen(s2);
        if (!l2) {
                return (char *)s1;
        }
        while (len >= l2) {
                len--;
                if (!memcmp(s1, s2, l2)) {
                        return (char *)s1;
                }
                s1++;
        }
        return NULL;
}

/**
 * @brief Compare two strings (128-base compare)
 * @param cs: (I) One string
 * @param ct: (I) Another string
 * @retval -1: cs < ct
 * @retval 0: cs == ct
 * @retval 1: cs > ct
 */
__xwlib_code
int strcmp(const char * cs, const char * ct)
{
        unsigned char c1, c2;

        while (1) {
                c1 = *cs++;
                c2 = *ct++;
                if (c1 != c2) {
                        return c1 < c2 ? -1 : 1;
                }
                if (!c1) {
                        break;
                }
        }
        return 0;
}

/**
 * @brief Compare two length-limited strings
 * @param cs: (I) One string
 * @param ct: (I) Another string
 * @param count: (I) The maximum number of bytes to compare
 * @retval -1: cs < ct
 * @retval 0: cs == ct
 * @retval 1: cs > ct
 */
__xwlib_code
int strncmp(const char * cs, const char * ct, xwsz_t count)
{
        unsigned char c1, c2;

        while (count) {
                c1 = *cs++;
                c2 = *ct++;
                if (c1 != c2) {
                        return c1 < c2 ? -1 : 1;
                }
                if (!c1) {
                        break;
                }
                count--;
        }
        return 0;
}

/**
 * @brief Find the first occurrence of a character in a string
 * @param s: (I) The string to be searched
 * @param c: (I) The character to search for
 * @retval pointer: matched character or null
 */
__xwlib_code
char * strchr(const char * s, int c)
{
        for (; *s != (char)c; ++s) {
                if (*s == '\0') {
                        return NULL;
                }
        }
        return (char *)s;
}

/**
 * @brief Find and return a character in a string, or end of string
 * @param s: (I) The string to be searched
 * @param c: (I) The character to search for
 * @retval pointer: matched character or the terminating null byte '\0'
 * @note
 * - Returns pointer to first occurrence of <b><em>c</em></b> in <b><em>s</em></b>.
 *   If <b><em>c</em></b> is not found, then return a pointer to the null byte
 *   at the string end.
 */
__xwlib_code
char * strchrnul(const char * s, int c)
{
        while (*s && *s != (char)c) {
                s++;
        }
        return (char *)s;
}

/**
 * @brief Find the last occurrence of a character in a string
 * @param s: (I) The string to be searched
 * @param c: (I) The character to search for
 * @retval pointer: matched character or null
 */
__xwlib_code
char * strrchr(const char * s, int c)
{
        const char * p = s + strlen(s);
        do {
                if (*p == (char)c) {
                        return (char *)p;
                }
        } while (--p >= s);
        return NULL;
}

/**
 * @brief Find a character in a length limited string
 * @param s: (I) The string to be searched
 * @param count: (I) The number of characters to be searched
 * @param c: (I) The character to search for
 * @retval pointer: matched character or null
 */
__xwlib_code
char * strnchr(const char * s, xwsz_t count, int c)
{
        for (; count-- && *s != '\0'; ++s) {
                if (*s == (char)c) {
                        return (char *)s;
                }
        }
        return NULL;
}
