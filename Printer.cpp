// vim:ts=3:sts=3:sw=3

#include "Printer.h"
#include "Logger.h"
#include "AxUtil.h"


void Printer::PrintSessionMap(SessionMap &smap)
{
   static const char *FN = "[SessionMap::Print] ";

   const char TAB[]        = "   ";
   const char SECTIONTAG[] = "      ";
   const char ELEMENTTAG[] = "         ";

   // DEBUG start
   {
      {
         DLOG(FN << TAB << "* IhSessionMap size:" << smap.m_IhSessionMap.size());

         std::map<int, SessionMap::IhSession>::iterator iter = smap.m_IhSessionMap.begin();
         for (; iter != smap.m_IhSessionMap.end(); ++iter) {
            DLOG(FN << TAB << TAB << "- ih-session-id:" << iter->first
                  << " type:" << iter->second.m_type
                  << " time:" << iter->second.m_time
                  << " call-count:" << iter->second.m_CallIdSet.size());

            // std::set<std::string>::iterator txit = iter->second.m_CallIdSet.begin();
            // for (; txit != iter->second.m_CallIdSet.end(); ++txit) {
            //    DLOG(FN << TAB << TAB << TAB << *txit);
            // }
         }
      }

      {
         DLOG(FN << TAB << "* DialogMap size:" << smap.m_DialogMap.size());

         std::map<std::string, std::set<__uint32_t> >::iterator iter = smap.m_DialogMap.begin();
         for (; iter != smap.m_DialogMap.end(); ++iter) {
            DLOG(FN << TAB << TAB << "- H-TR:" << iter->first << " ih-count:" << iter->second.size());

            // std::set<__uint32_t>::iterator ihit = iter->second.begin();
            // for (; ihit != iter->second.end(); ++ihit) {
            //    DLOG(FN << TAB << TAB << TAB << *ihit);
            // }
         }
      }
   }
   // DEBUG end

   STDOUT("================================================================================");
   STDOUT("# Session list"                                                                  );
   STDOUT("--------------------------------------------------------------------------------");
   STDOUT(" ");

   if (smap.m_SessionGroupMap.empty()) {
      STDOUT(SECTIONTAG << "empty!");

   } else {
      std::map<__uint32_t, SessionMap::SessionGroup>::iterator iter = smap.m_SessionGroupMap.begin();
      for (; iter != smap.m_SessionGroupMap.end(); ++iter) {
         STDOUT(SECTIONTAG << iter->first << ".");

         std::set<__uint32_t>::iterator setit = iter->second.m_IhSessionSet.begin();
         for (; setit != iter->second.m_IhSessionSet.end(); ++setit) {
            std::map<int, SessionMap::IhSession>::iterator ihit = smap.m_IhSessionMap.find(*setit);
            if (ihit != smap.m_IhSessionMap.end()) {
               STDOUT(ELEMENTTAG << "- time:" << ihit->second.m_time << " ih-session-id:" << ihit->first << " type:" << ihit->second.m_type);
            } else {
               STDOUT(ELEMENTTAG << "- (invalid) ih-session-id:" << *setit);
            }
         }
      }
   }

   STDOUT(" ");
   STDOUT("--------------------------------------------------------------------------------");
}

