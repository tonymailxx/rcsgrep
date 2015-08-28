// vim:ts=3:sts=3:sw=3


#include <ctype.h>
#include "AxString.h"
#include "AxUtil.h"

template <class T> char *p_unsigned2string(T value, T base, char *str)
{
   if (value >= base)
      str = p_unsigned2string<T>(value/base, base, str);
   value %= base;
   if (value < 10)
      *str = (char)(value + '0');
   else
      *str = (char)(value + 'A'-10);
   return str+1;
}

template <class T> char *p_signed2string(T value, T base, char *str)
{
   if (value >= 0)
      return p_unsigned2string<T>(value, base, str);

   *str = '-';
   return p_unsigned2string<T>(-value, base, str+1);
}

AxString::AxString(unsigned int iSize) : AxByteArray( iSize )
{
}

AxString::AxString(const AxString &rStr) : AxByteArray(0)
{
   (*this) = rStr;
}

AxString::AxString(const std::string &str) : AxByteArray(0)
{
   (*this) = str.c_str();
}

AxString::AxString(const char *cstr) : AxByteArray(0)
{
   (*this) = cstr;
}

AxString::AxString(const char *cstr, unsigned int iSize)
{
   if (AX_ISZERO(cstr)) 
      return;

   if (0 == iSize)
      return;

   Reserve(iSize+1);
   if (0 == m_iBufferSize) 
      return;

   char *pDst = (char*) m_pByteArray;
   char *pSrc = (char*) cstr;
   unsigned int i = 0;
   for (i = 0; (i < iSize) && *pSrc; i++) {
      *pDst = *pSrc;
      pDst++;
      pSrc++;
   }
   *pDst = 0;
   m_iDataLength = i;
}

bool AxString::operator==(AxString &rStr)
{
   return IsEqual(rStr.c_str());
}

bool AxString::operator==(const std::string &rStr)
{
   if (rStr.empty())
      return false;

   return IsEqual(rStr.c_str());
}

bool AxString::operator==(const char *pStr) 
{
   return IsEqual(pStr);
}

bool AxString::operator!=(AxString &rStr)
{
   return !IsEqual(rStr.c_str());
}

bool AxString::operator!=(const std::string &rStr)
{
   if (rStr.empty())
      return true;

   return !IsEqual(rStr.c_str());
}

bool AxString::operator!=(const char *pStr) 
{
   return !IsEqual(pStr);
}

AxString &AxString::operator=(const std::string &rStr)
{
   if (rStr.empty()) {
      Clear();
      return *this;
   }

   size_t iLen = rStr.size();
   if (iLen >= m_iBufferSize) {
      Clear();
      Reserve(iLen + 1);
   }

   if (m_iBufferSize) {
      axStrncpy((char*)m_pByteArray, rStr.c_str(), m_iBufferSize);
      m_iDataLength = iLen;
   }

   return *this;
}

AxString &AxString::operator=(const AxString &rStr)
{
   if (m_iBufferSize) {
      if (m_iBufferSize > rStr.m_iDataLength) {
         m_iDataLength = 0;
         m_pByteArray[0] = 0;
      } else { 
         Clear();
         Reserve(rStr.m_iDataLength + 1);
      }
   } else {
      Reserve(rStr.m_iDataLength + 1);
   }

   if( 0 == m_iBufferSize ) 
      return *this;

   memcpy(m_pByteArray, rStr.m_pByteArray, rStr.m_iDataLength);
   m_iDataLength = rStr.m_iDataLength;
   m_pByteArray[m_iDataLength] = 0;

   return *this;
}

AxString &AxString::operator=(const char *cstr)
{
   if (AX_ISZERO(cstr)) {
      Clear();
      return *this;
   }

   size_t iLen = strlen(cstr);
   if (iLen >= m_iBufferSize) {
      Clear();
      Reserve(iLen + 1);
   } 

   if (m_iBufferSize) {
      axStrncpy((char*)m_pByteArray, (char *)cstr, m_iBufferSize);
      m_iDataLength = iLen;
   }

   return *this;
}

AxString &AxString::operator=(const char chr)
{
   Clear();
   Reserve(2);

   if (m_iBufferSize) {
      m_pByteArray[0] = chr;
      m_pByteArray[1] = 0;
      m_iDataLength = 1;
   }

   return *this;
}

AxString &AxString::operator=(const __int16_t nValue)
{
   this->Csnprintf(32, "%d", nValue);
   return *this;
}

