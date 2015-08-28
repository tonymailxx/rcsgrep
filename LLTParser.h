#ifndef RCSGREP_LLT_PARSER_H__
#define RCSGREP_LLT_PARSER_H__

#include <string>

#include "AxString.h"
#include "AxRecordSet.h"

class LLTParser {
   public :
      LLTParser();

      static const char   *Version();
      void                 Clear();
      void                 Print();
                         
      bool                 Parse(char *STREAM);

   private :             
      bool                 ParseLLT(AxRecordSet &rSet);
                         
      bool                 ExtractLoggingTime(const char *pStr);
      bool                 ExtractMethod(const char *pStr);
      bool                 ExtractFromIhSessionInfo(const char *pStr);
      bool                 ExtractToIhSessionInfo(const char *pStr);
      bool                 ExtractPagerFromIhSessionInfo(const char *pStr);
      bool                 ExtractPagerToIhSessionInfo(const char *pStr);
                         
   public :              
      AxString             m_Time;
      AxString             m_Protocol;
      bool                 m_DirSend;

      int                  m_LocalIhSessionId;
      AxString             m_LocalIhSessionType;
      int                  m_RemoteIhSessionId;
      AxString             m_RemoteIhSessionType;

      bool                 m_IsRequest;
      AxString             m_Method;
      int                  m_StatusCode;

      AxString             H_RU;
      AxString             H_F;
      AxString             H_T;
      AxString             H_TR; // Call-ID

      AxString             C_IM;
      AxString             C_DN;
};


#endif
