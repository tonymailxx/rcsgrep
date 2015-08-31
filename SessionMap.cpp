// vim:ts=3:sts=3:sw=3

#include "SessionMap.h"
#include "AxUtil.h"
#include "Logger.h"

void SessionMap::IhSession::Clear()
{
   m_type               .clear();
   m_time               .clear();
   m_CallIdSet          .clear();
}

void SessionMap::SessionGroup::Clear()
{
   m_time               .clear();
   m_IhSessionSet       .clear();
   m_Logs               .clear();
}

void SessionMap::Clear()
{
   m_IhSessionMap       .clear();
   m_DialogMap          .clear();
   m_SessionGroupMap    .clear();
}

bool SessionMap::Compose(LLTParser &transaction)
{
   if ((0 > transaction.m_LocalIhSessionId) && (0 > transaction.m_RemoteIhSessionId))
      return false;
   if (transaction.H_TR.IsEmpty())
      return false;

   InsertIhSessionMap(transaction);
   InsertDialogMap(transaction);

   return true;
}

bool SessionMap::BuildSessionGroup()
{
   m_SessionGroupMap.clear();

   std::map<__uint32_t, SessionGroup> groupmap;

   {
      // Call-ID table로 임시 GroupMap 생성
      std::map<std::string, std::set<__uint32_t> >::iterator dlgit = m_DialogMap.begin();
      for (; dlgit != m_DialogMap.end(); ++dlgit) {
         std::map<__uint32_t, SessionGroup>::iterator gmit = groupmap.end();

         std::set<__uint32_t>::iterator ihit = dlgit->second.begin();
         for (; (ihit != dlgit->second.end()) && (gmit == groupmap.end()); ++ihit) {

            std::map<__uint32_t, SessionGroup>::iterator tmpit = groupmap.begin();
            for (; tmpit != groupmap.end(); ++tmpit) {
               if (tmpit->second.m_IhSessionSet.end() != tmpit->second.m_IhSessionSet.find(*ihit)) {
                  gmit = tmpit;
                  break;
               }
            }
         }

         if (gmit != groupmap.end()) {
            std::set<__uint32_t>::iterator ihit = dlgit->second.begin();
            for (; ihit != dlgit->second.end(); ++ihit) {
               gmit->second.m_IhSessionSet.insert(*ihit);
            }

         } else {
            __uint32_t seq = groupmap.size() + 1;
            SessionGroup sg;
            sg.m_IhSessionSet = dlgit->second;
            groupmap.insert(std::pair<__uint32_t, SessionGroup>(seq, sg));
         }
      }
   }

   {
      // 임시 GroupMap 으로 m_SessionGroupMap 생성
      std::map<__uint32_t, SessionGroup>::iterator gmit = groupmap.begin();
      for (; gmit != groupmap.end(); ++gmit) {
         std::map<__uint32_t, SessionGroup>::iterator sgit = m_SessionGroupMap.end();

         std::set<__uint32_t>::iterator ihit = gmit->second.m_IhSessionSet.begin();
         for (; (ihit != gmit->second.m_IhSessionSet.end()) && (sgit == m_SessionGroupMap.end()); ++ihit) {

            std::map<__uint32_t, SessionGroup>::iterator tmpit = m_SessionGroupMap.begin();
            for (; tmpit != m_SessionGroupMap.end(); ++tmpit) {
               if (tmpit->second.m_IhSessionSet.end() != tmpit->second.m_IhSessionSet.find(*ihit)) {
                  sgit = tmpit;
                  break;
               }
            }
         }

         if (sgit != m_SessionGroupMap.end()) {
            std::set<__uint32_t>::iterator ihit = gmit->second.m_IhSessionSet.begin();
            for (; ihit != gmit->second.m_IhSessionSet.end(); ++ihit) {
               sgit->second.m_IhSessionSet.insert(*ihit);
            }

         } else {
            __uint32_t seq = m_SessionGroupMap.size() + 1;
            m_SessionGroupMap.insert(std::pair<__uint32_t, SessionGroup>(seq, gmit->second));
         }
      }
   }

   return true;
}

