// vim:ts=2:sts=2:sw=2


#include "Grep.h"
#include "AxLib.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>

class PrevBuf {
  public :
    PrevBuf();
    ~PrevBuf();

    void                  Add(unsigned long idx, const char *buf);
    void                  eraseAll();
    void                  flush(Condition &m_Condition);
    void                  SetListCount(const int iValue);
    void                  SetFilename(string sName);

  private :
    map<unsigned long, string> m_prevList;
    unsigned int          iBufCount;
    string                sFilename;
};

PrevBuf::PrevBuf() 
{ 
  iBufCount = 0; 
}

PrevBuf::~PrevBuf() 
{ 
  eraseAll(); 
}

void PrevBuf::SetListCount(const int iValue)
{
  if (iValue > 0) iBufCount = iValue;
}

void PrevBuf::SetFilename(string sName)
{
  sFilename = sName;
}

void PrevBuf::flush(Condition &m_Condition)
{
  map<unsigned long, string>::iterator iter;
  while (!m_prevList.empty()) {
    iter = m_prevList.begin();
    OutStream(iter->first, sFilename, (iter->second.empty() ? "" : iter->second.c_str()), m_Condition);
  }
}

void PrevBuf::Add(unsigned long idx, const char *buf)
{
  if (0 == iBufCount) return;

  if (iBufCount > m_prevList.size()) {
    m_prevList.insert( pair<unsigned long, string>(idx, buf) );
  } else {
    m_prevList.erase(m_prevList.begin());
    m_prevList.insert( pair<unsigned long, string>(idx, buf) );
  }
}

void PrevBuf::eraseAll()
{
  m_prevList.clear();
}

//void Grep(Condition &m_Condition)
//{
//  const int iPrev = m_Condition.getPrintPrevLines();
//  const int iNext = m_Condition.getPrintNextLines();
//  unsigned long idx = 0;
//  char readbuf[40960];
//
//  PrevBuf PrevList;
//  int iNextPrint = 0;
//  PrevList.SetListCount(iPrev);
//
//  while (!feof(stdin)) {
//    idx++;
//    fgets(readbuf, sizeof(readbuf), stdin);
//    readbuf[sizeof(readbuf)-1] = 0;
//
//    int iLen = strlen(readbuf);
//    if (0 == iLen) continue;
//
//    for (char *ch = readbuf+iLen-1; (ch != readbuf) && (('\r' == *ch) || ('\n' == *ch)); ch--, iLen--) {
//      *ch = 0x00;
//    }
//
//    string sColorBuf;
//    if (!m_Condition.isPrintColor() && m_Condition.isMatchPattern((const char*) readbuf, (const int) iLen)) {
//      PrevList.flush();
//      iNextPrint = iNext;
//
//      if (m_Condition.isPrintLineNumber()) printf("%ld : ", idx);
//      printf("%s\n", readbuf);
//
//    } else if (m_Condition.isPrintColor() && m_Condition.isMatchPattern((const char*) readbuf, (const int) iLen, sColorBuf)) {
//      PrevList.flush();
//      iNextPrint = iNext;
//
//      if (m_Condition.isPrintLineNumber()) printf("%ld : ", idx);
//      printf("%s\n", sColorBuf.c_str());
//
//    } else {
//      if (iNextPrint > 0) {
//        iNextPrint--;
//
//        if (m_Condition.isPrintLineNumber()) printf("%ld : ", idx);
//        printf("%s\n", readbuf);
//
//      } else {
//        string sTmp;
//
//        if (m_Condition.isPrintLineNumber()) { 
//          char tmp[32];
//          sprintf(tmp, "%ld : ", idx);
//          sTmp = tmp;
//        }
//        sTmp += readbuf;
//
//        PrevList.add(sTmp.c_str());
//      }
//    }
//  }
//}

