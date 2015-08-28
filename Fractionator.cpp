// vim:ts=3:sts=3:sw=3

#include "Fractionator.h"
#include "Logger.h"
#include "AxUtil.h"


bool Fractionator::Fractionate(const char *file, const char *protocol, Condition &condition, SessionMap &smap)
{
   char           readbuf[10240];
   unsigned int   readlen;
   LLTParser      lltmsg;

   FILE *fp = fopen(file, "r");
   if (NULL == fp) {
      STDOUT("file[" << file << "] open fail.");
      return false;
   }

   for (; !feof(fp); ) {
      readbuf[0] = '\0';
      fgets(readbuf, sizeof(readbuf), fp);
      readbuf[sizeof(readbuf)-1] = '\0';

      readlen = strlen(readbuf);
      if (60 > readlen)
         continue;

      axRTrimBlank(readbuf, readlen);  // LLT는 CRLF를 삭제하고 처리한다.

      // LLT sample : 9F479700 2015-08-25T09:23:23.739115] ^[[1;32mLLT P:SIP DIR:RECV M:REQ:INVITE
      if (false == axPrefixMatch(readbuf + 8, " 20")) 
         continue;

      if (NULL == strstr(readbuf + 36, "LLT P:"))
         continue;


      DLOG("LLT : " << readbuf);

      // 마지막 column이 사용될 경우 ^[[0m 이 같이 포함되므로, 마지막 color 지정 tag 삭제한다.
      for (unsigned int idx = readlen-1; idx; --idx) {
         if (readbuf[idx] == '') {
            readbuf[idx] = ' ';
            break;
         }
      }


      // 일단 SIP만 처리함.
      lltmsg.Clear();
      if (false == lltmsg.Parse(readbuf))
         continue;
      if (false == lltmsg.m_Protocol.IsCaseEqual(protocol))
         continue;

      // condition check
      if (false == ConditionMatching(condition, lltmsg))
         continue;


      lltmsg.Print();

      smap.Compose(lltmsg);
   }

   fclose(fp);

   return true;
}

bool Fractionator::ExtractLog(const char *file, SessionMap &smap)
{
   static const char *FN = "[Fractionator::ExtractLog] ";

   char           readbuf[10240];
   unsigned int   readlen;
   LLTParser      lltmsg;

   FILE *fp = fopen(file, "r");
   if (NULL == fp) {
      STDOUT(FN << "file[" << file << "] open fail.");
      return false;
   }


   std::list<std::string> chunk;

   for (unsigned int cursor = 1; !feof(fp); ++cursor) {
      readbuf[0]  = '\0';
      readbuf[8]  = '\0';
      readbuf[9]  = '\0';
      readbuf[10] = '\0';
      readbuf[35] = '\0';
      readbuf[36] = '\0';
      fgets(readbuf, sizeof(readbuf), fp);
      readbuf[sizeof(readbuf)-1] = '\0';

      readlen = strlen(readbuf);

      // sample : 9F479700 2015-08-25T09:23:23.739115] ^[[1;32mLLT P:SIP DIR:RECV M:REQ:INVITE
      if ((false == axPrefixMatch(readbuf + 8, " 20")) && (false == axPrefixMatch(readbuf + 35, "] "))) {
         if (chunk.empty()) {
            DLOG(FN << "invalid case. line[" << cursor << "] " << readbuf);
         } else {
            chunk.push_back(readbuf);
         }

         continue;
      }

      if (chunk.empty()) {
         chunk.push_back(readbuf);
         continue;
      }

      // chunk 메시지가 조건에 맞는지 확인
      CollectLog(chunk, smap);

      chunk.clear();
      if (false == AX_ISZERO(readbuf)) { // 마지막 read한 line 처리
         chunk.push_back(readbuf);
      }
   }

   fclose(fp);

   return true;
}

