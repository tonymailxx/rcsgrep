// vim:ts=3:sts=3:sw=3

#include "Condition.h"
#include <iostream>
#include <sys/stat.h>
#include "AxUtil.h"
#include "AxString.h"
#include "AxRecordSet.h"
#include "Logger.h"

static const unsigned int MAX_WIDTH_SIZE = 1024;
static const unsigned int MIN_WIDTH_SIZE = 32;

Condition::Condition()
{
   Clear();
}

Condition::~Condition()
{
   Clear();
}

void Condition::Clear()
{
    // Selection and interpretation
    m_callIdTable.clear();
    m_ctnTable.clear();
    m_imdnIdTable.clear();
    m_service                 = td::cond::service::NOT_EXIST;
    m_startTime.clear();
    m_endTime.clear();
    m_inputFileTable.clear();

    m_interactive             = false;


    // Miscellaneous
    m_miscVersion             = false;
    m_miscHelp                = false;
    m_miscExample             = false;


    // Output control
    //m_printFilename         = false;
    //m_printLineNumber       = false;
    m_printColorFormat        = false;
    m_printStripCRchar        = false;
    m_printWidthSize          = 0;
    m_logFile.clear();
}

void Condition::Print()
{
   const char SECTIONTAG[] = "      ";
   const char ELEMENTTAG[] = "         ";

   STDOUT("================================================================================");
   STDOUT("# Condition"                                                                     );
   STDOUT("--------------------------------------------------------------------------------");

   {
      STDOUT("   [Selection and interpretation]");

      if (false == m_callIdTable.empty()) {
         STDOUT(SECTIONTAG << "* call-id");
         std::set<std::string>::iterator iter = m_callIdTable.begin();
         for (int idx = 1; iter != m_callIdTable.end(); ++iter, ++idx) {
            STDOUT(ELEMENTTAG << idx << ". " << *iter);
         }
      }

      if (false == m_ctnTable.empty()) {
         STDOUT(SECTIONTAG << "* ctn");
         std::set<std::string>::iterator iter = m_ctnTable.begin();
         for (int idx = 1; iter != m_ctnTable.end(); ++iter, ++idx) {
            STDOUT(ELEMENTTAG << idx << ". " << *iter);
         }
      }

      if (false == m_imdnIdTable.empty()) {
         STDOUT(SECTIONTAG << "* imdn-id");
         std::set<std::string>::iterator iter = m_imdnIdTable.begin();
         for (int idx = 1; iter != m_imdnIdTable.end(); ++iter, ++idx) {
            STDOUT(ELEMENTTAG << idx << ". " << *iter);
         }
      }

      if (td::cond::service::NOT_EXIST != m_service) {
         STDOUT(SECTIONTAG << "* service : " << td::cond::service::name(m_service));
      }

      if ((false == m_startTime.empty()) || (false == m_endTime.empty())) {
         AxString outstr;

         if (false == m_startTime.empty()) {
            outstr += m_startTime;
            outstr += " ";
         }
         outstr += "~";
         if (false == m_endTime.empty()) {
            outstr += " ";
            outstr += m_endTime;
         }

         STDOUT(SECTIONTAG << "* time-scope : " << outstr);
      }

      if (m_interactive) {
         STDOUT(SECTIONTAG << "* interaction : ON");
      }

      if (false == m_inputFileTable.empty()) {
         STDOUT(SECTIONTAG << "* input file");
         std::set<std::string>::iterator iter = m_inputFileTable.begin();
         for (int idx = 1; iter != m_inputFileTable.end(); ++iter, ++idx) {
            STDOUT(ELEMENTTAG << idx << ". " << *iter);
         }
      }
   }


   {
      STDOUT("   [Output control]");
      if (m_printColorFormat) {
         STDOUT(SECTIONTAG << "* color : ON");
      }

      if (m_printStripCRchar) {
         STDOUT(SECTIONTAG << "* unix-newline : ON");
      }

      if (m_printWidthSize) {
         STDOUT(SECTIONTAG << "* width-size : " << m_printWidthSize);
      }
      if (false == m_logFile.empty()) {
         STDOUT(SECTIONTAG << "* write : " << m_logFile);
      }

   }

   STDOUT("--------------------------------------------------------------------------------");
}

bool Condition::IsValid()
{
   // 조회 조건 확인
   do {
      if (false == m_callIdTable.empty())
         break;

      if (false == m_ctnTable.empty())
         break;

      if (false == m_imdnIdTable.empty())
         break;

      return false;
   } while (0);


   // 조회 파일 확인
   if (m_inputFileTable.empty())
      return false;


   return true;
}