void Grep(Condition &m_Condition)
{
  const int     iPrev = m_Condition.getPrintPrevLines();
  const int     iNext = m_Condition.getPrintNextLines();

  bool          bResult;
  int           iLen;
  int           iNextPrint = 0;
  char          readbuf[40960];
  string        sFilename;
  PrevBuf       PrevList;

  PrevList.SetListCount(iPrev);
  PrevList.SetFilename(sFilename);


  for (unsigned long idx = 1; !feof(stdin); ++idx) {

    readbuf[0] = 0x00;
    fgets(readbuf, sizeof(readbuf), stdin);
    readbuf[sizeof(readbuf)-1] = 0;

    iLen = strlen(readbuf);
    if (0 == iLen) continue;

    for (char *ch = readbuf+iLen-1; ('\r' == *ch) || ('\n' == *ch); --ch, --iLen) { *ch = 0x00; }


    if ( (!m_Condition.IsExistRegexpPattern()) &&  (!m_Condition.IsExistInvertPattern()) ) {
      // Print Options만 처리
      OutStream(idx, sFilename, readbuf, m_Condition);

    } else {
      // Pattern 검색
      bResult = m_Condition.isMatchPattern((const char*) readbuf, (const int) iLen);
      if (bResult) {
        PrevList.flush(m_Condition);
        iNextPrint = iNext;

        OutStream(idx, sFilename, readbuf, m_Condition);
      }

      if (!bResult) {
        if (iNextPrint > 0) {
          iNextPrint--;
          OutStream(idx, sFilename, readbuf, m_Condition);

        } else {
          PrevList.Add(idx, readbuf);
        }
      }
    }

  }
}

void Grep(const char *pFilename, Condition &m_Condition)
{
  const int     iPrev = m_Condition.getPrintPrevLines();
  const int     iNext = m_Condition.getPrintNextLines();

  bool          bResult;
  int           iLen;
  int           iNextPrint = 0;
  char          readbuf[40960];
  string        sFilename = pFilename;
  PrevBuf       PrevList;

  PrevList.SetListCount(iPrev);
  PrevList.SetFilename(sFilename);


  FILE *fp;
  if (NULL == (fp = fopen(pFilename, "r"))) {
    printf("**** file[%s] Open Error !!\n", pFilename);
    return ;
  }


  for (unsigned long idx = 1; !feof(fp); ++idx) {

    readbuf[0] = 0x00;
    fgets(readbuf, sizeof(readbuf), fp);
    readbuf[sizeof(readbuf)-1] = 0;

    iLen = strlen(readbuf);
    if (0 == iLen) continue;

    for (char *ch = readbuf+iLen-1; ('\r' == *ch) || ('\n' == *ch); --ch, --iLen) { *ch = 0x00; }


    if ( (!m_Condition.IsExistRegexpPattern()) &&  (!m_Condition.IsExistInvertPattern()) ) {
      // Print Options만 처리
      OutStream(idx, sFilename, readbuf, m_Condition);

    } else {

      bResult = m_Condition.isMatchPattern((const char*) readbuf, (const int) iLen);
      if (bResult) {
        PrevList.flush(m_Condition);
        iNextPrint = iNext;

        OutStream(idx, sFilename, readbuf, m_Condition);
      }

      if (!bResult) {
        if (iNextPrint > 0) {
          iNextPrint--;
          OutStream(idx, sFilename, readbuf, m_Condition);

        } else {
          PrevList.Add(idx, readbuf);
        }
      }
    }

  }

  fclose(fp);
}

