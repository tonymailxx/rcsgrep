// vim:ts=3:sts=3:sw=3


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AxTypeDef.h"

#ifndef RCSGREP_AXBYTEARRAY_H__
#define RCSGREP_AXBYTEARRAY_H__

class AxByteArray 
{
   public :
      AxByteArray(__uint32_t iSize = 0);
      AxByteArray(AXBYTE_T *pData, __uint32_t iDataSize);
      AxByteArray(__uint32_t iBufSize, AXBYTE_T *pData, __uint32_t iDataSize);
      ~AxByteArray();

      void                  Reserve(__uint32_t iSize);
      void                  Clear(); 

      bool                  IsEmpty() const;

      __uint32_t            GetSize();
      __uint32_t            SetSize(__uint32_t iSize);

      __uint32_t            GetBufferSize();
      AXBYTE_T             *GetBufferPtr();

      __uint32_t            SetByteArray(__uint32_t iBufSize, AXBYTE_T *pData, __uint32_t iDataSize);
      __uint32_t            SetByteArray(AXBYTE_T *pData, __uint32_t iDataSize);

      void                  ConvByte2Hex(AXBYTE_T *pSrc, __uint32_t iSize);
      void                  ConvHex2Byte(char *pSrc, __uint32_t iSize);

      bool                  Base64Encode(AXBYTE_T *pSrc, __uint32_t iSize);
      bool                  Base64Decode(AXBYTE_T *pSrc, __uint32_t iSize);

      AxByteArray          &operator=(const AxByteArray &rBA);
      bool                  operator==(const AxByteArray &rBA);
      bool                  operator!=(const AxByteArray &rBA);
      AxByteArray          *Clone();

   protected :
      AXBYTE_T             *m_pByteArray;
      __uint32_t            m_iBufferSize;
      __uint32_t            m_iDataLength;
};

#endif


