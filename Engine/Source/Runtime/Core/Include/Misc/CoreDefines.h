#pragma once

#ifdef KEXPORT
//Export
#define CORE_API	DLLEXPORT
#define RHI_API		DLLEXPORT
#else
//Import
#define CORE_API	DLLIMPORT
#define RHI_API		DLLIMPORT
#endif // KEXPORT