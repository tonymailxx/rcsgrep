// vim:ts=3:sts=3:sw=3


#ifndef RCSGREP_AXRECORDSET_H__
#define RCSGREP_AXRECORDSET_H__

#include <list>
#include <string>

class AxRecordSet
{
   public :
      AxRecordSet();
      AxRecordSet(const char *pszStr, const char delimiter = ' ');
      ~AxRecordSet();

      void                    Clear();
      bool                    IsEmpty();
      void                    Erase(__uint32_t index);
      bool                    Parse(const char *pszStr, const char delimiter = ' ');

      const char             *GetAt(__uint32_t index);
      __uint32_t              GetSize();

      void                    PushBack(const char *pszStr);

   private :
      std::list<std::string*> m_RecordTable;
      __uint32_t              m_iSize;
};

#endif
