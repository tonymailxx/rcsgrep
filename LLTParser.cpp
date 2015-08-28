#include "LLTParser.h"

#include "AxUtil.h"
#include "Logger.h"


LLTParser::LLTParser()
{
   Clear();
}

const char *LLTParser::Version()
{
   return "LLT v2.1.0";
}

void LLTParser::Clear()
{
   m_Time               .Clear();
   m_Protocol           .Clear();
   m_DirSend            = true;

   m_LocalIhSessionId   = -1;
   m_LocalIhSessionType .Clear();
   m_RemoteIhSessionId  = -1;
   m_RemoteIhSessionType.Clear();

   m_IsRequest          = true;
   m_Method             .Clear();
   m_StatusCode         = 0;

   H_RU                 .Clear();
   H_F                  .Clear();
   H_T                  .Clear();
   H_TR                 .Clear();
                        
   C_IM                 .Clear();
   C_DN                 .Clear();
}

void LLTParser::Print()
{
   static const char *FN = "[LLT::Print] ";

   DLOG(FN
         << "\n    ===================================================="
         << "\n    - time        : " << m_Time
         << "\n    - protocol    : " << m_Protocol
         << "\n    - direction   : " << (m_DirSend ? "SEND" : "RECV")
         << "\n    - session-info: local id:" << m_LocalIhSessionId << " / type:" << m_LocalIhSessionType
         << ", remote id:" << m_RemoteIhSessionId << " / type:" << m_RemoteIhSessionType
         << "\n    - start-line  : " << (m_IsRequest ? "REQ" : "RES") << " / " << m_Method << " / " << m_StatusCode
         << "\n    - H-RU        : " << H_RU
         << "\n    - H-F         : " << H_F
         << "\n    - H-T         : " << H_T
         << "\n    - H-TR        : " << H_TR
         << "\n    - C-IM        : " << C_IM
         << "\n    - C-DN        : " << C_DN
         << "\n");

//   DLOG(FN << "----------------------------------------------------");
}

bool LLTParser::Parse(char *STREAM)
{
   static const char *FN = "[LLT::Parse] ";

   //DLOG(FN << "RAW : " << STREAM);

   AxRecordSet rSet(STREAM, ' ');
   if (7 > rSet.GetSize()) {
      return false; 
   }

   const char *TAG = rSet.GetAt(2);
   if (AX_ISZERO(TAG)) {
      return false;
   }

   if (strstr(TAG, "LLT")) {
      return ParseLLT(rSet);
   }

   DLOG(FN << "invalid column(3):" << (AX_ISZERO(rSet.GetAt(2)) ? "(null)" : rSet.GetAt(2)) << " must be 'LLT'");
   return false;
}

bool LLTParser::ParseLLT(AxRecordSet &rSet)
{
   static const char *FN = "[LLT::ParseLLT] ";

   __uint32_t columns = rSet.GetSize();

   if (false == ExtractLoggingTime(rSet.GetAt(1))) {
      DLOG(FN << "invalid log-time:" << rSet.GetAt(1));
      return false;
   }

   const char *column;
   for (__uint32_t idx = 3; idx < columns; ++idx) {
      column = rSet.GetAt(idx);

      if (AX_ISZERO(column)) { continue; }

      if       (axPrefixMatch(column, "P:"))
      {
         m_Protocol = column+2;
      }
      else if  (axPrefixMatch(column, "D:"))
      {
         m_DirSend = (strstr(column, "SEND") ? true : false);
      }
      else if  (axPrefixMatch(column, "DIR:"))
      {
         m_DirSend = (strstr(column, "SEND") ? true : false);
      }
      else if  (axPrefixMatch(column, "M:"))
      {
         ExtractMethod(column+2);
      }
      else if  (axPrefixMatch(column, "H-RU:"))
      {
         H_RU = column + 5;
      }
      else if  (axPrefixMatch(column, "H-F:"))
      {
         H_F = column + 4;
         if (m_Method.IsCaseEqual("MESSAGE")) {
            ExtractPagerFromIhSessionInfo(H_F.c_str());
         } else {
            ExtractFromIhSessionInfo(H_F.c_str());
         }
      }
      else if  (axPrefixMatch(column, "H-T:"))
      {
         H_T = column + 4;
         if (m_Method.IsCaseEqual("MESSAGE")) {
            ExtractPagerToIhSessionInfo(H_T.c_str());
         } else {
            ExtractToIhSessionInfo(H_T.c_str());
         }
      }
      else if  (axPrefixMatch(column, "H-TR:"))
      {
         H_TR = column + 5;
      }
      else if  (axPrefixMatch(column, "C-IM:"))
      {
         C_IM = column + 5;
      }
      else if  (axPrefixMatch(column, "C-DN:"))
      {
         C_DN = column + 5;
      }
      else if  (axStrMatch(column, "ER"))
      {
         break;
      }
   }

   return true;
}

