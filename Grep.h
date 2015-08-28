// vim:ts=2:sts=2:sw=2


#ifndef __AX_GREP_H__
#define __AX_GREP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Condition.h"

void Grep(Condition &m_Condition);
void Grep(const char *pFilename, Condition &m_Condition);

void OutStream(unsigned long idx, string &rFilename, const char *pStream, Condition &m_Condition);

#endif
