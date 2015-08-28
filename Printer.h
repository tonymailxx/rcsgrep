// vim:ts=3:sts=3:sw=3


#ifndef RCSGREP_PRINTER_H__
#define RCSGREP_PRINTER_H__

#include "Condition.h"
#include "SessionMap.h"


class Printer {
  public :
     static void Print(Condition &condition, SessionMap &smap);

  private :
     static bool MakeLog(Condition &condition, std::list<std::string> &chunk, AxString &output);
};

#endif
