// vim:ts=3:sts=3:sw=3


#ifndef RCSGREP_MMI_H__
#define RCSGREP_MMI_H__


class Mmi {
  public :
     static int Choose();

  private :
     static int GetInput(const char *head, char *pBuf, const int nSize);
     static int Readline(char *pBuf, const int nSize);
};

#endif