AxString &AxString::operator=(const __uint16_t nValue)
{
   this->Csnprintf(32, "%u", nValue);
   return *this;
}

AxString &AxString::operator=(const __int32_t nValue)
{
   this->Csnprintf(32, "%d", nValue);
   return *this;
}

AxString &AxString::operator=(const __uint32_t nValue)
{
   this->Csnprintf(32, "%u", nValue);
   return *this;
}

AxString &AxString::operator=(const __int64_t nValue)
{
   this->Csnprintf(32, "%ld", nValue);
   return *this;
}

AxString &AxString::operator=(const __uint64_t nValue)
{
   this->Csnprintf(32, "%lu", nValue);
   return *this;
}

AxString &AxString::operator+=(const AxString &rStr)
{
   if (rStr.IsEmpty()) {
      return *this;
   }

   if (rStr.m_iDataLength >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + (((rStr.m_iDataLength / AxString::UNIT_SIZE) + 1) * AxString::UNIT_SIZE));
   }

   if (m_iBufferSize) {
      memcpy(&m_pByteArray[m_iDataLength], rStr.m_pByteArray, rStr.m_iDataLength);
      m_iDataLength += rStr.m_iDataLength;
      m_pByteArray[m_iDataLength] = 0;
   }

   return *this;
}

AxString &AxString::operator+=(const char *cstr)
{
   if (AX_ISZERO(cstr)) {
      return *this;
   }

   size_t iLen = strlen(cstr);
   if (iLen >= (m_iBufferSize - m_iDataLength)) {
      Reserve( m_iBufferSize + (((iLen / AxString::UNIT_SIZE) + 1) * AxString::UNIT_SIZE) );
   }

   if (m_iBufferSize) {
      memcpy( &m_pByteArray[m_iDataLength], cstr, iLen);
      m_iDataLength += iLen;
      m_pByteArray[m_iDataLength] = 0;
   }

   return *this;
}

AxString &AxString::operator+=(const char ch)
{
   if (1 >= (m_iBufferSize - m_iDataLength)) {
      Reserve( m_iBufferSize + AxString::UNIT_SIZE );
   }

   if (m_iBufferSize) {
      m_pByteArray[m_iDataLength] = ch;
      m_iDataLength++;
      m_pByteArray[m_iDataLength] = 0;
   }

   return *this;
}