bool LLTParser::ExtractLoggingTime(const char *pStr)
{
   if (false == m_Time.IsEmpty()) { return false; }

   const char *orig = pStr;
   size_t nSize = strlen(pStr);

   // YYYY-MM-DDTHH:MI:SS.ssssss
   // 2014-07-30T08:00:36.805352
   if (26 > nSize) { return false; }

   AxString YYYY(pStr, 4);  pStr += (4 + 1);
   if (false == YYYY.IsDigit()) { return false; }
   if ((1968 > YYYY.AsInt32()) || (2038 < YYYY.AsInt32())) { return false; }

   AxString MM(pStr, 2);    pStr += (2 + 1);
   if (false == MM.IsDigit()) { return false; }
   if ((1 > MM.AsInt32()) || (12 < MM.AsInt32())) { return false; }

   AxString DD(pStr, 2);    pStr += (2 + 1);
   if (false == DD.IsDigit()) { return false; }
   if ((1 > DD.AsInt32()) || (31 < DD.AsInt32())) { return false; }

   AxString HH(pStr, 2);    pStr += (2 + 1);
   if (false == HH.IsDigit()) { return false; }
   if ((0 > HH.AsInt32()) || (23 < HH.AsInt32())) { return false; }

   AxString MI(pStr, 2);    pStr += (2 + 1);
   if (false == MI.IsDigit()) { return false; }
   if ((0 > MI.AsInt32()) || (59 < MI.AsInt32())) { return false; }

   AxString SS(pStr, 2);    pStr += (2 + 1);
   if (false == SS.IsDigit()) { return false; }
   if ((0 > SS.AsInt32()) || (59 < SS.AsInt32())) { return false; }

   AxString ssssss(pStr, 6);   pStr += 6;
   if (false == ssssss.IsDigit()) { return false; }
   if ((0 > ssssss.AsInt32()) || (999999 < ssssss.AsInt32())) { return false; }

   m_Time = orig;
   m_Time.Resize(26);

   return true;
}

bool LLTParser::ExtractMethod(const char *pStr)
{
   if (false == m_Method.IsEmpty()) { return false; }

   AxRecordSet rSet(pStr, ':');
   AxString method;

   if (2 > rSet.GetSize())
      return false;

   if       (axStrMatch(rSet.GetAt(0), "REQ"))  { m_IsRequest = true;   }
   else if  (axStrMatch(rSet.GetAt(0), "RES"))  { m_IsRequest = false;  }
   else                                         { return false;         }

   method = rSet.GetAt(1); method.Trim();
   if (method.IsEmpty())
      return false;

   if (false == m_IsRequest) {
      AxString tmp = rSet.GetAt(2);
      tmp.Trim();

      m_StatusCode = tmp.AsInt32();

      if (0 == m_StatusCode) {
         return false;
      }
   }

   m_Method = method;

   return false;
}

bool LLTParser::ExtractFromIhSessionInfo(const char *pStr)
{
   static const char *FN = "[LLTParser::ExtractFromIhSessionInfo] ";

   if (AX_ISZERO(pStr))
      return false;

   if (m_Protocol.IsCaseEqual("sip")) {
      // IM에 해당되는 내용인데.. PS나 XDMS는 어찌해야할지..
      AxRecordSet rSet(pStr, '|');
      if (2 > rSet.GetSize())
         return false;

      AxRecordSet tagset(rSet.GetAt(1), '-');
      if (6 != tagset.GetSize())
         return false;

      DLOG(FN << "tag 4:" << tagset.GetAt(3) << " 6:" << tagset.GetAt(5));

      AxString ihsession = tagset.GetAt(3);
      if (false == ihsession.IsDigit()) {
         return false;
      }

      if (m_DirSend) {
         if (m_IsRequest) {
            m_LocalIhSessionId      = ihsession.AsInt32();
            m_LocalIhSessionType    = tagset.GetAt(5);
         } else {
            m_RemoteIhSessionId     = ihsession.AsInt32();
            m_RemoteIhSessionType   = tagset.GetAt(5);
         }
      } else {
         if (m_IsRequest) {
            m_RemoteIhSessionId     = ihsession.AsInt32();
            m_RemoteIhSessionType   = tagset.GetAt(5);
         } else {
            m_LocalIhSessionId      = ihsession.AsInt32();
            m_LocalIhSessionType    = tagset.GetAt(5);
         }
      }

      return true;
   }

   return false;
}

