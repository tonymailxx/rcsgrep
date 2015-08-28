// vim:ts=3:sts=3:sw=3

#include "AxUtil.h"
#include <strings.h>


void axRTrimBlank(char *pStr, int len)
{
   if (0 >= len) return;

   char ch;
   for (int idx = len - 1; idx; --idx) {
      ch = pStr[idx];

      if        (' '  == ch) {
      } else if ('\t' == ch) {
      } else if ('\r' == ch) {
      } else if ('\n' == ch) {
      } else {
         break;
      }

      pStr[idx] = 0x00;
   }
}

bool axStrMatch(const char *src, const char *dst)
{
   if ((NULL == src) || (NULL == dst)) return false;

   while (*src) {
      if (*src != *dst) return false;
      src++;
      dst++;
   }

   // '\0' compare 
   return ((*src == *dst) ? true : false);
}

bool axStrCaseMatch(const char *src, const char *dst)
{
   if ((NULL == src) || (NULL == dst)) return false;

   return (strcasecmp(src, dst) ? false : true);
}

bool axPrefixMatch(const char *src, const char *prefix)
{
   if ((NULL == src) || (NULL == prefix)) return false;

   while (*prefix) {
      if (*prefix != *src) return false;
      prefix++;
      src++;
   }

   return true;
}

bool axPrefixCaseMatch(const char *src, const char *prefix)
{
   if ((NULL == src) || (NULL == prefix)) return false;

   while (*prefix) {
      if (axToLower(*prefix) != axToLower(*src)) return false;
      prefix++;
      src++;
   }

   return true;
}

int axStrcmpRange(const char *left, const char *right)
{
   if ((NULL == left) || (NULL == right)) return 0;

   for (; *left && *right; left++, right++) {
      if (*left > *right)
         return -1;
      if (*left < *right)
         return 1;
   }

   return 0;
}

char axToLower(char ch)
{
   if (('A' <= ch) && ('Z' >= ch))
      return ch + 32;

   return ch;
}

char axToUpper(char ch)
{
   if (('a' <= ch) && ('z' >= ch))
      return ch - 32;

   return ch;
}

bool axStrncpy(char *dst, const char *src, unsigned int iDstBufSize)
{
   if (2 > iDstBufSize)
      return false;

   iDstBufSize--;
   for (unsigned int i = 0; (i < iDstBufSize) && *src; i++) {
      *dst = *src;
      dst++;
      src++;
   }
   *dst = 0;

   return true;
}