bool Condition::Parse(int argc, char **argv)
{
   if (1 >= argc) {
      std::cout << "invalid argc:" << argc << std::endl;
      return false;
   }


   AxString buf;
   for (int i = 1; i < argc; i++) 
   {
      // [Miscellaneous]
      if (axStrMatch(argv[i], "-V") || axStrMatch(argv[i], "--version")) {
         m_miscVersion = true;
         return true;
      }
      if (axStrMatch(argv[i], "--help")) {
         m_miscHelp = true;
         return true;
      }
      if (axStrMatch(argv[i], "--example")) {
         m_miscExample = true;
         return true;
      }

      // [Selection and interpretation]
      if          (axPrefixMatch(argv[i], "--call-id=")) {
         buf = argv[i] + strlen("--call-id=");
         buf.TrimQuoted();

         bool result = SetTable(buf.c_str(), m_callIdTable);
         if (!result) {
            std::cout << "invalid option[--call-id]" << std::endl;
            return false;
         }

      } else if   (axPrefixMatch(argv[i], "--ctn=")) {
         buf = argv[i] + strlen("--ctn=");
         buf.TrimQuoted();

         bool result = SetTable(buf.c_str(), m_ctnTable);
         if (!result) {
            std::cout << "invalid option[--ctn]" << std::endl;
            return false;
         }

      } else if   (axPrefixMatch(argv[i], "--imdn-id=")) {
         buf = argv[i] + strlen("--imdn-id=");
         buf.TrimQuoted();

         bool result = SetTable(buf.c_str(), m_imdnIdTable);
         if (!result) {
            std::cout << "invalid option[--imdn-id]" << std::endl;
            return false;
         }

      } else if   (axPrefixMatch(argv[i], "--service=")) {
         buf = argv[i] + strlen("--service=");
         buf.TrimQuoted();
         buf.Trim();

         m_service = td::cond::service::categorize(buf.c_log());
         if (td::cond::service::NOT_EXIST == m_service) {
            std::cout << "invalid option[--service]" << std::endl;
            return false;
         }

      } else if   (axPrefixMatch(argv[i], "--time-scope=")) {
         buf = argv[i] + strlen("--time-scope=");
         buf.TrimQuoted();

         AxRecordSet RSet(buf.c_str(), '|');

         AxString startTime   = RSet.GetAt(0);  startTime.Trim();
         AxString endTime     = RSet.GetAt(1);  endTime.Trim();
         if (startTime.IsEmpty() && endTime.IsEmpty()) {
            std::cout << "invalid option[--time-scope]" << std::endl;
            return false;
         }

         if (false == startTime.IsEmpty()) {
            if (false == IsDateTime(startTime.c_str())) {
               std::cout << "invalid option[--time-scope]" << std::endl;
               return false;
            }
            m_startTime = startTime.c_str();
         }

         if (false == endTime.IsEmpty()) {
            if (false == IsDateTime(endTime.c_str())) {
               std::cout << "invalid option[--time-scope]" << std::endl;
               return false;
            }
            m_endTime = endTime.c_str();
         }

      } else if   (axStrMatch   (argv[i], "--interactive")) {
         m_interactive = true;

      }

      // [Output control]
      else if   (axStrMatch   (argv[i], "--color")) {
         m_printColorFormat = true;

      } else if   (axStrMatch   (argv[i], "--unix-newline")) {
         m_printStripCRchar = true;

      } else if   (axPrefixMatch(argv[i], "--width=")) {
         buf = argv[i] + strlen("--width=");
         buf.TrimQuoted();
         buf.Trim();

         if (buf.IsEmpty() || (false == buf.IsDigit())) {
            std::cout << "invalid option[--width]" << std::endl;
            return false;
         }

         m_printWidthSize = buf.AsUInt32();
         if       (MIN_WIDTH_SIZE > m_printWidthSize) m_printWidthSize = MIN_WIDTH_SIZE;
         else if  (MAX_WIDTH_SIZE < m_printWidthSize) m_printWidthSize = MAX_WIDTH_SIZE;

      } else if   (axPrefixMatch(argv[i], "--write=")) {
         buf = argv[i] + strlen("--write=");
         buf.TrimQuoted();
         buf.Trim();

         if (buf.IsEmpty()) {
            std::cout << "invalid option[--write]" << std::endl;
            return false;
         }

         Logger::SetLogFile(buf.c_str());
         m_logFile = buf.c_str();

      } else if   (axPrefixMatch(argv[i], "--log-level=")) {
         buf = argv[i] + strlen("--log-level=");
         buf.TrimQuoted();
         buf.Trim();

         if (buf.IsEmpty() || (false == buf.IsDigit())) {
            std::cout << "invalid option[--log-level]" << std::endl;
            return false;
         }

         Logger::SetLogLevel(buf.AsUInt32());

      } else if   ('-' == argv[i][0]) {
         std::cout << "unsupport option[" << argv[i] << "]" << std::endl;
         return false;

      } else if   (IsFile(argv[i])) {
         m_inputFileTable.insert(argv[i]);

      } else {
         std::cout << "invalid file:" << argv[i] << std::endl;
         return false;
      }
   }

   return true;
}

