// vim:ts=3:sts=3:sw=3


#ifndef RCSGREP_FRACTIONATOR_H__
#define RCSGREP_FRACTIONATOR_H__

#include "Condition.h"
#include "SessionMap.h"


class Fractionator {
  public :
     static bool Fractionate(const char *file, const char *protocol, Condition &condition, SessionMap &smap);
     static bool ExtractLog(const char *file, SessionMap &smap);

  private :
     static bool ConditionMatching(Condition &condition, LLTParser &lltmsg);
     static bool CollectLog(std::list<std::string> &chunk, SessionMap &smap);
};

#endif
