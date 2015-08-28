// vim:ts=3:sts=3:sw=3

#include "Printer.h"
#include "Logger.h"
#include "AxUtil.h"


void Printer::Print(Condition &condition, SessionMap &smap)
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
               STDOUT("[EOF]");
               STDOUT(" ");
            }
         }
      }
   }
}


bool Printer::MakeLog(Condition &condition, std::list<std::string> &chunk, AxString &output)
{
   if (chunk.empty())
      return false;

   std::list<std::string>::iterator iter = chunk.begin();
   for (; iter != chunk.end(); ++iter) {
      output += *iter;
   }

   output.Trim();

   return true;
}