bool Fractionator::ConditionMatching(Condition &condition, LLTParser &lltmsg)
{
   static const char *FN = "[Fractionator::ConditionMatching] ";

   // time-scope
   if (false == condition.m_startTime.empty()) {
      if (0 > axStrcmpRange(condition.m_startTime.c_str(), lltmsg.m_Time.c_str()))
         return false;
   }

   if (false == condition.m_endTime.empty()) {
      if (0 < axStrcmpRange(condition.m_endTime.c_str(), lltmsg.m_Time.c_str()))
         return false;
   }

   // transaction
   if (false == condition.m_callIdTable.empty()) {
      std::set<std::string>::iterator iter = condition.m_callIdTable.begin();
      for (; iter != condition.m_callIdTable.end(); ++iter) {
         if (lltmsg.H_TR.IsEqual(*iter)) {
            DLOG(FN << "match H-TR:" << *iter);
            return true;
         }
      }
   }

   // ctn
   if (false == condition.m_ctnTable.empty()) {
      std::set<std::string>::iterator iter = condition.m_ctnTable.begin();
      for (; iter != condition.m_ctnTable.end(); ++iter) {
         if ((false == lltmsg.H_RU.IsEmpty()) && strstr(lltmsg.H_RU.c_str(), iter->c_str())) {
            DLOG(FN << "match H-RU:" << *iter);
            return true;
         }

         if ((false == lltmsg.H_F.IsEmpty()) && strstr(lltmsg.H_F.c_str(), iter->c_str())) {
            DLOG(FN << "match H-F:" << *iter);
            return true;
         }

         if ((false == lltmsg.H_T.IsEmpty()) && strstr(lltmsg.H_T.c_str(), iter->c_str())) {
            DLOG(FN << "match H-T:" << *iter);
            return true;
         }
      }
   }

   // imdn
   if (false == condition.m_imdnIdTable.empty()) {
      std::set<std::string>::iterator iter = condition.m_imdnIdTable.begin();
      for (; iter != condition.m_imdnIdTable.end(); ++iter) {
         if ((false == lltmsg.C_IM.IsEmpty()) && strstr(lltmsg.C_IM.c_str(), iter->c_str())) {
            DLOG(FN << "match H-IM:" << *iter);
            return true;
         }

         if ((false == lltmsg.C_DN.IsEmpty()) && strstr(lltmsg.C_DN.c_str(), iter->c_str())) {
            DLOG(FN << "match H-DN:" << *iter);
            return true;
         }
      }
   }

   return false;
}

bool Fractionator::CollectLog(std::list<std::string> &chunk, SessionMap &smap)
{
   static const char *FN = "[Fractionator::CollectLog] ";

   // 1. 외부 연동 메시지 확인
   std::list<std::string>::iterator iter = chunk.begin();
   if (iter == chunk.end()) {
      ELOG(FN << "chumk(empty)");
      return false;
   }
   if (iter->empty()) {
      ELOG(FN << "invalid first line");
      return false;
   }

   if       (strstr(iter->c_str(), "MSG SENDING TO"))       {}
   else if  (strstr(iter->c_str(), "MSG DELIVERED FROM"))   {}
   else                                                     { return false; }

   // 2. find Call-ID
   for (; iter != chunk.end(); ++iter) {
      if (iter->empty())                              continue;
      if (axPrefixMatch(iter->c_str(), "Call-ID: "))  break;
   }

   if (iter == chunk.end()) {
      DLOG(FN << "not-found Call-ID Header");
      return false;
   }

   // 3. condition check
   AxString callId = iter->c_str() + 9;
   callId.Trim();
   if (callId.IsEmpty()) {
      ELOG(FN << "invalid call-id");
      return false;
   }

   std::map<__uint32_t, SessionMap::SessionGroup>::iterator grit = smap.m_SessionGroupMap.begin();
   for (; grit != smap.m_SessionGroupMap.end(); ++grit) {

      // Session에 할당된 모든 ih-session에서 Call-ID maching
      std::set<__uint32_t>::iterator ihit = grit->second.m_IhSessionSet.begin();
      for (; ihit != grit->second.m_IhSessionSet.end(); ++ihit) {

         std::map<int, SessionMap::IhSession>::iterator found = smap.m_IhSessionMap.find(*ihit);
         if (found != smap.m_IhSessionMap.end()) {

            std::set<std::string>::iterator exist = found->second.m_CallIdSet.find(callId.c_str());
            if (exist != found->second.m_CallIdSet.end()) {

               AxRecordSet rSet(chunk.begin()->c_str(), ' ');
               AxString timeheader = rSet.GetAt(1);

               grit->second.m_Logs.insert(std::pair<std::string, std::list<std::string> >(timeheader.c_log(), chunk));
               return true;
            }
         }
      }
   }

   return false;
}