bool LLTParser::ExtractToIhSessionInfo(const char *pStr)
{
   static const char *FN = "[LLTParser::ExtractToIhSessionInfo] ";

   if (AX_ISZERO(pStr))
      return false;

   if (m_Protocol.IsCaseEqual("sip")) {
      // IM에 해당되는 내용인데.. PS나 XDMS는 어찌해야할지..
      AxRecordSet rSet(pStr, '|');
      if (2 > rSet.GetSize())
         return false;

      AxRecordSet tagset(rSet.GetAt(1), '-');
      if (6 != tagset.GetSize())
         return false;

      DLOG(FN << "tag 4:" << tagset.GetAt(3) << " 6:" << tagset.GetAt(5));

      AxString ihsession = tagset.GetAt(3);
      if (false == ihsession.IsDigit()) {
         return false;
      }

      if (m_DirSend) {
         if (m_IsRequest) {
            m_RemoteIhSessionId     = ihsession.AsInt32();
            m_RemoteIhSessionType   = tagset.GetAt(5);
         } else {
            m_LocalIhSessionId      = ihsession.AsInt32();
            m_LocalIhSessionType    = tagset.GetAt(5);
         }

      } else {
         if (m_IsRequest) {
            m_LocalIhSessionId      = ihsession.AsInt32();
            m_LocalIhSessionType    = tagset.GetAt(5);
         } else {
            m_RemoteIhSessionId     = ihsession.AsInt32();
            m_RemoteIhSessionType   = tagset.GetAt(5);
         }

      }

      return true;
   }

   return false;
}

bool LLTParser::ExtractPagerFromIhSessionInfo(const char *pStr)
{
   static const char *FN = "[LLTParser::ExtractPagerFromIhSessionInfo] ";

   if (AX_ISZERO(pStr))
      return false;

   if (m_Protocol.IsCaseEqual("sip")) {
      // IM에 해당되는 내용인데.. PS나 XDMS는 어찌해야할지..
      AxRecordSet rSet(pStr, '|');
      if (2 > rSet.GetSize())
         return false;

      AxRecordSet tagset(rSet.GetAt(1), '-');
      if (4 != tagset.GetSize())
         return false;

      DLOG(FN << "tag 2:" << tagset.GetAt(1) << " 4:" << tagset.GetAt(3));

      AxString ihsession = tagset.GetAt(1);
      if (false == ihsession.IsDigit()) {
         return false;
      }

      if (m_DirSend) {
         if (m_IsRequest) {
            m_LocalIhSessionId      = ihsession.AsInt32();
            m_LocalIhSessionType    = tagset.GetAt(3);
         } else {
            m_RemoteIhSessionId     = ihsession.AsInt32();
            m_RemoteIhSessionType   = tagset.GetAt(3);
         }
      } else {
         if (m_IsRequest) {
            m_RemoteIhSessionId     = ihsession.AsInt32();
            m_RemoteIhSessionType   = tagset.GetAt(3);
         } else {
            m_LocalIhSessionId      = ihsession.AsInt32();
            m_LocalIhSessionType    = tagset.GetAt(3);
         }
      }

      return true;
   }

   return false;
}

bool LLTParser::ExtractPagerToIhSessionInfo(const char *pStr)
{
   static const char *FN = "[LLTParser::ExtractPagerToIhSessionInfo] ";

   if (AX_ISZERO(pStr))
      return false;

   if (m_Protocol.IsCaseEqual("sip")) {
      // IM에 해당되는 내용인데.. PS나 XDMS는 어찌해야할지..
      AxRecordSet rSet(pStr, '|');
      if (2 > rSet.GetSize())
         return false;

      AxRecordSet tagset(rSet.GetAt(1), '-');
      if (4 != tagset.GetSize())
         return false;

      DLOG(FN << "tag 2:" << tagset.GetAt(1) << " 4:" << tagset.GetAt(3));

      AxString ihsession = tagset.GetAt(1);
      if (false == ihsession.IsDigit()) {
         return false;
      }

      if (m_DirSend) {
         if (m_IsRequest) {
            m_RemoteIhSessionId     = ihsession.AsInt32();
            m_RemoteIhSessionType   = tagset.GetAt(3);
         } else {
            m_LocalIhSessionId      = ihsession.AsInt32();
            m_LocalIhSessionType    = tagset.GetAt(3);
         }

      } else {
         if (m_IsRequest) {
            m_LocalIhSessionId      = ihsession.AsInt32();
            m_LocalIhSessionType    = tagset.GetAt(3);
         } else {
            m_RemoteIhSessionId     = ihsession.AsInt32();
            m_RemoteIhSessionType   = tagset.GetAt(3);
         }

      }

      return true;
   }

   return false;
}