bool Condition::IsDateTime(const char *pDateTime)
{
   if (AX_ISZERO(pDateTime)) {
      return false;
   }

   if (19 > strlen(pDateTime)) {
      return false;
   }

   // 2011-06-11T07:03:54
   AxString    sValue;
   __int32_t   nValue;


   // date-fullyear   = 4DIGIT
   sValue.Assign(pDateTime, 4);              pDateTime += 4;
   if (false == sValue.IsDigit())            { return false; }
   nValue = sValue.AsInt32();
   if ((1968 > nValue) || (2038 < nValue))   { return false; }

   if ('-' != *pDateTime) { return false; }  pDateTime += 1;

   // date-month      = 2DIGIT ; 01-12
   sValue.Assign(pDateTime, 2);              pDateTime += 2;
   if (false == sValue.IsDigit())            { return false; }
   nValue = sValue.AsInt32();
   if ((1 > nValue) || (12 < nValue))        { return false; }

   if ('-' != *pDateTime) { return false; }  pDateTime += 1;

   // date-mday       = 2DIGIT ; 01-28, 01-29, 01-30, 01-31 based on month/year
   sValue.Assign(pDateTime, 2);              pDateTime += 2;
   if (false == sValue.IsDigit())            { return false; }
   nValue = sValue.AsInt32();
   if ((1 > nValue) || (31 < nValue))        { return false; }

   if ('T' != *pDateTime) { return false; }  pDateTime += 1;

   // time-hour       = 2DIGIT ; 00-23
   sValue.Assign(pDateTime, 2);              pDateTime += 2;
   if (false == sValue.IsDigit())            { return false; }
   nValue = sValue.AsInt32();
   if ((0 > nValue) || (23 < nValue))        { return false; }

   if (':' != *pDateTime) { return false; }  pDateTime += 1;

   // time-minute     = 2DIGIT ; 00-59
   sValue.Assign(pDateTime, 2);              pDateTime += 2;
   if (false == sValue.IsDigit())            { return false; }
   nValue = sValue.AsInt32();
   if ((0 > nValue) || (59 < nValue))        { return false; }

   if (':' != *pDateTime) { return false; }  pDateTime += 1;

   // time-second     = 2DIGIT ; 00-58, 00-59, 00-60 based on leap second rules
   sValue.Assign(pDateTime, 2);              pDateTime += 2;
   if (false == sValue.IsDigit())            { return false; }
   nValue = sValue.AsInt32();
   if ((0 > nValue) || (59 < nValue))        { return false; }

   return true;
}

bool Condition::HasMiscellaneous()
{
   if (IsMiscVersion())    return true;
   if (IsMiscHelp())       return true;
   if (IsMiscExample())    return true;

   return false;
}

bool Condition::IsMiscVersion()
{
   return m_miscVersion;
}

bool Condition::IsMiscHelp()
{
   return m_miscHelp;
}

bool Condition::IsMiscExample()
{
   return m_miscExample;
}

bool Condition::SetTable(const char *pValue, std::set<std::string> &table)
{
   if (AX_ISZERO(pValue))
      return false;

   AxString buf;
   AxRecordSet RSet(pValue, '|');
   for (__uint32_t j = 0; j < RSet.GetSize(); ++j) {
      buf = RSet.GetAt(j);
      buf.Trim();

      if (false == buf.IsEmpty()) {
         table.insert(buf.c_str());
      }
   }

   return (table.empty() ? false : true);
}


bool Condition::IsFile(const char *pFileName)
{
   struct stat buf;
   if (lstat(pFileName, &buf) < 0) {
      return false;
   }

   if (S_ISREG(buf.st_mode)) {
      return true;
   }

   return false;
}

