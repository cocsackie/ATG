#ifndef __BASEFILE_H__
#define __BASEFILE_H__

#include "DynTab.h"

struct IntermediateData{};

struct IntermediateData * BaseFile_loadBaseFilesToIntermediateData(DynTab * fileNames);

#endif
