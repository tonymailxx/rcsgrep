// vim:ts=3:sts=3:sw=3


#ifndef RCSGREP_AXSTRING_H__
#define RCSGREP_AXSTRING_H__

#include <stdarg.h>
#include <string>
#include <bitset>

#include "AxByteArray.h"


class AxString : public AxByteArray
{
  public :
    static const unsigned int UNIT_SIZE = 32;
    static const int npos = -1;

    AxString(unsigned int iSize = 0); // buffersize √ ±‚»≠
    AxString(const AxString &rStr);
    AxString(const std::string &str);
    AxString(const char *cstr);
    AxString(const char *cstr, unsigned int iSize);

    bool                  operator==(AxString &rStr);
    bool                  operator==(const std::string &rStr);
    bool                  operator==(const char *pStr);
    bool                  operator!=(AxString &rStr);
    bool                  operator!=(const std::string &rStr);
    bool                  operator!=(const char *pStr);

    AxString             &operator=(const std::string &rStr);
    AxString             &operator=(const AxString &rStr);
    AxString             &operator=(const char *cstr);
    AxString             &operator=(const char chr);
    AxString             &operator=(const __int16_t nValue);
    AxString             &operator=(const __uint16_t nValue); AxString             &operator=(const __int32_t nValue);
    AxString             &operator=(const __uint32_t nValue);
    AxString             &operator=(const __int64_t nValue);
    AxString             &operator=(const __uint64_t nValue);
                         
    AxString             &operator+=(const AxString &rStr);
    AxString             &operator+=(const char *cstr);
    AxString             &operator+=(const char ch);
    AxString             &operator+=(short number);
    AxString             &operator+=(unsigned short number);
    AxString             &operator+=(int number);
    AxString             &operator+=(unsigned int number);
    AxString             &operator+=(long number);
    AxString             &operator+=(unsigned long number);
    AxString             &operator+=(float number);
    AxString             &operator+=(double number);
    char                 &operator[](unsigned int index);

    AxString             &operator<<(const AxString &right);
    AxString             &operator<<(const std::string &right);
    AxString             &operator<<(const bool right);
    AxString             &operator<<(const char *right);
    AxString             &operator<<(const unsigned char *right);
    AxString             &operator<<(const char right);
    AxString             &operator<<(const unsigned char right);
    AxString             &operator<<(const float right);
    AxString             &operator<<(const double right);
    AxString             &operator<<(const __int16_t right);
    AxString             &operator<<(const __uint16_t right);
    AxString             &operator<<(const __int32_t right);
    AxString             &operator<<(const __uint32_t right);
    AxString             &operator<<(const __int64_t right);
    AxString             &operator<<(const __uint64_t right);

    const char           *c_str();
    const char           *c_log();
    void                  Clear();
    void                  Resize(unsigned int iSize);
    void                  Csnprintf(unsigned int iLength, const char *cfmt, ...);
                        
    int                   Find(char ch, int iOffset = 0) const;
    int                   Find(const AxString &str, int iOffset = 0) const;
    int                   Find(const char *cstr, int iOffset = 0) const;
    int                   FindLast(char ch, int iOffset = AxString::npos) const;
    int                   FindLast(const AxString &str, int iOffset = AxString::npos) const;
    int                   FindLast(const char *cstr, int iOffset = AxString::npos) const;
                        
    AxString              Left(int iLen) const;
    AxString              Right(int iLen) const;
    AxString              Mid(int iStart, int iLen = AxString::npos) const;
                        
    void                  Replace(const char *pTarget, const char *pSubs, bool bAll = true, int iOffset = 0);
    __uint32_t            Count(const char ch);
                        
//    bool                  SetSubstr(const char *pSrc, unsigned int iLen);
//    bool                  AppendSubstr(const char *pSrc, unsigned int iLen);

    void                  Assign(const std::string &str);
    void                  Assign(AxString &str);
    void                  Assign(const char ch);
    void                  Assign(const char *str);
    void                  Assign(const char *str, unsigned int len);
    void                  Assign(const __int16_t value);
    void                  Assign(const __uint16_t value);
    void                  Assign(const __int32_t value);
    void                  Assign(const __uint32_t value);
    void                  Assign(const __int64_t value);
    void                  Assign(const __uint64_t value);

    void                  Append(const std::string &str);
    void                  Append(AxString &str);
    void                  Append(const char ch);
    void                  Append(const char *str);
    void                  Append(const char *str, unsigned int len);
    void                  Append(const __int16_t value);
    void                  Append(const __uint16_t value);
    void                  Append(const __int32_t value);
    void                  Append(const __uint32_t value);
    void                  Append(const __int64_t value);
    void                  Append(const __uint64_t value);

    bool                  IsEqual(const char chr);
    bool                  IsEqual(const char *pStr);
    bool                  IsEqual(const std::string &str);
    bool                  IsEqual(AxString &str);
    bool                  IsCaseEqual(const char *pStr);
    bool                  IsPrefixMatch(AxString &rPrefix);
    bool                  IsSuffixMatch(AxString &rSuffix);
                        
    void                  LTrim();
    void                  RTrim();
    void                  Trim();
    void                  TrimQuoted();
                        
    void                  LTrimNonAlnum();
    void                  RTrimNonAlnum();
    void                  TrimNonAlnum();
                        
    void                  ToLower();
    void                  ToUpper();
                        
    bool                  IsDigit();
    bool                  IsXDigit();
    bool                  IsAlnum();
    bool                  IsBitset(std::bitset<256> &filter);
                        
    int                   AsInt();
    __int16_t             AsInt16();
    __uint16_t            AsUInt16();
    __int32_t             AsInt32();
    __uint32_t            AsUInt32();
    __int64_t             AsInt64();
    __uint64_t            AsUInt64();
};

#endif
