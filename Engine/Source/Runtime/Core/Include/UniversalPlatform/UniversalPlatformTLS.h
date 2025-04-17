#pragma once

#include "CoreType.h"

struct HUniversalPlatformTLS
{
   static const UInt32 InValidTLSIndex = 0xFFFFFFFF;

   static bool IsValidTLSSlot(UInt32 SlotIndex)
   {
      return SlotIndex != InValidTLSIndex;
   }

   static UInt32 GetCurrentThreadID() 
   {
      return 0;
   }

   static UInt32 AllocateTLSSlot() { return InValidTLSIndex; }

   static void SetTLSValue(UInt32 SlotIndex, void* Value) {}

   static void* GetTLSValue(UInt32 SlotIndex)
   {
      return nullptr;
   }

   static void FreeTLSSlot(UInt32 SlotIndex) {}
};