void SessionMap::InsertIhSessionMap(LLTParser &transaction)
{
   static const char *FN = "[SessionMap::InsertIhSessionMap] ";

   // local
   if (0 <= transaction.m_LocalIhSessionId) {
      DLOG(FN << DCYAN("local ih:" << transaction.m_LocalIhSessionId << "/" << transaction.m_LocalIhSessionType
               << " time:" << transaction.m_Time << " H-TR:" << transaction.H_TR));

      std::map<int, IhSession>::iterator iter = m_IhSessionMap.find(transaction.m_LocalIhSessionId);
      if (iter != m_IhSessionMap.end()) {
         if (false == transaction.m_Time.IsEmpty()) {
            if (iter->second.m_time.empty()) {
               iter->second.m_time = transaction.m_Time.c_str();
            } else if (0 > axStrcmpRange(iter->second.m_time.c_str(), transaction.m_Time.c_str())) {
               iter->second.m_time = transaction.m_Time.c_str();
            }
         }
         iter->second.m_CallIdSet.insert(transaction.H_TR.c_log());

      } else {
         IhSession ih;
         ih.m_type = (transaction.m_LocalIhSessionType.IsEmpty() ? "" : transaction.m_LocalIhSessionType.c_str());
         ih.m_time = (transaction.m_Time.IsEmpty() ? "" : transaction.m_Time.c_str());
         ih.m_CallIdSet.insert(transaction.H_TR.c_log());

         m_IhSessionMap.insert(std::pair<int, IhSession>(transaction.m_LocalIhSessionId, ih));
      }
   }

   // remote
   if (0 <= transaction.m_RemoteIhSessionId) {
      DLOG(FN << DCYAN("remote ih:" << transaction.m_RemoteIhSessionId << "/" << transaction.m_RemoteIhSessionType
               << " time:" << transaction.m_Time << " H-TR:" << transaction.H_TR));

      std::map<int, IhSession>::iterator iter = m_IhSessionMap.find(transaction.m_RemoteIhSessionId);
      if (iter != m_IhSessionMap.end()) {
         if (false == transaction.m_Time.IsEmpty()) {
            if (iter->second.m_time.empty()) {
               iter->second.m_time = transaction.m_Time.c_str();
            } else if (0 > axStrcmpRange(iter->second.m_time.c_str(), transaction.m_Time.c_str())) {
               iter->second.m_time = transaction.m_Time.c_str();
            }
         }
         iter->second.m_CallIdSet.insert(transaction.H_TR.c_log());

      } else {
         IhSession ih;
         ih.m_type = (transaction.m_RemoteIhSessionType.IsEmpty() ? "" : transaction.m_RemoteIhSessionType.c_str());
         ih.m_time = (transaction.m_Time.IsEmpty() ? "" : transaction.m_Time.c_str());
         ih.m_CallIdSet.insert(transaction.H_TR.c_log());

         m_IhSessionMap.insert(std::pair<int, IhSession>(transaction.m_RemoteIhSessionId, ih));
      }
   }
}

void SessionMap::InsertDialogMap(LLTParser &transaction)
{
   static const char *FN = "[SessionMap::InsertIhSessionMap] ";

   DLOG(FN << DCYAN("local H-TR:" << transaction.H_TR << " ih : " << transaction.m_LocalIhSessionId << ", " << transaction.m_RemoteIhSessionId));

   std::map<std::string, std::set<__uint32_t> >::iterator iter = m_DialogMap.find(transaction.H_TR.c_log());
   if (iter != m_DialogMap.end()) {
      if (0 <= transaction.m_LocalIhSessionId)  iter->second.insert(transaction.m_LocalIhSessionId);
      if (0 <= transaction.m_RemoteIhSessionId) iter->second.insert(transaction.m_RemoteIhSessionId);

   } else {
      std::set<__uint32_t> ihMap;
      if (0 <= transaction.m_LocalIhSessionId)  ihMap.insert(transaction.m_LocalIhSessionId);
      if (0 <= transaction.m_RemoteIhSessionId) ihMap.insert(transaction.m_RemoteIhSessionId);

      m_DialogMap.insert(std::pair<std::string, std::set<__uint32_t> >(transaction.H_TR.c_log(), ihMap));
   }
}