void Printer::PrintSessionLog(Condition &condition, SessionMap &smap)
{
   const char TAB[]        = "   ";

   std::map<__uint32_t, SessionMap::SessionGroup>::iterator sgit = smap.m_SessionGroupMap.begin();
   for (; sgit != smap.m_SessionGroupMap.end(); ++sgit) {

      STDOUT(PURPLE("================================================================================"));
      STDOUT(PURPLE(TAB << "Session " << sgit->first));
      //STDOUT(" ");
      std::set<__uint32_t>::iterator setit = sgit->second.m_IhSessionSet.begin();
      for (; setit != sgit->second.m_IhSessionSet.end(); ++setit) {
         std::map<int, SessionMap::IhSession>::iterator ihit = smap.m_IhSessionMap.find(*setit);
         if (ihit != smap.m_IhSessionMap.end()) {
            STDOUT(PURPLE(TAB << "- time : " << ihit->second.m_time << ", ih-session-id : " << ihit->first << ", type : " << ihit->second.m_type));
         } else {
            STDOUT(PURPLE(TAB << "- (invalid) ih-session-id : " << *setit));
         }
      }
      STDOUT(PURPLE("--------------------------------------------------------------------------------"));

      if (sgit->second.m_Logs.empty()) {
         STDOUT(RED(TAB << "empty!"));

      } else {
         AxString output;
         output.Reserve(10240);

         std::multimap<std::string, std::list<std::string> >::iterator chunkit = sgit->second.m_Logs.begin();
         for (; chunkit != sgit->second.m_Logs.end(); ++chunkit) {
            output.Clear();
            if (Printer::MakeLog(condition, chunkit->second, output)) {
               STDOUT(output);
               STDOUT(" ");
            }
         }
      }
   }
}

void Printer::PrintSessionLog(__uint32_t choice, Condition &condition, SessionMap &smap)
{
   const char TAB[]        = "   ";

   std::map<__uint32_t, SessionMap::SessionGroup>::iterator sgit = smap.m_SessionGroupMap.find(choice);
   if (sgit != smap.m_SessionGroupMap.end()) {
      STDOUT(PURPLE("================================================================================"));
      STDOUT(PURPLE(TAB << "Session " << sgit->first));
      //STDOUT(" ");
      std::set<__uint32_t>::iterator setit = sgit->second.m_IhSessionSet.begin();
      for (; setit != sgit->second.m_IhSessionSet.end(); ++setit) {
         std::map<int, SessionMap::IhSession>::iterator ihit = smap.m_IhSessionMap.find(*setit);
         if (ihit != smap.m_IhSessionMap.end()) {
            STDOUT(PURPLE(TAB << "- time : " << ihit->second.m_time << ", ih-session-id : " << ihit->first << ", type : " << ihit->second.m_type));
         } else {
            STDOUT(PURPLE(TAB << "- (invalid) ih-session-id : " << *setit));
         }
      }
      STDOUT(PURPLE("--------------------------------------------------------------------------------"));

      if (sgit->second.m_Logs.empty()) {
         STDOUT(RED(TAB << "empty!"));

      } else {
         AxString output;
         output.Reserve(10240);

         std::multimap<std::string, std::list<std::string> >::iterator chunkit = sgit->second.m_Logs.begin();
         for (; chunkit != sgit->second.m_Logs.end(); ++chunkit) {
            output.Clear();
            if (Printer::MakeLog(condition, chunkit->second, output)) {
               STDOUT(output);
               STDOUT(" ");
            }
         }
      }

   } else {
      STDOUT(" ");
      STDOUT(RED(TAB << "not-found Session " << choice));
      STDOUT(" ");
   }
}

bool Printer::MakeLog(Condition &condition, std::list<std::string> &chunk, AxString &output)
{
   static const char *TAB = "    ";
   static const unsigned int TLEN = strlen(TAB);

   AxString buffer;
   unsigned int remain;
   unsigned int lfwidth = condition.m_printWidthSize - TLEN;

   if (chunk.empty())
      return false;

   std::list<std::string>::iterator iter = chunk.begin();
   for (; iter != chunk.end(); ++iter) {
      remain   = iter->size();

      output += TAB;
      if ((0 == condition.m_printWidthSize) || (remain <= condition.m_printWidthSize)) {
         output += *iter;
         continue;
      }

      const char *pCur = iter->c_str();
      output.Append(pCur, condition.m_printWidthSize);
      pCur     += condition.m_printWidthSize;
      remain   -= condition.m_printWidthSize;

      while (remain) {
         output   += "\n";
         output   += TAB;
         output   += TAB;

         if (lfwidth < remain) {
            output.Append(pCur, lfwidth);
            pCur     += lfwidth;
            remain   -= lfwidth;
            continue;
         }

         output.Append(pCur, remain);
         break;
      }
   }

   output.RTrim();

   //output += TAB;
   output += "[EOF]";

   return true;
}
