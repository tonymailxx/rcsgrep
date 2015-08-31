// vim:ts=3:sts=3:sw=3

#include <iostream>
#include "Logger.h"
#include "Condition.h"
#include "LLTParser.h"
#include "Fractionator.h"
#include "Printer.h"
#include "Mmi.h"

#define USAGE 		"Usage: rcsgrep [OPTION]..."
#define VERSION 	"rcsgrep 0.2.0"


void Usage()
{
   std::cout << USAGE << std::endl;
   std::cout << "Try `rcsgrep --help' for more information." << std::endl;
}

void Version()
{
   std::cout << VERSION << std::endl;
   std::cout << LLTParser::Version() << std::endl;
}

void Help()
{
   std::cout << USAGE << std::endl;
   std::cout << "Example: rcsgrep --ctn '1012345678' -f rcs_log1.txt rcs_log2.txt"                 << std::endl;
   std::cout << ""                                                                                 << std::endl;
   std::cout << "Selection and interpretation:"                                                    << std::endl;
   std::cout << "       --call-id=\"Call-ID\""                                                     << std::endl;
   std::cout << "       --ctn=\"Customer Telephone Number\""                                       << std::endl;
   std::cout << "       --imdn-id=\"message-id\""                                                  << std::endl;
   std::cout << "       --service=\"service name\"               IM"                               << std::endl;
   std::cout << "       --time-scope=\"start-time|end-time\"     format \"yyyy-mm-ddThh:mi:ss\""   << std::endl;
   std::cout << "       --interactive"                                                             << std::endl;
   std::cout << ""                                                                                 << std::endl;
   std::cout << "Miscellaneous"                                                                    << std::endl;
   std::cout << "   -V, --version                              print version information and exit" << std::endl;
   std::cout << "       --help                                 display this help and exit"         << std::endl;
   std::cout << "       --example"                                                                 << std::endl;
   std::cout << ""                                                                                 << std::endl;
   std::cout << "Output control:"                                                                  << std::endl;
   std::cout << "       --color"                                                                   << std::endl;
//   std::cout << "       --unix-newline                         strip CR characters"                << std::endl;
   std::cout << "       --width=\"width size\""                                                    << std::endl;
   std::cout << "       --write=\"log file\""                                                      << std::endl;
   std::cout << ""                                                                                 << std::endl;
}

void Example()
{
   std::cout << USAGE << std::endl;
   std::cout << "Example:" << std::endl;
   std::cout << "" << std::endl;
   std::cout << "   $ rcsgrep --ctn=1012345678 --time-scope=\"2015-08-25T09:00:00|2015-08-25T10:00:00\" rcs_log1.txt" << std::endl;
   std::cout << "   $ rcsgrep --ctn=\"33330001|44440002\" rcs_log1.txt rcs_log2.txt" << std::endl;
   std::cout << "" << std::endl;
}

int main(int argc, char **argv)
{
   if (1 == argc) {
      Usage();
      return 0;
   }


   Condition condition;
   SessionMap sessionmap;


   if (false == condition.Parse(argc, argv)) {
      Usage();
      return 0;
   }

   if (condition.HasMiscellaneous()) {
      if       (condition.IsMiscVersion())   { Version();   }
      else if  (condition.IsMiscHelp())      { Help();      }
      else if  (condition.IsMiscExample())   { Example();   }
      else                                   { Usage();     }
      return 0;
   }

   if (false == condition.IsValid()) {
      std::cout << "invalid condition" << std::endl << std::endl;
      Usage();
      return 0;
   }
   condition.Print();

   // Session 정보 수집
   {
      std::set<std::string>::iterator iter = condition.m_inputFileTable.begin();
      for (; iter != condition.m_inputFileTable.end(); ++iter) {

         if (false == Fractionator::Fractionate(iter->c_str(), "sip", condition, sessionmap))
            return 0;
      }

      sessionmap.BuildSessionGroup();
   }

   if (sessionmap.m_SessionGroupMap.empty()) {
      Printer::PrintSessionMap(sessionmap);
      return 0;
   }


   // Session Log 수집
   {
      std::set<std::string>::iterator iter = condition.m_inputFileTable.begin();
      for (; iter != condition.m_inputFileTable.end(); ++iter) {

         if (false == Fractionator::ExtractLog(iter->c_str(), sessionmap))
            return 0;
      }

      std::map<__uint32_t, SessionMap::SessionGroup>::iterator grit = sessionmap.m_SessionGroupMap.begin();
      for (; grit != sessionmap.m_SessionGroupMap.end(); ++grit) {
         DLOG("session-group:" << grit->first << " logs:" << grit->second.m_Logs.size());
      }
   }


   //if ((condition.m_interactive) && (2 <= sessionmap.m_SessionGroupMap.size())) {
   if ((condition.m_interactive) && (1 <= sessionmap.m_SessionGroupMap.size())) {

      while (true) {
         Printer::PrintSessionMap(sessionmap);

         int choice = Mmi::Choose();
         DLOG("choice : " << choice);

         if (0 > choice) {
            break;
         }

         if (0 == choice) {
            continue;
         }

         // 수집 Log 출력
         Printer::PrintSessionLog((__uint32_t) choice, condition, sessionmap);
      }

   } else {
      Printer::PrintSessionMap(sessionmap);

      // 수집 Log 출력
      Printer::PrintSessionLog(condition, sessionmap);
   }

   return 0;
}
