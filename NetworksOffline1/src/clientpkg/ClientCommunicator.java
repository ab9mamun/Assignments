package clientpkg;

import java.io.*;
import java.net.Socket;

public class ClientCommunicator implements Runnable {
    private Thread t;
    private ClientMain main;
    private Socket s;
    private String serverIP;
    private int port;
    private String stdID;


    public ClientCommunicator(String serverIP, int port, String stdID){
        main = ClientMain.main();
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

            if(!ack.contains("accepted")) return;

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
