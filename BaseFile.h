#ifndef __BASEFILE_H__
#define __BASEFILE_H__

#include "DynTab.h"
#include "IntermediateData.h"

IntermediateData * BaseFile_loadBaseFilesToIntermediateData(DynTab * files, int gramType);

#endif
