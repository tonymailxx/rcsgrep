// vim:ts=3:sts=3:sw=3


#include "AxByteArray.h"

AxByteArray::AxByteArray(__uint32_t iSize)
{
   m_pByteArray    = NULL;
   m_iBufferSize   = 0;
   m_iDataLength   = 0;

   if (iSize) 
      Reserve(iSize);
}

AxByteArray::AxByteArray(AXBYTE_T *pData, __uint32_t iDataSize)
{
   m_pByteArray    = NULL;
   m_iBufferSize   = 0;
   m_iDataLength   = 0;

   SetByteArray(pData, iDataSize);
}

AxByteArray::AxByteArray(__uint32_t iBufSize, AXBYTE_T *pData, __uint32_t iDataSize)
{
   m_pByteArray    = NULL;
   m_iBufferSize   = 0;
   m_iDataLength   = 0;

   SetByteArray(iBufSize, pData, iDataSize);
}

AxByteArray::~AxByteArray()
{
   Clear();
}

void AxByteArray::Clear() 
{
   if (m_pByteArray) {
      free(m_pByteArray);
      m_pByteArray = NULL;
   }
   m_iBufferSize   = 0;
   m_iDataLength   = 0;
}

__uint32_t AxByteArray::GetSize()       
{ 
   return m_iDataLength; 
}

__uint32_t AxByteArray::GetBufferSize() 
{ 
   return m_iBufferSize; 
}

AXBYTE_T *AxByteArray::GetBufferPtr()  
{ 
   return m_pByteArray; 
}

// 기존 데이터가 있다면 데이터를 유지한체 버퍼 크기를 변화시킨다.
void AxByteArray::Reserve(__uint32_t iSize) 
{
   if (0 == iSize) {
      Clear();
      return ;
   } 

   if (m_iBufferSize == iSize) {
      return ;
   }

   if (m_iBufferSize > iSize) {
      if (m_iDataLength > iSize) {
         m_iDataLength = iSize;
         m_pByteArray[m_iDataLength] = 0;
      }
      return ;
   }

   // m_pByteArray에 대한 초기화 롤은 AxByteArray를 상속해서 사용하는 class로 돌린다.
   if (0 == m_iBufferSize) {
      m_pByteArray = (AXBYTE_T*) malloc(iSize);
   } else {
      m_pByteArray = (AXBYTE_T*) realloc((void *)m_pByteArray, iSize);
   }
   m_iBufferSize = iSize;

   if (NULL == m_pByteArray) {
      abort();
   }
}

bool AxByteArray::IsEmpty() const
{
   return (0 == m_iDataLength) ? true : false;
}

__uint32_t AxByteArray::SetSize(__uint32_t iSize)
{
   if (iSize > m_iBufferSize)
      iSize = m_iBufferSize;

   m_iDataLength = iSize;

   if (m_iDataLength != m_iBufferSize) {
      m_pByteArray[m_iDataLength] = 0;
   }

   return m_iDataLength;
}

__uint32_t AxByteArray::SetByteArray(__uint32_t iBufSize, AXBYTE_T *pData, __uint32_t iDataSize)
{
   if (0 == iBufSize) {
      Clear();
      return 0;
   }

   if (iDataSize) {
      Reserve(iBufSize);
   } else {
      Clear();
      return 0;
   }

   if (iBufSize <= iDataSize) {
      iDataSize = iBufSize - 1;
   }

   m_iDataLength = iDataSize;
   memcpy(m_pByteArray, pData, m_iDataLength);

   // 문자열 처리등 오류 방지를 위해 마지막 데이터 뒤 1byte를 초기화 한다.
   m_pByteArray[m_iDataLength] = 0;

   return m_iDataLength;
}

__uint32_t AxByteArray::SetByteArray(AXBYTE_T *pData, __uint32_t iDataSize)
{
   return SetByteArray(iDataSize + 1, pData, iDataSize);
}

void AxByteArray::ConvByte2Hex(AXBYTE_T *pSrc, __uint32_t iSize)
{
   static AXBYTE_T HEXS[] = "0123456789ABCDEF";

   Reserve((iSize * 2) + 1);

   __uint8_t *pDst = m_pByteArray;

   for (__uint32_t i = 0; i < iSize; ++i) {
      *pDst = HEXS[ (pSrc[i] >> 4) & 0x0f ];
      ++pDst;
      *pDst = HEXS[ pSrc[i] & 0x0f ];
      ++pDst;
   }

   *pDst = 0;
   m_iDataLength = iSize * 2;
}