AxString &AxString::operator+=(short number)
{
   if ((sizeof(short)*3+1) >= (m_iBufferSize - m_iDataLength)) {
      Reserve( m_iBufferSize + AxString::UNIT_SIZE );
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, sizeof(short)*3+1);
      p_signed2string<short>(number, 10, (char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += strlen( (char*)&m_pByteArray[m_iDataLength] );
   }

   return *this;
}

AxString &AxString::operator+=(unsigned short number)
{
   if ((sizeof(unsigned short)*3+1) >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, sizeof(unsigned short)*3+1);
      p_unsigned2string<unsigned short>(number, 10, (char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += strlen( (char*)&m_pByteArray[m_iDataLength] );
   }

   return *this;
}

AxString &AxString::operator+=(int number)
{
   if ((sizeof(int)*3+1) >= (m_iBufferSize - m_iDataLength)) {
      Reserve( m_iBufferSize + AxString::UNIT_SIZE );
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, sizeof(int)*3+1);
      p_signed2string<int>(number, 10, (char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += strlen( (char*)&m_pByteArray[m_iDataLength] );
   }

   return *this;
}

AxString &AxString::operator+=(unsigned int number)
{
   if ((sizeof(unsigned int)*3+1) >= (m_iBufferSize - m_iDataLength)) {
      Reserve( m_iBufferSize + AxString::UNIT_SIZE );
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, sizeof(unsigned int)*3+1);
      p_unsigned2string<unsigned int>(number, 10, (char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += strlen( (char*)&m_pByteArray[m_iDataLength] );
   }

   return *this;
}

AxString &AxString::operator+=(long number)
{
   if ((sizeof(long)*3+1) >= (m_iBufferSize - m_iDataLength)) {
      Reserve( m_iBufferSize + AxString::UNIT_SIZE );
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, sizeof(long)*3+1);
      p_signed2string<long>(number, 10, (char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += strlen( (char*)&m_pByteArray[m_iDataLength] );
   }

   return *this;
}

AxString &AxString::operator+=(unsigned long number)
{
   if ((sizeof(unsigned long)*3+1) >= (m_iBufferSize - m_iDataLength)) {
      Reserve( m_iBufferSize + AxString::UNIT_SIZE );
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, sizeof(unsigned long)*3+1);
      p_unsigned2string<unsigned long>(number, 10, (char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += strlen( (char*)&m_pByteArray[m_iDataLength] );
   }

   return *this;
}

AxString &AxString::operator+=(float number)
{
   if (32 >= (m_iBufferSize - m_iDataLength)) {
      Reserve( m_iBufferSize + 32 );
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, 32);
      snprintf((char*)&m_pByteArray[m_iDataLength], 32, "%f", number);
      m_iDataLength += strlen( (char*)&m_pByteArray[m_iDataLength] );
   }

   return *this;
}

char &AxString::operator[](unsigned int index)
{
   static char cEmpty = 0;

   if (0 == m_iDataLength)
      return cEmpty;

   if (index > m_iDataLength)
      return cEmpty;

   return (char&)(m_pByteArray[index]);
}

AxString &AxString::operator+=(double number)
{
   if (32 >= (m_iBufferSize - m_iDataLength)) {
      Reserve( m_iBufferSize + 32 );
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, 32);
      snprintf((char*)&m_pByteArray[m_iDataLength], 32, "%f", number);
      m_iDataLength += strlen( (char*)&m_pByteArray[m_iDataLength] );
   }

   return *this;
}

const char *AxString::c_str() 
{
   if (0 == m_iDataLength) return NULL;
   return (const char*) m_pByteArray;
}

const char *AxString::c_log() 
{
   if (0 == m_iDataLength) return "(null)";
   return (const char*) m_pByteArray;
}

void AxString::Clear() 
{
   if (m_iBufferSize) {
      m_iDataLength   = 0;
      m_pByteArray[0] = 0;
   }
}

void AxString::Resize(unsigned int iSize)
{
   if (iSize < m_iDataLength) {
      m_iDataLength = iSize;
      m_pByteArray[iSize] = 0;
   } else if (iSize > m_iBufferSize) {
      Reserve(iSize);
   }
}

void AxString::Csnprintf(unsigned int iLength, const char *cfmt, ...)
{
   va_list args;
   va_start(args, cfmt);

   if (iLength > m_iBufferSize) {
      Clear();
      Reserve(iLength);
   }

   vsnprintf((char*)m_pByteArray, m_iBufferSize, cfmt, args);
   m_iDataLength = strlen((char*)m_pByteArray);

   va_end(args);
}

int AxString::Find(char ch, int iOffset) const
{
   if (iOffset < 0) iOffset = 0;

   int iLen = (int) m_iDataLength;
   for (; iOffset < iLen; iOffset++) {
      if (((char)m_pByteArray[iOffset]) == ch) 
         return iOffset;
   }

   return AxString::npos;
}

int AxString::Find(const AxString &str, int iOffset) const
{
   if ((0 == m_iDataLength) || (0 == str.m_iDataLength))
      return AxString::npos;

   if (iOffset < 0) 
      iOffset = 0;
   else if (iOffset >= (int)m_iDataLength)
      return AxString::npos;

   int iLen = (int)m_iDataLength - iOffset;
   const char *cstr = (const char*)str.m_pByteArray;

   iLen -= (int)str.m_iDataLength;
   if (iLen < 0) 
      return AxString::npos;

   iLen += iOffset;
   for (; iOffset <= iLen; iOffset++) {
      if (axPrefixMatch((const char*)(m_pByteArray + iOffset), cstr))
         return iOffset;
   }

   return AxString::npos;
}

int AxString::Find(const char *cstr, int iOffset) const
{
   if ((0 == m_iDataLength) || (AX_ISZERO(cstr))) 
      return AxString::npos;

   if (iOffset < 0) 
      iOffset = 0;
   else if (iOffset >= (int)m_iDataLength)
      return AxString::npos;

   int iLen = (int)m_iDataLength - iOffset;

   iLen -= strlen(cstr);
   if (iLen < 0) 
      return AxString::npos;

   iLen += iOffset;
   for (; iOffset <= iLen; iOffset++) {
      if (axPrefixMatch((const char*)(m_pByteArray + iOffset), cstr))
         return iOffset;
   }

   return AxString::npos;
}

int AxString::FindLast(char ch, int iOffset) const
{
   if (0 == m_iDataLength)
      return AxString::npos;

   if ((iOffset < 0) || (iOffset >= (int)m_iDataLength))
      iOffset = m_iDataLength - 1;

   while (((char)m_pByteArray[iOffset]) != ch) {
      if (0 == iOffset)
         return AxString::npos;
      iOffset--;
   }

   return iOffset;
}

int AxString::FindLast(const AxString &str, int iOffset) const
{
   if ((0 == m_iDataLength) || (0 == str.m_iDataLength))
      return AxString::npos;

   if ((iOffset < 0) || (iOffset >= (int)m_iDataLength))
      iOffset = m_iDataLength - 1;

   const char *cstr = (const char*)str.m_pByteArray;
   int iLen = str.m_iDataLength;
   if ((m_iDataLength - (m_iDataLength - (unsigned int)(iOffset+1))) < (unsigned int)iLen) 
      return AxString::npos;


   iOffset -= (iLen - 1);
   while (!axPrefixMatch((const char*)(m_pByteArray + iOffset), cstr)) {
      if (0 == iOffset)
         return AxString::npos;
      iOffset--;
   }

   return iOffset;
}

int AxString::FindLast(const char *cstr, int iOffset) const
{
   if ((0 == m_iDataLength) || (AX_ISZERO(cstr)))
      return AxString::npos;

   if ((iOffset < 0) || (iOffset >= (int)m_iDataLength))
      iOffset = m_iDataLength - 1;

   int iLen = strlen( cstr );
   if ((m_iDataLength - (m_iDataLength - (unsigned int)(iOffset+1))) < (unsigned int)iLen) 
      return AxString::npos;

   iOffset -= (iLen - 1);
   while (!axPrefixMatch((const char*)(m_pByteArray + iOffset), cstr)) {
      if (0 == iOffset )
         return AxString::npos;
      iOffset--;
   }

   return iOffset;
}

AxString AxString::Left(int iLen) const
{
   if (0 >= iLen) 
      return AxString((unsigned int)0);

   if (iLen >= (int)m_iDataLength) 
      return *this;

   return AxString((const char*)m_pByteArray, iLen);
}

AxString AxString::Right(int iLen) const
{
   if (0 >= iLen)
      return AxString((unsigned int)0);

   int iSrcLen = (int) m_iDataLength;
   if (iLen >= iSrcLen) 
      return *this;

   return AxString((const char*)(m_pByteArray+iSrcLen-iLen), iLen);
}

AxString AxString::Mid(int iStart, int iLen) const
{
   if ((0 > iStart) || (0 == iLen) || (iStart >= (int)m_iDataLength))
      return AxString((unsigned int)0);

   if (0 > iLen)
      iLen = (int) m_iDataLength;

   return AxString((const char*)(m_pByteArray+iStart), iLen);
}

void AxString::Assign(const std::string &str)
{
   (*this) = str;
}

void AxString::Assign(AxString &str)
{
   (*this) = str;
}

void AxString::Assign(const char ch)
{
   (*this) = ch;
}

void AxString::Assign(const char *str)
{
   (*this) = str;
}

void AxString::Assign(const char *str, unsigned int len)
{
   Clear();

   if (AX_ISZERO(str))     return;
   if (0 == len)           return;

   if (len >= m_iBufferSize) {
      Reserve(len + 1);
   } 

   memcpy(m_pByteArray, str, len);
   m_pByteArray[len] = 0x00;
   m_iDataLength = len;
}

void AxString::Assign(const __int16_t value)
{
   (*this) = value;
}

void AxString::Assign(const __uint16_t value)
{
   (*this) = value;
}

void AxString::Assign(const __int32_t value)
{
   (*this) = value;
}

void AxString::Assign(const __uint32_t value)
{
   (*this) = value;
}

void AxString::Assign(const __int64_t value)
{
   (*this) = value;
}

void AxString::Assign(const __uint64_t value)
{
   (*this) = value;
}

void AxString::Append(const std::string &str)
{
   (*this) += str;
}

void AxString::Append(AxString &str)
{
   (*this) += str;
}

void AxString::Append(const char ch)
{
   (*this) += ch;
}

void AxString::Append(const char *str)
{
   (*this) += str;
}

void AxString::Append(const char *str, unsigned int len)
{
   if (AX_ISZERO(str))     return ;
   if (0 == len)           return ;

   if (len >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + len  + AxString::UNIT_SIZE);
   } 

   memcpy(&m_pByteArray[m_iDataLength], str, len);
   m_iDataLength += len;
   m_pByteArray[m_iDataLength] = 0x00;
}

void AxString::Append(const __int16_t value)
{
   (*this) += value;
}

void AxString::Append(const __uint16_t value)
{
   (*this) += value;
}

void AxString::Append(const __int32_t value)
{
   (*this) += value;
}

void AxString::Append(const __uint32_t value)
{
   (*this) += value;
}

void AxString::Append(const __int64_t value)
{
   (*this) += value;
}

void AxString::Append(const __uint64_t value)
{
   (*this) += value;
}

void AxString::LTrim() 
{
   if (0 == m_iDataLength) return;

   char *tmp;
   char *pLStr = (char*) m_pByteArray;

   for (tmp = pLStr; *tmp ; tmp++) {
      if (' ' == *tmp) {
      } else if ('\t' == *tmp) {
      } else if ('\r' == *tmp) {
      } else if ('\n' == *tmp) {
      } else {
         break;
      }
   }

   if (tmp == pLStr) return;

   m_iDataLength -= (tmp - pLStr);
   for (*pLStr = *tmp; *pLStr; *pLStr = *tmp) {
      pLStr++;
      tmp++;
   }
}

void AxString::RTrim() 
{
   if (0 == m_iDataLength) return;

   __uint32_t i = m_iDataLength;
   while (i) {
      --i;

      switch (m_pByteArray[i]) {
         case ' ' :
         case '\t' :
         case '\r' :
         case '\n' :
            m_pByteArray[i] = 0;
            --m_iDataLength;
            break;

         default :
            i = 0;
            break;
      }
   }
}

void AxString::Trim()
{
   RTrim();
   LTrim();
}

void AxString::TrimQuoted()
{
   if (2 > m_iDataLength)
      return;

   if ('\"' != (char)m_pByteArray[0])
      return;

   if ('\"' != (char)m_pByteArray[m_iDataLength-1])
      return;

   char *tmp   = (char*) (m_pByteArray+1);
   char *pLStr = (char*) m_pByteArray;

   m_iDataLength = 0;
   for (*pLStr = *tmp; *pLStr; *pLStr = *tmp) {
      ++pLStr;
      ++tmp;
      ++m_iDataLength;
   }

   --m_iDataLength;
   m_pByteArray[m_iDataLength] = 0x00;
}

void AxString::LTrimNonAlnum()
{
   if (0 == m_iDataLength) return;

   char *tmp;
   char *pLStr = (char*) m_pByteArray;

   for (tmp = pLStr; *tmp ; ++tmp) {
      if (isalnum(*tmp)) {
         break;
      }
   }

   if (tmp == pLStr) return;

   m_iDataLength = 0;
   for (*pLStr = *tmp; *pLStr; *pLStr = *tmp) {
      pLStr++;
      tmp++;
      m_iDataLength++;
   }
}

void AxString::RTrimNonAlnum()
{
   if (0 == m_iDataLength) return;

   unsigned int i = m_iDataLength;
   while (i) {
      i--;
      if (isalnum(m_pByteArray[i])) {
         i = 0;
         break;
      } else {
         m_pByteArray[i] = 0;
         m_iDataLength--;
      }
   }
}

void AxString::TrimNonAlnum()
{
   RTrimNonAlnum();
   LTrimNonAlnum();
}

bool AxString::IsEqual(char chr)
{
   if (1 != m_iDataLength)
      return false;

   if (chr != (char)m_pByteArray[0])
      return false;

   return true;
}

bool AxString::IsEqual(const char *pStr)
{
   if (AX_ISZERO(pStr) || IsEmpty())
      return false;

   return axStrMatch(pStr, c_str());
}

bool AxString::IsEqual(const std::string &str)
{
   if (str.empty()) 
      return false;

   return axStrMatch(str.c_str(), c_str());
}

bool AxString::IsEqual(AxString &str)
{
   if (str.IsEmpty()) 
      return false;

   return axStrMatch(str.c_str(), c_str());
}

bool AxString::IsCaseEqual(const char *pStr)
{
   if (AX_ISZERO(pStr) || IsEmpty())
      return false;

   return axStrCaseMatch(pStr, c_str());;
}

bool AxString::IsPrefixMatch(AxString &rPrefix)
{
   return axPrefixMatch(c_str(), rPrefix.c_str());
}

bool AxString::IsSuffixMatch(AxString &rSuffix)
{
   if (rSuffix.IsEmpty()) {
      return false;
   }

   if (IsEmpty()) {
      return false;
   }

   if (GetSize() < rSuffix.GetSize()) {
      return false;
   } else if (GetSize() == rSuffix.GetSize()) {
      return IsEqual(rSuffix.c_str());
   }

   return axPrefixMatch(c_str() + GetSize() - rSuffix.GetSize(), rSuffix.c_str());
}

void AxString::Replace(const char *pTarget, const char *pSubs, bool bAll, int iOffset)
{
   if (AX_ISZERO(pTarget))
      return ;

   unsigned int iTargetLen  = strlen(pTarget);
   unsigned int iSubsLen    = AX_ISZERO(pSubs) ? 0 : strlen(pSubs);

   if (GetSize() < iTargetLen) 
      return;

   do {
      int pos = Find(pTarget, iOffset);
      if (AxString::npos == pos)
         return ;

      // splice
      unsigned int iNewLen = GetSize() - iTargetLen + iSubsLen;
      if (GetBufferSize() <= iNewLen) {
         Resize( GetBufferSize() * 2 );
      }

      if ((pos+iTargetLen) < GetSize()) {
         memmove((void*)(c_str()+pos+iSubsLen), (void*)(c_str()+pos+iTargetLen), GetSize()-pos-iTargetLen+1);
      }

      memcpy(m_pByteArray+pos, pSubs, iSubsLen);
      m_pByteArray[iNewLen] = '\0';
      m_iDataLength = iNewLen;

      iOffset = pos + iSubsLen;
   } while (bAll);
}

__uint32_t AxString::Count(const char ch)
{
   __uint32_t  count   = 0;
   __uint32_t  length  = GetSize();
   const char *cursor  = this->c_str();

   for (__uint32_t idx = 0; idx < length; ++idx) {
      if (*cursor == ch) {
         ++count;
      }
      ++cursor;
   }

   return count;
}

void AxString::ToLower()
{
   if (IsEmpty()) 
      return;

   for (char *cpos = (char *) m_pByteArray; *cpos; cpos++) {
      if (isupper(*cpos & 0xFF)) 
         *cpos = (char) tolower(*cpos & 0xFF);
   }
}

void AxString::ToUpper()
{
   if (IsEmpty()) 
      return;

   for (char *cpos = (char *) m_pByteArray; *cpos; cpos++) {
      if (islower(*cpos & 0xFF)) 
         *cpos = (char) toupper(*cpos & 0xFF);
   }
}

bool AxString::IsDigit()
{
   if (IsEmpty()) 
      return false;

   for (char *cpos = (char *) m_pByteArray; *cpos; cpos++) {
      if (!isdigit(*cpos & 0xFF)) 
         return false;
   }

   return true;
}

bool AxString::IsXDigit()
{
   if (IsEmpty()) 
      return false;

   for (char *cpos = (char *) m_pByteArray; *cpos; cpos++) {
      if (!isxdigit(*cpos & 0xFF)) 
         return false;
   }

   return true;
}

bool AxString::IsAlnum()
{
   if (IsEmpty()) 
      return false;

   for (char *cpos = (char *) m_pByteArray; *cpos; cpos++) {
      if (!isalnum(*cpos & 0xFF)) 
         return false;
   }

   return true;
}

bool AxString::IsBitset(std::bitset<256> &filter)
{
   if (IsEmpty()) 
      return false;

   for (char *cpos = (char *) m_pByteArray; *cpos; cpos++) {
      if (false == filter.test((unsigned char)(*cpos & 0xff))) {
         return false;
      }
   }

   return true;
}

int AxString::AsInt()
{
   if (0 == m_iDataLength) 
      return 0;
   return atoi( (char*)m_pByteArray );
}

__int16_t AxString::AsInt16()
{
   if (IsEmpty()) return 0;

   static const char   DIGITS[] = "0123456789";
   __int16_t           val = 0;
   bool                neg = false;

   char *s = (char *) m_pByteArray;
   while ((' ' == *s) || ('\t' == *s)) {
      ++s;
   }

   if ('-' == *s) {
      neg = true;
      ++s;
   } else if ('+' == *s) {
      ++s;
   }

   const char *where;
   unsigned digit;
   while (*s) {

      where = strchr(DIGITS, *s);
      if (NULL == where) {
         break;
      }

      digit = (where - DIGITS);
      val = (val * 10) + digit;
      ++s;
   }

   if (neg) {
      return -val;
   }

   return val;
}

__uint16_t AxString::AsUInt16()
{
   if (IsEmpty()) return 0;

   static const char   DIGITS[] = "0123456789";
   __uint16_t          val = 0;

   char *s = (char *) m_pByteArray;
   while ((' ' == *s) || ('\t' == *s)) {
      ++s;
   }

   if ('-' == *s) {
      ++s;
   } else if ('+' == *s) {
      ++s;
   }

   const char *where;
   unsigned digit;
   while (*s) {

      where = strchr(DIGITS, *s);
      if (NULL == where) {
         break;
      }

      digit = (where - DIGITS);
      val = (val * 10) + digit;
      ++s;
   }

   return val;
}

__int32_t AxString::AsInt32()
{
   if (IsEmpty()) return 0;

   static const char   DIGITS[] = "0123456789";
   __int32_t           val = 0;
   bool                neg = false;

   char *s = (char *) m_pByteArray;
   while ((' ' == *s) || ('\t' == *s)) {
      ++s;
   }

   if ('-' == *s) {
      neg = true;
      ++s;
   } else if ('+' == *s) {
      ++s;
   }

   const char *where;
   unsigned digit;
   while (*s) {

      where = strchr(DIGITS, *s);
      if (NULL == where) {
         break;
      }

      digit = (where - DIGITS);
      val = (val * 10) + digit;
      ++s;
   }

   if (neg) {
      return -val;
   }

   return val;
}

__uint32_t AxString::AsUInt32()
{
   if (IsEmpty()) return 0;

   static const char   DIGITS[] = "0123456789";
   __uint32_t          val = 0;

   char *s = (char *) m_pByteArray;
   while ((' ' == *s) || ('\t' == *s)) {
      ++s;
   }

   if ('-' == *s) {
      ++s;
   } else if ('+' == *s) {
      ++s;
   }

   const char *where;
   unsigned digit;
   while (*s) {

      where = strchr(DIGITS, *s);
      if (NULL == where) {
         break;
      }

      digit = (where - DIGITS);
      val = (val * 10) + digit;
      ++s;
   }

   return val;
}

__int64_t AxString::AsInt64()
{
   if (IsEmpty()) return 0;

   static const char   DIGITS[] = "0123456789";
   __int64_t           val = 0;
   bool                neg = false;

   char *s = (char *) m_pByteArray;
   while ((' ' == *s) || ('\t' == *s)) {
      ++s;
   }

   if ('-' == *s) {
      neg = true;
      ++s;
   } else if ('+' == *s) {
      ++s;
   }

   const char *where;
   unsigned digit;
   while (*s) {

      where = strchr(DIGITS, *s);
      if (NULL == where) {
         break;
      }

      digit = (where - DIGITS);
      val = (val * 10) + digit;
      ++s;
   }

   if (neg) {
      return -val;
   }

   return val;
}

__uint64_t AxString::AsUInt64()
{
   if (IsEmpty()) return 0;

   static const char   DIGITS[] = "0123456789";
   __uint64_t          val = 0;

   char *s = (char *) m_pByteArray;
   while ((' ' == *s) || ('\t' == *s)) {
      ++s;
   }

   if ('-' == *s) {
      ++s;
   } else if ('+' == *s) {
      ++s;
   }

   const char *where;
   unsigned digit;
   while (*s) {

      where = strchr(DIGITS, *s);
      if (NULL == where) {
         break;
      }

      digit = (where - DIGITS);
      val = (val * 10) + digit;
      ++s;
   }

   return val;
}

const char      *NULLSTR = "(null)";
const __uint32_t NULLLEN = strlen(NULLSTR);

AxString &AxString::operator<<(const AxString &right)
{
   if (right.IsEmpty()) {
      Append((char*) NULLSTR, NULLLEN);
      return *this;
   }

   Append((char*) right.m_pByteArray, right.m_iDataLength);
   return *this;
}

AxString &AxString::operator<<(const std::string &right)
{
   if (right.empty()) {
      Append((char*) NULLSTR, NULLLEN);
      return *this;
   }

   Append((char*) right.c_str(), right.size());
   return *this;
}

AxString &AxString::operator<<(const bool right)
{
   if (right) {
      Append((char*) "TRUE", 4);
   } else {
      Append((char*) "FALSE", 5);
   }
   return *this;
}

AxString &AxString::operator<<(const char *right)
{
   if (AX_ISZERO(right)) {
      Append((char*) NULLSTR, NULLLEN);
      return *this;
   }

   Append((char*) right, strlen(right));
   return *this;
}

AxString &AxString::operator<<(const unsigned char *right)
{
   if (AX_ISZERO(right)) {
      Append((char*) NULLSTR, NULLLEN);
      return *this;
   }

   Append((char*) right, strlen((char *)right));
   return *this;
}

AxString &AxString::operator<<(const char right)
{
   if (1 >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      m_pByteArray[m_iDataLength] = (AXBYTE_T) right;
      m_iDataLength += 1;
      m_pByteArray[m_iDataLength] = 0x00;
   }

   return *this;
}

AxString &AxString::operator<<(const unsigned char right)
{
   if (1 >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      m_pByteArray[m_iDataLength] = (AXBYTE_T) right;
      m_iDataLength += 1;
      m_pByteArray[m_iDataLength] = 0x00;
   }

   return *this;
}

AxString &AxString::operator<<(const float right)
{
   __uint32_t iLen = (sizeof(float) * 3) + 1;
   if (iLen >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + iLen + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, iLen);
      snprintf((char*)&m_pByteArray[m_iDataLength], iLen, "%f", right);
      __uint32_t nLen = strlen((char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += nLen;
   }

   return *this;
}

AxString &AxString::operator<<(const double right)
{
   __uint32_t iLen = (sizeof(double) * 3) + 1;
   if (iLen >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + iLen + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, iLen);
      snprintf((char*)&m_pByteArray[m_iDataLength], iLen, "%f", right);
      __uint32_t nLen = strlen((char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += nLen;
   }

   return *this;
}

AxString &AxString::operator<<(const __int16_t right)
{
   __uint32_t iLen = (sizeof(__int16_t) * 3) + 1;

   if (iLen >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + iLen + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, iLen);
      p_signed2string<__int16_t>(right, 10, (char*)&m_pByteArray[m_iDataLength]);
      __uint32_t nLen = strlen((char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += nLen;
   }

   return *this;
}

AxString &AxString::operator<<(const __uint16_t right)
{
   __uint32_t iLen = (sizeof(__uint16_t) * 3) + 1;

   if (iLen >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + iLen + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, iLen);
      p_unsigned2string<__uint16_t>(right, 10, (char*)&m_pByteArray[m_iDataLength]);
      __uint32_t nLen = strlen((char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += nLen;
   }

   return *this;
}

AxString &AxString::operator<<(const __int32_t right)
{
   __uint32_t iLen = (sizeof(__int32_t) * 3) + 1;

   if (iLen >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + iLen + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, iLen);
      p_signed2string<__int32_t>(right, 10, (char*)&m_pByteArray[m_iDataLength]);
      __uint32_t nLen = strlen((char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += nLen;
   }

   return *this;
}

AxString &AxString::operator<<(const __uint32_t right)
{
   __uint32_t iLen = (sizeof(__uint32_t) * 3) + 1;

   if (iLen >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + iLen + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, iLen);
      p_unsigned2string<__uint32_t>(right, 10, (char*)&m_pByteArray[m_iDataLength]);
      __uint32_t nLen = strlen((char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += nLen;
   }

   return *this;
}

AxString &AxString::operator<<(const __int64_t right)
{
   __uint32_t iLen = (sizeof(__int64_t) * 3) + 1;

   if (iLen >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + iLen + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, iLen);
      p_signed2string<__int64_t>(right, 10, (char*)&m_pByteArray[m_iDataLength]);
      __uint32_t nLen = strlen((char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += nLen;
   }

   return *this;
}

AxString &AxString::operator<<(const __uint64_t right)
{
   __uint32_t iLen = (sizeof(__uint64_t) * 3) + 1;

   if (iLen >= (m_iBufferSize - m_iDataLength)) {
      Reserve(m_iBufferSize + iLen + AxString::UNIT_SIZE);
   }

   if (m_iBufferSize) {
      memset(&m_pByteArray[m_iDataLength], 0x00, iLen);
      p_unsigned2string<__uint64_t>(right, 10, (char*)&m_pByteArray[m_iDataLength]);
      __uint32_t nLen = strlen((char*)&m_pByteArray[m_iDataLength]);
      m_iDataLength += nLen;
   }

   return *this;
}

