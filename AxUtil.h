// vim:ts=3:sts=3:sw=3


#ifndef RCSGREP_AXUTIL_H__
#define RCSGREP_AXUTIL_H__

#include <cstddef>

template<class T>
inline bool     AX_ISZERO(T s) { if (NULL == s) return true;  if (0 == *s) return true; return false;}

void            axLTrim(char *pStr);              // ' '
void            axRTrim(char *pStr);              // ' '
void            axTrim(char *pStr);               // ' '

void            axRTrimBlank(char *pStr, int len); // ' ', '\r', '\n', '\t'

bool            axStrMatch(const char *src, const char *dst);
bool            axStrCaseMatch(const char *src, const char *dst);
bool            axPrefixMatch(const char *src, const char *prefix);
bool            axPrefixCaseMatch(const char *src, const char *prefix);
int             axStrcmpRange(const char *left, const char *right);

char            axToLower(char ch);
char            axToUpper(char ch);

bool            axStrncpy(char *dst, const char *src, unsigned int iDstBufSize);

#endif
