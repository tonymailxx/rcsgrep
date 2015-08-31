// vim:ts=3:sts=3:sw=3

#include "Mmi.h"

#include <iostream>
#include <errno.h>
#include "Logger.h"
#include "AxUtil.h"


int Mmi::Choose()
{
   const char TAB[]        = "   ";

   char BUF[1024] = {0x00,};

   GetInput("Choose", BUF, sizeof(BUF));

   int len = strlen(BUF);
   if (0 == len) {
      return 0;
   }

   for (int idx = 0; idx < len; ++idx) {
      if (!isdigit(BUF[idx] & 0xFF)) {
         STDOUT(" ");
         STDOUT(RED(TAB << "Invalid Input : " << BUF));
         STDOUT(" ");
         return 0;
      }
   }

   int value = atoi(BUF);

   return ((0 == value) ? -1 : value);
}

int Mmi::GetInput(const char *head, char *pBuf, const int nSize)
{
   std::cout << head << "> ";
   fflush(stdout);

   return Readline(pBuf, nSize);
}

int Mmi::Readline(char *pBuf, const int nSize)
{
   memset(pBuf, 0x00, nSize);

   char *result = fgets(pBuf, nSize-1, stdin);
   if (result != pBuf) {
      ELOG(RED("fgets errno:" << errno));
      return -1;
   }

   axTrim(pBuf);

   for (int idx = 0; idx < nSize; ++idx) {
      if (('\r' == pBuf[idx]) || ('\n' == pBuf[idx])) {
         pBuf[idx] = 0x00;
         break;
      }
   }

   return strlen(pBuf);
}