void AxByteArray::ConvHex2Byte(char *pSrc, __uint32_t iSize)
{
   AXBYTE_T byte;
   iSize >>= 1;
   Reserve(iSize + 1);

   __uint32_t i = 0;
   for (i = 0; i < iSize; ++i) {
      if (('0' <= *pSrc) && ('9' >= *pSrc)) {
         byte = (*pSrc - '0') << 4;
      } else if (('a' <= *pSrc) && ('f' >= *pSrc)) {
         byte = (*pSrc - 'a' + 0xa) << 4;
      } else if (('A' <= *pSrc) && ('F' >= *pSrc)) {
         byte = (*pSrc - 'A' + 0xa) << 4;
      } else {
         break;
      }
      ++pSrc;

      if (('0' <= *pSrc) && ('9' >= *pSrc)) {
         m_pByteArray[i] = byte | (*pSrc - '0');
      } else if (('a' <= *pSrc) && ('f' >= *pSrc)) {
         m_pByteArray[i] = byte | (*pSrc - 'a' + 0xa);
      } else if (('A' <= *pSrc) && ('F' >= *pSrc)) {
         m_pByteArray[i] = byte | (*pSrc - 'A' + 0xa);
      } else {
         break;
      }
      ++pSrc;
   }

   m_pByteArray[i] = 0;
   m_iDataLength = i;
}

AxByteArray &AxByteArray::operator=(const AxByteArray &rBA)
{
   Clear();

   if (rBA.m_iDataLength) {
      Reserve(rBA.m_iBufferSize);

      m_iDataLength = rBA.m_iDataLength;
      m_iBufferSize = rBA.m_iBufferSize;
      memcpy(m_pByteArray, rBA.m_pByteArray, rBA.m_iDataLength);

      // 문자열 처리등 오류 방지를 위해 마지막 데이터 뒤 1byte를 초기화 한다.
      if (m_iDataLength < m_iBufferSize)
         m_pByteArray[m_iDataLength] = 0;    
   }

   return *this;
}

bool AxByteArray::operator==(const AxByteArray &rBA)
{
   // 데이터가 없다면 false처리한다.
   if ((0 == m_iDataLength) || (m_iDataLength != rBA.m_iDataLength)) return false;

   AXBYTE_T *pSrc = m_pByteArray;
   AXBYTE_T *pDst = rBA.m_pByteArray;

   for (__uint32_t i = m_iDataLength / sizeof(__uint32_t); i; i--) {
      if ((*((__uint32_t*)pSrc)) == (*((__uint32_t*)pDst))) {
         pSrc += sizeof(__uint32_t);
         pDst += sizeof(__uint32_t);
      } else {
         return false;
      }
   }

   for (__uint32_t i = m_iDataLength % sizeof(__uint32_t); i; i--) {
      if (*pSrc == *pDst) {
         pSrc++;
         pDst++;
      } else {
         return false;
      }
   }

   return true;
   // memcmp source: EXTERN_C int __cdecl memcmp(const void *Ptr1, const void *Ptr2, size_t Count)
   // memcmp source: {
   // memcmp source:     INT v = 0;
   // memcmp source:     BYTE *p1 = (BYTE *)Ptr1;
   // memcmp source:     BYTE *p2 = (BYTE *)Ptr2;
   // memcmp source: 
   // memcmp source:     while(Count-- > 0 && v == 0) {
   // memcmp source:         v = *(p1++) - *(p2++);
   // memcmp source:     }
   // memcmp source: 
   // memcmp source:     return v;
   // memcmp source: }
}

