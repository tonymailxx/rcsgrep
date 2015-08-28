// vim:ts=3:sts=3:sw=3


#ifndef RCSGREP_SESSIONMAP_H__
#define RCSGREP_SESSIONMAP_H__

#include <string>
#include <map>
#include <set>

#include "LLTParser.h"

class SessionMap {
  public :
     class IhSession {
        public :
           void Clear();
           std::string           m_type;
           std::string           m_time;
           std::set<std::string> m_CallIdSet;
     };

     class SessionGroup {
        public :
           void Clear();
           std::string           m_time;
           std::set<__uint32_t>  m_IhSessionSet;

           //            time,        log
           std::multimap<std::string, std::list<std::string> > m_Logs;
     };

  public :
     void Clear();
     void Print();
     bool Compose(LLTParser &transaction);
     bool BuildSessionGroup();

  private :
     void InsertIhSessionMap(LLTParser &transaction);
     void InsertDialogMap(LLTParser &transaction);

  public :
     std::map<int, IhSession>                      m_IhSessionMap;
     std::map<std::string, std::set<__uint32_t> >  m_DialogMap;
     std::map<__uint32_t, SessionGroup>            m_SessionGroupMap;
};

#endif
