// vim:ts=3:sts=3:sw=3


#ifndef RCSGREP_CONDITION_H__
#define RCSGREP_CONDITION_H__

#include <list>
#include <set>
#include <string>

#include "TypeDef.h"


class Condition {
  public :
    Condition();
    ~Condition();

    void                   Clear();
    void                   Print();

    bool                   IsValid();
    bool                   Parse(int argc, char **argv);
    bool                   IsDateTime(const char *pDateTime);

    bool                   HasMiscellaneous();
    bool                   IsMiscVersion();
    bool                   IsMiscHelp();
    bool                   IsMiscExample();

  private :
    bool                   SetTable(const char *pValue, std::set<std::string> &table);
    bool                   IsFile(const char *pFileName);

  public :
    // Selection and interpretation
    std::set<std::string>  m_callIdTable;
    std::set<std::string>  m_ctnTable;
    std::set<std::string>  m_imdnIdTable;
    td::cond::service::e   m_service;
    std::string            m_startTime;
    std::string            m_endTime;
    std::set<std::string>  m_inputFileTable;
    bool                   m_interactive;

    // Miscellaneous
    bool                   m_miscVersion;
    bool                   m_miscHelp;
    bool                   m_miscExample;

    // Output control
    //bool                   m_printFilename;
    //bool                   m_printLineNumber;
    bool                   m_printColorFormat;
    bool                   m_printStripCRchar;
    unsigned int           m_printWidthSize;
    std::string            m_logFile;
};

#endif
