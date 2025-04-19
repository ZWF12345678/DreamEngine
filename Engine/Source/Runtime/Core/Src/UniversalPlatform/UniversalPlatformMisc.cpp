#include "UniversalPlatform/UniversalPlatformMisc.h"
#include <string.h>

const ANSICHAR* HUniversalPlatformMisc::GetSystemErrorMessage(ANSICHAR* OutBuffer, Int32 BufferCount, Int32 Error)
{
    const ANSICHAR* Message = "No system errors available on this platform.";
	Error = 0;
	strcpy_s(OutBuffer, BufferCount - 1, Message);
	return OutBuffer;
}