package server;

import java.io.*;
import java.net.Socket;

public class ServerThread implements Runnable {
    Thread t;
    Socket socket;
    String stdid;

     PrintWriter writer;
     BufferedReader reader;
     InputStream fileDownloader;
     OutputStream fileUploader;

    public ServerThread(Socket sock){
        t = new Thread(this);
        t.start();
        socket = sock;
    }

    @Override
    public void run(){


        try{
            reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            writer = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));
            fileDownloader = socket.getInputStream();
            fileUploader = socket.getOutputStream();

            while(true){
                String msg = reader.readLine();
                String[] tokens = msg.split(":");
                if(msg.startsWith("stdid")){
                    stdid = tokens[1];
                    System.out.println(stdid);
                }
            }


        }
        catch (Exception e){
            e.printStackTrace();
        }
    }
}
