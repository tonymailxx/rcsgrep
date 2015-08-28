// vim:ts=3:sts=3:sw=3

#ifndef RCSGREP_LOGGER_H__
#define RCSGREP_LOGGER_H__

#include <pthread.h>
#include "AxString.h"


#define LOGOUTPUT(level, args) do {                                           \
   if (Logger::CanLogging(level)) {                                           \
      AxString oss;                                                           \
      Logger::MakeLogHeader(pthread_self(), oss);                             \
      oss << args << "\n";                                                    \
      Logger::Logging(oss);                                                   \
   }                                                                          \
} while (0)

#define STDOUTPUT(args) do {                                                  \
   AxString oss;                                                              \
   oss << args << "\n";                                                       \
   Logger::Logging(oss);                                                      \
} while (0)


#define LOG_LV_DEBUG             40  // 디버깅 로그
#define LOG_LV_INFO              30  // 메시지, 세션, 시스템등의 정보성 로그
#define LOG_LV_WARN              20  // 비즈니스로직 오류 발생할 수 있는 에러
#define LOG_LV_ERROR             10  // 프로세스 장애가 발생할 수 있는 에러

#define FILE_LINE_TAG            " [" << __FILE__ << ":" << __LINE__ << "]"

#define DLOG(A)                  LOGOUTPUT(LOG_LV_DEBUG, "  " << A << FILE_LINE_TAG)
#define ILOG(A)                  LOGOUTPUT(LOG_LV_INFO , "i " << A << FILE_LINE_TAG)
#define WLOG(A)                  LOGOUTPUT(LOG_LV_WARN , "W " << A << FILE_LINE_TAG)
#define ELOG(A)                  LOGOUTPUT(LOG_LV_ERROR, "E " << A << FILE_LINE_TAG)

#define STDOUT(A)                STDOUTPUT(A);


#define RED(data)            "[1;31m" << data << "[0m"
#define GREEN(data)          "[1;32m" << data << "[0m"
#define YELLOW(data)         "[1;33m" << data << "[0m"
#define BLUE(data)           "[1;34m" << data << "[0m"
#define PURPLE(data)         "[1;35m" << data << "[0m"
#define CYAN(data)           "[1;36m" << data << "[0m"
#define WHITE(data)          "[1;37m" << data << "[0m"

#define DRED(data)           "[31m" << data << "[0m"
#define DGREEN(data)         "[32m" << data << "[0m"
#define DYELLOW(data)        "[33m" << data << "[0m"
#define DBLUE(data)          "[34m" << data << "[0m"
#define DPURPLE(data)        "[35m" << data << "[0m"
#define DCYAN(data)          "[36m" << data << "[0m"
#define DWHITE(data)         "[37m" << data << "[0m"

#define BRED(data)           "[1;41m" << data << "[0m"
#define BGREEN(data)         "[1;42m" << data << "[0m"
#define BYELLOW(data)        "[1;43m" << data << "[0m"
#define BBLUE(data)          "[1;44m" << data << "[0m"
#define BPURPLE(data)        "[1;45m" << data << "[0m"
#define BCYAN(data)          "[1;46m" << data << "[0m"
#define BWHITE(data)         "[1;47m" << data << "[0m"

class Logger
{
   public :
      static bool          CanLogging(unsigned int level);
      static void          MakeLogHeader(pthread_t pid, AxString &oss);
      static void          Logging(AxString &oss);

      static unsigned int  SetLogLevel(unsigned int level);
      static unsigned int  GetLogLevel();
      static void          SetLogFile(const char *pFilename);
};

#endif