bool AxByteArray::operator!=(const AxByteArray &rBA)
{
   if (0 == m_iDataLength)                     return false;
   if (m_iDataLength != rBA.m_iDataLength)     return true;

   AXBYTE_T *pSrc = m_pByteArray;
   AXBYTE_T *pDst = rBA.m_pByteArray;

   for (__uint32_t i = m_iDataLength / sizeof(__uint32_t); i; i--) {
      if ((*((__uint32_t*)pSrc)) == (*((__uint32_t*)pDst))) {
         pSrc += sizeof(__uint32_t);
         pDst += sizeof(__uint32_t);
      } else {
         return true;
      }
   }

   for (__uint32_t i = m_iDataLength % sizeof(__uint32_t); i; i--) {
      if (*pSrc == *pDst) {
         pSrc++;
         pDst++;
      } else {
         return true;
      }
   }

   return false;
}

AxByteArray *AxByteArray::Clone()
{
   return new AxByteArray(m_pByteArray, m_iDataLength);
}

// -----------------------------------------------------------------------------
// RFC3548 Base64
static const char _Base64[] = {
   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
   'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
   'w', 'x', 'y', 'z', '0', '1', '2', '3',
   '4', '5', '6', '7', '8', '9', '+', '/'
};

static const char _DecodeBase64[256] = {
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
   52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
   -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
   15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
   -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
   41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

bool AxByteArray::Base64Encode(AXBYTE_T *pSrc, __uint32_t iSize)
{
   if (!iSize) {
      return false;
   }

   Clear();
   Reserve((4 * ((iSize / 3) + 1)) + 1);

   char         *pOutput = (char *) m_pByteArray;
   unsigned char c1, c2, c3;
   __uint32_t    iLen = 0;

   while (3 <= iSize) {
      c1 = *pSrc++;
      c2 = *pSrc++;
      c3 = *pSrc++;
      iSize -= 3;

      *pOutput++ = _Base64[c1 >> 2];
      *pOutput++ = _Base64[(((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4))];
      *pOutput++ = _Base64[(((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6))];
      *pOutput++ = _Base64[(c3 & 0x3F)];
      iLen += 4;
   }

   if (2 == iSize) {
      c1 = *pSrc++;
      c2 = *pSrc++;
      c3 = 0x00;

      *pOutput++ = _Base64[c1 >> 2];
      *pOutput++ = _Base64[(((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4))];
      *pOutput++ = _Base64[(((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6))];
      *pOutput++ = '=';
      iLen += 4;

   } else if (1 == iSize) {
      c1 = *pSrc++;
      c2 = 0x00;
      c3 = 0x00;

      *pOutput++ = _Base64[c1 >> 2];
      *pOutput++ = _Base64[(((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4))];
      *pOutput++ = '=';
      *pOutput++ = '=';
      iLen += 4;
   }

   *pOutput      = 0x00;
   m_iDataLength = iLen;

   return true;
}

bool AxByteArray::Base64Decode(AXBYTE_T *pSrc, __uint32_t iSize)
{
   if (!iSize) {
      return false;
   }

   Clear();
   Reserve(iSize + 1);

   bool          bIsFinal = false;
   char         *pOutput = (char *) m_pByteArray;
   unsigned char c1, c2, c3, c4;
   __uint32_t    iLen = 0;

   while (4 <= iSize) {
      c1 = *pSrc++;
      c2 = *pSrc++;
      c3 = *pSrc++;
      c4 = *pSrc++;
      iSize -= 4;

      if ('=' == c1) {
         bIsFinal = true;
         break;
      }

      if ('=' == c2) {
         *pOutput = 0x00;
         m_iDataLength = iLen;
         return false;
      }

      c1 = _DecodeBase64[c1];
      c2 = _DecodeBase64[c2];
      *pOutput++ = ((c1 << 2) | ((c2 & 0x30) >> 4));          ++iLen;

      if ('=' == c3) {
         bIsFinal = true;
         break;
      }

      c3 = _DecodeBase64[c3];
      *pOutput++ = (((c2 & 0XF) << 4) | ((c3 & 0x3C) >> 2));  ++iLen;

      if ('=' == c4) {
         bIsFinal = true;
         break;
      }

      c4 = _DecodeBase64[c4];
      *pOutput++ = (((c3 & 0x03) << 6) | c4);                 ++iLen;
   }

   if ((!bIsFinal) && iSize) {
      if ('=' != *pSrc) {
         *pOutput = 0x00;
         m_iDataLength = iLen;
         return false;
      }
   }

   *pOutput      = 0x00;
   m_iDataLength = iLen;

   return true;
}