void OutStream(unsigned long idx, string &rFilename, const char *pStream, Condition &m_Condition)
{
  if (m_Condition.isPrintFilename() && (!rFilename.empty()))    
    printf("%s:", rFilename.c_str());

  if (m_Condition.isPrintLineNumber())  
    printf("%ld:", idx);


  AxString OUTBUF;

  if (!m_Condition.m_DropColumn.empty()) {
    int   iCurColumn    = 0;
    const char *pCurPtr = pStream;

    set<int>::iterator iter = m_Condition.m_DropColumn.begin();
    for (; (!AX_ISZERO(pCurPtr)) && (iter != m_Condition.m_DropColumn.end());) {
      ++iCurColumn;

      if (iCurColumn == *iter) {
        ++iter;
        pCurPtr = axSkipBlank(pCurPtr);
        pCurPtr = axSkipNonBlank(pCurPtr);

        if (1 == iCurColumn) {
          pCurPtr = axSkipBlank(pCurPtr);
        }

      } else {
        const char *pEndPtr = axSkipBlank(pCurPtr);
        pEndPtr = axSkipNonBlank(pEndPtr);

        OUTBUF.AppendSubstr(pCurPtr, pEndPtr - pCurPtr);

        pCurPtr = pEndPtr;
      }
    }

    OUTBUF += pCurPtr;
  } else {
    OUTBUF = pStream;
  }

  // ===========================================================================
  // 20130401 Version 1.1.0 add Show Column Prefix
  // ---------------------------------------------------------------------------
  if (!m_Condition.m_ShowColumnPrefix.empty()) {
    if (OUTBUF.IsEmpty()) {
      return;
    }

    AxRecordSet rSet(OUTBUF.c_str(), ' ');

    // Condition 순으로 정렬하여 출력 포멧 정렬 목적도 달성한다.
    // 반대의 경우는 일단 필요없을 것 같다.
    OUTBUF.Clear();
    list<string>::iterator iter = m_Condition.m_ShowColumnPrefix.begin();
    for (; iter != m_Condition.m_ShowColumnPrefix.end(); ++iter) {
      for (__uint32_t IDX = 0; IDX < rSet.GetSize(); ++IDX) {
        if (axPrefixMatch(rSet.GetAt(IDX), iter->c_str())) {
          if (false == OUTBUF.IsEmpty()) {
            OUTBUF += " ";
          }
          OUTBUF += rSet.GetAt(IDX);
        }
      }
    }

    if (OUTBUF.IsEmpty()) {
      return;
    }
  }
  // ---------------------------------------------------------------------------

  // ===========================================================================
  // 20130403 Version 1.2.0 add Drop Column Prefix
  // ---------------------------------------------------------------------------
  if (!m_Condition.m_DropColumnPrefix.empty()) {
    if (OUTBUF.IsEmpty()) {
      return;
    }

    AxRecordSet rSet(OUTBUF.c_str(), ' ');

    OUTBUF.Clear();
    list<string>::iterator iter = m_Condition.m_DropColumnPrefix.begin();
    for (; iter != m_Condition.m_DropColumnPrefix.end(); ++iter) {
      for (__uint32_t IDX = 0; IDX < rSet.GetSize(); ++IDX) {
        if (axPrefixMatch(rSet.GetAt(IDX), iter->c_str())) {
          rSet.Erase(IDX);
        }
      }
    }

    for (__uint32_t IDX = 0; IDX < rSet.GetSize(); ++IDX) {
      if (AX_ISZERO(rSet.GetAt(IDX))) {
        continue;
      }

      if (false == OUTBUF.IsEmpty()) {
        OUTBUF += " ";
      }
      OUTBUF += rSet.GetAt(IDX);
    }

    if (OUTBUF.IsEmpty()) {
      return;
    }
  }
  // ---------------------------------------------------------------------------

  if (!m_Condition.m_DropString.empty()) {
    set<string>::iterator iter = m_Condition.m_DropString.begin();
    for (; iter != m_Condition.m_DropString.end(); ++iter) {
      OUTBUF.Replace(iter->c_str(), "");
    } 
  }

  if ((0 < m_Condition.m_LimitWidth) && ((int)OUTBUF.GetSize() > m_Condition.m_LimitWidth)) {
    OUTBUF.Resize(m_Condition.m_LimitWidth);
    OUTBUF += "[0m";
  }

  // TODO. Color 적용해야함.


  // Line-Highlight
  if (m_Condition.IsLineHighlight(OUTBUF.c_str())) {
    std::cout << "[1;44m" << OUTBUF.c_str() << "[0m" << std::endl;
    return;
  } 

  if (OUTBUF.IsEmpty()) {
    std::cout << std::endl;
  } else {
    std::cout << OUTBUF.c_str() << std::endl;
  }
}

