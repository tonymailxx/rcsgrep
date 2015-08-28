// vim:ts=3:sts=3:sw=3

#include "Logger.h"
#include <iostream>
#include <errno.h>
#include "AxUtil.h"


unsigned int   _logLevel = 0;
AxString       _logFile = "";

bool Logger::CanLogging(unsigned int level)
{
   return ((_logLevel < level) ? false : true);
}

void Logger::MakeLogHeader(pthread_t pid, AxString &oss)
{
   static const char *STR_TIME_FORMAT = "%Y-%m-%dT%H:%M:%S";

   char   dtnow[32];
   struct timespec tspec;
   struct tm   tm_now;

   // 리얼타임 시계로부터 현재 시간을 구한다.
   if (clock_gettime(CLOCK_REALTIME, &tspec) == -1) { 
      std::cout << "call clock_gettime(CLOCK_REALTIME) fail. errno:" << errno << std::endl;
      return;
   }

   // timespec 구조체의 초수 필드(tv_sec)를 tm 구조체로 변환
   localtime_r((time_t *)&tspec.tv_sec, &tm_now);

   if (strftime(dtnow, sizeof(dtnow), STR_TIME_FORMAT, &tm_now) == 0) {
      std::cout << "call strftime() fail. errno:" << errno << std::endl;
      return;
   }

   oss.Csnprintf(256, "%08X %s.%09ld", pid, dtnow, tspec.tv_nsec);
   oss.Resize(35);
   oss += " ";
}

void Logger::Logging(AxString &oss)
{
   if (oss.IsEmpty())
      return;

   // terminal 출력
   std::cout << oss.c_str();


   if (_logFile.IsEmpty())
      return;


   FILE *pFile = fopen(_logFile.c_str(), "a");
   if (NULL != pFile) {
      //fprintf(pFile, "%s", oss.c_str());
      fputs(oss.c_str(), pFile);
      fclose(pFile);

   } else {
      std::cout << "logfile[" << _logFile.c_log() << "] open fail. errno:" << errno << std::endl;
   }
}

unsigned int Logger::SetLogLevel(unsigned int level)
{
   if (LOG_LV_DEBUG < level) level = LOG_LV_DEBUG;

   _logLevel = level;

   return _logLevel;
}

unsigned int Logger::GetLogLevel()
{
   return _logLevel;
}

void Logger::SetLogFile(const char *pFilename)
{
   if (false == AX_ISZERO(pFilename)) {
      _logFile = pFilename;
   }
}

