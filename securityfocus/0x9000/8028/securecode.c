/*
*   Secure Code Analizer v1.0
*   
*   Tripbit Security Development
*   Author: posidron
*   Website: tripbit.org   
*
*
*   ABOUT
*
*   This tool scans your source code to different dangerous functions, 
*   like strcpy(), gets(), getenv(), sscanf() etc.
*
*
*   OPTIONS
*
*   [+] single source file   -s [SOURCE_FILE]
*
*   
*   FEATURES
*
*   [+] several source files -m [SOURCE_FILE, SOURCE_FILE ... ]
*
*
*   GREETS
*   
*   #hackerboard, #csec, #tripbit
*
*/

#include <stdio.h>
#define FALSE 0
#define TRUE  1

int getopt(char *, char *);
int single_source(char *);
void help(char *);
void version();

int main(int argc, char *argv[])
{
    int counter=3;
    char buffer[1024];
    
    if(argc == 1 || getopt(argv[1],"h") == TRUE ) { help(argv[0]); }
    else if(getopt(argv[1],"v") == TRUE) { version(); }
    else if(argc < 3) { help(argv[0]); }

    strcpy(buffer,argv[2]);
    printf("%s",buffer);
    while(argv[counter]!=NULL)
    {
        strcat(buffer, " ");
        strcat(buffer, argv[counter++]);
    }
    
    if(getopt(argv[1],"s") == TRUE)
       single_source(buffer);
    
    return 0;
}


int getopt(char *argument,char *option)
{
    if( argument[0]=='-' && argument[1]==option[0] )
        return TRUE;
            return FALSE;
}

int single_source(char *buffer)
{ 
    char puffer[256];
    int counter = 1;
    FILE *source_file;
    source_file = fopen(buffer, "rt");
    
    if(!source_file)
       perror("Unable to open file!\n");
    else
    {
        printf("File to open: %s\n\n", buffer);
        printf("Please wait...\n\n");
     
        while(fgets(puffer, 1044, source_file) != NULL)
        {
            if(strstr(puffer, "strcpy") != NULL)
            {
                printf("Line %d, strcpy() function was found, potential buffer overflow attack!\n", counter);
                printf("   Syntax: char * strcpy ( char * dest, const char * src );\n\n");
            }
            else if(strstr(puffer, "scanf") != NULL)
            {
                printf("Line %d, scanf() function was found, potential buffer overflow attack!\n", counter);
                printf("   Syntax: int scanf ( const char * format, ... );\n\n"); 
            }
            else if(strstr(puffer, "gets") != NULL)
            {
                printf("Line %d, gets() function was found!, potential buffer overflow attack!\n", counter);
                printf("   Syntax: char * gets( char * str );\n\n");
            }
            else if(strstr(puffer, "fscanf") != NULL)
            {
                printf("Line %d, fscanf() function was found, potential buffer overflow attack!\n", counter);
                printf("   Syntax: int fscanf ( FILE * stream, const char * format, ... );\n\n");
            }
            else if(strstr(puffer, "fgets") != NULL)
            {
                printf("Line %d fgets() function was found, potential buffer overflow attack!\n", counter);
                printf("   Syntax: char * fgets ( char * str, int n, FILE * stream );\n\n"); 

            }
            else if(strstr(puffer, "setenv") != NULL)
            {
                printf("Line %d setenv() function was found, potential buffer overflow attack!\n", counter);
                printf("   Syntax: int setenv ( const char * name, const char * value, int overwrite );\n\n");
            }
            else if(strstr(puffer, "getenv") != NULL)
            {
                printf("Line %d getenv() function was found, potential buffer overflow attack!\n", counter);
                printf("   Syntax: char * getenv( const char * name );\n\n"); 
            }
            else if(strstr(puffer, "sscanf") != NULL)
            {
                printf("Line %d sscanf() function was found, potential buffer overflow attack!\n", counter);
                printf("   Syntax: int sscanf ( const char * str, const char * format, ... );\n\n"); 
            }
            else if(strstr(puffer, "strcat") != NULL)
            {
                printf("Line %d strcat() function was found, potential buffer overflow attack!\n", counter);
                printf("    Syntax: char *  strcat ( char * dest, const char * src );\n\n");
            }
            else if(strstr(puffer, "fprintf") != NULL)
            {
                printf("Line %d fprintf() function was found, potential formatstring attack!\n", counter);
                printf("   Syntax: int fprintf ( FILE * stream, const char * format, ... );\n\n"); 
            }
            else if(strstr(puffer, "sprintf") != NULL)
            {
                printf("Line %d sprintf() function was found, potential formatstring attack!\n", counter);
                printf("   Syntax: int sprintf ( char * str, const char * format, ... );\n\n"); 
            }
            else if(strstr(puffer, "snprintf") != NULL)
            {
                printf("Line %d snprintf() function was found, potential formatstring attack!\n", counter);
                printf("   Syntax: int snprintf ( char *str, size_t n, const char * format, ... );\n\n"); 
            }
            else if(strstr(puffer, "system") != NULL)
            {
                printf("Line %d system() function was found, potential formatstring attack!\n", counter);
                printf("   Syntax: int system ( const char * string );\n\n");
            }
            else if(strstr(puffer, "syslog") != NULL)
            {
                printf("Line %d syslog() function was found, potential formatstring attack!\n", counter);
                printf("   Syntax: void syslog ( int priority, const char * message, ...)\n\n");

            }
            else if(strstr(puffer, "vsprintf") != NULL)
            {
                printf("Line %d vsprintf() function was found, potential formatstring attack!\n", counter);
                printf("   Syntax: int vsprintf ( const char * buf, const char * format, va_list ap);\n\n");
            }
            else if(strstr(puffer, "vsnprintf") != NULL)
            {
                printf("Line %d vsnprintf() function was found, potential formatstring attack!\n", counter);
                printf("   Syntax: int vsnprintf ( char * str, size_t n, const char * format, va_list ap);\n\n");
            }
            else if(strstr(puffer, "popen") != NULL)
            {
                printf("Line %d popen() function was found, potential formatstring attack!\n", counter);
                printf("   Syntax: FILE* popen ( const char * command, const char * mode );\n\n");
            }
            counter++;
        }
        if(counter == 1) printf("Nothing was found!\n");
    }
    
    fclose(source_file);
    
    printf("\nThe scan has finished!\n");
    
    return 0;
}

void help(char *file_name)
{
    printf("Usage: securecode [OPTION] [SPECIFICATION]\n");
    printf("[+] single source file       -s [SOURCE_FILE]\n");
    exit(0);
}

void version()
{
    printf("Secure Code Analizer [Version 1.0a] \n(C) Copyright 2003 Tripbit Security Development\n");
    exit(0);
}
