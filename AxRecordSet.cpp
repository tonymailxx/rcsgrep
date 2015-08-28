// vim:ts=3:sts=3:sw=3


#include "AxRecordSet.h"
#include <string.h>
#include <stdlib.h>
#include "AxUtil.h"

AxRecordSet::AxRecordSet()
{
   m_iSize = 0;
}

AxRecordSet::AxRecordSet(const char *pszStr, const char delimiter)
{
   m_iSize = 0;
   Parse(pszStr, delimiter);
}

AxRecordSet::~AxRecordSet()
{
   Clear();
}

void AxRecordSet::Clear() 
{
   while (!m_RecordTable.empty()) {
      delete m_RecordTable.front();
      m_RecordTable.pop_front();
   }
   m_iSize = 0;
}

bool AxRecordSet::IsEmpty()
{
   return m_RecordTable.empty();
}

__uint32_t AxRecordSet::GetSize() 
{ 
   return m_iSize; 
}

const char *AxRecordSet::GetAt(__uint32_t index)
{
   if (index >= m_iSize) return NULL;

   std::list<std::string*>::iterator iter = m_RecordTable.begin();
   for (__uint32_t idx = 0; iter != m_RecordTable.end(); idx++, iter++) {
      if (idx == index) break;
   }

   if (iter == m_RecordTable.end())
      return NULL;

   return (*iter)->empty() ? "" : (*iter)->c_str();
}

void AxRecordSet::Erase(__uint32_t index)
{
   if (index >= m_iSize) return ;

   std::list<std::string*>::iterator iter = m_RecordTable.begin();
   for (__uint32_t idx = 0; iter != m_RecordTable.end(); idx++, iter++) {
      if (idx == index) break;
   }

   if (iter == m_RecordTable.end())
      return ;

   delete *iter;
   m_RecordTable.erase(iter);
}

bool AxRecordSet::Parse(const char *pszStr, const char delimiter)
{
   if (AX_ISZERO(pszStr))
      return false;

   char *pBuf = strdup(pszStr);
   if (NULL == pBuf) 
      return false;

   char *p1, *p2;
   p1 = pBuf;
   while (*p1) {
      p2 = strchr(p1, delimiter);
      if (NULL == p2) {
         // 마지막이 delimiter가 아닌경우
         m_RecordTable.push_back(new std::string(p1));
         break;
      } else if (p1 == p2) { // 첫문자가 delimiter인 경우
         m_RecordTable.push_back(new std::string);
         p1++;
      } else {
         *p2 = '\0';
         m_RecordTable.push_back(new std::string(p1));
         p1 = p2+1;
      }

      if ('\0' == *p1) {
         // 마지막이 delimiter인 경우
         m_RecordTable.push_back(new std::string);
         break;
      }
   }

   free(pBuf);

   m_iSize = m_RecordTable.size();
   return true;
}

void AxRecordSet::PushBack(const char *pszStr)
{
   if (AX_ISZERO(pszStr)) {
      m_RecordTable.push_back(new std::string);
   } else {
      m_RecordTable.push_back(new std::string(pszStr));
   }

   m_iSize = m_RecordTable.size();
}

