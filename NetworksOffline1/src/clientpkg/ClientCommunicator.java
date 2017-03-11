package clientpkg;

import java.io.*;
import java.net.Socket;
import java.util.AbstractCollection;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class ClientCommunicator implements Runnable {
    private Thread t;
    private ClientMain main;
    private Socket s;
    private String serverIP;
    private int port;
    private String stdID;


    public ClientCommunicator(ClientMain main, String serverIP, int port, String stdID){
        this.main = main;
        this.serverIP = serverIP;
        this.port = port;
        this.stdID = stdID;
    }

    @Override
    public void run(){
        try {
            s = new Socket(serverIP, port); //sets up the connection
            BufferedReader reader = new BufferedReader(new InputStreamReader(s.getInputStream()));
            String ack = reader.readLine(); //receives the acknowledgement

            if(!ack.startsWith("CONNECTED")) return;
            StringTokenizer tok = new StringTokenizer(ack, ":");
            String str = tok.nextToken(); //first one is discarded, cause it is the message connected itself


            ArrayList<String> examNames = new ArrayList<>();
            while(tok.hasMoreTokens()){
                str = tok.nextToken();
                if(!str.equals("")){
                    examNames.add(str);
                }
            }
            main.setExamList(examNames);


            PrintWriter writer = new PrintWriter(new OutputStreamWriter(s.getOutputStream()));

            writer.println(stdID);
            writer.flush();
        }
        catch (Exception e){
            System.out.println(e);
        }

    }


    public void start(){
        t = new Thread(this);
        t.start();
    }
}
